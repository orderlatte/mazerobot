package robot_algorithm;

public class MazeSolver {

	public static final byte[] EAST = { 0x8, 0x1, 0x0 };
	public static final byte[] WEST = { 0x8, 0x2, 0x0 };
	public static final byte[] SOUTH = { 0x8, 0x4, 0x0 };
	public static final byte[] NORTH = { 0x8, 0x8, 0x0 };
	public static final byte[] FULLYMAPPED = { 0x8, 0x0, 0x1 };
	public static final byte[] ERROR = { 0x8, 0x0, 0x2 };

	public static final String MODE_FULLY_MAPPING = "MODE_FULLY_MAPPING";
	public static final String MODE_GOTO_POINT = "MODE_GOTO_POINT";

	private Maze maze;

	public MazeSolver(Maze maze) {
		this.maze = maze;
	}

	public byte[] getNext() {
		Cell robotCell = maze.getRobotCell();
		if (!robotCell.getWall().east)
			return EAST;
		if (!robotCell.getWall().west)
			return WEST;
		if (!robotCell.getWall().north)
			return NORTH;
		if (!robotCell.getWall().south)
			return SOUTH;

		return ERROR;
	}

}
