package robot_algorithm;

public class Cell {
	private byte[] data = new byte[8];

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

	public boolean hasCheckedCell() {
		if (data[7] == 0x1)
			return true;
		return false;
	}
	
	public void clearRobotInfo(){
		data[0] = 0x0;
	}

	public DirectionWallSet getWall() {
		DirectionWallSet ds = new DirectionWallSet();

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
	
	class DirectionWallSet {
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
