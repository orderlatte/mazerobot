package kr.mobilian.maze_rui;

import android.annotation.SuppressLint;
import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;

import java.util.ArrayList;
import java.util.List;

import static kr.mobilian.maze_rui.MazeCellInformationConstant.MAP_MAX_SIZE;
import static kr.mobilian.maze_rui.RobotMode.NONE;

@SuppressLint("HandlerLeak")
public class RobotStateReceiveService extends Service {
    private static final String TAG = "RobotStateReceiveService";
    private static boolean DEBUG = false;

    public interface IWallSignStateChangedCallback {
        void onWallSignStateChanged(List<MazeCellInformation> wallSignStateList);
    }
    private List<IWallSignStateChangedCallback> mWallSignStateChangedCallbacks = new ArrayList<IWallSignStateChangedCallback>();

    public interface IMazeMapStateChangedCallback {
        void onMazeMapChanged(List<MazeCellInformation> mazeCellInformationList);
    }
    private List<IMazeMapStateChangedCallback> mMazeMapStateChangedCallbacks = new ArrayList<IMazeMapStateChangedCallback>();

    public interface INetworkConnectionChangedCallback {
        void onNetworkConnectionChanged(boolean connected);
    }
    private List<INetworkConnectionChangedCallback> mNetworkConnectionChangedCallbacks = new ArrayList<INetworkConnectionChangedCallback>();

    public interface IRobotModeChangedCallback {
        void onRobotModeChanged(RobotMode mode);
    }
    private List<IRobotModeChangedCallback> mRobotModeChangedCallbacks = new ArrayList<IRobotModeChangedCallback>();

    private List<MazeCellInformation> mWallSignCellList = new ArrayList<MazeCellInformation>();
    private MazeCellInformation[][] mMazeCellInformation;

    private boolean mNetworkConnected;
    private NetworkStatusThread mNetworkStatusThread;
    private static Handler mNetworkStatusHandler;

    private MapDataReceiveThread mMapDataReceiveThread;
    private static Handler mMapDataReceiveHandler;

    private RobotMode mRobotMode = NONE;

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();

        ((MazeApplication) getApplicationContext()).registerService(this);

        if (mMapDataReceiveThread == null) {
            mMapDataReceiveHandler = new Handler() {
                @Override
                public void handleMessage(Message msg) {
                    super.handleMessage(msg);

                    byte[] mapData = (byte[])msg.obj;

                    mWallSignCellList.clear();
                    mMazeCellInformation = new MazeCellInformation[MAP_MAX_SIZE][MAP_MAX_SIZE];

                    if (mapData != null) {
                        for (int i = 0; i < MAP_MAX_SIZE; i++) {
                            for (int j = 0; j < MAP_MAX_SIZE; j++) {
                                int index = (i * MAP_MAX_SIZE + j) * 8;
                                int mapCellData = 0;
                                String hexData = "" + Integer.toHexString(mapData[index])
                                        + Integer.toHexString(mapData[index + 1])
                                        + Integer.toHexString(mapData[index + 2])
                                        + Integer.toHexString(mapData[index + 3])
                                        + Integer.toHexString(mapData[index + 4])
                                        + Integer.toHexString(mapData[index + 5])
                                        + Integer.toHexString(mapData[index + 6])
                                        + Integer.toHexString(mapData[index + 7]);
                                try {
                                    mapCellData = (int) Long.parseLong(hexData, 16);
                                } catch (NumberFormatException e) {
                                    mapCellData = 0;
                                    e.printStackTrace();
                                }

                                Log.d(DEBUG, TAG, "mapData[" + i + "][" + j + "], " + Integer.toHexString(mapCellData) + " = " + hexData);

                                mMazeCellInformation[i][j] = new MazeCellInformation(mapCellData);

                                if (mMazeCellInformation[i][j].isDiscovered()) {
                                    Log.d(TAG, "real Maze Discovered [" + i + "][" + j + "]=" + mMazeCellInformation[i][j]);
                                }

                                //TODO, need change to mMazeCellInformation
                                if (mMazeCellInformation[i][j].isDiscovered()
                                        && mMazeCellInformation[i][j].hasRedDot()) {
                                    mWallSignCellList.add(mMazeCellInformation[i][j]);
                                }

                            }
                        }
                    }

                    notifyWallSignStateChanged();
                    notifyMazeMapStateChanged();
                }
            };

            mMapDataReceiveThread = new MapDataReceiveThread(mMapDataReceiveHandler);
            mMapDataReceiveThread.start();
        }

        if (mNetworkStatusThread == null) {
            mNetworkStatusHandler = new Handler() {
                @Override
                public void handleMessage(Message msg) {
                    super.handleMessage(msg);

                    mNetworkConnected = msg.what == 0 ? false : true;
                    notifyNetworkConnectionChanged(mNetworkConnected);
                }
            };

            mNetworkStatusThread = new NetworkStatusThread(mNetworkStatusHandler);
            mNetworkStatusThread.start();
        }
    }

    @Override
    public void onDestroy() {
        if (mMapDataReceiveThread != null) {
            mMapDataReceiveThread.stopReceive();
            mMapDataReceiveThread = null;
        }

        if (mNetworkStatusThread != null) {
            mNetworkStatusThread.stopChecking();
            mNetworkStatusThread = null;
        }

        super.onDestroy();
    }

    public void resetMapData() {
        if (mMapDataReceiveHandler != null) {
            Message.obtain(mMapDataReceiveHandler, 0, null).sendToTarget();
        }
    }

    public void registerCallback(IWallSignStateChangedCallback callback) {
        mWallSignStateChangedCallbacks.add(callback);
        callback.onWallSignStateChanged(mWallSignCellList);

        Log.d(TAG, "registerCallback, IWallSignStateChangedCallback");
    }

    public void unregisterCallback(IWallSignStateChangedCallback callback) {
        mWallSignStateChangedCallbacks.remove(callback);
    }

    private void notifyWallSignStateChanged() {
        for(IWallSignStateChangedCallback callback : mWallSignStateChangedCallbacks) {
            callback.onWallSignStateChanged(mWallSignCellList);
        }
    }

    public void registerCallback(IMazeMapStateChangedCallback callback) {
        mMazeMapStateChangedCallbacks.add(callback);

        List<MazeCellInformation> list = mazeStateArrayToList();
        callback.onMazeMapChanged(list);

        Log.d(TAG, "registerCallback, IMazeMapStateChangedCallback");
    }

    public void unregisterCallback(IMazeMapStateChangedCallback callback) {
        mMazeMapStateChangedCallbacks.remove(callback);
    }

    private void notifyMazeMapStateChanged() {
        List<MazeCellInformation> list = mazeStateArrayToList();

        for(IMazeMapStateChangedCallback callback : mMazeMapStateChangedCallbacks) {
            callback.onMazeMapChanged(list);
        }
    }

    public void registerCallback(INetworkConnectionChangedCallback callback) {
        mNetworkConnectionChangedCallbacks.add(callback);

        callback.onNetworkConnectionChanged(mNetworkConnected);
    }

    public void unregisterCallback(INetworkConnectionChangedCallback callback) {
        mNetworkConnectionChangedCallbacks.remove(callback);
    }

    private void notifyNetworkConnectionChanged(boolean connected) {
        for (INetworkConnectionChangedCallback callback : mNetworkConnectionChangedCallbacks) {
            callback.onNetworkConnectionChanged(connected);
        }
    }

    public void registerCallback(IRobotModeChangedCallback callback) {
        mRobotModeChangedCallbacks.add(callback);

        callback.onRobotModeChanged(mRobotMode);
    }

    public void unregisterCallback(IRobotModeChangedCallback callback) {
        mRobotModeChangedCallbacks.remove(callback);
    }

    private void notifyRobotModeChanged(RobotMode mode) {
        for (IRobotModeChangedCallback callback : mRobotModeChangedCallbacks) {
            callback.onRobotModeChanged(mode);
        }
    }

    private List<MazeCellInformation> mazeStateArrayToList() {
        List<MazeCellInformation> list = new ArrayList<MazeCellInformation>();

        if (mMazeCellInformation == null) {
            mMazeCellInformation = new MazeCellInformation[MAP_MAX_SIZE][MAP_MAX_SIZE];
        }

        for (int i = 0; i < MAP_MAX_SIZE; i++) {
            for (int j = 0; j < MAP_MAX_SIZE; j++) {
                list.add(mMazeCellInformation[i][j]); //TODO, need Improve
            }
        }

        return list;
    }
}
