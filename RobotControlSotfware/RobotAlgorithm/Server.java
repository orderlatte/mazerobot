package robot_algorithm;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Arrays;

public class Server {

	Maze maze = new Maze();
	MazeSolver solver = new MazeSolver(maze);

	public Server() throws Exception {

		System.out.println("����: Wating Connect ..");
		Socket sock = new ServerSocket(19000).accept();

		InetAddress inetaddr = sock.getInetAddress();
		System.out.println("����:connecting from " + inetaddr.getHostAddress());

		PrintWriter pw = new PrintWriter(new OutputStreamWriter(sock.getOutputStream()));
		this.response(sock);

		sock.close();
	}

	private void response(Socket sock) throws Exception {
		InputStream in = sock.getInputStream();
		OutputStream out = sock.getOutputStream();
		byte[] bytes = new byte[13];

		while (true) {
			in.read(bytes);
			process(bytes, out);
		}
	}

	private void process(byte[] bytes, OutputStream out) throws Exception {
		switch (bytes[0]) {
		case 0x1:// full map ��û
			byte[] mazeBytes = maze.getMazeAsBytes();
			out.write(mazeBytes);
			out.flush();
			break;
		case 0x4:// ���� ���� ��û
			maze.setMaze(Arrays.copyOfRange(bytes, 1, 13));
			out.write(solver.getNext());
			out.flush();
			break;
		}
	}

	public static void main(String[] args) {
		try {
			new Server();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
