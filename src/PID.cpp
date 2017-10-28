//-----------------------------------------------------------------
// File: PID.cpp
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
//
// Description:
// This is the  Proportional Integral Derivative(PID) Controller 
// that controls the direction of the robot.
// The purpose of the PID controller is to minimize the error 
// in controlling the direction of the robots travel.
// The PID calculates the deviation from the intended direction
// based upon devaitiation from the desired direction and adjusts
// the output so that deviation from the desired direction is
// minimized thus providing greater accuracy.
//-----------------------------------------------------------------


#include <time.h>
#include <math.h>
#include <inttypes.h> 
#include "PID.h"


long GetTimeMs (void);

//-----------------------------------------------------------------
// GetTimeMs returns the current time in miliseconds
//-----------------------------------------------------------------
long GetTimeMs(void)
{
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
    return((intmax_t)s*1000+ms);
}
//-----------------------------------------------------------------
// END GetTimeMs
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// Init_PID Initializes the Proportional Integral Derivative(PID)
// Controller with desired constants
//-----------------------------------------------------------------
 void InitPID(TPID &PIDi,double Kpi, double Kii, double Kdi,double BaseSpeedi, double SpeedFudgeFactori) 
 {
  PIDi.ErrorTime =GetTimeMs();
  PIDi.Kp = Kpi;
  PIDi.Ki = Kii;
  PIDi.Kd = Kdi;

  PIDi.Error = 0;
  PIDi.BaseSpeed = BaseSpeedi;
  PIDi.SpeedFudgeFactor=SpeedFudgeFactori;
  PIDi.LastError = 0.0;
  PIDi.Integral = 0.0;
  PIDi.Derivative = 0.0;
 }
//-----------------------------------------------------------------
// END Init_PID 
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// SetError Set the PIDS current calculated errror from desired
// direction 
//-----------------------------------------------------------------
void SetError(TPID &PIDi,double ErrorIn)
{
 PIDi.Error = (PIDi.BaseSpeed / PIDi.SpeedFudgeFactor) * ErrorIn;
 PIDi.LastErrorTime =  PIDi.ErrorTime;
 PIDi.ErrorTime =GetTimeMs();
}
//-----------------------------------------------------------------
// END SetError 
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// RunPID calculates the corrected direction, returens the
// corrrected direction
//-----------------------------------------------------------------
double RunPID(TPID &PIDi) 
{
  long iterationTime = (long) (PIDi.ErrorTime - PIDi.LastErrorTime);

  PIDi.Integral = (PIDi.Error / iterationTime) + PIDi.Integral;

  PIDi.Derivative = (PIDi.Error - PIDi.LastError) / iterationTime;

  double correction = PIDi.Kp * PIDi.Error + PIDi.Ki * PIDi.Integral + PIDi.Kd * PIDi.Derivative;

  PIDi.LastError = PIDi.Error;

  return(correction);
}
//-----------------------------------------------------------------
// End RunPID 
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------

