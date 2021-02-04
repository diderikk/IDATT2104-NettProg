import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.*;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;

class AsyncServer {
    public static void main(String[] args) throws IOException, InterruptedException, ExecutionException {
        AsynchronousServerSocketChannel server = AsynchronousServerSocketChannel.open();
        server.bind(new InetSocketAddress("localhost",8080));
        System.out.println(server.getLocalAddress());

        Future<AsynchronousSocketChannel> acceptFuture = server.accept();
        AsynchronousSocketChannel worker = acceptFuture.get();
        System.out.println("Connection created");

        if((worker != null) && (worker.isOpen())){
            while(true){
                ByteBuffer buffer = ByteBuffer.allocate(256);
                Future<Integer> readResult = worker.read(buffer);

                

                readResult.get();

                System.out.println(new String(buffer.array()).trim());


                byte[] response = "Welcome".getBytes();
                buffer = ByteBuffer.wrap(response);

                Future<Integer> writeResult = worker.write(buffer);
                writeResult.get();
                buffer.clear();

            }
        }
        worker.close();
        server.close();
    }
}