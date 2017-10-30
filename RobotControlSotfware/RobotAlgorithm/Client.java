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

	public Client(String ip, int port) {
		try {
			// ������ ��û ������
			mSocket = new Socket(ip, port);
			System.out.println("Ŭ:" + ip + " �����");

			// ��� �ձ�
			mIn = new BufferedReader(new InputStreamReader(mSocket.getInputStream()));
			mOut = new PrintWriter(mSocket.getOutputStream());

			// �޼��� ����,
			byte[] inBytes = new byte[400];

			byte[] outByte = new byte[] { 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xb, 0x1, 0x0, 0x2, 0x0, 0x2 };
			mSocket.getOutputStream().write(outByte);
			mSocket.getOutputStream().flush();
			int length = mSocket.getInputStream().read(inBytes);
			System.out.println("Ŭ:" + length);

			outByte = new byte[] { 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf, 0x6, 0x1, 0x0, 0x2, 0x0, 0x3 };
			mSocket.getOutputStream().write(outByte);
			mSocket.getOutputStream().flush();
			length = mSocket.getInputStream().read(inBytes);
			System.out.println("Ŭ:" + length);

			outByte = new byte[] { 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf, 0x5, 0x1, 0x0, 0x3, 0x0, 0x3 };
			mSocket.getOutputStream().write(outByte);
			mSocket.getOutputStream().flush();
			length = mSocket.getInputStream().read(inBytes);
			System.out.println("Ŭ:" + length);

			outByte = new byte[] { 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf, 0x3, 0x1, 0x0, 0x3, 0x0, 0x2 };
			mSocket.getOutputStream().write(outByte);
			mSocket.getOutputStream().flush();
			length = mSocket.getInputStream().read(inBytes);
			System.out.println("Ŭ:" + length);

			outByte = new byte[] { 0x1 };
			mSocket.getOutputStream().write(outByte);
			mSocket.getOutputStream().flush();
			length = mSocket.getInputStream().read(inBytes);
			System.out.println("Ŭ:" + length);

		} catch (IOException e) {
		} finally {
			try {
				mSocket.close();
			} catch (IOException e) {
				System.out.println(e.getMessage());
			}
		}
	}

	public static void main(String[] args) {

		String ip = "127.0.0.1";
		int port = 19000;

		new Client(ip, port);
	}
}
