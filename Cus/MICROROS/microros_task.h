#ifndef MICROROS_TASK_H
#define MICROROS_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "microros_task.h"
#include "gpio.h"
#include "usart.h"
#include "tim.h"
#include "motor.h"

#include <rcl/error_handling.h>
#include <rcl/rcl.h>
#include <rclc/executor.h>
#include <rclc/rclc.h>
#include <rmw_microros/rmw_microros.h>
#include <rmw_microxrcedds_c/config.h>
#include <uxr/client/transport.h>
#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/float32.h>
#include <std_msgs/msg/string.h>
#include <sensor_msgs/msg/nav_sat_fix.h>
#include <geometry_msgs/msg/twist.h>
#include <std_srvs/srv/trigger.h>
#include <std_msgs/msg/float32_multi_array.h>


#include <math.h>

// MicroROS Task initialization and start function
void MicroROS_Init(void);
void MicroROS_TaskStart(void *argument);

// Transport functions declarations
bool cubemx_transport_open(struct uxrCustomTransport *transport);
bool cubemx_transport_close(struct uxrCustomTransport *transport);
size_t cubemx_transport_write(struct uxrCustomTransport *transport, const uint8_t *buf, size_t len, uint8_t *err);
size_t cubemx_transport_read(struct uxrCustomTransport *transport, uint8_t *buf, size_t len, int timeout, uint8_t *err);

// Memory management functions
void *microros_allocate(size_t size, void *state);
void microros_deallocate(void *pointer, void *state);
void *microros_reallocate(void *pointer, size_t size, void *state);
void *microros_zero_allocate(size_t number_of_elements, size_t size_of_element, void *state);

/* Vehicle type definitions */
typedef enum {
    VEHICLE_TYPE_ACKERMANN = 1,    // 阿克曼小车 (2WD1S)
    VEHICLE_TYPE_DIFFERENTIAL = 2,  // 两轮差速小车-履带车 (2WD)
    VEHICLE_TYPE_MECANUM = 3,      // 四轮全向车 (4WD)
    VEHICLE_TYPE_BOAT = 4          // 差速船 (DEV)
} VehicleType;

/* Vehicle parameters structure */
typedef struct {
    VehicleType type;
    float wheelRadius;
    float vehicleWidth;
    float vehicleLength;
} VehicleParams;

/* PID parameters structure */
typedef struct {
    float p;
    float i;
    float d;
} PIDParams;


// int should_publish_ros;
volatile uint8_t should_publish_ros ;

#ifdef __cplusplus
}
#endif

#endif // MICROROS_TASK_H