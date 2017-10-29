//------------------------------------------------------------------------------------------------
// File: PID.h
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
// Proportional Integral Derivative(PID) Controller inlcude file
//------------------------------------------------------------------------------------------------
#ifndef PIDH
#define PIDH
typedef struct
{
 double Kp ;             // proportional gain
 double Ki ;             // integral gain
 double Kd ;             // derivative gain

 double Error;           // error in the setpoint
 long   ErrorTime ;      // time of the error in the setpoint
 double BaseSpeed ;      // base speed of the robot used to compute magnigfy the error based on the speed of the robot
 double LastError;       // previous error in the setpoint,
 long   LastErrorTime;   // previous time of the previous error in the setpoint

 double Integral;        // computed integral;
 double Derivative;      // computed derivative
 double SpeedFudgeFactor;// fudge factor correction used in conjuction with the robot base speed
} TPID;

long   GetTimeMs (void);
void   InitPID(TPID &PIDi,double Kpi, double Kii, double Kdi,double BaseSpeedi,double SpeedFudgeFactori);
void   SetError(TPID &PIDi,double ErrorIn);
double RunPID(TPID &PIDi);
#endif
//------------------------------------------------------------------------------------------------
//END of Include
//------------------------------------------------------------------------------------------------

