package robot_algorithm;

import java.util.HashSet;
import java.util.Set;

import robot_algorithm.Cell.DirectionWallSet;

//eswn
public class MazeSolver_DepthFirst_skip2 implements MazeSolverAlgorithm {

	private Maze maze;
	private Set<Node> unvisitedNodes;
	private Set<Node> visitedNodes;
	private byte[] directionTogo;// 다음갈방향
	private Position positionTogo;// 다음갈포지션
	private Position curPosition;// 현재 위치

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

		// 지난번과 같은 셀 정보를 또 보낼경우에 다시 같은 방향을 알려준다
		if (this.curPosition != null && this.curPosition.equals(p) && this.directionTogo != null)
			return directionTogo;

		// 지난번에 가라고 한 방향과 다른 셀로 갔을 경우 에러를 리턴한다.
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
			if (this.unvisitedNodes.size() == 0) {// fully mapping 완료
				return FULLYMAPPED;
			} else {// 부모노드로 back 이동
				// 알고리즘
				// 해당 노드의 부모에서 root까지 올라가면서 아래의 조건을 검사한다.
				// 노드가 처음 노드의 near이고 아직 안간곳이 있으면 바로 해당 노드로 결정
				// 노드가 처음 노드의 near이고 더 갈 곳이 없으면 저장하고 다시 부모 검사
				
				Node nodeTogo = this.getNodeAsSmartskip(curNode, curNode.parent);
				
				curPosition = curNode.position;
				directionTogo = curNode.getDirectionAsByte(nodeTogo);
				positionTogo = nodeTogo.position;

				return directionTogo;
			}
		} else {// 새로운 노드로 이동
			curNode.chilren.add(togo);
			togo.parent = curNode;
			directionTogo = togoValue;
			positionTogo = togo.position;
			curPosition = curNode.position;
			return togoValue;
		}
	}

	private boolean hasUnvisitedNodeNear(Node src, Set<Node> unvisitedNodes) {
		for (Node n : unvisitedNodes) {
			if (src.isNearNode(n))
				return true;
		}
		return false;
	}

	//startNode를 기준으로 target노드를 계속 부모로 바꿔가면서 가야할 노드를 찾아낸다.
	private Node getNodeAsSmartskip(Node startNode, Node targetNode) {
		Node candidate = null;
		
		while(true){
			if(targetNode==null) break;
			if (startNode.isNearNode(targetNode)) {

				if (hasUnvisitedNodeNear(targetNode, unvisitedNodes)){
					candidate = targetNode;
					return targetNode;
				}

				if (this.getGeneraionOfParent(startNode, targetNode, 0) > 0) {
					candidate = targetNode;
				}
			}
			targetNode = targetNode.parent;
		}
		return candidate;
	}

	// src와 target간 세대 값. 세대가 아니라면 null 리턴. 부모-자식 관계는 1 리턴.
	// 처음 call시 num은 0으로 넣어준다
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
