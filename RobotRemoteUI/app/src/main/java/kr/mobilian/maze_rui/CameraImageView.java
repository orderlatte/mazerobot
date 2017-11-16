package kr.mobilian.maze_rui;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Handler;
import android.os.Message;
import android.support.v7.widget.AppCompatImageView;
import android.text.format.DateFormat;
import android.util.AttributeSet;

import java.util.Date;
import java.util.Locale;

@SuppressLint("HandlerLeak")
public class CameraImageView extends AppCompatImageView {
    private static final String TAG = "CameraImageView";
    private static boolean DEBUG = true;

    private static String DATE_TIME_FORMAT =  DateFormat.getBestDateTimePattern(Locale.getDefault(), "yyyy.MM.dd'at' HH:mm:ss");

    private Paint mTimePaint;
    private int mDrawingCount;

    private Handler mUpdateHandler;

    private CameraImageViewThread mCameraImageViewThread;

    public CameraImageView(Context context) {
        this(context, null);
    }

    public CameraImageView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public CameraImageView(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public CameraImageView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr);

        mTimePaint = new Paint();
        mTimePaint.setColor(Color.YELLOW);
        mTimePaint.setTextSize(getResources().getDimension(R.dimen.camera_display_text_size));
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

        if (mUpdateHandler == null) {
            mDrawingCount = 0;

            mUpdateHandler = new Handler() {
                @Override
                public void handleMessage(Message msg) {
                    mDrawingCount++;

                    if (msg.obj == null) {
                        setImageBitmap(null);
                        mDrawingCount = 0;
                    } else {
                        setImageBitmap(Bitmap.createScaledBitmap((Bitmap) msg.obj, getWidth(), getHeight(), false));
                    }

                    super.handleMessage(msg);
                }
            };
        }

        if (mCameraImageViewThread == null) {
            mCameraImageViewThread = new CameraImageViewThread();
            mCameraImageViewThread.setUpdateHanlder(mUpdateHandler);
            mCameraImageViewThread.start();
        }
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();

        if (mUpdateHandler != null) {
            mUpdateHandler = null;
        }

        if (mCameraImageViewThread != null) {
            mCameraImageViewThread.stopReceive();
            mCameraImageViewThread = null;
        }
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if (mTimePaint != null
                && mDrawingCount > 0) {
            String currentTime = DateFormat.format(DATE_TIME_FORMAT, new Date()).toString();
            canvas.drawText(currentTime + ", " + mDrawingCount, 10f, 30f, mTimePaint);
        }
    }
}
