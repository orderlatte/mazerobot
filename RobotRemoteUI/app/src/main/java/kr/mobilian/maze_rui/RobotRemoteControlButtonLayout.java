package kr.mobilian.maze_rui;

import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.widget.RelativeLayout;

public class RobotRemoteControlButtonLayout extends RelativeLayout {
    private static final String TAG = "RobotRemoteControlButtonLayout";
    private static boolean DEBUG = false;

    public RobotRemoteControlButtonLayout(Context context) {
        this(context, null);
    }

    public RobotRemoteControlButtonLayout(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public RobotRemoteControlButtonLayout(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public RobotRemoteControlButtonLayout(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr);
    }

    public void bottonPressed(int id) {
        final int childCount = getChildCount();
        Log.d(DEBUG, TAG, "bottonPressed, " + childCount);
        if (childCount == 0) {
            return;
        }

        for (int i = 0; i < childCount; i++) {
            View child = getChildAt(i);
            if (child != null && child instanceof RobotRemoteControlButton) {
                if (child.getId() == id) {
                    child.setEnabled(true);
                } else {
                    child.setEnabled(false);
                }
            }
        }
    }

    public void bottonReleased(int id) {
        final int childCount = getChildCount();
        Log.d(DEBUG, TAG, "bottonReleased, " + childCount);
        if (childCount == 0) {
            return;
        }

        for (int i = 0; i < childCount; i++) {
            View child = getChildAt(i);
            if (child != null && child instanceof RobotRemoteControlButton) {
                child.setEnabled(true);
            }
        }
    }
}
