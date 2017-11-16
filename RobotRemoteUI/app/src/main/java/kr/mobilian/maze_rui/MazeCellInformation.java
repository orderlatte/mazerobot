package kr.mobilian.maze_rui;

import static kr.mobilian.maze_rui.MazeCellInformationConstant.CELL_NAME_END;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.CELL_NAME_START;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.MAZE_CELL_DISCOVERED;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.MAZE_WALL_IN_EAST;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.MAZE_WALL_IN_EAST_CHECKED;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.MAZE_WALL_IN_NORTH;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.MAZE_WALL_IN_NORTH_CHECKED;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.MAZE_WALL_IN_SOUTH;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.MAZE_WALL_IN_SOUTH_CHECKED;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.MAZE_WALL_IN_WEST;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.MAZE_WALL_IN_WEST_CHECKED;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.RED_DOT_EXIST;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.ROBOT_DIRECTION_EAST;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.ROBOT_DIRECTION_NORTH;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.ROBOT_DIRECTION_SOUTH;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.ROBOT_DIRECTION_WEST;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.ROBOT_EXIST;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.WALL_SIGN_ON_EAST;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.WALL_SIGN_ON_NORTH;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.WALL_SIGN_ON_SOUTH;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.WALL_SIGN_ON_WEST;

public class MazeCellInformation {
    private static final String TAG = "MazeCellInformation";
    private static boolean DEBUG = true;

    private final int mCellData;
    private final WallSignIcon mSignIcon;

    public MazeCellInformation(int cellData) {
        mCellData = cellData;
        mSignIcon = WallSignIcon.getWallSignIcon(cellData);
    }

    public boolean isDiscovered() {
        return ((mCellData & MAZE_CELL_DISCOVERED) != 0) ? true : false;
    }

    public boolean isWallCheckedInEast() {
        return (mCellData & MAZE_WALL_IN_EAST_CHECKED) != 0 ? true : false;
    }

    public boolean isWallCheckedInWest() {
        return (mCellData & MAZE_WALL_IN_WEST_CHECKED) != 0 ? true : false;
    }

    public boolean isWallCheckedInSouth() {
        return (mCellData & MAZE_WALL_IN_SOUTH_CHECKED) != 0 ? true : false;
    }

    public boolean isWallCheckedInNorth() {
        return (mCellData & MAZE_WALL_IN_NORTH_CHECKED) != 0 ? true : false;
    }

    public boolean isWallExistInEast() {
        return (mCellData & MAZE_WALL_IN_EAST) != 0 ? true : false;
    }

    public boolean isWallExistInWest() {
        return (mCellData & MAZE_WALL_IN_WEST) != 0 ? true : false;
    }

    public boolean isWallExistInSouth() {
        return (mCellData & MAZE_WALL_IN_SOUTH) != 0 ? true : false;
    }

    public boolean isWallExistInNorth() {
        return (mCellData & MAZE_WALL_IN_NORTH) != 0 ? true : false;
    }

    public boolean isCellNameStart() {
        return (mCellData & CELL_NAME_START) != 0 ? true : false;
    }

    public boolean isCellNameEnd() {
        return (mCellData & CELL_NAME_END) != 0 ? true : false;
    }

    public boolean hasRedDot() {
        return ((mCellData & RED_DOT_EXIST) != 0) ? true : false;
    }

    public WallSignIcon getWallSign(){
        return mSignIcon;
    }

    public boolean isSignExistOnEast() {
        return (mCellData & WALL_SIGN_ON_EAST) != 0 ? true : false;
    }

    public boolean isSignExistOnWest() {
        return (mCellData & WALL_SIGN_ON_WEST) != 0 ? true : false;
    }

    public boolean isSignExistOnSouth() {
        return (mCellData & WALL_SIGN_ON_SOUTH) != 0 ? true : false;
    }

    public boolean isSignExistOnNorth() {
        return (mCellData & WALL_SIGN_ON_NORTH) != 0 ? true : false;
    }

    public boolean robotIsLookingToEast() {
        return (mCellData & ROBOT_DIRECTION_EAST) != 0 ? true : false;
    }

    public boolean robotIsLookingToWest() {
        return (mCellData & ROBOT_DIRECTION_WEST) != 0 ? true : false;
    }

    public boolean robotIsLookingToSouth() {
        return (mCellData & ROBOT_DIRECTION_SOUTH) != 0 ? true : false;
    }

    public boolean robotIsLookingToNorth() {
        return (mCellData & ROBOT_DIRECTION_NORTH) != 0 ? true : false;
    }

    public boolean isRobotExist() {
        return (mCellData & ROBOT_EXIST) != 0 ? true : false;
    }

    @Override
    public String toString() {
        return new StringBuilder()
                .append(String.format("0x%08x", mCellData)).append(", ")
                .append(isDiscovered() ? "Known" : "Unknown").append(", ")
                .append("WallChecked: ").append(isWallCheckedInEast() ? "E" : "").append(isWallCheckedInWest() ? "W" : "").append(isWallCheckedInSouth() ? "S" : "").append(isWallCheckedInNorth() ? "N" : "").append(", ")
                .append("WallExist: ").append(isWallExistInEast()? "E" : "").append(isWallExistInWest() ? "W" : "").append(isWallExistInSouth() ? "S" : "").append(isWallExistInNorth() ? "N" : "").append(", ")
                .append("HasRedDot: ").append(hasRedDot() ? "TRUE" : "FALSE").append(", ")
                .append("HasSign: ").append(getWallSign().toString()).append(isSignExistOnEast()? "on E" : "").append(isSignExistOnWest() ? "on W" : "").append(isSignExistOnSouth() ? "on S" : "").append(isSignExistOnNorth() ? "on N" : "").append(", ")
                .append("RobotExist: ").append(isRobotExist() ? "TRUE" : "FALSE").append(robotIsLookingToEast()? " looking to E" : "").append(robotIsLookingToWest() ? " looking to W" : "").append(robotIsLookingToSouth() ? " looking to S" : "").append(robotIsLookingToNorth() ? " looking to N" : "")
                .toString();
    }
}
