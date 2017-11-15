package robot_algorithm;

import java.util.HashSet;
import java.util.Set;

class Node {
	Position position;
	boolean visited = false;
	Node parent;
	Set<Node> chilren = new HashSet<>();

	public Node() {
	}

	public Node(Position p) {
		position = p;
	}

	public boolean isNearNode(Node tgt) {
		return this.position.isNear(tgt.position);
	}

	public byte[] getDirectionAsByte(Node tgt){
		int px = tgt.position.x;
		int py = tgt.position.y;
		if (px == position.x) {
			if (py > position.y)
				return MazeSolverAlgorithm.SOUTH;
			else
				return MazeSolverAlgorithm.NORTH;
		} else {
			if (px > position.x)
				return MazeSolverAlgorithm.EAST;
			else
				return MazeSolverAlgorithm.WEST;
		}
	}
	
	public byte[] getParentDirectionAsByte() {
		int px = parent.position.x;
		int py = parent.position.y;
		if (px == position.x) {
			if (py > position.y)
				return MazeSolverAlgorithm.SOUTH;
			else
				return MazeSolverAlgorithm.NORTH;
		} else {
			if (px > position.x)
				return MazeSolverAlgorithm.EAST;
			else
				return MazeSolverAlgorithm.WEST;
		}
	}

	@Override
	public int hashCode() {
		return position.hashCode();
	}

	@Override
	public boolean equals(Object obj) {
		Position p = ((Node) obj).position;
		return this.position.equals(p);
	}

	@Override
	public String toString() {
		return "Node[" + position;
	}

}