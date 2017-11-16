package kr.mobilian.maze_rui;

public enum RobotConnectionState {
    STATE_DISCONNECTED(0),
    STATE_CONNECTING(1),
    STATE_CONNECTED(2);

    private int mIndex;
    RobotConnectionState(int index) {
        this.mIndex = index;
    }

    public int toIndex() {
        return toIndex(this);
    }

    public static int toIndex(RobotConnectionState state) {
        return state.mIndex;
    }

    public static RobotConnectionState toState(int state) {
        switch (state) {
            default:
            case 0:
                return STATE_DISCONNECTED;

            case 1:
                return STATE_CONNECTING;

            case 2:
                return STATE_CONNECTED;
        }
    }
}
