package kr.mobilian.maze_rui;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.AttributeSet;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;

import static kr.mobilian.maze_rui.CommanderService.ICommandStateChangedCallback;

public class WindowCongraturationLayout extends FrameLayout {
    private static final String TAG = "WindowCongraturationLayout";
    private static boolean DEBUG = true;

    public static final String CONGRATURATION = "CONGRATURATION";

    private static final String CUSTOM_ATTRIBUTE_NAME_SPACE = "http://schemas.mobilian.kr/apk/res/android";
    private static final String CUSTOM_ATTRIBUTE_LAYOUT_ID = "layoutID";

    private static final int INVALID_ID = 0;

    private ICommandStateChangedCallback mCommandStateChangedCallback;

    private Context mContext;
    private int mChildLayoutId = INVALID_ID;
    private int mClickCount;

    private BroadcastReceiver mCongraturationReceiver;

    public WindowCongraturationLayout(Context context) {
        this(context, null);
    }

    public WindowCongraturationLayout(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public WindowCongraturationLayout(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public WindowCongraturationLayout(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
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
    protected void onFinishInflate() {
        super.onFinishInflate();

        final ImageView congraturation = findViewById(R.id.congraturation);
        if (congraturation != null) {
            congraturation.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    mClickCount++;

                    if (mClickCount >= 5) {
                        mClickCount = 0;

                        setVisibility(View.GONE);
                    }
                }
            });
        }
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

        if (mCommandStateChangedCallback == null) {
            mCommandStateChangedCallback = new ICommandStateChangedCallback() {
                @Override
                public void onCommandStateChanged(CommandState mode) {
                    if (mode == CommandState.SUCCESS) {
                        setVisibility(View.VISIBLE);
                    }
                }
            };
            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().registerCallback(mCommandStateChangedCallback);
        }


        if (mCongraturationReceiver == null) {
            mCongraturationReceiver = new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    if (CONGRATURATION.equals(intent.getAction())) {
                        setVisibility(VISIBLE);
                    }
                }
            };

            mContext.registerReceiver(mCongraturationReceiver, new IntentFilter(CONGRATURATION));
        }
    }

    @Override
    protected void onDetachedFromWindow() {
        if (mCongraturationReceiver != null) {
            mContext.unregisterReceiver(mCongraturationReceiver);
        }

        if (mCommandStateChangedCallback != null) {
            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().unregisterCallback(mCommandStateChangedCallback);
            mCommandStateChangedCallback = null;
        }

        super.onDetachedFromWindow();
    }
}
