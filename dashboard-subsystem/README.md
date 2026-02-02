How to test this
open 3 DIFFERENT terminals and start them in this order:
python backend.py
python simulator.py
python dashboard.py


poi apri: http://localhost:8057


---

# Dashboard subsystem 
Frontend/webapp running on the PC or any device

- Front-end for remote operators to visualise data and interact with the system
- It interacts via HTTP with the Control Unit Subsystem

---

The DBS subsystem is a dashboard for visualising the state of the Tank Monitoring system, including:
- A **graph** of hte rainwater level, considering the last N measurements
- A current value of teh valve opening percentage
- The state of the system: MANUAL, AUTOMATIC, UNCONNECTED or NOT AVAILABLE.
    - NOT AVAILABLE only if CUS is not reachable.
- a GUI button to switch MANUAL and AUTOMATIC
- a GUI widget to control the opening level of the valve in the WCS when in MANUAL