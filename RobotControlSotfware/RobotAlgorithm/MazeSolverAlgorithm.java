package robot_algorithm;

public interface MazeSolverAlgorithm {

	static final byte[] EAST = { 0x8, 0x1, 0x0 };
	static final byte[] WEST = { 0x8, 0x2, 0x0 };
	static final byte[] SOUTH = { 0x8, 0x4, 0x0 };
	static final byte[] NORTH = { 0x8, 0x8, 0x0 };
	static final byte[] FULLYMAPPED = { 0x8, 0x0, 0x1 };
	static final byte[] ERROR = { 0x8, 0x0, 0x2 };
	static final byte[] INIT_OK = { 0x8, 0x0, 0x0 };

	void init();

	byte[] getNext();
	
	Maze getMaze();

}