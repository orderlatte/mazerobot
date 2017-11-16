package kr.mobilian.maze_rui;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketException;

public class CameraImageViewThread extends Thread {
    private static final String TAG = "CameraImageViewThread";
    private static boolean DEBUG = true;

    private static final int RECEIVE_PORT = 31000;
    private static final int DATAGRAM_MAX_SIZE = 65507;

    private static final int UPDATE_INTERVAL = 0;
    private static final int UPDATE_TIMEOUT = 500;

    private DatagramSocket mReceiverSocket;
    private byte[] mImageBuffer = new byte[DATAGRAM_MAX_SIZE];

    private Handler mUpdateHandler;
    private final Handler mDisconnectHandler = new Handler();

    @Override
    public void run() {
        super.run();

        try {
            Thread.currentThread().setName(TAG);

            if (mReceiverSocket == null) {
                mReceiverSocket = new DatagramSocket(null);
                mReceiverSocket.setReuseAddress(true);
                mReceiverSocket.bind(new InetSocketAddress(RECEIVE_PORT));
            }

            while (mReceiverSocket != null
                    && !Thread.currentThread().isInterrupted()) {
                DatagramPacket dp = new DatagramPacket(mImageBuffer, mImageBuffer.length);
                mReceiverSocket.receive(dp);

                byte[] data = dp.getData();
                Bitmap bitmap = BitmapFactory.decodeByteArray(data, 0, data.length);

                if (DEBUG) {
                    Log.d(TAG, "datagram packet: " + dp + ", bitmap: " + bitmap);
                }

                mDisconnectHandler.removeCallbacksAndMessages(null);
                mDisconnectHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        Message.obtain(mUpdateHandler, 0, (Object) null).sendToTarget();

                        Log.i(TAG, "Timeout: Camera disconnected.");

                    }
                }, UPDATE_TIMEOUT);

                if (mUpdateHandler != null) {
                    Message.obtain(mUpdateHandler, 0, (Object) bitmap).sendToTarget();
                }

                if (UPDATE_INTERVAL > 0) {
                    SystemClock.sleep(UPDATE_INTERVAL);
                }
            }
        } catch (SocketException exception) {
            exception.printStackTrace();
            Log.e(TAG, "SocketException: " + exception.getMessage());
        } catch (IOException exception) {
            exception.printStackTrace();
            Log.e(TAG, "IOException: " + exception.getMessage());
        }
    }

    public void stopReceive() {
        if (mReceiverSocket != null) {
            mReceiverSocket.close();
            mReceiverSocket = null;
        }
    }

    public void setUpdateHanlder(Handler updateHandler) {
        mUpdateHandler = updateHandler;
    }
}
