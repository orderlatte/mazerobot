package kr.mobilian.maze_rui;

import android.os.Handler;
import android.os.SystemClock;

import java.io.IOException;
import java.net.InetAddress;
import java.net.UnknownHostException;

public class NetworkStatusThread extends Thread {
    private static final String TAG = "NetworkStatusThread";
    private static boolean DEBUG = true;

    private static final String TARGET_HOST_ROUTER = "172.20.10.1";
    private static final int UPDATE_INTERVAL = 1 * 1000;

    private Handler mTargetHandler;

    private InetAddress mTargetIPAddress;
    private boolean mNetworkConnected;


    public NetworkStatusThread(Handler handler) {
        mTargetHandler = handler;

        if (mTargetIPAddress == null) {
            try {
                mTargetIPAddress = InetAddress.getByName(TARGET_HOST_ROUTER);
            } catch (UnknownHostException e) {
                e.printStackTrace();
                mTargetIPAddress = null;
            }
        }
    }

    @Override
    public void run() {
        super.run();

        while (mTargetHandler != null) {
            boolean connected = isNetworkReachable();

            if (mNetworkConnected != connected) {
                mNetworkConnected = connected;

                mTargetHandler.sendEmptyMessage(mNetworkConnected ? 1 : 0);
            }

            SystemClock.sleep(UPDATE_INTERVAL);
        }
    }

    public void stopChecking() {
        if (mTargetIPAddress != null) {
            mTargetIPAddress = null;
        }
    }

    private boolean isNetworkReachable() {
        boolean reachable = false;

        if (mTargetIPAddress != null) {
            try {
                reachable = mTargetIPAddress.isReachable(300);
            } catch (IOException e) {
                e.printStackTrace();
                reachable = false;
            }
        }

        return reachable;
    }
}
