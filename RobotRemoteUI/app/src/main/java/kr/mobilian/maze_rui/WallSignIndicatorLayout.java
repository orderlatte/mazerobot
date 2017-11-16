package kr.mobilian.maze_rui;

import android.content.Context;
import android.os.Handler;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.util.List;

import kr.mobilian.maze_rui.RobotStateReceiveService.IWallSignStateChangedCallback;

import static kr.mobilian.maze_rui.WallSignIcon.WALL_SIGN_NONE;

public class WallSignIndicatorLayout extends LinearLayout {
    private static final String TAG = "WallSignIndicatorLayout";
    private static boolean DEBUG = true;

    private static final String CUSTOM_ATTRIBUTE_NAME_SPACE = "http://schemas.mobilian.kr/apk/res/android";
    private static final String CUSTOM_ATTRIBUTE_LAYOUT_ID = "layoutID";
    private static final String CUSTOM_ATTRIBUTE_COLLAPSABLE = "collapsable";
    private static final String CUSTOM_ATTRIBUTE_AUTOHIDE = "autoHide";
    private static final int INVALID_ID = 0;

    private Context mContext;
    private int mChildLayoutId = INVALID_ID;
    private boolean mCollapsable;
    private boolean mAutoHideEnabled;

    private IWallSignStateChangedCallback mWallSignStateChangedCallback;

    public WallSignIndicatorLayout(Context context) {
        this(context, null);
    }

    public WallSignIndicatorLayout(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public WallSignIndicatorLayout(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public WallSignIndicatorLayout(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr);

        mContext = context;

        if (attrs != null) {
            mChildLayoutId = attrs.getAttributeResourceValue(CUSTOM_ATTRIBUTE_NAME_SPACE, CUSTOM_ATTRIBUTE_LAYOUT_ID, INVALID_ID);
            mCollapsable = attrs.getAttributeBooleanValue(CUSTOM_ATTRIBUTE_NAME_SPACE, CUSTOM_ATTRIBUTE_COLLAPSABLE, mCollapsable);
            mAutoHideEnabled = attrs.getAttributeBooleanValue(CUSTOM_ATTRIBUTE_NAME_SPACE, CUSTOM_ATTRIBUTE_AUTOHIDE, mAutoHideEnabled);
        }

        initLayout();
    }

    @Override
    protected void onFinishInflate() {
        super.onFinishInflate();

        final View view = findViewById(R.id.wall_sign_counter);
        final View viewSigns = findViewById(R.id.wall_signs);
        if (mCollapsable
                && view != null
                && viewSigns != null) {
            view.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (viewSigns.getVisibility() == View.VISIBLE) {
                        viewSigns.setVisibility(View.GONE);
                    } else {
                        viewSigns.setVisibility(View.VISIBLE);

                        if (mAutoHideEnabled) {
                            hideWallSigns(5 * 1000);
                        }
                    }
                }
            });
        }
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

        registerWallSignStateReceiver();
    }

    @Override
    protected void onDetachedFromWindow() {
        unregisterWallSignStateReceiver();

        super.onDetachedFromWindow();
    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        super.onLayout(changed, l, t, r, b);
    }

    private void initLayout() {
        if (mChildLayoutId != INVALID_ID) {
            View inflateView = View.inflate(mContext, mChildLayoutId, (ViewGroup) getParent());
            addView(inflateView);
        }
    }

    private void registerWallSignStateReceiver() {
        if (mWallSignStateChangedCallback == null) {
            mWallSignStateChangedCallback = new IWallSignStateChangedCallback() {
                @Override
                public void onWallSignStateChanged(List<MazeCellInformation> wallSignCellList) {
                    updateWallSignState(wallSignCellList);
                }
            };

            ((MazeApplication) getContext().getApplicationContext()).getRobotStateReceiveService().registerCallback(mWallSignStateChangedCallback);
        }
    }

    private void unregisterWallSignStateReceiver() {
        if (mWallSignStateChangedCallback != null) {
            ((MazeApplication) getContext().getApplicationContext()).getRobotStateReceiveService().unregisterCallback(mWallSignStateChangedCallback);
            mWallSignStateChangedCallback = null;
        }
    }

    private void updateWallSignState(List<MazeCellInformation> wallSignCellList) {
        if (wallSignCellList == null) {
            return;
        }

        final View viewSigns = findViewById(R.id.wall_signs);
        if (viewSigns == null
                || !(viewSigns instanceof ViewGroup)) {
            return;
        }

        ((ViewGroup)viewSigns).removeAllViews();

        for (MazeCellInformation wallSignCell : wallSignCellList) {
            if (wallSignCell.getWallSign() == WALL_SIGN_NONE) {
                return;
            }

            WallSignIconView view = new WallSignIconView(mContext);
            view.updateMazeCellInformation(wallSignCell);
            view.setImageResource(wallSignCell.getWallSign().getDrawableId());
            ((ViewGroup)viewSigns).addView(view);
        }

        updateWallSignCounter(((ViewGroup)viewSigns).getChildCount());
    }

    private void updateWallSignCounter(int visibleCount) {
        View view = findViewById(R.id.wall_sign_counter);
        if (view != null
                && view instanceof TextView) {
            ((TextView) view).setText("" + visibleCount);

            if (!mCollapsable) {
                view.setVisibility(visibleCount > 0 ? View.VISIBLE : View.GONE);
            }
        }

        if (mAutoHideEnabled
                && visibleCount > 0) {
            final View viewSigns = findViewById(R.id.wall_signs);
            if (viewSigns != null
                    && mCollapsable) {
                viewSigns.setVisibility(View.VISIBLE);
                hideWallSigns(5 * 1000);
            }
        }
    }

    private void hideWallSigns(int delay) {
        final View viewSigns = findViewById(R.id.wall_signs);
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                if (viewSigns != null
                        && mCollapsable) {
                    viewSigns.setVisibility(View.GONE);
                }
            }
        }, 5 * 1000);
    }
}
