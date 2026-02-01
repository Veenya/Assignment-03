import requests, random, time

SYSTEMDATA_URL = "http://127.0.0.1:8050/api/systemdata"
COMMAND_URL    = "http://127.0.0.1:8050/api/postdata"

while True:
    # 1) leggi l'ultimo comando dal backend
    try:
        cmd = requests.get(COMMAND_URL, timeout=2).json()
    except requests.RequestException as e:
        print("command GET error:", e)
        time.sleep(1)
        continue

    is_manual = bool(cmd.get("isManual", True))
    commanded_valve = int(cmd.get("valveValue", 0))

    # 2) genera dati di sistema
    wl = random.randint(1, 100)

    if is_manual:
        valve_value = commanded_valve          # <-- segue la dashboard
        status = "MANUAL"
    else:
        valve_value = random.randint(1, 100)   # <-- oppure metti qui la tua logica AUTO
        status = "AUTO"

    payload = {
        "wl": wl,
        "status": status,
        "valveValue": valve_value
    }

    # 3) invia i systemdata al backend
    try:
        r = requests.post(SYSTEMDATA_URL, json=payload, timeout=2)
        print("sent", payload, "->", r.status_code)
    except requests.RequestException as e:
        print("systemdata POST error:", e)

    time.sleep(1)
