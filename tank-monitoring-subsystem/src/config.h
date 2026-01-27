// Sotto sta il mio
#ifndef __CONFIG__
#define __CONFIG__

#define SCHEDULER_PERIOD     100 // interrupt fa ripartire lo scheduler ogni n millisecondi
#define STATE_UPDATE_PERIOD  500 // periodo di invio stato da CommunicationCenter in ms


#define L1_PIN               9 // green 
#define L2_PIN               2 // red

#define SONAR_TRIG_PIN      8
#define SONAR_ECHO_PIN      7

#define COMMUNICATION_PERIOD 500       // tempo tick communication center
#define SONAR_TASK           1000      // tempo misurazione livello acqua


// TODO livelli acqua
#define MAXTEMP      40                 // temperatura oltre la quale scatta prealarm e poi alarm
#define MAXTEMPTIME  10000              // ms di prealarm dopo i quali scatta allarm

#endif
