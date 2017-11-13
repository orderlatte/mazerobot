#include "Sonar.h"
#include "sensor_manager.h"

#include "vl53l0x.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#ifndef UBUNTU
#include <wiringPi.h>
#endif

#define ObjectNum_0 0
#define ObjectNum_1 1
#define VL53L0X_GOOD_ACCURACY_MODE       0  // Good Accuracy mode
#define VL53L0X_BETTER_ACCURACY_MODE     1  // Better Accuracy mode
#define VL53L0X_BEST_ACCURACY_MODE       2  // Best Accuracy mode
#define VL53L0X_LONG_RANGE_MODE          3  // Longe Range mode
#define VL53L0X_HIGH_SPEED_MODE          4  // High Speed mode#
static unsigned int GetTiming(int object_number);


double sonar(void);
int32_t laser_right(void);
int32_t laser_left(void);

T_SensorData sensor_data = {0,};
T_CellData cell_data = {0, };
fp_robot_stop stopRobot;

unsigned int timing;

using namespace std;

//----------------------
// Sonar Thread
//----------------------
void *sonar_thread(void *value) {

	while (1) {
		sensor_data.sonar_distance = sonar();

		if (sensor_data.sonar_distance < 4 && sensor_data.sonar_distance > 0) {	// Below 2 cm
			printf("sonar_distance: %ld\n", sensor_data.sonar_distance);
			stopRobot(SENSOR_TYPE_SONAR);
		}
	}
}

//----------------------
// Laser Thread
//----------------------
void *laser_thread(void *value) {

	while (1) {
		sensor_data.laser_left_distance = laser_left();
		sensor_data.laser_right_distance = laser_right();

		if (sensor_data.laser_left_distance < 40 && sensor_data.laser_left_distance > 0) {	// Below 40mm (4cm)
			printf("laser_left_distance: %d\n", sensor_data.laser_left_distance);
			stopRobot(SENSOR_TYPE_LASER_LEFT);
		}

		if (sensor_data.laser_right_distance < 40  && sensor_data.laser_right_distance > 0) {	// Below 40mm (4cm)
			printf("laser_right_distance: %d\n", sensor_data.laser_right_distance);
			stopRobot(SENSOR_TYPE_LASER_RIGHT);
		}
	}
}


void creat_thread(void)
{
	pthread_t thread1;
	pthread_t thread2;
	int x = 0;
	// Sonar thread
	pthread_create(&thread1, NULL, &sonar_thread, &x);
	// Lasor thread
	 pthread_create(&thread2, NULL, &laser_thread, &x);
}

void laser_init(void)
{
	#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.
	
	if(VL53L0X_i2c_init("/dev/i2c-1")==-1)
	{
		printf("VL53L0X_i2c_init failed\n");
		exit(0);
	}
	  
	// Start ranging on TCA9548A bus 1 
	startRanging(ObjectNum_0, VL53L0X_BETTER_ACCURACY_MODE, 0x29, 1, 0x70);
	// Start ranging on TCA9548A bus 2
	startRanging(ObjectNum_1, VL53L0X_BETTER_ACCURACY_MODE, 0x29, 2, 0x70);

	#endif //UBUNTU
	timing = GetTiming(ObjectNum_0);
	  
	if (timing < 20000) timing = 20000;
	printf("Timing %d ms\n", timing/1000);
}



void sensor_manager_init(fp_robot_stop fpstop)
{
	static unsigned char sensor_manager_initialize_flag = 0;

	if(sensor_manager_initialize_flag == 0)
	{
#ifndef UBUNTU
		wiringPiSetup();
#endif
		SonarInit(28,29);
		laser_init();
		creat_thread();
		sensor_manager_initialize_flag = 1;
		stopRobot = fpstop;
	}
}



double sonar(void)
{
	double sonar_distance_temp = 0;
	double sonar_distance_temp_min = 0;
	double sonar_distance_temp_max = 0;
	double sonar_distance_temp_sum = 0;
	double sonar_distance;

	for(int i = 0 ; i<3 ; i++)
	{
		sonar_distance_temp = SonarDistance(30000);
		
		if(i == 0)
		{	
			sonar_distance_temp_min = sonar_distance_temp;
			sonar_distance_temp_max = sonar_distance_temp;
			sonar_distance_temp_sum = sonar_distance_temp;
		}
		else
		{
			sonar_distance_temp_sum = sonar_distance_temp_sum + sonar_distance_temp;
			if(sonar_distance_temp_max < sonar_distance_temp)
			{
				sonar_distance_temp_max = sonar_distance_temp;
			}	
			if(sonar_distance_temp_min > sonar_distance_temp)
			{
				sonar_distance_temp_min = sonar_distance_temp;
			}
		}
	}

	sonar_distance = sonar_distance_temp_sum - sonar_distance_temp_max - sonar_distance_temp_min;
	
	//cout << "Distance is " << sonar_distance << " cm." << endl;
	
	return sonar_distance;
}

int32_t laser_right(void)
{
#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.

	int32_t distance;
	unsigned int	count=0;
	// Get distance from VL53L0X  on TCA9548A bus 1 
	distance=getDistance(ObjectNum_0);
	
	usleep(timing);
	count++;
	return distance;

#else //UBUNTU
	return 0;
#endif //UBUNTU
}

int32_t laser_left(void)
{
#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.

	int32_t distance;
	unsigned int	count=0;
	// Get distance from VL53L0X  on TCA9548A bus 1 
	distance=getDistance(ObjectNum_1);
	
	usleep(timing);
	count++;
	return distance;

#else //UBUNTU
	return 0;
#endif //UBUNTU
}




//----------------------------------------------------------------
// VL53L0X_GetTiming
//----------------------------------------------------------------
unsigned int GetTiming(int object_number)
{
#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.

	VL53L0X_Error status;
	uint32_t      budget;
	VL53L0X_DEV   dev;

	dev=getDev(object_number);
	status=VL53L0X_GetMeasurementTimingBudgetMicroSeconds(dev,&budget);
	if  (status==VL53L0X_ERROR_NONE)
	{
		printf("sleep time %d\n",budget);
		return(budget+1000);
	}
	else return (0);
 
#else //UBUNTU
	return 0;
#endif //UBUNTU
}



void sensor_manager_main(fp_robot_stop fpstop)
{
	sensor_manager_init(fpstop);
}

T_SensorData get_sensor_data(void)
{
	return sensor_data;
}

T_CellData get_CellData(void) {
	return cell_data;
}

