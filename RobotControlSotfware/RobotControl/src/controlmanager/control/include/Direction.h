/*
 * Direction.h
 *
 *  Created on: November 6, 2017
 *      Author: lg
 */

#ifndef DirectionH
#define DirectionH

#define EAST 1
#define WEST 2
#define SOUTH 4
#define NORTH 8

typedef enum
{
	ROBOT_MOVING_DIRECTION_FORWARD = 0,
	ROBOT_MOVING_DIRECTION_LEFT,
	ROBOT_MOVING_DIRECTION_RIGHT,
	ROBOT_MOVING_DIRECTION_BACK,
	ROBOT_MOVING_DIRECTION_STOP,
	ROBOT_MOVING_DIRECTION_ERROR,
	ROBOT_MOVING_DIRECTION_MAX
} T_robot_moving_direction;

#endif /* DirectionH */
