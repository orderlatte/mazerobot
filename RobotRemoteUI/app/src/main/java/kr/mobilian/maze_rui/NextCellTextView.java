package kr.mobilian.maze_rui;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.support.v7.widget.AppCompatTextView;
import android.util.AttributeSet;
import android.view.View;

import kr.mobilian.maze_rui.CommanderService.INextCellChangedCallback;

import static kr.mobilian.maze_rui.SettingDebugRobotLayout.ACTION_HIDDEN_SETTING;
import static kr.mobilian.maze_rui.SettingDebugRobotLayout.EXTRA_SHOW_MAZE_NEXT_CELL;

public class NextCellTextView extends AppCompatTextView {
    private static final String TAG = "NextCellTextView";
    private static boolean DEBUG = true;

    private BroadcastReceiver mHiddenMenuReceiver;
    private INextCellChangedCallback mNextCellChangedCallback;

    private Context mContext;
    private boolean mShowNextCell;

    public NextCellTextView(Context context) {
        this(context, null);
    }

    public NextCellTextView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public NextCellTextView(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public NextCellTextView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr);

        mContext = context;
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

        if (mHiddenMenuReceiver == null) {
            mHiddenMenuReceiver = new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    if (intent != null) {
                        mShowNextCell = intent.getBooleanExtra(EXTRA_SHOW_MAZE_NEXT_CELL, mShowNextCell);
                        setVisibility(mShowNextCell ? View.VISIBLE : View.INVISIBLE);
                    }
                }
            };

            mContext.registerReceiver(mHiddenMenuReceiver, new IntentFilter(ACTION_HIDDEN_SETTING));
        }

        if (mNextCellChangedCallback == null) {
            mNextCellChangedCallback = new INextCellChangedCallback() {
                @Override
                public void onNextCellChanged(int nextX, int nextY) {
                    if (nextX < 0
                            || nextY < 0) {
                        setText("");
                        return;
                    }

                    //setText(nextX + "," + nextY); //CAUSION!!! convert X <--> Y
                    setText(nextY + "," + nextX);
                }
            };
            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().registerCallback(mNextCellChangedCallback);
        }
    }

    @Override
    protected void onDetachedFromWindow() {
        if (mHiddenMenuReceiver != null) {
            mContext.unregisterReceiver(mHiddenMenuReceiver);
            mHiddenMenuReceiver = null;
        }

        if (mNextCellChangedCallback != null) {
            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().unregisterCallback(mNextCellChangedCallback);
            mNextCellChangedCallback = null;
        }

        super.onDetachedFromWindow();
    }

    @Override
    public void setVisibility(int visibility) {
        visibility = mShowNextCell ? visibility : View.INVISIBLE;

        super.setVisibility(visibility);
    }
}
