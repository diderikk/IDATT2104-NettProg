import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;

public class WebServer {
    public static void main(String[] args) throws IOException {
        ArrayList<String> headerList = new ArrayList<>();
        final int PORT = 80;
        ServerSocket server = new ServerSocket(PORT);
        System.out.println("Server listening on port: " + PORT + "...");
        Socket conn = server.accept();

        InputStreamReader inputConn = new InputStreamReader(conn.getInputStream());
        BufferedReader input = new BufferedReader(inputConn);
        PrintWriter output = new PrintWriter(conn.getOutputStream(), true);

        String inputLine = input.readLine();
        while(!inputLine.equals("")){
            System.out.println(inputLine);
            headerList.add("<LI>"+inputLine+"</LI>");
            inputLine = input.readLine();
        }

        output.println("HTTP/1.0 200 OK\nContent-Type: text/html; charset=utf-8\n\n");
        output.println("<HTML><BODY>\n<H1>Du har koblet deg opp til min enkle web-tjener </h1>");
        output.println("<H2>Headere fra klient: </H2>\n<UL>");
        for(String str : headerList) output.println(str);
        output.println("</UL>");
        output.println("</BODY></HTML>");        
        

        conn.close();

        output.close();
        input.close();
        server.close();
    }
}
