// DeltaKinematics.c

#include "dk.h"

void DeltaKinematics_Init(DeltaKinematics *dk, double _ArmLength, double _RodLength, double _BassTri, double _PlatformTri) {
    if (dk != NULL) {

        // dk->ArmLength = _ArmLength; //232
        // dk->RodLength = _RodLength;  //336
        // dk->BassTri = _BassTri;  //119
        // dk->PlatformTri = _PlatformTri; //120

        // dk->ArmLength = 232;  //77.7
        // dk->RodLength = 336;  //120
        // dk->BassTri = 119;    // 65.6
        // dk->PlatformTri = 120;  //65

        dk->ArmLength = 77.7;  //77.7
        dk->RodLength = 120;  //120
        dk->BassTri = 65.6;    // 65.6
        dk->PlatformTri = 65;  //65

        dk->a = 0;
        dk->b = 0;
        dk->c = 0;

        // // 计算 x, y, z 的范围
        // dk->xMin = -dk->ArmLength * sin(dk->a) * cos(dk->b);
        // dk->xMax = dk->ArmLength * sin(dk->a) * cos(dk->b);
        // dk->yMin = -dk->ArmLength * sin(dk->a) * sin(dk->b);
        // dk->yMax = dk->ArmLength * sin(dk->a) * sin(dk->b);
        // dk->zMin = 0;
        // dk->zMax = dk->ArmLength;

        // // 计算 thetaA, thetaB, thetaC 的范围
        // dk->thetaAMin = asin(dk->zMin / dk->ArmLength);
        // dk->thetaAMax = asin(dk->zMax / dk->ArmLength);
        // dk->thetaBMin = -90.0;
        // dk->thetaBMax = 90.0;
        // dk->thetaCMin = 0.0;
        // dk->thetaCMax = 180.0;

        // 打印初始化结果

        // printf("ArmLength: %f\n", dk->ArmLength);
        // printf("RodLength: %f\n", dk->RodLength);
        // printf("BassTri: %f\n", dk->BassTri);
        // printf("PlatformTri: %f\n", dk->PlatformTri);
        // printf("x: %f (%.2f, %.2f)\n", dk->x, dk->xMin, dk->xMax);
        // printf("y: %f (%.2f, %.2f)\n", dk->y, dk->yMin, dk->yMax);
        // printf("z: %f (%.2f, %.2f)\n", dk->z, dk->zMin, dk->zMax);
        // printf("thetaA: %f (%.2f, %.2f)\n", dk->a, dk->thetaAMin, dk->thetaAMax);
        // printf("thetaB: %f (%.2f, %.2f)\n", dk->b, dk->thetaBMin, dk->thetaBMax);
        // printf("thetaC: %f (%.2f, %.2f)\n", dk->c, dk->thetaCMin, dk->thetaCMax);

    }
}


// Constants
const double sqrt3 = 1.7320508075688772935274463415059;
const double pi = 3.1415926535897932384626433832795; // PI
const double sin120 = sqrt3 / 2.0;
const double cos120 = -0.5;
const double tan60 = sqrt3;
const double sin30 = 0.5;
const double tan30 = 1.0 / sqrt3;

const int non_existing_point_error = -2;
const int no_error = 1;

int DeltaKinematics_forward(DeltaKinematics *dk, double thetaA, double thetaB, double thetaC) {
    // Check for a valid pointer
    if (dk == NULL) {
        return non_existing_point_error; // Or other error code for invalid pointer
    }

    // Implement the logic as the C++ forward method
    double t = (dk->BassTri - dk->PlatformTri) * tan30 / 2.0;
    double dtr = pi / 180.0;

    thetaA *= dtr;
    thetaB *= dtr;
    thetaC *= dtr;

    double y1 = -(t + dk->ArmLength * cos(thetaA));
    double z1 = -dk->ArmLength * sin(thetaA);

    double y2 = (t + dk->ArmLength * cos(thetaB)) * sin30;
    double x2 = y2 * tan60;
    double z2 = -dk->ArmLength * sin(thetaB);

    double y3 = (t + dk->ArmLength * cos(thetaC)) * sin30;
    double x3 = -y3 * tan60;
    double z3 = -dk->ArmLength * sin(thetaC);

    double dnm = (y2 - y1) * x3 - (y3 - y1) * x2;

    double w1 = y1 * y1 + z1 * z1;
    double w2 = x2 * x2 + y2 * y2 + z2 * z2;
    double w3 = x3 * x3 + y3 * y3 + z3 * z3;

    // x = (a1*z + b1)/dnm
    double a1 = (z2 - z1) * (y3 - y1) - (z3 - z1) * (y2 - y1);
    double b1 = -((w2 - w1) * (y3 - y1) - (w3 - w1) * (y2 - y1)) / 2.0;

    // y = (a2*z + b2)/dnm;
    double a2 = -(z2 - z1) * x3 + (z3 - z1) * x2;
    double b2 = ((w2 - w1) * x3 - (w3 - w1) * x2) / 2.0;

    // a*z^2 + b*z + c = 0
    double aV = a1 * a1 + a2 * a2 + dnm * dnm;
    double bV = 2.0 * (a1 * b1 + a2 * (b2 - y1 * dnm) - z1 * dnm * dnm);
    double cV = (b2 - y1 * dnm) * (b2 - y1 * dnm) + b1 * b1 + dnm * dnm * (z1 * z1 - dk->RodLength * dk->RodLength);

    // discriminant
    double dV = bV * bV - 4.0 * aV * cV;
    if (dV < 0.0) {
        return non_existing_point_error; // non-existing point. return error,x,y,z
    }

    dk->z = -0.5 * (bV + sqrt(dV)) / aV;
    dk->x = (a1 * dk->z + b1) / dnm;
    dk->y = (a2 * dk->z + b2) / dnm;

    // Return value based on success or failure
    return no_error; // or return non_existing_point_error based on calculation results
}

int DeltaKinematics_calcAngleYZ(DeltaKinematics *dk, double *Angle, double x0, double y0, double z0) {
    
    if (dk == NULL || Angle == NULL) {
        return non_existing_point_error; // Or other error code for invalid pointer
    }

    // 确保 z0 不为零，以避免除以零的错误
    if (z0 == 0) {
        return non_existing_point_error; // 返回适当的错误代码
    }

    double y1 = -0.5 * tan30 * dk->BassTri;
    y0 -= 0.5 * tan30 * dk->PlatformTri;

    double aV = (x0 * x0 + y0 * y0 + z0 * z0 + dk->ArmLength * dk->ArmLength - dk->RodLength * dk->RodLength - y1 * y1) / (2.0 * z0);
    double bV = (y1 - y0) / z0;

    double dV = -(aV + bV * y1) * (aV + bV * y1) + dk->ArmLength * (bV * bV * dk->ArmLength + dk->ArmLength);
    if (dV < 0) {
        return non_existing_point_error; // non-existing point. return error, theta
    }

    double yj = (y1 - aV * bV - sqrt(dV)) / (bV * bV + 1); // choosing outer point
    double zj = aV + bV * yj;
    *Angle = atan2(-zj, (y1 - yj)) * 180.0 / pi;

    return no_error; // return error, theta
}

int DeltaKinematics_inverse(DeltaKinematics *dk, double x0, double y0, double z0) {
    if (dk == NULL) {
        return non_existing_point_error; // Or other error code for invalid pointer
    }

    // dk->a = 0;
    // dk->b = 0;
    // dk->c = 0;

    int error = DeltaKinematics_calcAngleYZ(dk, &(dk->a), x0, y0, z0);
    if (error != no_error) return error;

    error = DeltaKinematics_calcAngleYZ(dk, &(dk->b), x0 * cos120 + y0 * sin120, y0 * cos120 - x0 * sin120, z0);
    if (error != no_error) return error;

    error = DeltaKinematics_calcAngleYZ(dk, &(dk->c), x0 * cos120 - y0 * sin120, y0 * cos120 + x0 * sin120, z0);
    
    return error; // or no_error if all calculations are successful
}