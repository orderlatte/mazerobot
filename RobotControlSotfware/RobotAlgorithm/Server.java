package robot_algorithm;

import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Arrays;

public class Server {

	public Server() throws Exception {
		System.out.println("Maze Solver Server Ready. Wait on port 31000");
		Socket sock = new ServerSocket(31000).accept();
		this.response(sock);
		sock.close();
	}

	private void response(Socket sock) throws Exception {
		InputStream in = sock.getInputStream();
		OutputStream out = sock.getOutputStream();

		MazeSolverAlgorithm solver = new MazeSolver_DepthFirst();
		solver.init();

		byte[] bytes = new byte[13];
		while (true) {
			in.read(bytes);
			System.out.println("==================================");
			System.out.println("From Robot:" + Client.byteArrayToHex(bytes));
			process(bytes, out, solver, sock);			
			if(sock.isClosed()) break;
		}
	}

	private void process(byte[] bytes, OutputStream out, MazeSolverAlgorithm solver, Socket sock) throws Exception {

		switch (bytes[0]) {
		case 0x1:// full map 요청
			byte[] mazeBytes = solver.getMaze().getMazeAsBytes();
			out.write(mazeBytes);
			out.flush();
			break;
		case 0x4:// 다음 방향 요청
			try {
				solver.getMaze().setCell(Arrays.copyOfRange(bytes, 1, 13));
				byte[] rv = solver.getNext();
				System.out.println("Next Direction: " + Client.byteArrayToHex(rv));
				out.write(rv);
				out.flush();
			} catch (Exception e) {
				out.write(MazeSolverAlgorithm.ERROR);
				out.flush();
			}
			break;
		case 0x9:// reset 요청
			solver.init();
			out.write(MazeSolverAlgorithm.INIT_OK);			
			out.flush();
			System.out.println("==============INIT_OK=================");
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
