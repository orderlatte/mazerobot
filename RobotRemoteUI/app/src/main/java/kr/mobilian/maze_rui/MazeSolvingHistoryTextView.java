package kr.mobilian.maze_rui;

import android.content.Context;
import android.support.v7.widget.AppCompatTextView;
import android.util.AttributeSet;

import static kr.mobilian.maze_rui.CommanderService.NEXT_CELL_INIT;

public class MazeSolvingHistoryTextView extends AppCompatTextView {
    private static final String TAG = "MazeSolvingHistoryTextView";
    private static boolean DEBUG = true;

    private CommanderService.INextCellChangedCallback mNextCellChangedCallback;

    private Context mContext;
    private int mLastX = NEXT_CELL_INIT;
    private int mLastY = NEXT_CELL_INIT;

    public MazeSolvingHistoryTextView(Context context) {
        this(context, null);
    }

    public MazeSolvingHistoryTextView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public MazeSolvingHistoryTextView(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public MazeSolvingHistoryTextView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr);

        mContext = context;

    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

        if (mNextCellChangedCallback == null) {
            mNextCellChangedCallback = new CommanderService.INextCellChangedCallback() {
                @Override
                public void onNextCellChanged(int nextX, int nextY) {
                    if (nextX < 0
                            || nextY < 0) {
                        setText("");
                        return;
                    }

                    if (mLastX == nextX
                            && mLastY == nextY) {
                        return;
                    }

                    mLastX = nextX;
                    mLastY = nextY;

                    String oldString = getText().toString() + "[" + nextY + "," + nextX + "]\n"; //CAUSION!!! convert X <--> Y
                    setText(oldString);
                }
            };
            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().registerCallback(mNextCellChangedCallback);
        }
    }

    @Override
    protected void onDetachedFromWindow() {
        if (mNextCellChangedCallback != null) {
            ((MazeApplication) getContext().getApplicationContext()).getCommanderService().unregisterCallback(mNextCellChangedCallback);
            mNextCellChangedCallback = null;
        }

        super.onDetachedFromWindow();
    }
}
