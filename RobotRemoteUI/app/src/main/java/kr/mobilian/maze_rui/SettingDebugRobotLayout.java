package kr.mobilian.maze_rui;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.FrameLayout;
import android.widget.TextView;

public class SettingDebugRobotLayout extends FrameLayout implements CompoundButton.OnCheckedChangeListener {
    private static final String TAG = "SettingDebugRobotLayout";
    private static boolean DEBUG = true;

    private static final String CUSTOM_ATTRIBUTE_NAME_SPACE = "http://schemas.mobilian.kr/apk/res/android";
    private static final String CUSTOM_ATTRIBUTE_LAYOUT_ID = "layoutID";

    private static final int INVALID_ID = 0;

    public static final String ACTION_HIDDEN_SETTING = "kr.mobilian.maze_ui.ACTION_HIDDEN_SETTING";
    public static final String EXTRA_SHOW_MAZE_CELL_INDEX = "SHOW_MAZE_CELL_INDEX";
    public static final String EXTRA_SHOW_MAZE_NEXT_CELL = "SHOW_MAZE_NEXT_CELL";
    public static final String EXTRA_SHOW_MAZE_SOLVING_HISTORY = "SHOW_MAZE_SOLVING_HISTORY";
    public static final String EXTRA_HIDE_MAP_BACKGROUND = "HIDE_MAP_BACKGROUND";

    private Context mContext;
    private int mChildLayoutId = INVALID_ID;
    private Typeface mTypeFace;

    public SettingDebugRobotLayout(Context context) {
        this(context, null);
    }

    public SettingDebugRobotLayout(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public SettingDebugRobotLayout(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public SettingDebugRobotLayout(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
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

        CheckBox checkBox = findViewById(R.id.show_maze_cell_index);
        if (checkBox != null) {
            checkBox.setOnCheckedChangeListener(this);
            checkBox.setChecked(getStickyIntent(EXTRA_SHOW_MAZE_CELL_INDEX, checkBox.isChecked()));
        }

        checkBox = findViewById(R.id.show_maze_next_cell);
        if (checkBox != null) {
            checkBox.setOnCheckedChangeListener(this);
            checkBox.setChecked(getStickyIntent(EXTRA_SHOW_MAZE_NEXT_CELL, checkBox.isChecked()));
        }

        checkBox = findViewById(R.id.show_maze_solving_history);
        if (checkBox != null) {
            checkBox.setOnCheckedChangeListener(this);
            checkBox.setChecked(getStickyIntent(EXTRA_SHOW_MAZE_SOLVING_HISTORY, checkBox.isChecked()));
        }

        checkBox = findViewById(R.id.hide_map_background);
        if (checkBox != null) {
            checkBox.setOnCheckedChangeListener(this);
            checkBox.setChecked(getStickyIntent(EXTRA_HIDE_MAP_BACKGROUND, checkBox.isChecked()));
        }

        TextView close = findViewById(R.id.apply_button);
        if (close != null) {
            close.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View view) {
                    SettingDebugRobotLayout.this.setVisibility(View.GONE);
                }
            });
        }
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        switch (buttonView.getId()) {
            case R.id.show_maze_cell_index:
                sendStickyIntent(EXTRA_SHOW_MAZE_CELL_INDEX, isChecked);
                break;
            case R.id.show_maze_next_cell:
                sendStickyIntent(EXTRA_SHOW_MAZE_NEXT_CELL, isChecked);
                break;
            case R.id.show_maze_solving_history:
                sendStickyIntent(EXTRA_SHOW_MAZE_SOLVING_HISTORY, isChecked);
                break;
            case R.id.hide_map_background:
                sendStickyIntent(EXTRA_HIDE_MAP_BACKGROUND, isChecked);
                break;
        }
    }

    private void sendStickyIntent(String extraName, boolean extraValue) {
        Intent intent = mContext.registerReceiver(null, new IntentFilter(ACTION_HIDDEN_SETTING));
        if (intent != null) {
            intent.removeExtra(extraName);
            intent.putExtra(extraName, extraValue);
            mContext.sendStickyBroadcast(intent);
        }
    }

    private boolean getStickyIntent(String extraName, boolean defaultValue) {
        Intent intent = mContext.registerReceiver(null, new IntentFilter(ACTION_HIDDEN_SETTING));
        if (intent != null) {
            return intent.getBooleanExtra(extraName, defaultValue);
        }

        return defaultValue;
    }
}
