// #ifndef DeltaKinematics_h
// #define DeltaKinematics_h

// #ifdef __cplusplus
// extern "C" {
// #endif

// // C wrapper functions
// void* DeltaKinematics_create(double _ArmLength, double _RodLength, double _BassTri, double _PlatformTri);
// void DeltaKinematics_destroy(void* dk);
// int DeltaKinematics_forward_with_angles(void* dk, double thetaA, double thetaB, double thetaC);
// int DeltaKinematics_inverse_with_positions(void* dk, double x0, double y0, double z0);

// #ifdef __cplusplus
// }
// #endif

// #ifdef __cplusplus

// class DeltaKinematics {
// public:
//     DeltaKinematics(double _ArmLength, double _RodLength, double _BassTri, double _PlatformTri);
//     int forward();
//     int forward(double thetaA, double thetaB, double thetaC);
//     int inverse();
//     int inverse(double x0, double y0, double z0);

// private:
//     int delta_calcAngleYZ(double *Angle, double x0, double y0, double z0);

//     double ArmLength;
//     double RodLength;
//     double BassTri;
//     double PlatformTri;
//     double x, y, z;
//     double a, b, c;
// };

// #endif // __cplusplus
// #endif // DeltaKinematics_h
