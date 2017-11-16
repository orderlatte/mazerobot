package kr.mobilian.maze_rui;

public class MazeCellInformationConstant {
    public static final int MAP_MAX_SIZE = 11;

    public static final int MAZE_CELL_DISCOVERED = 0x00000001;

    public static final int MAZE_WALL_IN_EAST = 0x00000010;
    public static final int MAZE_WALL_IN_WEST = 0x00000020;
    public static final int MAZE_WALL_IN_SOUTH = 0x00000040;
    public static final int MAZE_WALL_IN_NORTH = 0x00000080;
    public static final int MAZE_WALL_EXIST = MAZE_WALL_IN_EAST | MAZE_WALL_IN_WEST | MAZE_WALL_IN_SOUTH | MAZE_WALL_IN_NORTH;

    public static final int MAZE_WALL_IN_EAST_CHECKED = 0x00000100;
    public static final int MAZE_WALL_IN_WEST_CHECKED = 0x00000200;
    public static final int MAZE_WALL_IN_SOUTH_CHECKED = 0x00000400;
    public static final int MAZE_WALL_IN_NORTH_CHECKED = 0x00000800;
    public static final int MAZE_WALL_CHECKED_EXIST = MAZE_WALL_IN_EAST_CHECKED | MAZE_WALL_IN_WEST_CHECKED | MAZE_WALL_IN_SOUTH_CHECKED | MAZE_WALL_IN_NORTH_CHECKED;

    public static final int RED_DOT_EXIST = 0x00001000;

    public static final int WALL_SIGN_INDEX_GO = 0x00010000;
    public static final int WALL_SIGN_INDEX_STOP = 0x00020000;
    public static final int WALL_SIGN_INDEX_ARROW_UP = 0x00040000;
    public static final int WALL_SIGN_INDEX_BASKET_BALL = 0x00080000;
    public static final int WALL_SIGN_EXIST = WALL_SIGN_INDEX_GO | WALL_SIGN_INDEX_STOP | WALL_SIGN_INDEX_ARROW_UP | WALL_SIGN_INDEX_BASKET_BALL;

    public static final int CELL_NAME_START = 0x00100000;
    public static final int CELL_NAME_END = 0x00200000;

    public static final int WALL_SIGN_ON_EAST = 0x01000000;
    public static final int WALL_SIGN_ON_WEST = 0x02000000;
    public static final int WALL_SIGN_ON_SOUTH = 0x04000000;
    public static final int WALL_SIGN_ON_NORTH = 0x08000000;

    public static final int ROBOT_DIRECTION_EAST = 0x10000000;
    public static final int ROBOT_DIRECTION_WEST = 0x20000000;
    public static final int ROBOT_DIRECTION_SOUTH = 0x40000000;
    public static final int ROBOT_DIRECTION_NORTH = 0x80000000;
    public static final int ROBOT_EXIST = ROBOT_DIRECTION_EAST | ROBOT_DIRECTION_WEST | ROBOT_DIRECTION_SOUTH | ROBOT_DIRECTION_NORTH;
}
