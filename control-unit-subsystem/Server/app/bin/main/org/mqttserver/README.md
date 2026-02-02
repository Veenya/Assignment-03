MQTT server how to:
What do we want to do? See data in the dashboard
How?

lancia 
il main del progetto java in control unit subsystem

cd .\control-unit-subsystem\MQTTtest\  
python .\subscribe.py  
python .\publish.py   

 cd .\dashboard-subsystem\ 
  python .\dashboard.py  

publish.py (demtro MQTTtest)
subscribe.py (demtro MQTTtest)
dashboard.py