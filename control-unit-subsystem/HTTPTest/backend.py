from flask import Flask, request, jsonify

app = Flask(__name__)

latest_systemdata = None
latest_command = {"isManual": True, "valveValue": 0}

@app.post("/api/systemdata")
def post_systemdata():
    global latest_systemdata
    latest_systemdata = request.get_json(force=True)
    return jsonify({"ok": True})

@app.get("/api/systemdata")
def get_systemdata():
    # la dashboard fa response.json()[0], quindi restituiamo una LISTA con 1 elemento
    if latest_systemdata is None:
        return jsonify([])
    return jsonify([latest_systemdata])

@app.post("/api/postdata")
def post_command():
    global latest_command
    data = request.get_json(force=True)
    latest_command.update(data)
    return jsonify({"ok": True, "command": latest_command})

@app.get("/api/postdata")
def get_command():
    return jsonify(latest_command)

if __name__ == "__main__":
    app.run(host="127.0.0.1", port=8050, debug=True)
