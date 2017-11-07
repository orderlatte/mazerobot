package robot_algorithm;

import java.util.HashSet;
import java.util.Set;

import robot_algorithm.Cell.DirectionSet;

public class MazeSolver_DepthFirst extends MazeSolverAlgorithm {

	private Set<Node> unvisitedNodes = new HashSet<>();

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

		Position p = maze.getRobotPosition();
		Node curNode = new Node(p);
		curNode.visited = true;
		this.unvisitedNodes.remove(curNode);

		Node togo = null;
		byte[] togoValue = null;
		DirectionSet ds = maze.getRobotCell().getWall();

		if (ds.east) {
			Node east = new Node(p.getEast());
			if (!curNode.chilren.contains(east) && !curNode.parent.equals(east)) {
				this.unvisitedNodes.add(east);
				togo = east;
				togoValue = EAST;
			}
		}
		if (ds.south) {
			Node south = new Node(p.getSouth());
			if (!curNode.chilren.contains(south) && !curNode.parent.equals(south)) {
				this.unvisitedNodes.add(south);
				if (togo == null) {
					togo = south;
					togoValue = SOUTH;
				}
			}
		}
		if (ds.west) {
			Node west = new Node(p.getWest());
			if (!curNode.chilren.contains(west) && !curNode.parent.equals(west)) {
				this.unvisitedNodes.add(west);
				if (togo == null) {
					togo = west;
					togoValue = WEST;
				}
			}
		}
		if (ds.north) {
			Node north = new Node(p.getNorth());
			if (!curNode.chilren.contains(north) && !curNode.parent.equals(north)) {
				this.unvisitedNodes.add(north);
				if (togo == null) {
					togo = north;
					togoValue = NORTH;
				}
			}
		}

		if (togo == null) {
			if (this.unvisitedNodes.size() == 0) {// fully mapping �Ϸ�
				return FULLYMAPPED;
			} else {// �θ���� back �̵�
				return curNode.getParentDirection();
			}
		} else {//���ο� ���� �̵�
			curNode.chilren.add(togo);
			togo.parent = curNode;
			return togoValue;
		}
	}

}

class Node {
	Position cur;
	boolean visited = false;
	Node parent;
	Set<Node> chilren = new HashSet<>();

	public Node() {
	}

	public Node(Position p) {
		cur = p;
	}

	public byte[] getParentDirection() {
		int px = parent.cur.x;
		int py = parent.cur.y;
		if (px - cur.x == 0) {
			if (py > cur.y)
				return MazeSolverAlgorithm.NORTH;
			else
				return MazeSolverAlgorithm.SOUTH;
		} else {
			if (px > cur.x)
				return MazeSolverAlgorithm.EAST;
			else
				return MazeSolverAlgorithm.WEST;
		}
	}

	@Override
	public int hashCode() {
		return cur.hashCode();
	}

	@Override
	public boolean equals(Object obj) {
		return this.cur.equals(((Node) obj).cur);
	}

}
