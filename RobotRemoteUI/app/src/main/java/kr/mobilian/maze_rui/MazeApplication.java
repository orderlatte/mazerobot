package kr.mobilian.maze_rui;

import android.app.Application;
import android.content.ComponentName;
import android.content.Intent;

public class MazeApplication extends Application {
    private static final String TAG = "MazeApplication";
    private static boolean DEBUG = true;

    private RobotStateReceiveService mRobotStateReceiveService;
    private CommanderService mCommanderService;

    @Override
    public void onCreate() {
        super.onCreate();

        try {
            startService(new Intent().setComponent(new ComponentName("kr.mobilian.maze_rui", "kr.mobilian.maze_rui.RobotStateReceiveService")));
            startService(new Intent().setComponent(new ComponentName("kr.mobilian.maze_rui", "kr.mobilian.maze_rui.CommanderService")));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void registerService(RobotStateReceiveService service) {
        mRobotStateReceiveService = service;
    }

    public RobotStateReceiveService getRobotStateReceiveService() {
        return mRobotStateReceiveService;
    }

    public void registerService(CommanderService service) {
        mCommanderService = service;
    }

    public CommanderService getCommanderService() {
        return mCommanderService;
    }
}
