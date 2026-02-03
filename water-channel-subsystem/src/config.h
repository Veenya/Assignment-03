// Sotto sta il mio
#ifndef __CONFIG__
#define __CONFIG__

#define SCHEDULER_PERIOD     100 // interrupt fa ripartire lo scheduler ogni n millisecondi
#define STATE_UPDATE_PERIOD  500 // periodo di invio stato da CommunicationCenter in ms


#define POTENTIOMETER_PIN   A0 
#define BUTTON_PIN          5
#define LCD_ADDR            0x27
#define SERVO_PIN           3

#define COMMUNICATION_PERIOD 500       // tempo tick communication center

#define MIN_ANALOG_READ     0
#define MAX_ANALOG_READ     1000
#define MIN_POTENTIOMETER   0
#define MAX_POTENTIOMETER   100
#define POTENTIOMETER_TOLERANCE 20


#endif
