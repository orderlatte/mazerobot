package kr.mobilian.maze_rui;

import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.widget.GridLayout;

import java.util.ArrayList;
import java.util.List;

import kr.mobilian.maze_rui.RobotStateReceiveService.IMazeMapStateChangedCallback;

import static kr.mobilian.maze_rui.MazeCellInformationConstant.MAP_MAX_SIZE;

public class MazeMapGridLayout extends GridLayout {
    private static final String TAG = "MazeMapGridLayout";
    private static boolean DEBUG = true;

    private Context mContext;
    private List<MazeCellView> mMazeCellViewList = new ArrayList<MazeCellView>();
    private MazeCellView[][] mMazeCellViewArray;

    private IMazeMapStateChangedCallback mMazeMapStateChangedCallback;

    public MazeMapGridLayout(Context context) {
        this(context, null);
    }

    public MazeMapGridLayout(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public MazeMapGridLayout(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public MazeMapGridLayout(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr);

        mContext = context;

        initLayout();
    }

    private void initLayout() {
        for (int i = 0; i < MAP_MAX_SIZE; i++) {
            for (int j = 0; j < MAP_MAX_SIZE; j++) {
                MazeCellView cellView = (MazeCellView)View.inflate(mContext, R.layout.maze_map, null);
                cellView.setTag(i + "," + j);
                addView(cellView);
                mMazeCellViewList.add(cellView);
            }
        }
    }

    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
        super.onLayout(changed, left, top, right, bottom);

        final int heightMax = (int)(4 * getContext().getResources().getDimension(R.dimen.map_cell_size_x));
        final int width = right - left;
        final int height = bottom - top;

        if (height > width
                && height > heightMax) {
            setRotation(90f);
        } else {
            setRotation(0f);
        }
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

        registerMazeMapChangedReceiver();
    }

    @Override
    protected void onDetachedFromWindow() {
        unregisterMazeMapChangedReceiver();

        super.onDetachedFromWindow();
    }

    private void registerMazeMapChangedReceiver() {
        if (mMazeMapStateChangedCallback == null) {
            mMazeMapStateChangedCallback = new IMazeMapStateChangedCallback() {
                @Override
                public void onMazeMapChanged(List<MazeCellInformation> mazeCellInformationList) {
                    updateMazeMapInformation(mazeCellInformationList);
                }
            };

            ((MazeApplication) getContext().getApplicationContext()).getRobotStateReceiveService().registerCallback(mMazeMapStateChangedCallback);
        }
    }

    private void unregisterMazeMapChangedReceiver() {
        if (mMazeMapStateChangedCallback != null) {
            ((MazeApplication) getContext().getApplicationContext()).getRobotStateReceiveService().unregisterCallback(mMazeMapStateChangedCallback);
            mMazeMapStateChangedCallback = null;
        }
    }

    private void updateMazeMapInformation(List<MazeCellInformation> mazeCellInformationList) {
        mMazeCellViewArray = null;
        mMazeCellViewArray = new MazeCellView[MAP_MAX_SIZE][MAP_MAX_SIZE];
        for (int i = 0; i < MAP_MAX_SIZE; i++) {
            for (int j = 0; j < MAP_MAX_SIZE; j++) {
                int index = i * MAP_MAX_SIZE + j;
                mMazeCellViewList.get(index).updateMazeCellState(mazeCellInformationList.get(index));

                mMazeCellViewArray[i][j] = mMazeCellViewList.get(index);

                if (i == MAP_MAX_SIZE / 2
                        && j == MAP_MAX_SIZE / 2) {
                    mMazeCellViewArray[i][j].setAsCenterView(true);
                } else {
                    mMazeCellViewArray[i][j].setAsCenterView(false);
                }
            }
        }

        for (int i = 0; i < MAP_MAX_SIZE; i++) {
            boolean isAllInvisible = true;

            for (int j = 0; j < MAP_MAX_SIZE; j++) {
                if (mMazeCellViewArray[i][j].getVisibility() == View.VISIBLE) {
                    isAllInvisible = false;
                    break;
                }
            }

            if (isAllInvisible) {
                for (int j = 0; j < MAP_MAX_SIZE; j++) {
                    mMazeCellViewArray[i][j].setVisibility(View.GONE);
                }
            }
        }

        for (int j = 0; j < MAP_MAX_SIZE; j++) {
            boolean isAllInvisible = true;

            for (int i = 0; i < MAP_MAX_SIZE; i++) {
                if (mMazeCellViewArray[i][j].getVisibility() == View.VISIBLE) {
                    isAllInvisible = false;
                    break;
                }
            }

            if (isAllInvisible) {
                for (int i = 0; i < MAP_MAX_SIZE; i++) {
                    mMazeCellViewArray[i][j].setVisibility(View.GONE);
                }
            }
        }
    }
}
