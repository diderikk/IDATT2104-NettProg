import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class UDPClientHandler extends Thread {
    private DatagramSocket socket;
    private final int PORT = 4445;

    public UDPClientHandler() throws IOException {
        this.socket = new DatagramSocket(PORT);
    }

    public void run() {
        System.out.println("Server is listening on port: " + PORT + "...");
        try {
            // Recieves 50 packets before it closes.
            for (int i = 0; i < 50; i++) {
                byte[] buffer = new byte[256];
                // First packet som not contain any information,
                // still need to recieve it to find packet destination
                DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
                socket.receive(packet);
                String inputLine = new String(packet.getData(), 0, packet.getLength());
                System.out.println("Client wrote : " + inputLine);

                String response = parseProtocall(inputLine);
                buffer = response.getBytes();

                // To send the response we need to find address and port from the sender.
                InetAddress address = packet.getAddress();
                int port = packet.getPort();
                packet = new DatagramPacket(buffer, buffer.length, address, port);
                socket.send(packet);
            }

        } catch (IOException ex) {
            ex.printStackTrace();
        }
        socket.close();
    }

    // Same as last oving
    private String parseProtocall(String inputLine) {

        // Protocall: operation num num. Only have add and subtract.
        try {
            int first = inputLine.indexOf(' ');
            int second = inputLine.indexOf(' ', first + 1);
            String operation = inputLine.substring(0, first);

            if (!operation.toLowerCase().equals("add") && !operation.toLowerCase().equals("subtract")) {
                throw new IllegalArgumentException("Operation value is invalid");
            }

            int num1 = Integer.parseInt(inputLine.substring(first + 1, second));
            int num2 = Integer.parseInt(inputLine.substring(second + 1, inputLine.length()));
            return operation + " " + num1 + " " + num2 + " = " + Operation.doOperation(num1, num2, operation);
        } catch (NumberFormatException ex) {
            return ("Number(s) is in the wrong format");
        } catch (IllegalArgumentException ex) {
            return (ex.getLocalizedMessage());
        } catch (Exception ex) {
            return "Could not parse the request";
        }
    }

}
