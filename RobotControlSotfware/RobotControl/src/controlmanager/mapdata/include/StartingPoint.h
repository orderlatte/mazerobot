/*
 * StaringPoint.h
 *
 *  Created on: November 7, 2017
 *      Author: lg
 */

#ifndef StartingPointH
#define StartingPointH

class StartingPoint {

public:
	int PositionX;
	int PositionY;

public:
	StartingPoint(int initialPositionX, int initialPositionY);
	void reset(int robotEWSNDirection, int robotPositionX, int robotPositionY);
	bool isStartingPoint(int robotPositionX, int robotPositionY);

private:
	void resetEast(int robotPositionX, int robotPositionY);
	void resetWest(int robotPositionX, int robotPositionY);
	void resetSouth(int robotPositionX, int robotPositionY);
	void resetNorth(int robotPositionX, int robotPositionY);
};


#endif /* StartingPointH */
