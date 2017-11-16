package kr.mobilian.maze_rui;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Typeface;
import android.os.Handler;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.FrameLayout;
import android.widget.TextView;

public class SettingRobotLayout extends FrameLayout implements CompoundButton.OnCheckedChangeListener {
    private static final String TAG = "SettingRobotLayout";
    private static boolean DEBUG = true;

    private static final String CUSTOM_ATTRIBUTE_NAME_SPACE = "http://schemas.mobilian.kr/apk/res/android";
    private static final String CUSTOM_ATTRIBUTE_LAYOUT_ID = "layoutID";

    private static final int INVALID_ID = 0;

    public static final String ACTION_ROBOT_SETTING = "kr.mobilian.maze_ui.ACTION_ROBOT_SETTING";
    public static final String EXTRA_MAZE_ALGORITHM_001 = "MAZE_ALGORITHM_001";
    public static final String EXTRA_MAZE_ALGORITHM_002 = "MAZE_ALGORITHM_002";
    public static final String EXTRA_MAZE_ALGORITHM_003 = "MAZE_ALGORITHM_003";
    public static final String EXTRA_MAZE_ALGORITHM_004 = "MAZE_ALGORITHM_004";
    public static final String EXTRA_MAZE_ALGORITHM_005 = "MAZE_ALGORITHM_005";
    public static final String EXTRA_MOVING_PATH = "MOVING_PATH";

    private Context mContext;
    private int mChildLayoutId = INVALID_ID;
    private Typeface mTypeFace;

    private CheckBox mAlgorithm001;
    private CheckBox mAlgorithm002;
    private CheckBox mAlgorithm003;
    private CheckBox mAlgorithm004;
    private CheckBox mAlgorithm005;

    public SettingRobotLayout(Context context) {
        this(context, null);
    }

    public SettingRobotLayout(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public SettingRobotLayout(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public SettingRobotLayout(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr);

        mContext = context;

        if (attrs != null) {
            mChildLayoutId = attrs.getAttributeResourceValue(CUSTOM_ATTRIBUTE_NAME_SPACE, CUSTOM_ATTRIBUTE_LAYOUT_ID, INVALID_ID);
        }

        mTypeFace = Typeface.createFromAsset(getContext().getAssets(), "fonts/handwriting.ttf");

        initLayout();
    }

    private void initLayout() {
        if (mChildLayoutId != INVALID_ID) {
            View inflateView = View.inflate(mContext, mChildLayoutId, (ViewGroup) getParent());
            addView(inflateView);

            applyFont(inflateView);
        }
    }

    private void applyFont(View view) {
        if (mTypeFace != null
                && view != null) {
            if (view instanceof ViewGroup) {
                ViewGroup viewGroup = (ViewGroup) view;
                for (int i = 0; i < viewGroup.getChildCount(); i++) {
                    applyFont(viewGroup.getChildAt(i));
                }
            } else if (view instanceof TextView) {
                ((TextView) view).setTypeface(mTypeFace);
            }

            if (view instanceof CheckBox) {
                ((CheckBox)view).setButtonDrawable(R.drawable.checkbox);
            }
        }
    }

    @Override
    protected void onFinishInflate() {
        super.onFinishInflate();

        mAlgorithm001 = findViewById(R.id.maze_solving_algorithm_001);
        if (mAlgorithm001 != null) {
            mAlgorithm001.setOnCheckedChangeListener(this);
            mAlgorithm001.setChecked(getStickyIntent(EXTRA_MAZE_ALGORITHM_001, mAlgorithm001.isChecked()));
        }

        mAlgorithm002 = findViewById(R.id.maze_solving_algorithm_002);
        if (mAlgorithm002 != null) {
            mAlgorithm002.setOnCheckedChangeListener(this);
            mAlgorithm002.setChecked(getStickyIntent(EXTRA_MAZE_ALGORITHM_002, mAlgorithm002.isChecked()));
        }

        mAlgorithm003 = findViewById(R.id.maze_solving_algorithm_003);
        if (mAlgorithm003 != null) {
            mAlgorithm003.setOnCheckedChangeListener(this);
            mAlgorithm003.setChecked(getStickyIntent(EXTRA_MAZE_ALGORITHM_003, mAlgorithm003.isChecked()));
        }

        mAlgorithm004 = findViewById(R.id.maze_solving_algorithm_004);
        if (mAlgorithm004 != null) {
            mAlgorithm004.setOnCheckedChangeListener(this);
            mAlgorithm004.setChecked(getStickyIntent(EXTRA_MAZE_ALGORITHM_004, mAlgorithm004.isChecked()));
        }

        mAlgorithm005 = findViewById(R.id.maze_solving_algorithm_005);
        if (mAlgorithm005 != null) {
            mAlgorithm005.setOnCheckedChangeListener(this);
            mAlgorithm005.setChecked(getStickyIntent(EXTRA_MAZE_ALGORITHM_005, mAlgorithm005.isChecked()));
        }

        CheckBox checkBox = findViewById(R.id.moving_path);
        if (checkBox != null) {
            checkBox.setOnCheckedChangeListener(this);
            checkBox.setChecked(getStickyIntent(EXTRA_MOVING_PATH, checkBox.isChecked()));
        }

        TextView back = findViewById(R.id.back_button);
        if (back != null) {
            back.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View view) {
                    SettingRobotLayout.this.setVisibility(View.GONE);
                }
            });
        }

        TextView close = findViewById(R.id.apply_button);
        if (close != null) {
            close.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View view) {
                    ((MazeApplication) getContext().getApplicationContext()).getCommanderService().sendResetCommand();

                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            SettingRobotLayout.this.setVisibility(View.GONE);
                        }
                    }, 500);
                }
            });
        }
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        switch (buttonView.getId()) {
            case R.id.maze_solving_algorithm_001:
                if (isChecked) {
                    if (mAlgorithm002 != null) {
                        mAlgorithm002.setChecked(false);
                    }

                    if (mAlgorithm003 != null) {
                        mAlgorithm003.setChecked(false);
                    }

                    if (mAlgorithm004 != null) {
                        mAlgorithm004.setChecked(false);
                    }

                    if (mAlgorithm005 != null) {
                        mAlgorithm005.setChecked(false);
                    }
                }
                sendStickyIntent(EXTRA_MAZE_ALGORITHM_001, isChecked);

                break;

            case R.id.maze_solving_algorithm_002:
                if (isChecked) {
                    if (mAlgorithm001 != null) {
                        mAlgorithm001.setChecked(false);
                    }

                    if (mAlgorithm003 != null) {
                        mAlgorithm003.setChecked(false);
                    }

                    if (mAlgorithm004 != null) {
                        mAlgorithm004.setChecked(false);
                    }

                    if (mAlgorithm005 != null) {
                        mAlgorithm005.setChecked(false);
                    }
                }
                sendStickyIntent(EXTRA_MAZE_ALGORITHM_002, isChecked);

                break;

            case R.id.maze_solving_algorithm_003:
                if (isChecked) {
                    if (mAlgorithm001 != null) {
                        mAlgorithm001.setChecked(false);
                    }

                    if (mAlgorithm002 != null) {
                        mAlgorithm002.setChecked(false);
                    }

                    if (mAlgorithm004 != null) {
                        mAlgorithm004.setChecked(false);
                    }

                    if (mAlgorithm005 != null) {
                        mAlgorithm005.setChecked(false);
                    }
                }
                sendStickyIntent(EXTRA_MAZE_ALGORITHM_003, isChecked);

                break;

            case R.id.maze_solving_algorithm_004:
                if (isChecked) {
                    if (mAlgorithm001 != null) {
                        mAlgorithm001.setChecked(false);
                    }

                    if (mAlgorithm002 != null) {
                        mAlgorithm002.setChecked(false);
                    }

                    if (mAlgorithm003 != null) {
                        mAlgorithm003.setChecked(false);
                    }

                    if (mAlgorithm005 != null) {
                        mAlgorithm005.setChecked(false);
                    }
                }
                sendStickyIntent(EXTRA_MAZE_ALGORITHM_004, isChecked);

                    break;

            case R.id.maze_solving_algorithm_005:
                if (isChecked) {
                    if (mAlgorithm001 != null) {
                        mAlgorithm001.setChecked(false);
                    }

                    if (mAlgorithm002 != null) {
                        mAlgorithm002.setChecked(false);
                    }

                    if (mAlgorithm003 != null) {
                        mAlgorithm003.setChecked(false);
                    }

                    if (mAlgorithm004 != null) {
                        mAlgorithm004.setChecked(false);
                    }
                }
                sendStickyIntent(EXTRA_MAZE_ALGORITHM_005, isChecked);

                    break;

            case R.id.moving_path:
                sendStickyIntent(EXTRA_MOVING_PATH, isChecked);
                break;
        }

        Log.d(DEBUG, TAG, "Algorithm001: " + getStickyIntent(EXTRA_MAZE_ALGORITHM_001, false)
                + ", Algorithm002: " + getStickyIntent(EXTRA_MAZE_ALGORITHM_002, false)
                + ", Algorithm003: " + getStickyIntent(EXTRA_MAZE_ALGORITHM_003, false)
                + ", Algorithm004: " + getStickyIntent(EXTRA_MAZE_ALGORITHM_004, false)
                + ", Algorithm005: " + getStickyIntent(EXTRA_MAZE_ALGORITHM_005, false));
    }

    private void sendStickyIntent(String extraName, boolean extraValue) {
        Intent intent = mContext.registerReceiver(null, new IntentFilter(ACTION_ROBOT_SETTING));
        if (intent != null) {
            intent.putExtra(extraName, extraValue);
            mContext.sendStickyBroadcast(intent);
        }
    }

    private boolean getStickyIntent(String extraName, boolean defaultValue) {
        Intent intent = mContext.registerReceiver(null, new IntentFilter(ACTION_ROBOT_SETTING));
        if (intent != null) {
            return intent.getBooleanExtra(extraName, defaultValue);
        }

        return defaultValue;
    }
}
