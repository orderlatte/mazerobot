package kr.mobilian.maze_rui;

import static kr.mobilian.maze_rui.MazeCellInformationConstant.WALL_SIGN_INDEX_ARROW_UP;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.WALL_SIGN_INDEX_BASKET_BALL;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.WALL_SIGN_INDEX_GO;
import static kr.mobilian.maze_rui.MazeCellInformationConstant.WALL_SIGN_INDEX_STOP;

public enum WallSignIcon {
    WALL_SIGN_NONE("NONE", R.drawable.wall_sign_empty),
    WALL_SIGN_GO("GO", R.drawable.wall_sign_go),
    WALL_SIGN_STOP("STOP", R.drawable.wall_sign_stop),
    WALL_SIGN_ARROW_UP("ARROW_UP", R.drawable.wall_sign_arrow_up),
    WALL_SIGN_BASKET_BALL("BASKET_BALL", R.drawable.wall_sign_basket_ball);

    private String mName;
    private int mIconDrawableId;

    WallSignIcon(String name, int iconDrawable) {
        mName = name;
        mIconDrawableId = iconDrawable;
    }

    public int getDrawableId() {
        return mIconDrawableId;
    }

    @Override
    public String toString() {
        return mName;
    }

    public static WallSignIcon getWallSignIcon(int wallSignIcon) {
        if ((wallSignIcon & WALL_SIGN_INDEX_GO) != 0) {
            return WALL_SIGN_GO;
        }

        if ((wallSignIcon & WALL_SIGN_INDEX_STOP) != 0) {
            return WALL_SIGN_STOP;
        }

        if ((wallSignIcon & WALL_SIGN_INDEX_ARROW_UP) != 0) {
            return WALL_SIGN_ARROW_UP;
        }

        if ((wallSignIcon & WALL_SIGN_INDEX_BASKET_BALL) != 0) {
            return WALL_SIGN_BASKET_BALL;
        }

        return WALL_SIGN_NONE;
    }
}