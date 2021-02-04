import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class SocketServer {
    public static void main(String[] args) throws IOException {
        final int PORT = 8080;
        ServerSocket server = new ServerSocket(PORT);
        System.out.println("Server listening on port: " + PORT + "...");

        for(int i = 0; i<200; i++){
            Socket conn = server.accept();

            ClientHandler thread = new ClientHandler(conn);
            thread.start();
        }

        server.close();

        
        
    }
}