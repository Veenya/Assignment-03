Driver
https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads

Avviare / verificare Mosquitto
Apri Servizi Windows:
Premi Win + R → digita services.msc → Invio
Cerca Mosquitto Broker nella lista
Deve essere Running (in esecuzione). Se non lo è → clic destro → Avvia
Oppure avvia manualmente da Prompt dei comandi (come admin):
cd "C:\Program Files\mosquitto"
mosquitto.exe -v
(il -v mostra log dettagliati; premi Ctrl+C per fermarlo quando hai finito il test)
Per default Mosquitto ascolta su localhost:1883 (porta MQTT standard, senza autenticazione).