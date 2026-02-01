from flask import Flask
import dash
from dash import html, dcc, no_update
from dash.dependencies import Input, Output, State
import plotly.graph_objs as go
from collections import deque
import datetime
import requests

#TODO: disable slider and valve button if we are in auto 
# if in manual make the valve value controlled from dshboard (here)
# La dashboard deve leggere la modalità (dal suo mode-store o dal backend) e impostare disabled=True su slider/bottone quando isManual=False.

# if auto, disable set valve value, take info from backend
# Il sistema reale (o simulatore) deve leggere /api/postdata e comportarsi di conseguenza (in auto ignora i comandi manuali, in manuale usa valveValue comandato, ecc.).

data_queue = deque(maxlen=60)

server = Flask(__name__)
app = dash.Dash(__name__, server=server, title="Dashboard")

app.layout = html.Div([
    dcc.Interval(id="interval-component", interval=1000, n_intervals=0),

    html.H1("River Monitoring Dashboard", style={"textAlign": "center", "fontSize": "48px"}),

    html.Div([
        html.Div([dcc.Graph(id="water-level-graph")], style={"width": "50%", "display": "inline-block"}),
        html.Div([dcc.Graph(id="valve-level-graph")], style={"width": "50%", "display": "inline-block"}),
    ]),

    html.Div(id="status-display", style={"textAlign": "center", "fontSize": "24px"}),

    html.Div([
        html.H3("Set Valve Value"),
        html.Div([
            dcc.Slider(
                id="valveValue",
                min=0, max=100, value=0,
                marks={i: f"{i}%" for i in range(0, 101, 10)},
                disabled=False,
            )
        ], style={"width": "33%", "display": "inline-block"}),

        html.Button("Set Valve Value", id="send-valveValue", n_clicks=0, style={"marginRight": "1%"}, disabled=False),
        html.Button("Switch to AUTO", id="send-autoMode", n_clicks=0),
    ], style={"display": "flex", "justifyContent": "center", "alignItems": "center"}),

    # output separati per evitare conflitto
    html.Div(id="dummy-output-mode", style={"display": "none"}),
    html.Div(id="dummy-output-valve", style={"display": "none"}),
    html.Div(id="dummy-output1", style={"display": "none"}),

    dcc.Store(id="mode-store", data={"isManual": True}),
])

@app.callback(
    Output("dummy-output1", "children"),
    Input("interval-component", "n_intervals")
)
def get_post_data(n):
    try:
        r = requests.get("http://localhost:8050/api/systemdata", timeout=2)
        r.raise_for_status()
        j = r.json()
        if not j:
            return ""  # ancora niente dati
        data = j[0]
    except Exception as e:
        return f"GET error: {e}"

    data["wl"] = float(data["wl"])
    data["valveValue"] = int(data["valveValue"])
    data["timestamp"] = datetime.datetime.now().strftime("%H:%M:%S")
    data_queue.append(data)
    return ""

@app.callback(
    Output("valveValue", "disabled"),
    Output("send-valveValue", "disabled"),
    Output("valveValue", "value"),
    Input("mode-store", "data"),
    Input("interval-component", "n_intervals"),
)
def update_manual_ui(mode_store, n):
    is_manual = bool(mode_store.get("isManual", True))

    # In MANUAL: controlli attivi e NON tocchiamo il valore dello slider (lo decide l'utente)
    if is_manual:
        return False, False, no_update

    # In AUTO: controlli disabilitati e slider "segue" il valore reale che arriva dai dati
    if data_queue:
        backend_valve = int(data_queue[-1].get("valveValue", 0))
        return True, True, backend_valve

    # Se non ci sono ancora dati, disabilita e lascia value com'è
    return True, True, no_update


@app.callback(
    Output("mode-store", "data"),
    Output("dummy-output-mode", "children"),
    Output("send-autoMode", "children"),
    Input("send-autoMode", "n_clicks"),
    State("mode-store", "data"),
    prevent_initial_call=True
)
def toggle_mode(n_clicks, store):
    is_manual = bool(store.get("isManual", True))
    new_is_manual = not is_manual

    try:
        r = requests.post("http://localhost:8050/api/postdata",
                          json={"isManual": new_is_manual}, timeout=2)
        msg = f"isManual={new_is_manual} -> {r.status_code}"
    except requests.RequestException as e:
        return store, f"POST failed: {e}", no_update

    button_text = "Switch to AUTO" if new_is_manual else "Switch to MANUAL"
    return {"isManual": new_is_manual}, msg, button_text

@app.callback(
    Output("dummy-output-valve", "children"),
    Input("send-valveValue", "n_clicks"),
    State("valveValue", "value"),
    prevent_initial_call=True
)
def send_valveValue(n_clicks, valveValue):
    r = requests.post("http://localhost:8050/api/postdata",
                      json={"valveValue": valveValue}, timeout=2)
    return f"Valve={valveValue} -> {r.status_code}"

@app.callback(Output("water-level-graph", "figure"),
              Input("interval-component", "n_intervals"))
def update_water(n):
    return go.Figure(
        data=[go.Scatter(
            x=[d["timestamp"] for d in data_queue],
            y=[d["wl"] for d in data_queue],
            mode="lines+markers"
        )],
        layout=go.Layout(title="Livello acqua", xaxis={"title": "Ora"}, yaxis={"title": "wl"})
    )

@app.callback(Output("valve-level-graph", "figure"),
              Input("interval-component", "n_intervals"))
def update_valve(n):
    return go.Figure(
        data=[go.Scatter(
            x=[d["timestamp"] for d in data_queue],
            y=[d["valveValue"] for d in data_queue],
            mode="lines+markers"
        )],
        layout=go.Layout(title="Valvola", xaxis={"title": "Ora"}, yaxis={"title": "valveValue"})
    )

@app.callback(Output("status-display", "children"),
              Input("interval-component", "n_intervals"))
def update_status(n):
    if data_queue:
        return f'Stato del sistema: {data_queue[-1].get("status","?")}'
    return "In attesa di dati..."

if __name__ == "__main__":
    app.run(debug=True, port=8057)
