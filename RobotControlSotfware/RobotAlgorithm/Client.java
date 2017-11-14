package robot_algorithm;

import java.io.BufferedReader;
import java.io.IOException;
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
		// test(true, true, true, true, (byte) 0x5, (byte) 0x5);

		// test(false, true, false, true, (byte) 5, (byte) 5);//1
		//// test(false, true, false, true, (byte) 5, (byte) 5);//error
		// test(false, false, false, true, (byte) 6, (byte) 5);//2
		// test(true, false, false, true, (byte) 7, (byte) 5);//3
		// test(true, true, true, false, (byte) 7, (byte) 4);//4
		//// test(false, true, false, true, (byte) 5, (byte) 5);//error
		// test(true, false, false, true, (byte) 7, (byte) 5);//3
		// test(false, false, false, true, (byte) 6, (byte) 5);//2
		// test(true, false, false, false, (byte) 6, (byte) 4);//5
		// test(true, true, true, false, (byte) 6, (byte) 3);//6
		// test(true, false, false, false, (byte) 6, (byte) 4);//5
		// test(false, true, true, false, (byte) 5, (byte) 4);//7

		// test(false, false, true, true, (byte) 5, (byte) 5);//1
		// test(true, false, false, true, (byte) 6, (byte) 5);//2
		// test(true, false, true, false, (byte) 6, (byte) 6);//3
		// test(false, true, true, false, (byte) 5, (byte) 6);//4
		// test(true, false, true, false, (byte) 6, (byte) 6);//3
		// test(true, false, false, true, (byte) 6, (byte) 5);//2
		// test(false, false, false, true, (byte) 5, (byte) 5);//1
		// test(false, true, true, true, (byte) 4, (byte) 5);//1

		// test(false, true, true, false, (byte) 5, (byte) 5);// 1
		// test(true, false, true, false, (byte) 6, (byte) 5);// 2
		// test(true, false, false, false, (byte) 6, (byte) 4);// 3
		// test(false, true, false, false, (byte) 5, (byte) 4);// 4
		// test(false, true, false, true, (byte) 5, (byte) 3);// 5
		// test(true, false, false, true, (byte) 6, (byte) 3);// 6

//		test(0, 1, 1, 0,(byte)5,(byte)5);// 1
//		test(0, 1, 1, 0,(byte)5,(byte)5);// 1 ==> 이동못하고 같은 장소 다시 보내는 상황
//		test(0, 0, 1, 0,(byte)6,(byte)5);// 2
//		test(0, 0, 1, 0,(byte)7,(byte)5);// 3
//		test(1, 0, 1, 0,(byte)8,(byte)5);// 4
//		test(1, 1, 0, 1,(byte)8,(byte)4);// 5
//		test(1, 0, 1, 0,(byte)8,(byte)5);// 4
//		test(0, 0, 1, 0,(byte)7,(byte)5);// 3
//		test(1, 0, 0, 0,(byte)7,(byte)4);// 6
//		test(0, 0, 0, 1,(byte)6,(byte)4);// 7
//		test(0, 1, 0, 0,(byte)5,(byte)4);// 8
//		test(1, 1, 0, 0,(byte)5,(byte)3);// 9
//		test(1, 1, 0, 0,(byte)5,(byte)2);// 10
//		test(0, 1, 0, 0,(byte)5,(byte)1);// 11
//		test(1, 0, 1, 0,(byte)6,(byte)1);// 12
//		test(0, 0, 0, 1,(byte)6,(byte)0);// 13
//		test(0, 0, 1, 1,(byte)7,(byte)0);// 14
//		test(1, 0, 0, 1,(byte)8,(byte)0);// 15
//		test(1, 1, 1, 0,(byte)8,(byte)1);// 16
//		test(1, 0, 0, 1,(byte)8,(byte)0);// 15
//		test(0, 0, 1, 1,(byte)7,(byte)0);// 14
//		test(0, 0, 0, 1,(byte)6,(byte)0);// 13
//		test(0, 1, 0, 1,(byte)5,(byte)0);// 21
//		test(0, 0, 0, 1,(byte)6,(byte)0);// 13
//		test(1, 0, 1, 0,(byte)6,(byte)1);// 12
//		test(0, 1, 0, 0,(byte)5,(byte)1);// 11
//		test(1, 1, 0, 0,(byte)5,(byte)2);// 10
//		test(1, 1, 0, 0,(byte)5,(byte)3);// 9
//		test(0, 1, 0, 0,(byte)5,(byte)4);// 8
//		test(0, 0, 0, 1,(byte)6,(byte)4);// 7
//		test(1, 0, 0, 0,(byte)7,(byte)4);// 6
//		test(1, 1, 0, 1,(byte)7,(byte)3);// 22
//		
//		testGetMapData();
//		test(0, 0, 0, 0,(byte)5,(byte)5);// 1
//		testGetMapData();
//		sendReset();
//		testGetMapData();
		
		//직진우선테스트
//		test(1, 1, 1, 0,(byte)5,(byte)5);// 1
//		test(0, 1, 0, 0,(byte)5,(byte)4);// 2
//		test(1, 1, 0, 1,(byte)5,(byte)3);// 3
//		test(0, 1, 0, 0,(byte)5,(byte)4);// 2
//		test(1, 0, 1, 1,(byte)6,(byte)4);// 4
		
		//skip first 알고리즘 테스트
		test(1, 1, 0, 0,(byte)5,(byte)5);// 1
		test(0, 1, 0, 0,(byte)5,(byte)6);// 2
		test(0, 0, 0, 1,(byte)6,(byte)6);// 3
		test(1, 0, 0, 1,(byte)7,(byte)6);// 4
		test(1, 0, 1, 0,(byte)7,(byte)7);// 5
		test(0, 0, 1, 0,(byte)6,(byte)7);// 6
		test(0, 1, 1, 0,(byte)5,(byte)7);// 7
		test(0, 1, 0, 0,(byte)5,(byte)6);// 2
		test(1, 1, 0, 0,(byte)5,(byte)5);// 1
		test(1, 1, 0, 1,(byte)5,(byte)4);// 8
		
		System.out.println("The End of Client");
		
		mIn.close();
		mOut.close();
		mSocket.close();
	}
	
	public void sendReset() throws IOException{
		byte[] inBytes = new byte[3];
		mSocket.getOutputStream().write(new byte[]{0x09, 0x01});
		mSocket.getOutputStream().flush();
		mSocket.getInputStream().read(inBytes);
		System.out.println(" 클:" + this.byteArrayToHex(inBytes));
	}

	public void testGetMapData() throws Exception{
		byte[] inBytes = new byte[11 * 11 * 8 + 1];
		mSocket.getOutputStream().write(new byte[]{0x01});
		mSocket.getOutputStream().flush();
		mSocket.getInputStream().read(inBytes);
		System.out.println(" 클:" + this.byteArrayToHex(inBytes));
	}
	// 벽이 있으면 true
	public void test(int east, int west, int south, int north, byte x, byte y) throws Exception {
		byte[] inBytes = new byte[3];
		mSocket.getOutputStream().write(this.getDirectionAndPositionAsBytes(east, west, south, north, x, y));
		mSocket.getOutputStream().flush();
		mSocket.getInputStream().read(inBytes);
		System.out.println(" 클:" + this.byteArrayToHex(inBytes));
	}

	public byte[] getDirectionAndPositionAsBytes(int east, int west, int south, int north, byte x, byte y) {
		byte direction = 0x0;
		if (east == 1)
			direction += 0x1;
		if (west == 1)
			direction += 0x2;
		if (south == 1)
			direction += 0x4;
		if (north == 1)
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

		// String ip = "172.20.10.14";
		String ip = "127.0.0.1";

		int port = 31000;

		try {
			new Client(ip, port);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
