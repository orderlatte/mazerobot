package robot_algorithm;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class Client {
	private Socket mSocket;

	private BufferedReader mIn;
	private PrintWriter mOut;

	public Client(String ip, int port) throws Exception {
		// 서버에 요청 보내기
		mSocket = new Socket(ip, port);
		System.out.println("클:" + ip + " 연결됨");

		// 통로 뚫기
		mIn = new BufferedReader(new InputStreamReader(mSocket.getInputStream()));
		mOut = new PrintWriter(mSocket.getOutputStream());

		// 메세지 전달,
//		test(true, true, true, true, (byte) 0x5, (byte) 0x5);

		 test(false, true, false, true, (byte) 5, (byte) 5);//1
//		 test(false, true, false, true, (byte) 5, (byte) 5);//error
		 test(false, false, false, true, (byte) 6, (byte) 5);//2
		 test(true, false, false, true, (byte) 7, (byte) 5);//3
		 test(true, true, true, false, (byte) 7, (byte) 4);//4
//		 test(false, true, false, true, (byte) 5, (byte) 5);//error
		 test(true, false, false, true, (byte) 7, (byte) 5);//3
		 test(false, false, false, true, (byte) 6, (byte) 5);//2
		 test(true, false, false, false, (byte) 6, (byte) 4);//5
		 test(true, true, true, false, (byte) 6, (byte) 3);//6
		 test(true, false, false, false, (byte) 6, (byte) 4);//5
		 test(false, true, true, false, (byte) 5, (byte) 4);//7

		System.out.println("The End of Client");

		mIn.close();
		mOut.close();
		mSocket.close();

	}

	//벽이 있으면 true
	public void test(boolean east, boolean west, boolean south, boolean north, byte x, byte y) throws Exception {
		byte[] inBytes = new byte[3];
		mSocket.getOutputStream().write(this.getDirectionAndPositionAsBytes(east, west, south, north, x, y));
		mSocket.getOutputStream().flush();
		mSocket.getInputStream().read(inBytes);
		System.out.println(" 클:" + this.byteArrayToHex(inBytes));
	}

	public byte[] getDirectionAndPositionAsBytes(boolean east, boolean west, boolean south, boolean north, byte x,
			byte y) {
		byte direction = 0x0;
		if (east)
			direction += 0x1;
		if (west)
			direction += 0x2;
		if (south)
			direction += 0x4;
		if (north)
			direction += 0x8;

		return new byte[] { 0x4, 0x1, 0x0, 0x0, 0x0, 0x0, 0xf, direction, 0x1, 0x0, x, 0x0, y };
	}

	public static String byteArrayToHex(byte[] a) {
		StringBuilder sb = new StringBuilder();
		for (final byte b : a)
			sb.append(String.format("%02x ", b & 0xff));
		return sb.toString();
	}

	public static void main(String[] args) {

//		String ip = "172.20.10.14";
		String ip = "127.0.0.1";
		
		int port = 31000;

		try {
			new Client(ip, port);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
