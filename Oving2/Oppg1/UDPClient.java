import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.Scanner;

class UDPClient {
    private static byte[] buffer;
    private static final int PORT = 4445;
    private static Scanner scanner = new Scanner(System.in);

    public static void main(String[] args) throws IOException {
        //UDP requires us to send independent packages to the server as
        //we don't create a end-to-end connection.
        for(int i = 0; i<50; i++) {
            //Input from is required to be in the protocall:
            //operation number number. Ex add 2 2 or subtract 2 2. 
            String inputLine = scanner.nextLine();

            DatagramSocket socket = new DatagramSocket();
            buffer = inputLine.getBytes();
            InetAddress address = InetAddress.getLocalHost();
            DatagramPacket packet = new DatagramPacket(buffer, buffer.length, address, PORT);
            socket.send(packet);

            //Recieves the response from the server.
            buffer = new byte[256];
            packet = new DatagramPacket(buffer, buffer.length);
            socket.receive(packet);
            String receive = new String(packet.getData(), 0, packet.getLength());
            System.out.println(receive);
            socket.close();

        }
        scanner.close();

    }

}