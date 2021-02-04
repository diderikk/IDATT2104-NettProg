import java.io.IOException;

class UDPServer {
    public static void main(String[] args) throws IOException {
        UDPClientHandler serverThread = new UDPClientHandler();
        serverThread.start();
    }
}