package kr.mobilian.maze_rui;

import android.os.Handler;
import android.os.SystemClock;

import java.io.IOException;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

import static kr.mobilian.maze_rui.CommanderService.COMMAND_INVALID;
import static kr.mobilian.maze_rui.CommanderService.COMMAND_TYPE_CONTROL;
import static kr.mobilian.maze_rui.CommanderService.COMMAND_TYPE_RESET;
import static kr.mobilian.maze_rui.RobotConnectionState.STATE_CONNECTED;
import static kr.mobilian.maze_rui.RobotConnectionState.STATE_CONNECTING;
import static kr.mobilian.maze_rui.RobotConnectionState.STATE_DISCONNECTED;

public class CommandToServerThread extends Thread {
    private static final String TAG = "CommandToServerThread";
    private static boolean DEBUG = true;

    private static final int INTERVAL = 1000;

    private static final String COMMAND_SERVER_IP = "172.20.10.14";
    private static final int COMMAND_SERVER_PORT = 21000;

    private boolean mDoLoop;

    private Handler mStateChangedHandler;
    private Socket mServerSocket;
    private OutputStream mCommandStream;

    public CommandToServerThread(Handler handler) {
        mStateChangedHandler = handler;

        if (mStateChangedHandler != null) {
            mStateChangedHandler.sendEmptyMessage(STATE_DISCONNECTED.toIndex());
        }
    }

    @Override
    public void run() {
        mDoLoop = true;

        while (mDoLoop) {
            checkServerReachable();
            connectServer();
            notifyConnectionState();

            SystemClock.sleep(INTERVAL);
        }

        disconnectServer();
    }

    private void checkServerReachable() {
        try {
            InetAddress server = InetAddress.getByName(COMMAND_SERVER_IP);
            if (server != null
                    && server.isReachable(INTERVAL)) {
                return;
            }
        } catch (UnknownHostException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        disconnectServer();
    }

    public void stopSending() {
        mDoLoop = false;
    }

    private void connectServer() {
        if (mServerSocket != null) {
            return;
        }

        if (mStateChangedHandler != null) {
            mStateChangedHandler.sendEmptyMessage(STATE_CONNECTING.toIndex());
        }

        try {
            mServerSocket = new Socket(COMMAND_SERVER_IP, COMMAND_SERVER_PORT);
            if (mServerSocket != null) {
                mServerSocket.setReuseAddress(true);
                mCommandStream = mServerSocket.getOutputStream();
            }

            Log.i(TAG, "Connect to CommandServer(" + COMMAND_SERVER_IP + ":" + COMMAND_SERVER_PORT + ") SUCCEEDED");
        } catch (IOException e) {
            Log.e(TAG, "Connect to CommandServer(" + COMMAND_SERVER_IP + ":" + COMMAND_SERVER_PORT + ") FAILED");
            e.printStackTrace();
            closeServerSocket();
        }
    }

    private void closeServerSocket() {
        if (mServerSocket == null) {
            return;
        }

        try {
            mServerSocket.close();

            Log.i(DEBUG, TAG, "Disconnect from CommandServer(" + COMMAND_SERVER_IP + ":" + COMMAND_SERVER_PORT + ") SUCCEEDED");
        } catch (IOException e) {
            Log.e(TAG, "Disconnect from CommandServer(" + COMMAND_SERVER_IP + ":" + COMMAND_SERVER_PORT + ") FAILED");
            e.printStackTrace();
        } finally {
            mServerSocket = null;
        }
    }

    private void notifyConnectionState() {
        if (mStateChangedHandler == null) {
            return;
        }

        if (mServerSocket != null
                && mCommandStream != null) {
            mStateChangedHandler.sendEmptyMessage(STATE_CONNECTED.toIndex());
        } else {
            mStateChangedHandler.sendEmptyMessage(STATE_DISCONNECTED.toIndex());
        }
    }

    private void disconnectServer() {
        if (mCommandStream != null) {
            try {
                mCommandStream.close();

                Log.i(DEBUG, TAG, "Close CommandStream SUCCEEDED");
            } catch (IOException e) {
                Log.i(DEBUG, TAG, "Close CommandStream FAILED");
                e.printStackTrace();
            } finally {
                mCommandStream = null;
            }
        }

        closeServerSocket();
    }

    public boolean sendResetCommand(int mapSolvingAlgorithmIndex) {
        return sendCommands(COMMAND_TYPE_RESET, mapSolvingAlgorithmIndex, COMMAND_INVALID, COMMAND_INVALID);
    }

    public boolean sendCommands(int commandMode, int commandMove, int commandView) {
        return sendCommands(COMMAND_TYPE_CONTROL, commandMode, commandMove, commandView);
    }

    private boolean sendCommands(int commandType, int commandMode, int commandMove, int commandView) {
        if (mServerSocket == null
                || mCommandStream == null) {
            Log.e(TAG, "CommandStream is NULL, discard command.");
            return false;
        }

        try{
            byte[] commands = commandToBytes(commandType, commandMode, commandMove, commandView);
            mCommandStream.write(commands);
            mCommandStream.flush();
            Log.i(DEBUG, TAG, "sendCommands(" + commandType + ", " + commandMode + ", " + commandMove + ", " + commandView + ") = Type[" + commands[0] + "][Mode:" + commands[1] + "][Move:" + commands[2] + "][View:" + commands[3] + "]");

        } catch (IOException e){
            Log.e(TAG, "Sending commands FAILED.");
            e.printStackTrace();

            if (mServerSocket != null) {
                try {
                    mServerSocket.close();
                } catch (IOException e1) {
                    e1.printStackTrace();
                }
                mServerSocket = null;
            }
        }

        return mServerSocket != null;
    }

    private byte[] commandToBytes(Integer commandType, Integer commandMode, Integer commandMove, Integer commandView) {
        commandType = commandType % 10;
        commandMode = commandMode % 10;
        commandMove = commandMove % 10;
        commandView = commandView % 10;
        return new byte[] {commandType.byteValue(), commandMode.byteValue(), commandMove.byteValue(), commandView.byteValue()};
    }
}
