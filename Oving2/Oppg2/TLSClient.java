import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Scanner;

import javax.net.ssl.SSLSocketFactory;
//java -Djavax.net.ssl.trustStore=/mnt/c/Users/dider/NettProg/Oving2/examplestore -Djavax.net.ssl.trustStorePassword=password TLSClient
public class TLSClient {
    private static final int PORT = 4545;

    public static void main(String[] args) throws IOException {
        SSLSocketFactory sslSocketFactory = (SSLSocketFactory)SSLSocketFactory.getDefault();
        Socket conn = sslSocketFactory.createSocket("localhost", PORT);
        PrintWriter output = new PrintWriter(conn.getOutputStream(), true);
        BufferedReader input = new BufferedReader(new InputStreamReader(conn.getInputStream()));
        Scanner scanner = new Scanner(System.in);
        while(true){
            System.out.println("Enter something:");
            String inputLine = scanner.nextLine();
            if(inputLine.equals("q")){
                break;
            }
            
            output.println(inputLine);
            System.out.println(input.readLine());
        }

        conn.close();
        scanner.close();
    }
}
