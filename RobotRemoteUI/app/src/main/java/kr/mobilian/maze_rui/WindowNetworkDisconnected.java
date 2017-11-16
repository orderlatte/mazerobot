package kr.mobilian.maze_rui;

import android.content.Context;
import android.os.Handler;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import kr.mobilian.maze_rui.RobotStateReceiveService.INetworkConnectionChangedCallback;

public class WindowNetworkDisconnected extends FrameLayout {
    private static final String TAG = "WindowNetworkDisconnected";
    private static boolean DEBUG = true;

    private static final String CUSTOM_ATTRIBUTE_NAME_SPACE = "http://schemas.mobilian.kr/apk/res/android";
    private static final String CUSTOM_ATTRIBUTE_LAYOUT_ID = "layoutID";

    private static final int INVALID_ID = 0;

    private Context mContext;
    private int mChildLayoutId = INVALID_ID;

    private INetworkConnectionChangedCallback mNetworkConnectionCallback;

    private Handler mVisibilityHandler = new Handler();

    public WindowNetworkDisconnected(Context context) {
        this(context, null);
    }

    public WindowNetworkDisconnected(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public WindowNetworkDisconnected(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public WindowNetworkDisconnected(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr);

        mContext = context;

        if (attrs != null) {
            mChildLayoutId = attrs.getAttributeResourceValue(CUSTOM_ATTRIBUTE_NAME_SPACE, CUSTOM_ATTRIBUTE_LAYOUT_ID, INVALID_ID);
        }

        initLayout();

        setVisibility(View.GONE);
    }

    private void initLayout() {
        if (mChildLayoutId != INVALID_ID) {
            View inflateView = View.inflate(mContext, mChildLayoutId, (ViewGroup) getParent());
            addView(inflateView);
        }
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

        if (mNetworkConnectionCallback == null) {
            mNetworkConnectionCallback = new INetworkConnectionChangedCallback() {
                @Override
                public void onNetworkConnectionChanged(final boolean connected) {
                    mVisibilityHandler.removeCallbacksAndMessages(null);
                    mVisibilityHandler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            setVisibility(connected ? View.GONE : View.VISIBLE);
                        }
                    }, 1000);
                }
            };

            ((MazeApplication) getContext().getApplicationContext()).getRobotStateReceiveService().registerCallback(mNetworkConnectionCallback);
        }
    }

    @Override
    protected void onDetachedFromWindow() {
        if (mNetworkConnectionCallback != null) {
            ((MazeApplication) getContext().getApplicationContext()).getRobotStateReceiveService().unregisterCallback(mNetworkConnectionCallback);
            mNetworkConnectionCallback = null;
        }

        super.onDetachedFromWindow();
    }

    @Override
    protected void onFinishInflate() {
        super.onFinishInflate();

        View closeButton = findViewById(R.id.close_button);
        if (closeButton != null) {
            closeButton.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View view) {
                    WindowNetworkDisconnected.this.setVisibility(View.GONE);
                }
            });
        }
    }
}
