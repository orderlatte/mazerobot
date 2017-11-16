package kr.mobilian.maze_rui;

public enum CommandState {
    NONE(0, "NONE"),
    MANUAL(1, "Manual Mode"),
    AUTO(2, "Auto Mode"),
    SUSPEND(3, "Suspend Mode"),
    SUCCESS(4, "SUCCESS");

    private int mCommandStateIndex;
    private String mCommandStateName;

    CommandState(int state, String stateName) {
        mCommandStateIndex = state;
        mCommandStateName = stateName;
    }

    public String toString() {
        return toString(this);
    }

    public static String toString(CommandState state) {
        return state.mCommandStateName;
    }

    public int getIndex() {
        return getIndex(this);
    }

    public static int getIndex(CommandState mode) {
        return mode.mCommandStateIndex;
    }

    public static CommandState getAsCommandState(int index) {
        switch (index) {
            default:
            case 0:
                return NONE;
            case 1:
                return MANUAL;
            case 2:
                return AUTO;
            case 3:
                return SUSPEND;
        }
    }
}
