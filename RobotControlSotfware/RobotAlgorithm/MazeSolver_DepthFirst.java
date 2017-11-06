package robot_algorithm;

import robot_algorithm.Cell.DirectionSet;
import robot_algorithm.Maze.Position;

public class MazeSolver_DepthFirst extends MazeSolverAlgorithm {

	// 모드 정의
	public final int mode_init = 1;
	public final int mode_down = 2;

	// 방향정의
	public final int direction_east = 1;
	public final int direction_west = 2;
	public final int direction_north = 4;
	public final int direction_south = 8;

	// 필드변수
	private int mode = mode_init;
	private Node rootNode = null;
	private Node curNode = null;
	private Node lastNode = null;
	private int lastDirection = 0;

	@Override
	public byte[] getNext() {

		Position p = maze.getRobotPosition();
		switch (mode) {
		case mode_init: {
			this.curNode = new Node();
			this.curNode.cur = maze.getRobotPosition();
			this.rootNode = this.curNode;
			this.curNode.visited = true;
			DirectionSet ds = maze.getRobotCell().getWall();
			if (ds.east) {
				lastDirection = this.direction_east;
				this.mode = mode_down;
				return EAST;
			}
			if (ds.south) {
				lastDirection = this.direction_east;
				this.mode = mode_down;
				return SOUTH;
			}
			if (ds.west) {
				lastDirection = this.direction_east;
				this.mode = mode_down;
				return WEST;
			}
			if (ds.north) {
				lastDirection = this.direction_east;
				this.mode = mode_down;
				return NORTH;
			}
			return FULLYMAPPED;
		}

		case mode_down: {
			this.lastNode = this.curNode;
			this.curNode = new Node();						
			this.curNode.cur = maze.getRobotPosition();
			DirectionSet ds = maze.getRobotCell().getWall();

			
		}
		}//end of switch
		return EAST;		
	}//end of getNext()
}

class Node {
	Position cur;
	Node e = null;
	Node w = null;
	Node s = null;
	Node n = null;
	boolean visited = false;
}
