package robot_algorithm;

import java.util.HashSet;
import java.util.Set;

import robot_algorithm.Cell.DirectionWallSet;

public class MazeSolver_DepthFirst extends MazeSolverAlgorithm {

	private Set<Node> unvisitedNodes = new HashSet<>();
	private Set<Node> visitedNodes = new HashSet<>();

	// <<트래버설 알고리즘>>
	// 1.동서남북 순서로 안간곳이면 거기로 간다
	// 1-1 이동할때 큐에 있는 곳으로 가면 해당 큐애 있느 노드를 가져와서 사용(큐에서는 삭제)
	// 2.동서남북이 막혀있거나 간곳이면 부모로 돌아간다.
	// <<fully mapping 알고리즘>>
	// 각셀에서안간방향이남아있으면해당셀을큐에넣어둔다.
	// 노드방문시큐에있는셀이면해당셀삭제한다
	// 갈곳이없어서부모로돌아갈때위큐가비어있다면 풀리매핑!
	@Override
	public byte[] getNext() {
		Position p = new Position(maze.getRobotPosition());
		Node curNode = getNodeFromSet(p, unvisitedNodes);
		if(curNode==null) curNode = getNodeFromSet(p, visitedNodes);
		if(curNode==null) curNode = new Node(p);

		System.out.println("==================================");
		System.out.println("Current Node : "+curNode);
		
		curNode.visited = true;
		this.unvisitedNodes.remove(curNode);
		this.visitedNodes.add(curNode);

		Node togo = null;
		byte[] togoValue = null;
		DirectionWallSet ds = maze.getRobotCell().getWall();

		
		
		if (!ds.east) {
			Node east = new Node(p.getEast());
			if (!visitedNodes.contains(east)) {
				this.unvisitedNodes.add(east);
				togo = east;
				togoValue = EAST;
			}
		}
		if (!ds.south) {
			Node south = new Node(p.getSouth());
			if (!visitedNodes.contains(south)) {
				this.unvisitedNodes.add(south);
				if (togo == null) {
					togo = south;
					togoValue = SOUTH;
				}
			}
		}
		if (!ds.west) {
			Node west = new Node(p.getWest());
			if (!visitedNodes.contains(west)) {
				this.unvisitedNodes.add(west);
				if (togo == null) {
					togo = west;
					togoValue = WEST;
				}
			}
		}
		if (!ds.north) {
			Node north = new Node(p.getNorth());
			if (!visitedNodes.contains(north)) {
				this.unvisitedNodes.add(north);
				if (togo == null) {
					togo = north;
					togoValue = NORTH;
				}
			}
		}

		System.out.println("Unvisited : "+this.unvisitedNodes);
		System.out.println("Visited : "+this.visitedNodes);
		
		if (togo == null) {
			if (this.unvisitedNodes.size() == 0) {// fully mapping 완료
				return FULLYMAPPED;
			} else {// 부모노드로 back 이동
				return curNode.getParentDirectionAsByte();
			}
		} else {//새로운 노드로 이동
			curNode.chilren.add(togo);
			togo.parent = curNode;
			return togoValue;
		}
	}
	
	private Node getNodeFromSet(Position p, Set<Node> set){
		for(Node n : set)
			if(n.position.equals(p)) return n;
		return null;
	}
}

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

	public byte[] getParentDirectionAsByte() {
		int px = parent.position.x;
		int py = parent.position.y;
		if (px - position.x == 0) {
			if (py > position.y)
				return MazeSolverAlgorithm.NORTH;
			else
				return MazeSolverAlgorithm.SOUTH;
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
		Position p = ((Node)obj).position;
		return this.position.equals(p);
	}

	@Override
	public String toString() {
		return "Node[" + position;
	}
	
	

}
