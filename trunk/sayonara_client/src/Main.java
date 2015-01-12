import java.io.IOException;


public class Main {

	

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		
		int port = 1234;
		if(args.length >= 2)
			port = Integer.parseInt(args[1]);
		
		if(port <= 0) port = 1024;
	
		System.out.println("Adress = " + args[0] + ", Port = " + port);
		
		try {
			SocketClient client = new SocketClient(args[0], port);
			AppWindow gui = new AppWindow(client);
			
		} catch (IOException e) {
			e.printStackTrace();
		}
		

		
	}
}
