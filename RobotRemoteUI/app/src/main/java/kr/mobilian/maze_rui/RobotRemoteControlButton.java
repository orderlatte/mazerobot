package kr.mobilian.maze_rui;

import android.content.Context;
import android.os.Handler;
import android.support.v7.widget.AppCompatImageButton;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewParent;

import static kr.mobilian.maze_rui.CommanderService.COMMAND_INVALID;

public class RobotRemoteControlButton extends AppCompatImageButton {
    private static final String TAG = "RobotRemoteControlButton";
    private static boolean DEBUG = true;

    private int mCommandPress = COMMAND_INVALID;
    private int mCommandRelease = COMMAND_INVALID;

    private PressEventHandler mPressEventHandler = new PressEventHandler();

    public RobotRemoteControlButton(Context context) {
        this(context, null);
    }

    public RobotRemoteControlButton(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public RobotRemoteControlButton(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public RobotRemoteControlButton(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr);

        registerPressHandler();
    }

    public void setCommand(int commandPress, int commandRelease) {
        mCommandPress = commandPress;
        mCommandRelease = commandRelease;
    }

    private void registerPressHandler() {
        setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                if (event.getAction() == MotionEvent.ACTION_DOWN) {
                    mPressEventHandler.pressed(false);
                } else if (event.getAction() == MotionEvent.ACTION_UP) {
                    mPressEventHandler.released();
                }

                return false;
            }
        });
    }

    private class PressEventHandler extends Handler {
        private final int REPEAT_INTERVAL = 100;
        private int repeatCount;
        private boolean doRepeate;

        public void pressed(boolean repeat) {
            Log.d(DEBUG, TAG, "pressed");

            doRepeate = repeat;
            repeatCount = 0;

            if (doRepeate) {
                removeCallbacksAndMessages(null);
            }

            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().sendCommand(mCommandPress);

            ViewParent parent = getParent();
            if (parent != null && parent instanceof RobotRemoteControlButtonLayout) {
                ((RobotRemoteControlButtonLayout) parent).bottonPressed(RobotRemoteControlButton.this.getId());
            }

            if (repeat) {
                repeat();
            }
        }

        private void repeat() {
            if (!doRepeate) {
                return;
            }

            Log.d(DEBUG, TAG, "repeat, " + repeatCount);
            postDelayed(new Runnable() {
                @Override
                public void run() {
                    repeat();
                }
            }, REPEAT_INTERVAL);

            repeatCount++;
        }

        public void released() {
            Log.d(DEBUG, TAG, "released");
            if (doRepeate) {
                removeCallbacksAndMessages(null);
            }

            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().sendCommand(mCommandRelease);

            ViewParent parent = getParent();
            if (parent != null && parent instanceof RobotRemoteControlButtonLayout) {
                ((RobotRemoteControlButtonLayout) parent).bottonReleased(RobotRemoteControlButton.this.getId());
            }
        }
    }
}
