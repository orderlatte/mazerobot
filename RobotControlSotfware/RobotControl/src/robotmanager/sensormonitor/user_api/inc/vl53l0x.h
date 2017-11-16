#ifndef _VL53l0X
#define _VL53l0X

#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.

#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"



#ifdef __cplusplus
extern "C" {
#endif
#define VERSION_REQUIRED_MAJOR 1
#define VERSION_REQUIRED_MINOR 0
#define VERSION_REQUIRED_BUILD 2

#define VL53L0X_DEFAULT_ADDRESS 0x29

#define VL53L0X_GOOD_ACCURACY_MODE      0   // Good Accuracy mode
#define VL53L0X_BETTER_ACCURACY_MODE    1   // Better Accuracy mode
#define VL53L0X_BEST_ACCURACY_MODE      2   // Best Accuracy mode
#define VL53L0X_LONG_RANGE_MODE         3   // Longe Range mode
#define VL53L0X_HIGH_SPEED_MODE         4   // High Speed mode

#define MAX_DEVICES                     16

void print_pal_error(VL53L0X_Error Status);

VL53L0X_Error WaitMeasurementDataReady(VL53L0X_DEV Dev);

VL53L0X_Error WaitStopCompleted(VL53L0X_DEV Dev);

void startRanging(int object_number, int mode, uint8_t i2c_address,
                  uint8_t TCA9548A_Device, uint8_t TCA9548A_Address);

int32_t getDistance(int object_number);
void stopRanging(int object_number);

VL53L0X_DEV getDev(int object_number);
#ifdef __cplusplus
}

#endif	// UBUNTU

#endif
#endif
