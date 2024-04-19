// PID.c
#include "pid.h"

void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd, float setpoint) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->setpoint = setpoint;
    pid->integral = 0.0f;
    pid->last_error = 0.0f;
    pid->output = 0.0f;
}

float PID_Update(PID_Controller *pid, float measurement) {
    float error = pid->setpoint - measurement;
    pid->integral += error;  // Update integral
    float derivative = error - pid->last_error;  // Calculate derivative
    pid->output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;  // Calculate output
    pid->last_error = error;  // Update last error
    return pid->output;
}

void PID_SetSetpoint(PID_Controller *pid, float setpoint) {
    pid->setpoint = setpoint;  // Update setpoint
}
