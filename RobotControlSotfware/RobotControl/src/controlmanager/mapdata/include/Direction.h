/*
 * Direction.h
 *
 *  Created on: November 6, 2017
 *      Author: lg
 */

#ifndef DirectionH
#define DirectionH

#define EAST 0x1
#define WEST 0x2
#define SOUTH 0x4
#define NORTH 0x8

typedef enum
{
	ROBOT_MOVING_DIRECTION_FORWARD = 0,
	ROBOT_MOVING_DIRECTION_LEFT_FORWARD,
	ROBOT_MOVING_DIRECTION_RIGHT_FORWARD,
	ROBOT_MOVING_DIRECTION_BACK_FORWARD,
	ROBOT_MOVING_DIRECTION_STOP,
	ROBOT_MOVING_DIRECTION_ERROR,
	ROBOT_MOVING_DIRECTION_MAX
} T_robot_moving_direction;

#endif /* DirectionH */
