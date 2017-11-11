package robot_algorithm;

import java.nio.ByteBuffer;
import java.util.Arrays;

public class Maze {

	Cell[][] cellArray;
	Position robotPosition = new Position(0, 0);

	public static final int mx = 11;
	public static final int my = 11;

	public Maze() {
		cellArray = new Cell[my][mx];
		for (int i = 0; i < my; i++)
			for (int j = 0; j < mx; j++)
				cellArray[i][j] = new Cell();
	}

	public byte[] getMazeAsBytes() {
		byte[] rv = new byte[mx * my * 8 + 1];
		int c = 0;
		rv[c++] = 0x2;
		for (int i = 0; i < my; i++)
			for (int j = 0; j < mx; j++)
				for (int k = 0; k < 8; k++)
					rv[c++] = cellArray[i][j].getCellAsBytes()[k];
		return rv;
	}

	public Cell[][] getMaze() {
		return cellArray;
	}

	public Cell getMaze(int x, int y) {
		return this.cellArray[y][x];
	}

	public void setCell(int x, int y, Cell val) {
		this.clearRobotInfoOfMaze(this.cellArray);
		this.cellArray[y][x] = val;
		this.robotPosition.x = x;
		this.robotPosition.y = y;
	}
	
	private void clearRobotInfoOfMaze(Cell[][] maze){
		for (int i = 0; i < my; i++)
			for (int j = 0; j < mx; j++)
					maze[i][j].clearRobotInfo();
	}

	public void setCell(byte[] val) {
		Cell c = new Cell(Arrays.copyOfRange(val, 0, 8));

		char x = ByteBuffer.wrap(Arrays.copyOfRange(val, 8, 10)).getChar();
		char y = ByteBuffer.wrap(Arrays.copyOfRange(val, 10, 12)).getChar();
		this.setCell(x, y, c);
	}

	public Cell getCellOfRobot() {
		return cellArray[this.robotPosition.y][this.robotPosition.x];
	}

	public Position getPositionOfRobot() {
		return this.robotPosition;
	}
}
