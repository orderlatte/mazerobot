package kr.mobilian.maze_rui;

import android.annotation.SuppressLint;
import android.app.Service;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;

import java.util.ArrayList;
import java.util.List;

import static kr.mobilian.maze_rui.RobotConnectionState.STATE_DISCONNECTED;
import static kr.mobilian.maze_rui.SettingRobotLayout.ACTION_ROBOT_SETTING;
import static kr.mobilian.maze_rui.SettingRobotLayout.EXTRA_MAZE_ALGORITHM_001;
import static kr.mobilian.maze_rui.SettingRobotLayout.EXTRA_MAZE_ALGORITHM_002;
import static kr.mobilian.maze_rui.SettingRobotLayout.EXTRA_MAZE_ALGORITHM_003;

@SuppressLint("HandlerLeak")
public class CommanderService extends Service {
    private static final String TAG = "CommanderService";
    private static boolean DEBUG = true;

    public static final int NEXT_CELL_INIT = -1;

    private RobotConnectionState mRobotConnectionState = STATE_DISCONNECTED;
    public interface IConectionStateChangedCallback {
        void onConnectionStateChanged(RobotConnectionState state);
    }
    private List<IConectionStateChangedCallback> mConectionStateChangedCallbacks = new ArrayList<IConectionStateChangedCallback>();

    public interface INextCellChangedCallback {
        void onNextCellChanged(int nextX, int nextY);
    }
    private List<INextCellChangedCallback> mNextCellChangedCallbacks = new ArrayList<INextCellChangedCallback>();
    private int mNextX = NEXT_CELL_INIT;
    private int mNextY = NEXT_CELL_INIT;

    public interface ICommandStateChangedCallback {
        void onCommandStateChanged(CommandState mode);
    }
    private List<ICommandStateChangedCallback> mCommandStateChangedCallbacks = new ArrayList<ICommandStateChangedCallback>();
    private CommandState mCommandState = CommandState.NONE;

    private static boolean COMMAND_TYPE_SINGLE = true;

    public static final int COMMAND_INVALID = 0;

    public static final int COMMAND_TYPE_CONTROL = 1001;
    public static final int COMMAND_TYPE_RESET = 1002;

    public static final int COMMAND_MODE_NONE = 2000;
    public static final int COMMAND_MODE_MANUAL = 2001;
    public static final int COMMAND_MODE_AUTO = 2002;
    public static final int COMMAND_MODE_SUSPEND = 2003;

    public static final int COMMAND_MOVE_STOP = 3005;
    public static final int COMMAND_MOVE_FORWARD = 3001;
    public static final int COMMAND_MOVE_BACKWARD = 3002;
    public static final int COMMAND_MOVE_LEFT = 3003;
    public static final int COMMAND_MOVE_RIGHT = 3004;

    public static final int COMMAND_VIEW_CENTER = 4005;
    public static final int COMMAND_VIEW_UP = 4001;
    public static final int COMMAND_VIEW_DOWN = 4002;
    public static final int COMMAND_VIEW_LEFT = 4003;
    public static final int COMMAND_VIEW_RIGHT = 4004;

    private CommandToServerThread mCommandToServerThread;
    private Handler mCommandToServerThreadDeliver;

    private int mCommandMode = COMMAND_MODE_MANUAL;
    private int mCommandMove = COMMAND_MOVE_STOP;
    private int mCommandView = COMMAND_VIEW_CENTER;

    private CommandFromServerThread mCommandFromServerThread;
    private Handler mCommandFromServerThreadDeliver;

    private Handler mConntectionStateHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);

            notifyConnectionStateChanged(RobotConnectionState.toState(msg.what));
        }
    };

    public CommanderService() {
    }

    @Override
    public void onCreate() {
        super.onCreate();

        createCommandToServerThread();
        createCommandFromServerThread();

        ((MazeApplication) getApplicationContext()).registerService(this);
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }

    private void createCommandToServerThread() {
        mCommandToServerThreadDeliver = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);

                if (mCommandToServerThread != null) {
                    mCommandToServerThread.sendCommands(mCommandMode, mCommandMove, mCommandView);
                }

                Log.i(TAG, "Command: " + getCommandName(msg.what));
            }
        };

        mCommandToServerThread = new CommandToServerThread(mConntectionStateHandler);
        if (mCommandToServerThread != null) {
            mCommandToServerThread.start();
        }
    }

    private void createCommandFromServerThread() {
        mCommandFromServerThreadDeliver = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);

                int bufferSize = msg.what;
                byte[] bufferData = (byte[])msg.obj;

                String commandType = Integer.toHexString(bufferData[0]);
                Log.d(DEBUG, TAG, "commandType: " + commandType);
                if ("3".equals(commandType)) { //NextCell Index
                    String nextPositionX = Integer.toHexString(bufferData[1]) + Integer.toHexString(bufferData[2]);
                    String nextPositionY = Integer.toHexString(bufferData[3]) + Integer.toHexString(bufferData[4]);

                    int nextCellX = NEXT_CELL_INIT;
                    int nextCellY = NEXT_CELL_INIT;
                    try {
                        nextCellX = (int) Long.parseLong(nextPositionX, 16);
                        nextCellY = (int) Long.parseLong(nextPositionY, 16);
                    } catch (NumberFormatException e) {
                        nextCellX = NEXT_CELL_INIT;
                        nextCellY = NEXT_CELL_INIT;
                        Log.e(TAG, "Number Converting FAILED, commandType: " + commandType + "nextPositionX: " + nextPositionX + ", nextPositionY: " + nextPositionY);

                        e.printStackTrace();
                    }

                    mNextX = nextCellX;
                    mNextY = nextCellY;
                    notifyNextCellChanged();
                } else if ("5".equals(commandType)) { //CommandState
                    int state = (int) Long.parseLong(Integer.toHexString(bufferData[1]), 16);
                    mCommandState = CommandState.getAsCommandState(state);
                    notifyCommandStateChanged();

                    Log.d(DEBUG, TAG, "NextCommandState: " + mCommandState);
//                } else if ("6".equals(commandType)) { //Fully Mapped
//                    mCommandState = CommandState.SUCCESS;
//                    notifyCommandStateChanged();
//
//                    Log.d(DEBUG, TAG, "SUCCESS State: " + mCommandState);
                }
            }
        };

        mCommandFromServerThread = new CommandFromServerThread(mCommandFromServerThreadDeliver);
        if (mCommandFromServerThread != null) {
            mCommandFromServerThread.start();
        }
    }

    @Override
    public void onDestroy() {
        notifyConnectionStateChanged(STATE_DISCONNECTED);

        if (mCommandFromServerThread != null) {
            mCommandFromServerThread.stopReceiving();
            mCommandFromServerThread = null;
        }

        if (mCommandToServerThread != null) {
            mCommandToServerThread.stopSending();
            mCommandToServerThread = null;
        }

        super.onDestroy();
    }

    public void registerCallback(IConectionStateChangedCallback callback) {
        mConectionStateChangedCallbacks.add(callback);
        callback.onConnectionStateChanged(mRobotConnectionState);
    }

    public void unregisterCallback(IConectionStateChangedCallback callback) {
        mConectionStateChangedCallbacks.remove(callback);
    }

    private void notifyConnectionStateChanged(RobotConnectionState robotConnectionState) {
        for(IConectionStateChangedCallback callback : mConectionStateChangedCallbacks) {
            callback.onConnectionStateChanged(robotConnectionState);
        }
    }

    public void registerCallback(INextCellChangedCallback callback) {
        mNextCellChangedCallbacks.add(callback);
        callback.onNextCellChanged(mNextX, mNextY);
    }

    public void unregisterCallback(INextCellChangedCallback callback) {
        mNextCellChangedCallbacks.remove(callback);
    }

    private void notifyNextCellChanged() {
        for(INextCellChangedCallback callback : mNextCellChangedCallbacks) {
            callback.onNextCellChanged(mNextX, mNextY);
        }
    }

    public void registerCallback(ICommandStateChangedCallback callback) {
        mCommandStateChangedCallbacks.add(callback);
        callback.onCommandStateChanged(mCommandState);
    }

    public void unregisterCallback(ICommandStateChangedCallback callback) {
        mCommandStateChangedCallbacks.remove(callback);
    }

    private void notifyCommandStateChanged() {
        for(ICommandStateChangedCallback callback : mCommandStateChangedCallbacks) {
            callback.onCommandStateChanged(mCommandState);
        }
    }

    public void sendResetCommand() {
        ((MazeApplication) getApplicationContext()).getCommanderService().sendCommand(COMMAND_MODE_MANUAL);
        ((MazeApplication) getApplicationContext()).getRobotStateReceiveService().resetMapData();

        mNextX = NEXT_CELL_INIT;
        mNextY = NEXT_CELL_INIT;
        notifyNextCellChanged();

        if (mCommandToServerThread != null) {
            int algorithmIndex = 1; // 1 is default index

            Intent intent = registerReceiver(null, new IntentFilter(ACTION_ROBOT_SETTING));
            if (intent != null) {
                boolean algorithm001 = intent.getBooleanExtra(EXTRA_MAZE_ALGORITHM_001, false);
                boolean algorithm002 = intent.getBooleanExtra(EXTRA_MAZE_ALGORITHM_002, false);
                boolean algorithm003 = intent.getBooleanExtra(EXTRA_MAZE_ALGORITHM_003, false);

                if (algorithm001) {
                    algorithmIndex = 1;
                } else if (algorithm002) {
                    algorithmIndex = 2;
                } else if (algorithm003) {
                    algorithmIndex = 3;
                }
            }

            mCommandToServerThread.sendResetCommand(algorithmIndex);
        }
    }

    public void sendCommand(int command) {
        Log.d(TAG, "ReceivedCommand: " + command);
        switch (command) {
            case COMMAND_MODE_AUTO:
            case COMMAND_MODE_MANUAL:
            case COMMAND_MODE_SUSPEND:
                mCommandMode = command;

                if (COMMAND_TYPE_SINGLE) {
                    mCommandMove = mCommandView = COMMAND_INVALID;
                }

                break;

            case COMMAND_MOVE_STOP:
            case COMMAND_MOVE_FORWARD:
            case COMMAND_MOVE_BACKWARD:
            case COMMAND_MOVE_LEFT:
            case COMMAND_MOVE_RIGHT:
                mCommandMove = command;

                if (COMMAND_TYPE_SINGLE) {
                    mCommandMode = mCommandView = COMMAND_INVALID;
                }

                break;

            case COMMAND_VIEW_CENTER:
            case COMMAND_VIEW_UP:
            case COMMAND_VIEW_DOWN:
            case COMMAND_VIEW_LEFT:
            case COMMAND_VIEW_RIGHT:
                mCommandView = command;

                if (COMMAND_TYPE_SINGLE) {
                    mCommandMode = mCommandMove = COMMAND_INVALID;
                }

                break;

            default:
                Log.e(TAG, "Unknown Command(" + command + ")");
                return;
        }

        mCommandToServerThreadDeliver.removeCallbacksAndMessages(null);
        mCommandToServerThreadDeliver.sendEmptyMessage(command);
    }

    public static String getCommandName(int command) {
        switch (command) {
            case COMMAND_MODE_AUTO:
                return "COMMAND_MODE_AUTO";

            case COMMAND_MODE_MANUAL:
                return "COMMAND_MODE_MANUAL";

            case COMMAND_MODE_SUSPEND:
                return "COMMAND_MODE_SUSPEND";

            case COMMAND_MOVE_STOP:
                return "COMMAND_MOVE_STOP";

            case COMMAND_MOVE_FORWARD:
                return "COMMAND_MOVE_FORWARD";

            case COMMAND_MOVE_BACKWARD:
                return "COMMAND_MOVE_BACKWARD";

            case COMMAND_MOVE_LEFT:
                return "COMMAND_MOVE_LEFT";

            case COMMAND_MOVE_RIGHT:
                return "COMMAND_MOVE_RIGHT";

            case COMMAND_VIEW_CENTER:
                return "COMMAND_VIEW_CENTER";

            case COMMAND_VIEW_UP:
                return "COMMAND_VIEW_UP";

            case COMMAND_VIEW_DOWN:
                return "COMMAND_VIEW_DOWN";

            case COMMAND_VIEW_LEFT:
                return "COMMAND_VIEW_LEFT";

            case COMMAND_VIEW_RIGHT:
                return "COMMAND_VIEW_RIGHT";

            default:
                return "Unknown Command(" + command + ")";
        }
    }
}
