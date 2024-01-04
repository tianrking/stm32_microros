#ifndef __MC_INCLUDE_H__
#define __MC_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "include.h"

// service
void wheel_speeds_callback(const void *msgin);

// publisher
void publish_current_state(int32_t current_state);
// void publish_current_state(void);
void setup_publisher(void);

// subscriber
void wheel_speeds_callback(const void *msgin);
void status_set_callback(const void * msgin);
void setup_subscriber(void);

#endif