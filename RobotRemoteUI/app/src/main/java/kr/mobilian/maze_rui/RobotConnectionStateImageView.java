package kr.mobilian.maze_rui;

import android.content.Context;
import android.content.Intent;
import android.support.annotation.Nullable;
import android.support.v7.widget.AppCompatImageView;
import android.util.AttributeSet;
import android.view.View;

import kr.mobilian.maze_rui.CommanderService.IConectionStateChangedCallback;

import static kr.mobilian.maze_rui.WindowCongraturationLayout.CONGRATURATION;

public class RobotConnectionStateImageView extends AppCompatImageView {
    private static final String TAG = "RobotConnectionStateImageView";
    private static boolean DEBUG = true;

    private Context mContext;
    private IConectionStateChangedCallback mConectionStateChangedCallback;

    public RobotConnectionStateImageView(Context context) {
        this(context, null);
    }

    public RobotConnectionStateImageView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public RobotConnectionStateImageView(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public RobotConnectionStateImageView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr);

        mContext = context;
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

        if (mConectionStateChangedCallback == null) {
            mConectionStateChangedCallback = new IConectionStateChangedCallback() {
                @Override
                public void onConnectionStateChanged(RobotConnectionState state) {
                    switch (state) {
                        case STATE_DISCONNECTED:
                            setImageResource(R.drawable.robot_disconnected);
                            break;

                        case STATE_CONNECTING:
                            setImageResource(R.drawable.robot_connecting);
                            break;

                        case STATE_CONNECTED:
                            setImageResource(R.drawable.robot_connected);
                            //setImageDrawable(null);
                            break;
                    }
                }
            };
            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().registerCallback(mConectionStateChangedCallback);
        }

        setOnLongClickListener(new OnLongClickListener() {
            @Override
            public boolean onLongClick(View view) {
                Intent intent = new Intent(CONGRATURATION);
                if (intent != null) {
                    mContext.sendBroadcast(intent);
                }

                return false;
            }
        });
    }

    @Override
    protected void onDetachedFromWindow() {
        if (mConectionStateChangedCallback != null) {
            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().unregisterCallback(mConectionStateChangedCallback);
            mConectionStateChangedCallback = null;
        }

        super.onDetachedFromWindow();
    }
}
