import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

import javax.net.ssl.SSLServerSocketFactory;
//java -Djavax.net.ssl.keyStore=/mnt/c/Users/dider/NettProg/Oving2/examplestore -Djavax.net.ssl.keyStorePassword=password TLSServer
public class TLSServer {
    private static final int PORT = 4545;

    public static void main(String[] args) throws IOException {
        SSLServerSocketFactory sslServerSocketFactory = 
        (SSLServerSocketFactory) SSLServerSocketFactory.getDefault();

        ServerSocket sslServerSocket = sslServerSocketFactory.createServerSocket(PORT);
        System.out.println("SSL server running on port: " + PORT + "...");
        System.out.println(sslServerSocket.toString());

        Socket conn = sslServerSocket.accept();
        System.out.println("Server connected");
        
        PrintWriter output = new PrintWriter(conn.getOutputStream(), true);
        BufferedReader input = new BufferedReader(new InputStreamReader(conn.getInputStream()));

        String line = input.readLine();
        while(line != null){
            System.out.println(line);
            output.println(line);
            line = input.readLine();
        }
        sslServerSocket.close();
    }
}
