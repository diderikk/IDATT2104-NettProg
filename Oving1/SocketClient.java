import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Scanner;

public class SocketClient {
    public static void main(String[] args) throws UnknownHostException, IOException {
        final int PORT = 8080;
        Socket conn = new Socket("localhost",PORT);
        System.out.println("Connected to localhost");

        InputStreamReader inputConn = new InputStreamReader(conn.getInputStream());
        BufferedReader input = new BufferedReader(inputConn);
        PrintWriter output = new PrintWriter(conn.getOutputStream(), true);

        System.out.println(input.readLine());
        System.out.println(input.readLine());

        Scanner scanner = new Scanner(System.in);

        String inputLine = scanner.nextLine();
        while(!inputLine.equals("")){
            output.println(inputLine);
            System.out.println(input.readLine());
            inputLine = scanner.nextLine();
        }

        scanner.close();
        input.close();
        output.close();
        conn.close();
    }
}
