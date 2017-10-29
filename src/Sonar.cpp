#include <wiringPi.h>
#include "Sonar.h"

static int  trigger=-1;
static int  echo=-1;

void SonarInit(int _trigger, int _echo)
{
    trigger=_trigger;
    echo=_echo;
    pinMode(trigger, OUTPUT);
    pinMode(echo, INPUT);
    digitalWrite(trigger, LOW);
    delay(500);
}

double SonarDistance(int timeout)
{
  long startTimeUsec,endTimeUsec,travelTimeUsec,timeoutstart;
  double distanceCm;

    if ((trigger==-1) || (echo==-1)) return(-1.0);
    
    delay(10);
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);

    timeoutstart=micros();

    while (digitalRead(echo) == LOW)
       {
         if ( micros()-timeoutstart>=timeout) return (-1.0);
       }
    startTimeUsec = micros();
    while ( digitalRead(echo) == HIGH )
       {
        endTimeUsec = micros();
        if (endTimeUsec-timeoutstart>=timeout) return (-1.0);
       }

    travelTimeUsec = endTimeUsec - startTimeUsec;
    distanceCm = travelTimeUsec/58.0;

    return distanceCm;
}

