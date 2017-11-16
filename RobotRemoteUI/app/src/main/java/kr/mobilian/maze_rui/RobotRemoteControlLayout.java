package kr.mobilian.maze_rui;

import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import static kr.mobilian.maze_rui.CommanderService.COMMAND_INVALID;
import static kr.mobilian.maze_rui.CommanderService.COMMAND_MOVE_BACKWARD;
import static kr.mobilian.maze_rui.CommanderService.COMMAND_MOVE_FORWARD;
import static kr.mobilian.maze_rui.CommanderService.COMMAND_MOVE_LEFT;
import static kr.mobilian.maze_rui.CommanderService.COMMAND_MOVE_RIGHT;
import static kr.mobilian.maze_rui.CommanderService.COMMAND_MOVE_STOP;
import static kr.mobilian.maze_rui.CommanderService.COMMAND_VIEW_CENTER;
import static kr.mobilian.maze_rui.CommanderService.COMMAND_VIEW_DOWN;
import static kr.mobilian.maze_rui.CommanderService.COMMAND_VIEW_LEFT;
import static kr.mobilian.maze_rui.CommanderService.COMMAND_VIEW_RIGHT;
import static kr.mobilian.maze_rui.CommanderService.COMMAND_VIEW_UP;

public class RobotRemoteControlLayout extends FrameLayout {
    private static final String TAG = "RobotRemoteControlLayout";
    private static boolean DEBUG = true;

    private static boolean USE_PRESS_COMMAND_MODE = true;

    private static final String CUSTOM_ATTRIBUTE_NAME_SPACE = "http://schemas.mobilian.kr/apk/res/android";
    private static final String CUSTOM_ATTRIBUTE_LAYOUT_ID = "layoutID";

    private static final int INVALID_ID = 0;

    private Context mContext;
    private int mChildLayoutId = INVALID_ID;
    private OnClickListener mClickListener;

    public RobotRemoteControlLayout(Context context) {
        this(context, null);
    }

    public RobotRemoteControlLayout(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public RobotRemoteControlLayout(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public RobotRemoteControlLayout(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr);

        mContext = context;

        if (attrs != null) {
            mChildLayoutId = attrs.getAttributeResourceValue(CUSTOM_ATTRIBUTE_NAME_SPACE, CUSTOM_ATTRIBUTE_LAYOUT_ID, INVALID_ID);
        }

        initLayout();
        registerCommandListener();
    }

    private void initLayout() {
        if (mChildLayoutId != INVALID_ID) {
            View inflateView = View.inflate(mContext, mChildLayoutId, (ViewGroup) getParent());
            addView(inflateView);
        }
    }

    private void registerCommandListener() {
        if (!USE_PRESS_COMMAND_MODE) {
            mClickListener = new OnClickListener() {
                @Override
                public void onClick(View view) {
                    switch (view.getId()) {
                        case R.id.remote_go_stop:
                            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().sendCommand(COMMAND_MOVE_STOP);
                            break;

                        case R.id.remote_go_forward:
                            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().sendCommand(COMMAND_MOVE_FORWARD);
                            break;

                        case R.id.remote_go_backward:
                            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().sendCommand(COMMAND_MOVE_BACKWARD);
                            break;

                        case R.id.remote_go_left:
                            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().sendCommand(COMMAND_MOVE_LEFT);
                            break;

                        case R.id.remote_go_right:
                            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().sendCommand(COMMAND_MOVE_RIGHT);
                            break;

                        case R.id.remote_view_center:
                            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().sendCommand(COMMAND_VIEW_CENTER);
                            break;

                        case R.id.remote_view_up:
                            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().sendCommand(COMMAND_VIEW_UP);
                            break;

                        case R.id.remote_view_down:
                            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().sendCommand(COMMAND_VIEW_DOWN);
                            break;

                        case R.id.remote_view_left:
                            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().sendCommand(COMMAND_VIEW_LEFT);
                            break;

                        case R.id.remote_view_right:
                            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().sendCommand(COMMAND_VIEW_RIGHT);
                            break;

                        default:
                            Log.e(TAG, "Unknown Command for " + String.format("0x08x", view.getId()));
                            break;
                    }
                }
            };
        }

        RobotRemoteControlButton robotRemoteControlButton = findViewById(R.id.remote_go_stop);
        if (robotRemoteControlButton != null) {
            robotRemoteControlButton.setCommand(COMMAND_MOVE_STOP, COMMAND_INVALID); //TODO, check release command ok.
            robotRemoteControlButton.setOnClickListener(mClickListener);
        }

        robotRemoteControlButton = findViewById(R.id.remote_go_forward);
        if (robotRemoteControlButton != null) {
            robotRemoteControlButton.setCommand(COMMAND_MOVE_FORWARD, COMMAND_MOVE_STOP);
            robotRemoteControlButton.setOnClickListener(mClickListener);
        }

        robotRemoteControlButton = findViewById(R.id.remote_go_backward);
        if (robotRemoteControlButton != null) {
            robotRemoteControlButton.setCommand(COMMAND_MOVE_BACKWARD, COMMAND_MOVE_STOP);
            robotRemoteControlButton.setOnClickListener(mClickListener);
        }

        robotRemoteControlButton = findViewById(R.id.remote_go_left);
        if (robotRemoteControlButton != null) {
            robotRemoteControlButton.setCommand(COMMAND_MOVE_LEFT, COMMAND_MOVE_STOP);
            robotRemoteControlButton.setOnClickListener(mClickListener);
        }

        robotRemoteControlButton = findViewById(R.id.remote_go_right);
        if (robotRemoteControlButton != null) {
            robotRemoteControlButton.setCommand(COMMAND_MOVE_RIGHT, COMMAND_MOVE_STOP);
            robotRemoteControlButton.setOnClickListener(mClickListener);
        }


        robotRemoteControlButton = findViewById(R.id.remote_view_center);
        if (robotRemoteControlButton != null) {
            robotRemoteControlButton.setCommand(COMMAND_VIEW_CENTER, COMMAND_INVALID); //TODO, check release command ok.
            robotRemoteControlButton.setOnClickListener(mClickListener);
        }

        robotRemoteControlButton = findViewById(R.id.remote_view_up);
        if (robotRemoteControlButton != null) {
            robotRemoteControlButton.setCommand(COMMAND_VIEW_UP, COMMAND_INVALID);
            robotRemoteControlButton.setOnClickListener(mClickListener);
        }

        robotRemoteControlButton = findViewById(R.id.remote_view_down);
        if (robotRemoteControlButton != null) {
            robotRemoteControlButton.setCommand(COMMAND_VIEW_DOWN, COMMAND_INVALID);
            robotRemoteControlButton.setOnClickListener(mClickListener);
        }

        robotRemoteControlButton = findViewById(R.id.remote_view_left);
        if (robotRemoteControlButton != null) {
            robotRemoteControlButton.setCommand(COMMAND_VIEW_LEFT, COMMAND_INVALID);
            robotRemoteControlButton.setOnClickListener(mClickListener);
        }

        robotRemoteControlButton = findViewById(R.id.remote_view_right);
        if (robotRemoteControlButton != null) {
            robotRemoteControlButton.setCommand(COMMAND_VIEW_RIGHT, COMMAND_INVALID);
            robotRemoteControlButton.setOnClickListener(mClickListener);
        }
    }
}
