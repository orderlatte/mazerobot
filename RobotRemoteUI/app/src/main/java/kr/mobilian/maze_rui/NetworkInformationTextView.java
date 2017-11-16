package kr.mobilian.maze_rui;

import android.content.Context;
import android.net.DhcpInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.support.v7.widget.AppCompatTextView;
import android.util.AttributeSet;

public class NetworkInformationTextView extends AppCompatTextView {
    private static final String TAG = "NetworkInformationTextView";
    private static boolean DEBUG = true;

    private Handler mUpdateHandler = new Handler();

    public NetworkInformationTextView(Context context) {
        this(context, null);
    }

    public NetworkInformationTextView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public NetworkInformationTextView(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public NetworkInformationTextView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr);
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

        update();
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
    }

    private void update() {
        setText(getIpAddress());

        mUpdateHandler.removeCallbacksAndMessages(null);
        mUpdateHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                update();
            }
        }, 1000);
    }

    private String getIpAddress() {
        WifiManager wifiManager = (WifiManager)getContext().getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        if (wifiManager == null) {
            return "Unknown Manager";
        }

        String ssidName = "Unknown SSID";
        WifiInfo wifiInfo = wifiManager.getConnectionInfo ();
        if (wifiInfo != null) {
            if (wifiInfo.getIpAddress() == 0) {
                return "Not Connected";
            }

            ssidName = wifiInfo.getSSID();
        } else {
            return ssidName;
        }

        DhcpInfo dhcpInfo = wifiManager.getDhcpInfo();
        if (dhcpInfo == null) {
            return "Unknown DHCP";
        }

        String ipAddress = ssidName + "\nIP: " + getAsIPString(dhcpInfo.ipAddress) + "\nGateway: " + getAsIPString(dhcpInfo.gateway);

        return ipAddress;
    }

    private String getAsIPString(int ip) {
        return String.format("%d.%d.%d.%d", (ip & 0xff), (ip >> 8 & 0xff), (ip >> 16 & 0xff), (ip >> 24 & 0xff));
    }
}