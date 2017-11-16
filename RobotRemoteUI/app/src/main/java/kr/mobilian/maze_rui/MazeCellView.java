package kr.mobilian.maze_rui;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.support.v7.widget.AppCompatImageView;
import android.util.AttributeSet;
import android.view.View;

import static kr.mobilian.maze_rui.SettingDebugRobotLayout.ACTION_HIDDEN_SETTING;
import static kr.mobilian.maze_rui.SettingDebugRobotLayout.EXTRA_SHOW_MAZE_CELL_INDEX;

public class MazeCellView extends AppCompatImageView {
    private static final String TAG = "MazeCellView";
    private static boolean DEBUG = true;

    private static int WALL_THIKNESS = 3;
    private static int RED_DOT_THIKNESS = 10;

    private Context mContext;

    private BroadcastReceiver mHiddenMenuReceiver;
    private boolean mShowMazeCellIndex;

    private MazeCellInformation mMazeCellInformation;

    private Paint mRedDotPaint;
    private Rect mSignDrawingRect;

    private Drawable mRobotDrawable;
    private Rect mRobotDrawingRect;

    private Rect mWallSize;
    private Rect mSignWallSize;
    private Paint mCheckedWallPaint;
    private Paint mUnCheckWallPaint;

    private boolean mIsCenterView;

    public MazeCellView(Context context) {
        this(context, null);
    }

    public MazeCellView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public MazeCellView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        mContext = context;

        mRedDotPaint = new Paint();
        mRedDotPaint.setColor(Color.RED);
        mRedDotPaint.setTextSize(30);

        mRobotDrawable = getResources().getDrawable(R.drawable.robot_car);

        mCheckedWallPaint = new Paint();
        mCheckedWallPaint.setColor(Color.WHITE);

        mUnCheckWallPaint = new Paint();
        mUnCheckWallPaint.setColor(Color.BLUE);
    }

    public void updateMazeCellState(MazeCellInformation mazeCellInformation) {
        mMazeCellInformation = mazeCellInformation;
        //TODO if diff, invlidate...

        if (mMazeCellInformation != null
                && mMazeCellInformation.isDiscovered()) {
            setVisibility(View.VISIBLE);
        } else {
            setVisibility(View.INVISIBLE);
        }

        invalidate();
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

        if (mHiddenMenuReceiver == null) {
            mHiddenMenuReceiver = new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    if (intent != null) {
                        mShowMazeCellIndex = intent.getBooleanExtra(EXTRA_SHOW_MAZE_CELL_INDEX, mShowMazeCellIndex);
                        invalidate();
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

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        final int sizeX = (int)getResources().getDimension(R.dimen.map_cell_size_x);
        final int sizeY = (int)getResources().getDimension(R.dimen.map_cell_size_y);
        mWallSize = new Rect(0, 0, sizeX, WALL_THIKNESS);
        mSignWallSize = new Rect(WALL_THIKNESS, WALL_THIKNESS, sizeX - WALL_THIKNESS, WALL_THIKNESS * 5);

        final float signScale = 0.55f;
        final int signLeft = (int)(sizeX * (1 - signScale) / 2);
        final int signTop = 0;
        final int signRight = (int)(signLeft + sizeX * signScale);
        final int signBottom = (int)(signTop + sizeY * signScale);
        mSignDrawingRect = new Rect(signLeft, signTop, signRight, (int)(signBottom * 0.8));

        final float robotScale = 0.8f;
        final int robotLeft = (int)(sizeX * (1 - robotScale) / 2);
        final int robotTop = (int)(sizeY * (1 - robotScale) / 2);
        final int robotRight = (int)(robotLeft + sizeX * robotScale);
        final int robotBottom = (int)(robotTop + sizeY * robotScale);
        mRobotDrawingRect = new Rect(robotLeft, robotTop, robotRight, robotBottom);

        setMeasuredDimension(sizeX, sizeY);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        drawFloor(canvas);
        drawWall(canvas);
        drawRedDot(canvas);
        drawRobot(canvas);
        drawSign(canvas);

        super.onDraw(canvas);

        if (mShowMazeCellIndex) {
            canvas.save();
            String test = (String) getTag();
            canvas.drawText(test, 30, 30, mRedDotPaint);
            canvas.restore();
        }
    }

    private void drawFloor(Canvas canvas) {
        if (mMazeCellInformation == null
                || !mMazeCellInformation.isDiscovered()) {
            return;
        }

        if (mMazeCellInformation.isCellNameStart()) {
            canvas.save();
            canvas.drawARGB(255, 0, 220, 0);
            canvas.restore();
        }

        if (mMazeCellInformation.isCellNameEnd()) {
            canvas.save();
            canvas.drawARGB(255, 0, 0, 180);
            canvas.restore();
        }

        if (mIsCenterView) {
            final Drawable compassDrawable = mContext.getResources().getDrawable(R.drawable.wall_floor_compass);

            canvas.save();
            compassDrawable.setBounds(0, 0, canvas.getWidth(), canvas.getHeight());
            compassDrawable.draw(canvas);
            canvas.restore();
        }
    }

    private void drawWall(Canvas canvas) {
        if (mMazeCellInformation == null
                || !mMazeCellInformation.isDiscovered()) {
            return;
        }

        if (!mMazeCellInformation.isWallCheckedInEast()) {
            canvas.save();
            canvas.rotate(90f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            canvas.drawRect(mWallSize, mUnCheckWallPaint);
            canvas.restore();
        }

        if (!mMazeCellInformation.isWallCheckedInWest()) {
            canvas.save();
            canvas.rotate(270f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            canvas.drawRect(mWallSize, mUnCheckWallPaint);
            canvas.restore();
        }

        if (!mMazeCellInformation.isWallCheckedInSouth()) {
            canvas.save();
            canvas.rotate(180f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            canvas.drawRect(mWallSize, mUnCheckWallPaint);
            canvas.restore();
        }

        if (!mMazeCellInformation.isWallCheckedInNorth()) {
            canvas.save();
            canvas.rotate(0f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            canvas.drawRect(mWallSize, mUnCheckWallPaint);
            canvas.restore();
        }

        if (mMazeCellInformation.isWallExistInEast()) {
            canvas.save();
            canvas.rotate(90f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            canvas.drawRect(mWallSize, mCheckedWallPaint);
            canvas.restore();
        }

        if (mMazeCellInformation.isWallExistInWest()) {
            canvas.save();
            canvas.rotate(270f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            canvas.drawRect(mWallSize, mCheckedWallPaint);
            canvas.restore();
        }

        if (mMazeCellInformation.isWallExistInSouth()) {
            canvas.save();
            canvas.rotate(180f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            canvas.drawRect(mWallSize, mCheckedWallPaint);
            canvas.restore();
        }

        if (mMazeCellInformation.isWallExistInNorth()) {
            canvas.save();
            canvas.rotate(0f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            canvas.drawRect(mWallSize, mCheckedWallPaint);
            canvas.restore();
        }
    }

    private void drawRedDot(Canvas canvas) {
        if (mMazeCellInformation == null
                || !mMazeCellInformation.hasRedDot()) {
            return;
        }

        canvas.save();
        canvas.drawCircle(canvas.getWidth() / 2, canvas.getHeight() / 2, RED_DOT_THIKNESS, mRedDotPaint);
        canvas.restore();
    }

    private void drawSign(Canvas canvas) {
        if (mMazeCellInformation == null
                || !mMazeCellInformation.hasRedDot()) {
            return;
        }

        if (mMazeCellInformation.isSignExistOnEast()) {
            canvas.save();
            canvas.rotate(90f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            canvas.drawRect(mSignWallSize, mRedDotPaint);
            canvas.restore();
        }

        if (mMazeCellInformation.isSignExistOnWest()) {
            canvas.save();
            canvas.rotate(270f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            canvas.drawRect(mSignWallSize, mRedDotPaint);
            canvas.restore();
        }

        if (mMazeCellInformation.isSignExistOnSouth()) {
            canvas.save();
            canvas.rotate(180f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            canvas.drawRect(mSignWallSize, mRedDotPaint);
            canvas.restore();
        }

        if (mMazeCellInformation.isSignExistOnNorth()) {
            canvas.save();
            canvas.rotate(0f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            canvas.drawRect(mSignWallSize, mRedDotPaint);
            canvas.restore();
        }

        final WallSignIcon wallSignIcon = mMazeCellInformation.getWallSign();
        final Drawable signIconDrawable = mContext.getResources().getDrawable(wallSignIcon.getDrawableId());
        if (wallSignIcon == null
                || signIconDrawable == null) {
            return;
        }

        if (mMazeCellInformation.isSignExistOnEast()) {
            canvas.save();
            canvas.rotate(90f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            signIconDrawable.setBounds(mSignDrawingRect);
            signIconDrawable.draw(canvas);
            canvas.restore();
        }

        if (mMazeCellInformation.isSignExistOnWest()) {
            canvas.save();
            canvas.rotate(270f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            signIconDrawable.setBounds(mSignDrawingRect);
            signIconDrawable.draw(canvas);
            canvas.restore();
        }

        if (mMazeCellInformation.isSignExistOnSouth()) {
            canvas.save();
            canvas.rotate(180f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            signIconDrawable.setBounds(mSignDrawingRect);
            signIconDrawable.draw(canvas);
            canvas.restore();
        }

        if (mMazeCellInformation.isSignExistOnNorth()) {
            canvas.save();
            canvas.rotate(0f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            signIconDrawable.setBounds(mSignDrawingRect);
            signIconDrawable.draw(canvas);
            canvas.restore();
        }
    }

    private void drawRobot(Canvas canvas) {
        if (mMazeCellInformation == null
                || mRobotDrawable == null
                || !mMazeCellInformation.isDiscovered()) {
            return;
        }

        canvas.save();

        boolean needDraw = false;
        if (mMazeCellInformation.robotIsLookingToEast()) {
            canvas.rotate(90f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            needDraw = true;
        } else if (mMazeCellInformation.robotIsLookingToWest()) {
            canvas.rotate(270f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            needDraw = true;
        } else if (mMazeCellInformation.robotIsLookingToSouth()) {
            canvas.rotate(180f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            needDraw = true;
        } else if (mMazeCellInformation.robotIsLookingToNorth()) {
            canvas.rotate(0f, canvas.getWidth() / 2, canvas.getHeight() / 2);
            needDraw = true;
        }

        if (needDraw) {
            mRobotDrawable.setBounds(mRobotDrawingRect);
            mRobotDrawable.draw(canvas);
        }

        canvas.restore();
    }

    public void setAsCenterView(boolean isCenterView) {
        mIsCenterView = isCenterView;
    }
}
