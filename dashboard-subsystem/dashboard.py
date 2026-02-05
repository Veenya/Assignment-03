from flask import Flask
import dash
from dash import html, dcc, no_update
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

    # store single “latest snapshot” from backend
    dcc.Store(id="latest-store", data=None),
    dcc.Store(id="mode-store", data={"isManual": True}),
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
        data = r.json()

        # Backend returns either {"...":...} or [{"...":...}, ...]
        if isinstance(data, dict):
            return data, ""

        if isinstance(data, list):
            if not data:
                return None, "Empty list from backend"
            item = data[-1]  # latest
            if not isinstance(item, dict):
                return None, f"List item is not an object: {type(item).__name__}"
            return item, ""

        return None, f"Unexpected JSON type: {type(data).__name__}"

    except Exception as e:
        return None, f"GET error: {e}"

# ---------- 2) UPDATE HISTORY (python deque) when new data arrives ----------
@app.callback(
    Output("dummy-history", "children"),
    Input("latest-store", "data"),
    prevent_initial_call=True
)
def push_history(latest):
    if not isinstance(latest, dict):
        return ""

    try:
        data_queue.append({
            "wl": float(latest.get("wl", 0)),
            "valveValue": int(latest.get("valveValue", 0)),
            "status": str(latest.get("status", "?")),
            "timestamp": datetime.datetime.now().strftime("%H:%M:%S"),
        })
    except Exception:
        pass

    return ""

# create hidden dummy div for history callback
app.layout.children.append(html.Div(id="dummy-history", style={"display": "none"}))

# ---------- 3) MODE button: POST /api/mode ----------
@app.callback(
    Output("mode-store", "data"),
    Output("send-autoMode", "children"),
    Output("conn-status", "children", allow_duplicate=True),
    Input("send-autoMode", "n_clicks"),
    State("mode-store", "data"),
    prevent_initial_call=True
)
def toggle_mode(n_clicks, store):
    is_manual = bool(store.get("isManual", True))
    new_is_manual = not is_manual

    try:
        r = requests.post(
            f"{CUS_BASE}/api/mode",
            json={"isManual": new_is_manual},
            timeout=REQ_TIMEOUT
        )
        # 200 ok expected
        if r.status_code >= 400:
            return store, no_update, f"POST /api/mode failed: {r.status_code} {r.text}"
    except Exception as e:
        return store, no_update, f"POST /api/mode error: {e}"

    button_text = "Switch to AUTO" if new_is_manual else "Switch to MANUAL"
    return {"isManual": new_is_manual}, button_text, ""

# ---------- 4) VALVE button: POST /api/valve ----------
@app.callback(
    Output("conn-status", "children", allow_duplicate=True),
    Input("send-valveValue", "n_clicks"),
    State("valveValue", "value"),
    State("mode-store", "data"),
    prevent_initial_call=True
)
def send_valve_value(n_clicks, valve_value, mode_store):
    # avoid calling backend in AUTO
    if not bool(mode_store.get("isManual", True)):
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
    is_manual = bool(mode_store.get("isManual", True))

    if is_manual:
        return False, False, no_update

    if isinstance(latest, dict):
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

@app.callback(
    Output("status-display", "children"),
    Input("latest-store", "data")
)
def update_status(latest):
    if isinstance(latest, dict):
        return f"Stato del sistema: {latest.get('status', '?')}"
    return "In attesa di dati..."

if __name__ == "__main__":
    # debug=True can create 2 processes; keep it True for dev, but if issues persist set False
    app.run(debug=True, port=8057, use_reloader=False)
