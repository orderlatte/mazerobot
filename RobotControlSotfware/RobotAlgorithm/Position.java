package robot_algorithm;

class Position {

	public int x;
	public int y;

	public Position(int x, int y) {
		this.x = x;
		this.y = y;
	}
	
	public Position(Position p) {
		this.x = p.x;
		this.y = p.y;
	}
	
	public boolean isNear(Position tgt){
		if(tgt.equals(ofEast())) return true;
		if(tgt.equals(ofWest())) return true;
		if(tgt.equals(ofNorth())) return true;
		if(tgt.equals(ofSouth())) return true;
		return false;
	}

	public Position ofEast() {
		return new Position(x + 1, y);
	}

	public Position ofSouth() {
		return new Position(x, y + 1);
	}

	public Position ofWest() {
		return new Position(x - 1, y);
	}

	public Position ofNorth() {
		return new Position(x, y - 1);
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + x;
		result = prime * result + y;
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		Position other = (Position) obj;
		if (x != other.x)
			return false;
		if (y != other.y)
			return false;
		return true;
	}

	@Override
	public String toString() {
		return "[x=" + x + ", y=" + y + "]";
	}
	
	
}