package kr.mobilian.maze_rui;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.support.v7.widget.AppCompatImageView;
import android.util.AttributeSet;

public class WallSignIconView extends AppCompatImageView {
    private static final String TAG = "WallSignIconView";
    private static boolean DEBUG = true;

    private static int WALL_THIKNESS = 5;

    private static final String CUSTOM_ATTRIBUTE_NAME_SPACE = "http://schemas.mobilian.kr/apk/res/android";
    private static final String CUSTOM_ATTRIBUTE_LAYOUT_ID = "drawWall";

    private MazeCellInformation mMazeCellInformation;

    private boolean mDrawWall = true;
    private Paint mWallPaint;
    private Rect mWallSize;

    public WallSignIconView(Context context) {
        this(context, null);
    }

    public WallSignIconView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public WallSignIconView(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public WallSignIconView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr);

        if (attrs != null) {
            mDrawWall = attrs.getAttributeBooleanValue(CUSTOM_ATTRIBUTE_NAME_SPACE, CUSTOM_ATTRIBUTE_LAYOUT_ID, mDrawWall);
        }

        if (mDrawWall) {
            mWallPaint = new Paint();
            mWallPaint.setColor(Color.RED);
        }

        setPadding(10, 10, 10, 10);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        final int sizeX = (int)getResources().getDimension(R.dimen.sign_icon_size);
        final int sizeY = (int)getResources().getDimension(R.dimen.sign_icon_size);

        setMeasuredDimension(sizeX, sizeY);
    }

    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
        super.onLayout(changed, left, top, right, bottom);

        mWallSize = new Rect(WALL_THIKNESS, WALL_THIKNESS, right - left - WALL_THIKNESS, WALL_THIKNESS * 2);
    }

    public void updateMazeCellInformation(MazeCellInformation mazeCellInformation) {
        mMazeCellInformation = mazeCellInformation;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if (mWallPaint != null
                && mMazeCellInformation != null) {
            if (mMazeCellInformation.isSignExistOnEast()) {
                canvas.save();
                canvas.rotate(90f, canvas.getWidth() / 2, canvas.getHeight() / 2);
                canvas.drawRect(mWallSize, mWallPaint);
                canvas.restore();
            }

            if (mMazeCellInformation.isSignExistOnWest()) {
                canvas.save();
                canvas.rotate(270f, canvas.getWidth() / 2, canvas.getHeight() / 2);
                canvas.drawRect(mWallSize, mWallPaint);
                canvas.restore();
            }

            if (mMazeCellInformation.isSignExistOnSouth()) {
                canvas.save();
                canvas.rotate(180f, canvas.getWidth() / 2, canvas.getHeight() / 2);
                canvas.drawRect(mWallSize, mWallPaint);
                canvas.restore();
            }

            if (mMazeCellInformation.isSignExistOnNorth()) {
                canvas.save();
                canvas.rotate(0f, canvas.getWidth() / 2, canvas.getHeight() / 2);
                canvas.drawRect(mWallSize, mWallPaint);
                canvas.restore();
            }
        }
    }
}
