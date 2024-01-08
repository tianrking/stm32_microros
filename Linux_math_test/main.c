// gcc -o main main.c dk.c dk.h -lm
#include "dk.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define no_error 1

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <mode> <thetaA> <thetaB> <thetaC>\n", argv[0]);
        return 1;
    }

    int mode = atoi(argv[1]);
    if (mode != 0 && mode != 1) {
        printf("Invalid mode.\n");
        return 1;
    }

    double thetaA = atof(argv[2]);
    double thetaB = atof(argv[3]);
    double thetaC = atof(argv[4]);

    clock_t start, end;

    for (int i = 0; i < 1; i++) {
        // 判断模式
        if (mode == 0) {
            // 正解
            start = clock();
            DeltaKinematics dk;
            DeltaKinematics_Init(&dk, 100, 200, 300, 400); // 使用默认的杆长、臂长、底座三角形边长和平台三角形边长
            DeltaKinematics_forward(&dk, thetaA, thetaB, thetaC);
            end = clock();

            // 输出正解计算时间
            printf("Iteration %d: Forward Kinematics time: %.6f seconds\n", i, (double)(end - start) / CLOCKS_PER_SEC);

            // 逆解
            start = clock();
            DeltaKinematics_inverse(&dk, dk.x, dk.y, dk.z);
            end = clock();

            // 输出逆解计算时间
            printf("Iteration %d: Inverse Kinematics time: %.6f seconds\n", i, (double)(end - start) / CLOCKS_PER_SEC);

            // 输出正解计算结果
            printf("Iteration %d: Forward Kinematics: (x, y, z) = (%f, %f, %f)\n", i, dk.x, dk.y, dk.z);

            // 输出逆解计算结果
            printf("Iteration %d: Inverse Kinematics: (thetaA, thetaB, thetaC) = (%f, %f, %f)\n", i, dk.a, dk.b, dk.c);
        } else {
            // 正解
            start = clock();
            DeltaKinematics dk;
            DeltaKinematics_Init(&dk, 100, 200, 300, 400);  // 使用默认的杆长、臂长、底座三角形边长和平台三角形边长
            DeltaKinematics_inverse(&dk, thetaA, thetaB, thetaC);
            end = clock();

            // 输出正解计算时间
            printf("Iteration %d: Forward Kinematics time: %.6f seconds\n", i, (double)(end - start) / CLOCKS_PER_SEC);

            // 逆解
            start = clock();
            DeltaKinematics_forward(&dk, dk.a, dk.b, dk.c);
            end = clock();

            // 输出逆解计算时间
            printf("Iteration %d: Inverse Kinematics time: %.6f seconds\n", i, (double)(end - start) / CLOCKS_PER_SEC);

            // 输出正解计算结果
            printf("Iteration %d: Forward Kinematics: (x, y, z) = (%f, %f, %f)\n", i, dk.x, dk.y, dk.z);

            // 输出逆解计算结果
            printf("Iteration %d: Inverse Kinematics: (thetaA, thetaB, thetaC) = (%f, %f, %f)\n", i, dk.a, dk.b, dk.c);
        }

    }

    return 0;
}
