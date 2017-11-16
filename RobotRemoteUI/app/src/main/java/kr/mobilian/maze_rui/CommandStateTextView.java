package kr.mobilian.maze_rui;

import android.content.Context;
import android.support.v7.widget.AppCompatTextView;
import android.util.AttributeSet;
import android.view.View;

import static kr.mobilian.maze_rui.CommanderService.ICommandStateChangedCallback;

public class CommandStateTextView extends AppCompatTextView {
    private static final String TAG = "CommandStateTextView";
    private static boolean DEBUG = true;

    private ICommandStateChangedCallback mCommandStateChangedCallback;

    public CommandStateTextView(Context context) {
        this(context, null);
    }

    public CommandStateTextView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public CommandStateTextView(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public CommandStateTextView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr);

        setVisibility(View.VISIBLE);
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

        if (mCommandStateChangedCallback == null) {
            mCommandStateChangedCallback = new ICommandStateChangedCallback() {
                @Override
                public void onCommandStateChanged(CommandState mode) {
                    setText(mode.toString());
                }
            };
            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().registerCallback(mCommandStateChangedCallback);
        }
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();

        if (mCommandStateChangedCallback != null) {
            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().unregisterCallback(mCommandStateChangedCallback);
            mCommandStateChangedCallback = null;
        }
    }

    @Override
    public void setVisibility(int visibility) {
        super.setVisibility((View.GONE));
        //super.setVisibility(visibility);
    }
}
