package robot_algorithm;

import java.util.HashSet;
import java.util.Set;

import robot_algorithm.Cell.DirectionWallSet;

//eswn
public class MazeSolver_DepthFirst_skip implements MazeSolverAlgorithm {

	private Maze maze;
	private Set<Node> unvisitedNodes;
	private Set<Node> visitedNodes;
	private byte[] directionTogo;// ����������
	private Position positionTogo;// ������������
	private Position curPosition;// ���� ��ġ

	@Override
	public void init() {
		this.maze = new Maze();
		unvisitedNodes = new HashSet<>();
		visitedNodes = new HashSet<>();
		directionTogo = null;
		positionTogo = null;
		curPosition = null;
	}

	@Override
	public byte[] getNext() {
		Position p = new Position(maze.getPositionOfRobot());
		System.out.println("Current Position :" + p);

		// �������� ���� �� ������ �� ������쿡 �ٽ� ���� ������ �˷��ش�
		if (this.curPosition != null && this.curPosition.equals(p) && this.directionTogo != null)
			return directionTogo;

		// �������� ����� �� ����� �ٸ� ���� ���� ��� ������ �����Ѵ�.
		if (positionTogo != null && !positionTogo.equals(p))
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
				curPosition = curNode.position;
				directionTogo = curNode.getParentDirectionAsByte();
				positionTogo = curNode.parent.position;

				int maxGen = 0;
				if (!ds.east) {
					Node east = getNodeFromSet(p.ofEast(), visitedNodes);
					if (east != null) {
						Integer genNum = this.getGeneraionOfParent(curNode, east, 0);
						if (genNum != null && genNum > maxGen) {
							maxGen = genNum;
							directionTogo = EAST;
							positionTogo = east.position;
						}
					}
				}
				if (!ds.south) {
					Node south = getNodeFromSet(p.ofSouth(), visitedNodes);
					if (south != null) {
						Integer genNum = this.getGeneraionOfParent(curNode, south, 0);
						if (genNum != null && genNum > maxGen) {
							maxGen = genNum;
							directionTogo = SOUTH;
							positionTogo = south.position;
						}
					}
				}
				if (!ds.west) {
					Node west = getNodeFromSet(p.ofWest(), visitedNodes);
					if (west != null) {
						Integer genNum = this.getGeneraionOfParent(curNode, west, 0);
						if (genNum != null && genNum > maxGen) {
							maxGen = genNum;
							directionTogo = WEST;
							positionTogo = west.position;
						}
					}
				}
				if (!ds.north) {
					Node north = getNodeFromSet(p.ofNorth(), visitedNodes);
					if (north != null) {
						Integer genNum = this.getGeneraionOfParent(curNode, north, 0);
						if (genNum != null && genNum > maxGen) {
							maxGen = genNum;
							directionTogo = NORTH;
							positionTogo = north.position;
						}
					}
				}
				return directionTogo;
			}
		} else {// ���ο� ���� �̵�
			curNode.chilren.add(togo);
			togo.parent = curNode;
			directionTogo = togoValue;
			positionTogo = togo.position;
			curPosition = curNode.position;
			return togoValue;
		}
	}

	// ó�� call�� num�� 0���� �־��ش�
	private Integer getGeneraionOfParent(Node src, Node target, Integer num) {
		num++;
		if (src.parent == null)
			return null;
		if (src.parent.equals(target))
			return num;
		else
			return getGeneraionOfParent(src.parent, target, num);
	}

	private Node getExistNode(Position p) {
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

	@Override
	public Maze getMaze() {
		return this.maze;
	}
}
