package robot_algorithm;

class Position {

	private final Maze Position;
	public int x;
	public int y;

	public Position(Maze maze, int x, int y) {
		Position = maze;
		this.x = x;
		this.y = y;
	}

	public Position getEast() {
		return new Position(Position, x + 1, y);
	}

	public Position getSouth() {
		return new Position(Position, x, y - 1);
	}

	public Position getWest() {
		return new Position(Position, x - 1, y);
	}

	public Position getNorth() {
		return new Position(Position, x, y + 1);
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
}