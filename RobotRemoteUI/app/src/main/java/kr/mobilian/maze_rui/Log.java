package kr.mobilian.maze_rui;

public class Log {
    private static final String TAG = "[MAZE]";

    public static void d(boolean enabled, String tag, String msg) {
        if (enabled) {
            android.util.Log.d(TAG + tag, msg);
        }
    }

    public static void i(boolean enabled, String tag, String msg) {
        if (enabled) {
            android.util.Log.i(TAG + tag, msg);
        }
    }

    public static void e(boolean enabled, String tag, String msg) {
        if (enabled) {
            android.util.Log.e(TAG + tag, msg);
        }
    }

    public static void v(boolean enabled, String tag, String msg) {
        if (enabled) {
            android.util.Log.v(TAG + tag, msg);
        }
    }

    public static void w(boolean enabled, String tag, String msg) {
        if (enabled) {
            android.util.Log.w(TAG + tag, msg);
        }
    }

    public static void d(String tag, String msg) {
        android.util.Log.d(TAG + tag, msg);
    }

    public static void i(String tag, String msg) {
        android.util.Log.i(TAG + tag, msg);
    }

    public static void e(String tag, String msg) {
        android.util.Log.e(TAG + tag, msg);
    }

    public static void v(String tag, String msg) {
        android.util.Log.v(TAG + tag, msg);
    }

    public static void w(String tag, String msg) {
        android.util.Log.w(TAG + tag, msg);
    }
}
