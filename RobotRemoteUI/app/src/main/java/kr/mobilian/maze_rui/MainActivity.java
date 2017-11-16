package kr.mobilian.maze_rui;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

import static kr.mobilian.maze_rui.SettingDebugRobotLayout.ACTION_HIDDEN_SETTING;
import static kr.mobilian.maze_rui.SettingDebugRobotLayout.EXTRA_HIDE_MAP_BACKGROUND;
import static kr.mobilian.maze_rui.SettingDebugRobotLayout.EXTRA_SHOW_MAZE_CELL_INDEX;
import static kr.mobilian.maze_rui.SettingDebugRobotLayout.EXTRA_SHOW_MAZE_NEXT_CELL;
import static kr.mobilian.maze_rui.SettingDebugRobotLayout.EXTRA_SHOW_MAZE_SOLVING_HISTORY;
import static kr.mobilian.maze_rui.SettingRobotLayout.ACTION_ROBOT_SETTING;
import static kr.mobilian.maze_rui.SettingRobotLayout.EXTRA_MAZE_ALGORITHM_001;
import static kr.mobilian.maze_rui.SettingRobotLayout.EXTRA_MAZE_ALGORITHM_002;
import static kr.mobilian.maze_rui.SettingRobotLayout.EXTRA_MAZE_ALGORITHM_003;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivity";
    private static boolean DEBUG = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Intent intentRobotSetting = new Intent(ACTION_ROBOT_SETTING);
        removeStickyBroadcast(intentRobotSetting);

        if (intentRobotSetting != null) {
            intentRobotSetting.putExtra(EXTRA_MAZE_ALGORITHM_001, true);
            intentRobotSetting.putExtra(EXTRA_MAZE_ALGORITHM_002, false);
            intentRobotSetting.putExtra(EXTRA_MAZE_ALGORITHM_003, false);

            sendStickyBroadcast(intentRobotSetting);
        }

        Intent intentHiddenSetting = new Intent(ACTION_HIDDEN_SETTING);
        removeStickyBroadcast(intentHiddenSetting);

        if (intentHiddenSetting != null) {
            intentHiddenSetting.putExtra(EXTRA_SHOW_MAZE_CELL_INDEX, false);
            intentHiddenSetting.putExtra(EXTRA_SHOW_MAZE_NEXT_CELL, false);
            intentHiddenSetting.putExtra(EXTRA_SHOW_MAZE_SOLVING_HISTORY, false);
            intentHiddenSetting.putExtra(EXTRA_HIDE_MAP_BACKGROUND, false);

            sendStickyBroadcast(intentHiddenSetting);
        }

        makeFullScreen();
        
        setContentView(R.layout.activity_main);
    }

    @Override
    public void onAttachedToWindow() {
        super.onAttachedToWindow();

        View robotSetting = findViewById(R.id.robot_setting);
        if (robotSetting != null) {
            final View robotSettingLayout = findViewById(R.id.robot_setting_layout);
            robotSetting.setOnLongClickListener(new View.OnLongClickListener() {
                @Override
                public boolean onLongClick(View view) {
                    if (robotSettingLayout != null) {
                        robotSettingLayout.setVisibility(View.VISIBLE);
                    }
                    return true;
                }
            });
        }

        View hiddenSetting = findViewById(R.id.hidden_setting);
        if (hiddenSetting != null) {
            final View hiddenSettingLayout = findViewById(R.id.hidden_setting_layout);
            hiddenSetting.setOnLongClickListener(new View.OnLongClickListener() {
                @Override
                public boolean onLongClick(View view) {
                    if (hiddenSettingLayout != null) {
                        hiddenSettingLayout.setVisibility(View.VISIBLE);
                    }
                    return true;
                }
            });
        }
    }

    @Override
    public void onDetachedFromWindow() {
        super.onDetachedFromWindow();
    }

    @Override
    protected void onResume() {
        super.onResume();
        
        makeFullScreen();
    }
    
    private void makeFullScreen() {
        View decorView = getWindow().getDecorView();
        if (decorView != null) {
            decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
        }
    }
}
