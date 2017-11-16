package kr.mobilian.maze_rui;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.AttributeSet;
import android.view.View;
import android.widget.FrameLayout;

import static kr.mobilian.maze_rui.SettingDebugRobotLayout.ACTION_HIDDEN_SETTING;
import static kr.mobilian.maze_rui.SettingDebugRobotLayout.EXTRA_SHOW_MAZE_SOLVING_HISTORY;

public class MazeSolvingHistoryLayout extends FrameLayout {
    private static final String TAG = "MazeSolvingHistoryLayout";
    private static boolean DEBUG = true;

    private static final String CUSTOM_ATTRIBUTE_NAME_SPACE = "http://schemas.mobilian.kr/apk/res/android";
    private static final String CUSTOM_ATTRIBUTE_LAYOUT_ID = "layoutID";

    private static final int INVALID_ID = 0;

    private BroadcastReceiver mHiddenMenuReceiver;
    private boolean mShowMazeSolvingHistory;

    private Context mContext;
    private int mChildLayoutId = INVALID_ID;

    public MazeSolvingHistoryLayout(Context context) {
        this(context, null);
    }

    public MazeSolvingHistoryLayout(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public MazeSolvingHistoryLayout(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public MazeSolvingHistoryLayout(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr);

        mContext = context;

        if (attrs != null) {
            mChildLayoutId = attrs.getAttributeResourceValue(CUSTOM_ATTRIBUTE_NAME_SPACE, CUSTOM_ATTRIBUTE_LAYOUT_ID, INVALID_ID);
        }

        initLayout();
    }

    private void initLayout() {
        if (mChildLayoutId != INVALID_ID) {
            View inflateView = View.inflate(mContext, mChildLayoutId, null/*(ViewGroup) getParent()*/);
            addView(inflateView);
        }
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

        if (mHiddenMenuReceiver == null) {
            mHiddenMenuReceiver = new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    if (intent != null) {
                        mShowMazeSolvingHistory = intent.getBooleanExtra(EXTRA_SHOW_MAZE_SOLVING_HISTORY, mShowMazeSolvingHistory);
                        setVisibility(mShowMazeSolvingHistory ? View.VISIBLE : View.INVISIBLE);
                    }
                }
            };

            mContext.registerReceiver(mHiddenMenuReceiver, new IntentFilter(ACTION_HIDDEN_SETTING));
        }
    }

    @Override
    protected void onDetachedFromWindow() {
        if (mHiddenMenuReceiver != null) {
            mContext.unregisterReceiver(mHiddenMenuReceiver);
            mHiddenMenuReceiver = null;
        }

        super.onDetachedFromWindow();
    }
}
