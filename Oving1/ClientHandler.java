import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;


public class ClientHandler extends Thread {
    private Socket conn;
    private BufferedReader input;
    private PrintWriter output;

    public ClientHandler(Socket conn) throws IOException {
        this.conn = conn;
        InputStreamReader inputConn = new InputStreamReader(conn.getInputStream());
        this.input  = new BufferedReader(inputConn);
        this.output  = new PrintWriter(conn.getOutputStream(), true);
    }

    public void run(){
        try{
            output.println("You have connected. Write workload");
            output.println("Protocall: operation num num (add/subtract 2 2):");

            String inputLine = input.readLine();    
            while(inputLine != null){
                System.out.println("Client wrote: " + inputLine);
                parseProtocall(inputLine);
                inputLine = input.readLine();
            }

            input.close();
            output.close();
            conn.close();
        }catch(IOException ex){
            ex.printStackTrace();
        }
    }

    private void parseProtocall(String inputLine){
        int first = inputLine.indexOf(' ');
        int second = inputLine.indexOf(' ', first+1);
        String operation = inputLine.substring(0,first);


        try{
            if(!operation.toLowerCase().equals("add") && !operation.toLowerCase().equals("subtract")){
                throw new IllegalArgumentException("Operation value is invalid");
            }

            int num1 = Integer.parseInt(inputLine.substring(first+1,second));
            int num2 = Integer.parseInt(inputLine.substring(second+1, inputLine.length()));
            output.println(operation + " " + num1 + " " + num2 + " = " + Operation.doOperation(num1, num2, operation));
        }catch(NumberFormatException ex){
            output.println("Number(s) is in the wrong format");
        }catch(IllegalArgumentException ex){
            output.println(ex.getLocalizedMessage());
        }
    }
    
}
