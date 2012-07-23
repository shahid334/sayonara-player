import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;





public class SocketClient {

	Socket _s;
	PrintWriter _w;
	OutputStreamWriter _osw;
	
	public SocketClient(String host, int port) throws IOException{
		while(port < 1240){
			try{
				_s = new Socket(host, port);
				_w = new PrintWriter( _s.getOutputStream(), true);
				break;
			} catch(Exception e){
			
				port++;
				
			}
		}
		
		if(port == 1240) {
			System.out.println("Cannot connect");
			System.exit(1);
		}
		
		System.out.println("Connected to port: " + port);
	}
	

	private void write(String text) {
		
		if(_s.isClosed()) {
			System.out.println("Socket is closed");
			return;
		}
		
		_w.println(text + '\0');
		
		System.out.println(text);
	}
	
	
	public void nextSong(){
		write("next");
	}
	
	public void prevSong(){
		write("prev");
	}
	
	public void play(){
		write("play");
	}
	
	public void pause(){
		write("pause");
	}
	
	public void stop(){
		write("stop");
	}
	
	public void louder(){
		write("vol+");
	}
	
	public void leiser(){
		write("vol-");
	}
	
	public void quit(){
		write("q");
	}
	
	public void close_socket(){
		try {
			if(!_s.isClosed()){
				quit();
				_s.close();
			}
		} catch (IOException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		}
	
	}
	
	

}
