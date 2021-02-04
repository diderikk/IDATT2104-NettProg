import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.*;
import java.util.Scanner;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;

public class AsyncClient {
    private static AsynchronousSocketChannel client;

    public static void main(String[] args) throws IOException, InterruptedException, ExecutionException {
        client = AsynchronousSocketChannel.open();
        Future<Void> future = client.connect(new InetSocketAddress("localhost", 8080));
        future.get();

        Scanner scanner = new Scanner(System.in);
        

        for(int i = 0; i<50; i++) {

            String response = sendMessage(scanner.nextLine());
            System.out.println(response);
        }
        client.close();

    }

    private static String sendMessage(String message) throws InterruptedException, ExecutionException {
        byte[] msg = message.getBytes();
        ByteBuffer buffer = ByteBuffer.wrap(msg);

        Future<Integer> writeResult = client.write(buffer);
        writeResult.get();
        buffer = ByteBuffer.allocate(256);
        Future<Integer> readResult = client.read(buffer);

        readResult.get();
        String echo = new String(buffer.array()).trim();
        buffer.clear();
        return echo;
    }
}
