#ifndef _DK_H
#define _DK_H

#include <stdint.h>
#include "math.h"

// Define a structure to hold all the data members of the class
typedef struct {
    double ArmLength; // 杆长
    double RodLength; // 臂长
    double BassTri; // 底座三角形边长
    double PlatformTri; // 平台三角形边长
    double x, y, z; // 末端执行器位置
    double a, b, c; // 电机或关节角度
    double xMin, xMax; // x 范围
    double yMin, yMax; // y 范围
    double zMin, zMax; // z 范围
    double thetaAMin, thetaAMax; // thetaA 范围
    double thetaBMin, thetaBMax; // thetaB 范围
    double thetaCMin, thetaCMax; // thetaC 范围
} DeltaKinematics;

// Declare a function to initialize the structure (akin to a constructor)
void DeltaKinematics_Init(DeltaKinematics *dk, double _ArmLength, double _RodLength, double _BassTri, double _PlatformTri);
int DeltaKinematics_forward(DeltaKinematics *dk, double thetaA, double thetaB, double thetaC);
int DeltaKinematics_calcAngleYZ(DeltaKinematics *dk, double *Angle, double x0, double y0, double z0);
int DeltaKinematics_inverse(DeltaKinematics *dk, double x0, double y0, double z0);

#endif