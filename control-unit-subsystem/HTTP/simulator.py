import requests, random, time

url = "http://127.0.0.1:8050/api/systemdata"

while True:
    payload = {
        "wl": random.randint(1, 100),
        "status": "SAMPLE_STATUS",
        "valveValue": random.randint(1, 100)
    }
    r = requests.post(url, json=payload, timeout=2)
    print("sent", payload, "->", r.status_code)
    time.sleep(1)
