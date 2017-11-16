package kr.mobilian.maze_rui;

import android.content.Context;
import android.os.Handler;
import android.util.AttributeSet;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;

import static kr.mobilian.maze_rui.CommanderService.COMMAND_MODE_AUTO;
import static kr.mobilian.maze_rui.CommanderService.COMMAND_MODE_MANUAL;
import static kr.mobilian.maze_rui.RobotMode.AUTO;
import static kr.mobilian.maze_rui.RobotMode.FREEZE;
import static kr.mobilian.maze_rui.RobotMode.MANUAL;
import static kr.mobilian.maze_rui.RobotMode.RESET;
import static kr.mobilian.maze_rui.RobotStateReceiveService.IRobotModeChangedCallback;

public class RobotModeSelectLayout extends LinearLayout {
    private static final String TAG = "RobotModeSelectLayout";
    private static boolean DEBUG = true;

    private RobotMode mCurrentRobotMode = RobotMode.NONE;
    private IRobotModeChangedCallback mRobotModeChangedCallback;

    private Button mAutoModeButton;
    private Button mManualModeButton;
    private Button mResetModeButton;

    public RobotModeSelectLayout(Context context) {
        this(context, null);
    }

    public RobotModeSelectLayout(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public RobotModeSelectLayout(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public RobotModeSelectLayout(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr);
    }

    @Override
    protected void onFinishInflate() {
        super.onFinishInflate();

        mAutoModeButton = (Button)findViewById(R.id.auto_mode_button);
        if (mAutoModeButton != null) {
            mAutoModeButton.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View view) {
                    ((MazeApplication) getContext().getApplicationContext()).getCommanderService().sendCommand(COMMAND_MODE_AUTO);
                    setModeAs(AUTO);
                }
            });
        }

        mManualModeButton = (Button)findViewById(R.id.manual_mode_button);
        if (mManualModeButton != null) {
            mManualModeButton.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View view) {
                    ((MazeApplication) getContext().getApplicationContext()).getCommanderService().sendCommand(COMMAND_MODE_MANUAL);
                    setModeAs(MANUAL);
                }
            });
        }

        mResetModeButton = (Button)findViewById(R.id.reset_mode_button);
        if (mResetModeButton != null) {
            mResetModeButton.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View view) {
                    setModeAs(FREEZE);
                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().sendResetCommand();
                            setModeAs(RESET);
                        }
                    }, 1 * 1000);
                }
            });
        }

        setModeAs(mCurrentRobotMode);
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

        registerRobotModeChangedReceiver();
    }

    @Override
    protected void onDetachedFromWindow() {
        unregisterRobotModeChangedReceiver();

        super.onDetachedFromWindow();
    }

    private void registerRobotModeChangedReceiver() {
        if (mRobotModeChangedCallback == null) {
            mRobotModeChangedCallback = new RobotStateReceiveService.IRobotModeChangedCallback() {
                @Override
                public void onRobotModeChanged(RobotMode mode) {
                    setModeAs(mode);
                }
            };

            ((MazeApplication) getContext().getApplicationContext()).getRobotStateReceiveService().registerCallback(mRobotModeChangedCallback);
        }
    }

    private void unregisterRobotModeChangedReceiver() {
        if (mRobotModeChangedCallback != null) {
            ((MazeApplication) getContext().getApplicationContext()).getRobotStateReceiveService().unregisterCallback(mRobotModeChangedCallback);
            mRobotModeChangedCallback = null;
        }
    }

    public void setModeAs(RobotMode mode) {
        mCurrentRobotMode = mode;

        updateState();
    }

    private void updateState() {
        switch (mCurrentRobotMode) {
            case FREEZE:
                if (mAutoModeButton != null) {
                    mAutoModeButton.setEnabled(false);
                }

                if (mManualModeButton != null) {
                    mManualModeButton.setEnabled(false);
                }

                if (mResetModeButton != null) {
                    mResetModeButton.setEnabled(false);
                }

                break;

            case AUTO:
                if (mAutoModeButton != null) {
                    mAutoModeButton.setEnabled(false);
                }

                if (mManualModeButton != null) {
                    mManualModeButton.setEnabled(true);
                }

                if (mResetModeButton != null) {
                    mResetModeButton.setEnabled(true);
                }

                break;

            case MANUAL:
                if (mAutoModeButton != null) {
                    mAutoModeButton.setEnabled(true);
                }

                if (mManualModeButton != null) {
                    mManualModeButton.setEnabled(false);
                }

                if (mResetModeButton != null) {
                    mResetModeButton.setEnabled(true);
                }

                break;

            default:
                if (mAutoModeButton != null) {
                    mAutoModeButton.setEnabled(true);
                }

                if (mManualModeButton != null) {
                    mManualModeButton.setEnabled(true);
                }

                if (mResetModeButton != null) {
                    mResetModeButton.setEnabled(true);
                }

                break;
        }
    }
}
