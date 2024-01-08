# DeltaKinematics.cpp

这是一个用于计算 Delta 机器人运动学的 C++ 库。

## 初始化

```c++
// Initialize the DeltaKinematics structure
DeltaKinematics dk;
DeltaKinematics_Init(&dk, 100, 200, 300, 400); // Replace with your actual values
``````

此代码将初始化 `DeltaKinematics` 结构。该结构包含用于计算运动学的所有必要数据。

`DeltaKinematics_Init()` 函数接受四个参数：

* `dk`：`DeltaKinematics` 结构的指针。
* `l1`：支架 1 的长度。
* `l2`：支架 2 的长度。
* `l3`：支架 3 的长度。

您可以根据您的实际 Delta 机器人设置替换这些值。

## 正运动学

```c++
// For Forward Kinematics
// Set your theta values for joints
double thetaA = 45, thetaB = 45, thetaC = 45;
if (DeltaKinematics_forward(&dk, thetaA, thetaB, thetaC) == no_error) {
    printf("Forward Kinematics: (x, y, z) = (%f, %f, %f)\n", dk.x, dk.y, dk.z);
} else {
    printf("Forward Kinematics computation failed.\n");
}
```

此代码将计算 Delta 机器人的正运动学。

`DeltaKinematics_forward()` 函数接受三个参数：

* `dk`：`DeltaKinematics` 结构的指针。
* `thetaA`：电机 A 的角度。
* `thetaB`：电机 B 的角度。
* `thetaC`：电机 C 的角度。

如果计算成功，函数将返回 `no_error`。否则，它将返回错误代码。

如果计算成功，函数将更新 `dk` 结构中的 `x`、`y` 和 `z` 成员变量。这些变量分别表示末端执行器的 x、y 和 z 坐标。

## 逆运动学

```c++
// For Inverse Kinematics
// Set your desired end effector position
double x0 = 10, y0 = 10, z0 = -10;
if (DeltaKinematics_inverse(&dk, x0, y0, z0) == no_error) {
    printf("Inverse Kinematics: (thetaA, thetaB, thetaC) = (%f, %f, %f)\n", dk.a, dk.b, dk.c);
} else {
    printf("Inverse Kinematics computation failed.\n");
}
```

此代码将计算 Delta 机器人的逆运动学。

`DeltaKinematics_inverse()` 函数接受三个参数：

* `dk`：`DeltaKinematics` 结构的指针。
* `x0`：末端执行器的 x 坐标。
* `y0`：末端执行器的 y 坐标。
* `z0`：末端执行器的 z 坐标。

如果计算成功，函数将返回 `no_error`。否则，它将返回错误代码。

如果计算成功，函数将更新 `dk` 结构中的 `a`、`b` 和 `c` 成员变量。这些变量分别表示电机 A、B 和 C 的角度。

## 注释

以下是代码中的注释：

```c++
// Initialize the DeltaKinematics structure
// 初始化 `DeltaKinematics` 结构
DeltaKinematics dk;
DeltaKinematics_Init(&dk, 100, 200, 300, 400); // Replace with your actual values
```