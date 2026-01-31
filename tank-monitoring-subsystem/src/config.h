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
#define SONAR_NO_OBJ_DETECTED -1


// Livelli acqua
#define L1 10
#define L2 20
// #define T1 5
#define DEBUG_LEVEL 20

#define MAXTEMP      40                 // temperatura oltre la quale scatta prealarm e poi alarm
#define MAXTEMPTIME  10000              // ms di prealarm dopo i quali scatta allarm

#define WIFI_SSID          "star"
#define WIFI_PASSWORD      "cicciopasticcio"

#define MAX_TIME_US    10 // TODO capire a cosa serviva e se serve ancora

#endif
