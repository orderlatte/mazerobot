package robot_algorithm;

import java.nio.ByteBuffer;
import java.util.Arrays;

public class Maze {

	Cell[][] maze;
	Position robot = new Position(0, 0);

	public static final int mx = 11;
	public static final int my = 11;

	public Maze() {
		maze = new Cell[my][mx];
		for (int i = 0; i < my; i++)
			for (int j = 0; j < mx; j++)
				maze[i][j] = new Cell();
	}

	public byte[] getMazeAsBytes() {
		byte[] rv = new byte[mx * my * 8 + 1];
		int c = 0;
		rv[c++] = 0x2;
		for (int i = 0; i < my; i++)
			for (int j = 0; j < mx; j++)
				for (int k = 0; k < 8; k++)
					rv[c++] = maze[i][j].getCellAsBytes()[k];
		return rv;
	}

	public Cell[][] getMaze() {
		return maze;
	}

	public Cell getMaze(int x, int y) {
		return this.maze[y][x];
	}

	public void setMaze(int x, int y, Cell val) {
		this.maze[y][x] = val;
		this.robot.x = x;
		this.robot.y = y;
	}

	public void setMaze(byte[] val) {
		Cell c = new Cell(Arrays.copyOfRange(val, 0, 8));

		char x = ByteBuffer.wrap(Arrays.copyOfRange(val, 8, 10)).getChar();
		char y = ByteBuffer.wrap(Arrays.copyOfRange(val, 10, 12)).getChar();
		this.setMaze(x, y, c);
	}

	public Cell getRobotCell() {
		return maze[this.robot.y][this.robot.x];
	}

	public Position getRobotPosition() {
		return this.robot;
	}
}
