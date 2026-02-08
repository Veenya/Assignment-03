from flask import Flask
import dash
from dash import html, dcc, no_update
from dash import callback_context
from dash.dependencies import Input, Output, State
import plotly.graph_objs as go
from collections import deque
import datetime
import requests

# -------- CONFIG --------
CUS_BASE = "http://127.0.0.1:8050"   # <-- metti la porta corretta del tuo Java
POLL_MS = 2000                       # <-- aumenta per evitare overload
REQ_TIMEOUT = 0.8                    # <-- più basso del POLL_MS
MAX_POINTS = 60

server = Flask(__name__)
app = dash.Dash(__name__, server=server, title="Dashboard")

# storico WL/valve tenuto in memoria python
data_queue = deque(maxlen=MAX_POINTS)

app.layout = html.Div([
    dcc.Interval(id="interval-component", interval=POLL_MS, n_intervals=0),

    html.H1("River Monitoring Dashboard", style={"textAlign": "center", "fontSize": "48px"}),

    # mostra eventuali errori di polling (così non sono "invisibili")
    html.Div(id="conn-status", style={"textAlign": "center", "color": "red", "marginBottom": "10px"}),

    html.Div([
        html.Div([dcc.Graph(id="water-level-graph")], style={"width": "50%", "display": "inline-block"}),
        html.Div([dcc.Graph(id="valve-level-graph")], style={"width": "50%", "display": "inline-block"}),
    ]),

    html.Div(id="system-status-display", style={"textAlign": "center", "fontSize": "24px"}),
    html.Div(id="arduino-status-display", style={"textAlign": "center", "fontSize": "24px"}),
    html.Div(id="esp-status-display", style={"textAlign": "center", "fontSize": "24px"}),

    html.Div([
        html.H3("Set Mode"),

        html.Button("MANUAL_LOCAL", id="btn-manual-local", n_clicks=0, style={"marginRight": "8px"}),
        html.Button("MANUAL_REMOTE", id="btn-manual-remote", n_clicks=0, style={"marginRight": "8px"}),
        html.Button("AUTO", id="btn-auto", n_clicks=0, style={"marginRight": "16px"}),

        html.H3("Set Valve Value", style={"marginLeft": "16px"}),
        html.Div([
            dcc.Slider(
                id="valveValue",
                min=0, max=100, value=0,
                marks={i: f"{i}%" for i in range(0, 101, 10)},
                disabled=False,
            )
        ], style={"width": "33%", "display": "inline-block"}),

        html.Button("Set Valve Value", id="send-valveValue", n_clicks=0, style={"marginLeft": "12px"}, disabled=False),
    ], style={"display": "flex", "justifyContent": "center", "alignItems": "center"}),


    # store single “latest snapshot” from backend
    dcc.Store(id="latest-store", data=None),
    dcc.Store(id="mode-store", data={"mode": "MANUAL_LOCAL"}),

])

# ---------- 1) SINGLE POLL: only this callback does HTTP GET ----------
@app.callback(
    Output("latest-store", "data"),
    Output("conn-status", "children"),
    Input("interval-component", "n_intervals")
)
def poll_backend(n):
    try:
        r = requests.get(f"{CUS_BASE}/api/systemdata", timeout=REQ_TIMEOUT)
        r.raise_for_status()
        data = r.json()  # must be an object: {status,isManual,valveValue,wl}
        if not data:
            return None, "No data yet"
        return data, ""
    except Exception as e:
        return None, f"GET error: {e}"

# ---------- 2) UPDATE HISTORY (python deque) when new data arrives ----------
@app.callback(
    Output("dummy-history", "children"),
    Input("latest-store", "data"),
    prevent_initial_call=True
)
def push_history(latest):
    # dummy output created dynamically below to avoid clutter
    if not latest:
        return ""

    try:
        data_queue.append({
            "wl": float(latest.get("wl", 0)),
            "valveValue": int(latest.get("valveValue", 0)),
            "status": str(latest.get("status", "?")),
            "timestamp": datetime.datetime.now().strftime("%H:%M:%S"),
        })
    except Exception:
        # ignore parse errors
        pass
    return ""

# create hidden dummy div for history callback
app.layout.children.append(html.Div(id="dummy-history", style={"display": "none"}))

# ---------- 3) MODE button: POST /api/mode ----------

@app.callback(
    Output("mode-store", "data"),
    Output("conn-status", "children", allow_duplicate=True),
    Input("btn-manual-local", "n_clicks"),
    Input("btn-manual-remote", "n_clicks"),
    Input("btn-auto", "n_clicks"),
    State("mode-store", "data"),
    prevent_initial_call=True
)
def set_mode(n_local, n_remote, n_auto, store):
    ctx = callback_context
    if not ctx.triggered:
        return store, no_update

    triggered_id = ctx.triggered[0]["prop_id"].split(".")[0]

    if triggered_id == "btn-manual-local":
        new_mode = "MANUAL_LOCAL"
    elif triggered_id == "btn-manual-remote":
        new_mode = "MANUAL_REMOTE"
    elif triggered_id == "btn-auto":
        new_mode = "AUTO"

    try:
        r = requests.post(
            f"{CUS_BASE}/api/mode",
            json={"mode": new_mode},
            timeout=REQ_TIMEOUT
        )
        if r.status_code >= 400:
            return store, f"POST /api/mode failed: {r.status_code} {r.text}"
    except Exception as e:
        return store, f"POST /api/mode error: {e}"

    return {"mode": new_mode}, ""



# ---------- 4) VALVE button: POST /api/valve ----------
@app.callback(
    Output("conn-status", "children", allow_duplicate=True),
    Input("send-valveValue", "n_clicks"),
    State("valveValue", "value"),
    State("mode-store", "data"),
    prevent_initial_call=True
)
def send_valve_value(n_clicks, valve_value, mode_store):
    mode = (mode_store or {}).get("mode", "MANUAL_LOCAL")
    if not mode.startswith("MANUAL"):
        return "Valve can be set only in MANUAL mode"

    try:
        r = requests.post(
            f"{CUS_BASE}/api/valve",
            json={"valveValue": int(valve_value)},
            timeout=REQ_TIMEOUT
        )
        if r.status_code >= 400:
            return f"POST /api/valve failed: {r.status_code} {r.text}"
        return ""
    except Exception as e:
        return f"POST /api/valve error: {e}"

# ---------- 5) ENABLE/DISABLE controls based on mode-store + backend latest ----------
@app.callback(
    Output("valveValue", "disabled"),
    Output("send-valveValue", "disabled"),
    Output("valveValue", "value"),
    Input("mode-store", "data"),
    Input("latest-store", "data"),
)
def update_manual_ui(mode_store, latest):
    mode = (mode_store or {}).get("mode", "MANUAL_LOCAL")
    is_manual = mode.startswith("MANUAL")

    if is_manual:
        return False, False, no_update

    # AUTO: disable and sync slider with backend (if available)
    if latest:
        return True, True, int(latest.get("valveValue", 0))
    return True, True, no_update

# ---------- 6) Graphs + status read from deque ----------
@app.callback(Output("water-level-graph", "figure"),
              Input("latest-store", "data"))
def update_water(_latest):
    return go.Figure(
        data=[go.Scatter(
            x=[d["timestamp"] for d in data_queue],
            y=[d["wl"] for d in data_queue],
            mode="lines+markers"
        )],
        layout=go.Layout(title="Livello acqua", xaxis={"title": "Ora"}, yaxis={"title": "wl"})
    )

@app.callback(Output("valve-level-graph", "figure"),
              Input("latest-store", "data"))
def update_valve(_latest):
    return go.Figure(
        data=[go.Scatter(
            x=[d["timestamp"] for d in data_queue],
            y=[d["valveValue"] for d in data_queue],
            mode="lines+markers"
        )],
        layout=go.Layout(title="Valvola", xaxis={"title": "Ora"}, yaxis={"title": "valveValue"})
    )

@app.callback(Output("system-status-display", "children"),
              Input("latest-store", "data"))
def update_system_status(latest):
    if latest:
        return f'Stato del sistema: {latest.get("systemStatus", "?")}'
    return "In attesa di dati..."

@app.callback(Output("arduino-status-display", "children"),
              Input("latest-store", "data"))
def update_arduino_status(latest):
    if latest:
        return f'Connessione Arduino: {latest.get("arduinoConnectionStatus", "?")}'
    return "In attesa di dati..."

@app.callback(Output("esp-status-display", "children"),
              Input("latest-store", "data"))
def update_esp_status(latest):
    if latest:
        return f'Connessione ESP: {latest.get("espConnectionStatus", "?")}'
    return "In attesa di dati..."

if __name__ == "__main__":
    # debug=True can create 2 processes; keep it True for dev, but if issues persist set False
    app.run(debug=True, port=8057)
