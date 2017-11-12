package robot_algorithm;

public class MazeSolver_Test extends MazeSolverAlgorithm {

	@Override
	public byte[] getNext() {
		Cell robotCell = maze.getRobotCell();
		if (!robotCell.getWall().east)
			return EAST;
		if (!robotCell.getWall().south)
			return SOUTH;
		if (!robotCell.getWall().west)
			return WEST;
		if (!robotCell.getWall().north)
			return NORTH;
		return ERROR;
	}

}
