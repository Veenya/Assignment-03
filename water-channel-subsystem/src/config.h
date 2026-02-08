// Sotto sta il mio
#ifndef __CONFIG__
#define __CONFIG__

#define SCHEDULER_PERIOD     50     // interrupt fa ripartire lo scheduler ogni n millisecondi
#define STATE_UPDATE_PERIOD  500    // periodo minimo di invio stato da CommunicationCenter in ms
#define COMMUNICATION_PERIOD 1000   // tempo tick communication center
#define CONTROLLER_PERIOD    1000   // assumo che la valvola ci metta 1 sec 
#define POTENTIOMETER_PERIOD 500    // periodo campionamento potenziometro
#define BUTTON_PERIOD        50     // periodo campionamento tasto cambio modalita'


#define POTENTIOMETER_PIN    A0 
#define BUTTON_PIN           5
#define LCD_ADDR             0x27
#define SERVO_PIN            3



#define MIN_ANALOG_READ      0
#define MAX_ANALOG_READ      1023
#define MIN_POTENTIOMETER    0
#define MAX_POTENTIOMETER    100
#define POTENTIOMETER_TOLERANCE 20


#endif
