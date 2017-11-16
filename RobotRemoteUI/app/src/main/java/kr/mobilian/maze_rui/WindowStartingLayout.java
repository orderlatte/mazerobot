package kr.mobilian.maze_rui;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

@SuppressLint("HandlerLeak")
public class WindowStartingLayout extends FrameLayout {
    private static final String TAG = "WindowStartingLayout";
    private static boolean DEBUG = true;

    private static final String CUSTOM_ATTRIBUTE_NAME_SPACE = "http://schemas.mobilian.kr/apk/res/android";
    private static final String CUSTOM_ATTRIBUTE_LAYOUT_ID = "layoutID";

    private static final int INVALID_ID = 0;

    private Context mContext;
    private int mChildLayoutId = INVALID_ID;

    private static int mDuration = 4000;
    private static boolean mShowing = true;
    private Handler mHideHandler;

    public WindowStartingLayout(Context context) {
        this(context, null);
    }

    public WindowStartingLayout(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public WindowStartingLayout(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public WindowStartingLayout(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr);

        mContext = context;

        if (attrs != null) {
            mChildLayoutId = attrs.getAttributeResourceValue(CUSTOM_ATTRIBUTE_NAME_SPACE, CUSTOM_ATTRIBUTE_LAYOUT_ID, INVALID_ID);
        }

        if (mShowing) {
            initLayout();
        } else {
            setVisibility(View.GONE);
        }
    }

    private void initLayout() {
        if (mChildLayoutId != INVALID_ID) {
            View inflateView = View.inflate(mContext, mChildLayoutId, (ViewGroup) getParent());
            addView(inflateView);
        }
    }

    @Override
    protected void onAttachedToWindow() {
        if (mShowing) {
            super.onAttachedToWindow();
        }

        if (mHideHandler == null) {
            mHideHandler = new Handler() {
                @Override
                public void handleMessage(Message msg) {
                    super.handleMessage(msg);

                    View colorLogo = findViewById(R.id.team_logo_color);
                    if (colorLogo != null) {
                        colorLogo.setVisibility(mShowing ? View.VISIBLE : View.INVISIBLE);
                        mShowing = !mShowing;
                    }

                    mDuration = mDuration - 100;
                    if (mDuration > 0) {
                        sendEmptyMessageDelayed(0, 100);
                    } else {
                        setVisibility(View.GONE);
                    }
                }
            };
            mHideHandler.sendEmptyMessageDelayed(0, 100);
        }
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
    }
}
