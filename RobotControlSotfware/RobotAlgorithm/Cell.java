package robot_algorithm;

import robot_algorithm.Maze.Position;

public class Cell {
	byte[] data = new byte[8];

	public static final byte MASK_EAST = 0x01;
	public static final byte MASK_WEST = 0x02;
	public static final byte MASK_SOUTH = 0x04;
	public static final byte MASK_NORTTH = 0x08;

	public Cell() {}
	
	public Cell(byte[] cell){
		this.data = cell;
	}
	
	public byte[] getCellAsBytes(){
		return data;
	}

	public void setCell(byte[] cell) {

	}

	public boolean hasCheckedCell() {
		if (data[7] == 0x1)
			return true;
		return false;
	}

	public DirectionSet getWall() {
		DirectionSet ds = new DirectionSet();

		if ((this.data[6] & MASK_EAST) == MASK_EAST)
			ds.east = true;
		if ((this.data[6] & MASK_WEST) == MASK_WEST)
			ds.west = true;
		if ((this.data[6] & MASK_SOUTH) == MASK_SOUTH)
			ds.south = true;
		if ((this.data[6] & MASK_NORTTH) == MASK_NORTTH)
			ds.north = true;

		return ds;
	}
	
	public boolean hasRobot() {
		if (this.data[0] != 0x0)
			return true;
		return false;
	}

	class DirectionSet {
		boolean east = false;
		boolean west = false;
		boolean south = false;
		boolean north = false;

		@Override
		public String toString() {
			return "DirectionSet [east=" + east + ", west=" + west + ", south=" + south + ", north=" + north + "]";
		}

	}
}
