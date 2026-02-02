Come testare il codice parte HTTP:

3) Test HTTP (dashboard API) con curl (PowerShell)
3.1 GET stato
curl http://localhost:8080/api/systemdata


Atteso: JSON con campi simili a:

status

isManual

valveValue

wl

3.2 GET storico WL
curl "http://localhost:8080/api/wlhistory?n=10"


Atteso: array JSON (10 numeri).

3.3 Passa a MANUAL
curl -Method POST http://localhost:8080/api/mode `
  -H "Content-Type: application/json" `
  -Body '{"isManual": true}'


Verifica:

curl http://localhost:8080/api/systemdata


Atteso: isManual: true e/o status = MANUAL.

3.4 Imposta valvola in MANUAL
curl -Method POST http://localhost:8080/api/valve `
  -H "Content-Type: application/json" `
  -Body '{"valveValue": 40}'


Verifica:

curl http://localhost:8080/api/systemdata


Atteso: valveValue = 40.

3.5 Torna AUTOMATIC
curl -Method POST http://localhost:8080/api/mode `
  -H "Content-Type: application/json" `
  -Body '{"isManual": false}'

3.6 Valvola in automatico deve essere bloccata (409)
curl -i -Method POST http://localhost:8080/api/valve `
  -H "Content-Type: application/json" `
  -Body '{"valveValue": 30}'


Atteso: HTTP 409 + messaggio “Valve can be set only in MANUAL mode”.

Se questi passi passano, HTTP layer ok.

---
