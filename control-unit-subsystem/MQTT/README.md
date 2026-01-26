# HOW TO




### Crea venv
Posizionati in \control-unit-subsystem\MQTT\
`python3 -m venv .venv`

---

### Avvia venv su linux
`source .venv/bin/activate`

### Avvia venv da powershell
`.\.venv\Scripts\Activate.ps1`

### Avvia venv da cmd
`.venv\Scripts\activate.bat`

---

## AVVIO

Con mosquitto avviato

```sh
# avvia venv
python .\subscribe.py
python .\publish.py
```

---




https://mosquitto.org/download/

`netstat -an | findstr 1883`
risultato 
```
  TCP    127.0.0.1:1883         0.0.0.0:0              LISTENING
  TCP    [::1]:1883             [::]:0                 LISTENING
```

