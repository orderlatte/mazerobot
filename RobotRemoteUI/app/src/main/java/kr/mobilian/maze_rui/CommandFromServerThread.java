package kr.mobilian.maze_rui;

import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class CommandFromServerThread extends Thread {
    private static final String TAG = "CommandFromServerThread";
    private static boolean DEBUG = true;

    private static final int INTERVAL = 1000;

    private static final int COMMAND_RECEIVER_PORT = 22000;

    private final int COMMAND_BUFFER_SIZE = 10;
    private byte[] mReceivedCommandBuffer = new byte[COMMAND_BUFFER_SIZE];

    private ServerSocket mServerSocket;
    private Socket mCommandReceiveSocket;

    private boolean mDoLoop;

    private Handler mCommandReceiveHandler;

    public CommandFromServerThread(Handler handler) {
        Log.i(TAG, "CommandFromServerThread created.");

        mCommandReceiveHandler = handler;
    }

    public void stopReceiving() {
        mDoLoop = false;
    }

    @Override
    public void run() {
        mDoLoop = true;

        while (mDoLoop) {
            try{
                if (mServerSocket == null) {
                    if (mCommandReceiveSocket != null) {
                        mCommandReceiveSocket.close();
                        mCommandReceiveSocket = null;
                    }

                    mServerSocket = new ServerSocket(COMMAND_RECEIVER_PORT);

                    if (mCommandReceiveSocket == null) {
                        mCommandReceiveSocket = mServerSocket.accept();
                    }
                }

                int receivedBufferSize = 0;
                if (mCommandReceiveSocket != null) {
                    receivedBufferSize = mCommandReceiveSocket.getInputStream().read(mReceivedCommandBuffer);
                    Message.obtain(mCommandReceiveHandler, receivedBufferSize, (Object)mReceivedCommandBuffer).sendToTarget();
                }

                Log.d(TAG, "mServerSocket: " + mServerSocket + ", mCommandReceiveSocket: " + mCommandReceiveSocket + ", mCommandReceiveHandler: " + mCommandReceiveHandler);
                if (receivedBufferSize > 0) {
                    Log.i(DEBUG, TAG, "ReceivedData(" + mCommandReceiveSocket + "): " + receivedBufferSize + ", :" + mReceivedCommandBuffer);
                }
            } catch (IOException e){
                Log.e(TAG, "Exception found while receiving command from server.");
                e.printStackTrace();
            }

            SystemClock.sleep(INTERVAL);
        }

        try {
            if (mCommandReceiveSocket != null) {
                mCommandReceiveSocket.close();
            }
        } catch (IOException e) {
            Log.e(TAG, "Exception found while receiving command from server.");
            e.printStackTrace();
        } finally {
            mCommandReceiveSocket = null;
        }

        Log.i(TAG, "Disconnected to CommandServer");
    }
}
