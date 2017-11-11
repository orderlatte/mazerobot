package robot_algorithm;

import java.util.HashSet;
import java.util.Set;

import robot_algorithm.Cell.DirectionWallSet;

public class MazeSolver_DepthFirst implements MazeSolverAlgorithm {

	private Maze maze;
	private Set<Node> unvisitedNodes;
	private Set<Node> visitedNodes;
	private byte[] oldDirectionTogo;
	private Position oldPositionTogo;

	@Override
	public void init(Maze maze) {
		this.maze = maze;
		unvisitedNodes = new HashSet<>();
		visitedNodes = new HashSet<>();
		oldDirectionTogo = null;
		oldPositionTogo = null;
	}

	// <<Ʈ������ �˰���>>
	// 1.�������� ������ �Ȱ����̸� �ű�� ����
	// 1-1 �̵��Ҷ� ť�� �ִ� ������ ���� �ش� ť�� �ִ� ��带 �����ͼ� ���(ť������ ����)
	// 2.���������� �����ְų� �����̸� �θ�� ���ư���.
	// <<fully mapping �˰���>>
	// ���������Ȱ������̳����������ش缿��ť���־�д�.
	// ���湮��ť���ִ¼��̸��ش缿�����Ѵ�
	// �����̾���θ�ε��ư�����ť������ִٸ� Ǯ������!
	@Override
	public byte[] getNext() {
		Position p = new Position(maze.getPositionOfRobot());
		System.out.println("Current Position :" + p);
		if (oldPositionTogo != null && !oldPositionTogo.equals(p))
			return ERROR;

		Node curNode = getExistNode(p);

		System.out.println("Current Node : " + curNode);

		curNode.visited = true;
		this.unvisitedNodes.remove(curNode);
		this.visitedNodes.add(curNode);

		Node togo = null;
		byte[] togoValue = null;
		DirectionWallSet ds = maze.getCellOfRobot().getWall();

		if (!ds.east) {
			Node east = getExistNode(p.ofEast());
			if (!visitedNodes.contains(east)) {
				this.unvisitedNodes.add(east);
				if (togo == null) {
					togo = east;
					togoValue = EAST;
				}
			}
		}
		if (!ds.south) {
			Node south = getExistNode(p.ofSouth());
			if (!visitedNodes.contains(south)) {
				this.unvisitedNodes.add(south);
				if (togo == null) {
					togo = south;
					togoValue = SOUTH;
				}
			}
		}
		if (!ds.west) {
			Node west = getExistNode(p.ofWest());
			if (!visitedNodes.contains(west)) {
				this.unvisitedNodes.add(west);
				if (togo == null) {
					togo = west;
					togoValue = WEST;
				}
			}
		}
		if (!ds.north) {
			Node north = getExistNode(p.ofNorth());
			if (!visitedNodes.contains(north)) {
				this.unvisitedNodes.add(north);
				if (togo == null) {
					togo = north;
					togoValue = NORTH;
				}
			}
		}

		System.out.println("Unvisited : " + this.unvisitedNodes);
		System.out.println("Visited : " + this.visitedNodes);

		if (togo == null) {
			if (this.unvisitedNodes.size() == 0) {// fully mapping �Ϸ�
				return FULLYMAPPED;
			} else {// �θ���� back �̵�
				oldDirectionTogo = curNode.getParentDirectionAsByte();
				oldPositionTogo = curNode.parent.position;
				return oldDirectionTogo;
			}
		} else {// ���ο� ���� �̵�
			curNode.chilren.add(togo);
			togo.parent = curNode;
			oldDirectionTogo = togoValue;
			oldPositionTogo = togo.position;
			return togoValue;
		}
	}

	public Node getExistNode(Position p) {
		Node node = getNodeFromSet(p, unvisitedNodes);
		if (node == null)
			node = getNodeFromSet(p, visitedNodes);
		if (node == null)
			node = new Node(p);
		return node;
	}

	private Node getNodeFromSet(Position p, Set<Node> set) {
		for (Node n : set)
			if (n.position.equals(p))
				return n;
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
