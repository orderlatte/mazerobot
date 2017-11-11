package robot_algorithm;

public abstract class MazeSolverAlgorithm {

	protected static final byte[] EAST = { 0x8, 0x1, 0x0 };
	protected static final byte[] WEST = { 0x8, 0x2, 0x0 };
	protected static final byte[] SOUTH = { 0x8, 0x4, 0x0 };
	protected static final byte[] NORTH = { 0x8, 0x8, 0x0 };
	protected static final byte[] FULLYMAPPED = { 0x8, 0x0, 0x1 };
	protected static final byte[] ERROR = { 0x8, 0x0, 0x2 };
	protected static final byte[] INIT_OK = { 0x8, 0x0, 0x0 };

	protected Maze maze;

	public void init(Maze maze) {
		this.maze = maze;
	}

	public abstract byte[] getNext();

}