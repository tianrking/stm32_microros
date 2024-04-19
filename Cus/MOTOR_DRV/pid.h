// PID.h
#ifndef PID_H
#define PID_H

typedef struct {
    float Kp;  // Proportional gain
    float Ki;  // Integral gain
    float Kd;  // Derivative gain
    float setpoint;  // Desired target value
    float integral;  // Integral term accumulator
    float last_error;  // Last error for derivative calculation
    float output;  // Last output value
} PID_Controller;

void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd, float setpoint);
float PID_Update(PID_Controller *pid, float measurement);
void PID_SetSetpoint(PID_Controller *pid, float setpoint);  // Interface to update setpoint
#endif // PID_H
