package kr.mobilian.maze_rui;

import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketException;

import static kr.mobilian.maze_rui.MazeCellInformationConstant.MAP_MAX_SIZE;

public class MapDataReceiveThread extends Thread {
    private static final String TAG = "MapDataReceiveThread";
    private static boolean DEBUG = true;

    private static final int RECEIVE_PORT = 32000;

    private static final int UPDATE_INTERVAL = 300;
    private static final int UPDATE_TIMEOUT = 200;

    private DatagramSocket mReceiverSocket;
    private byte[] mMapBuffer;

    private Handler mUpdateHandler;

    public MapDataReceiveThread(Handler updateHandler) {
        mMapBuffer = new byte[MAP_MAX_SIZE * MAP_MAX_SIZE * 8];
        mUpdateHandler = updateHandler;
    }

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
                DatagramPacket dp = new DatagramPacket(mMapBuffer, mMapBuffer.length);
                mReceiverSocket.receive(dp);

                byte[] data = dp.getData();
                if (mUpdateHandler != null) {
                    Message.obtain(mUpdateHandler, data.length, (Object)data).sendToTarget();
                }

                SystemClock.sleep(UPDATE_INTERVAL);
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
}
