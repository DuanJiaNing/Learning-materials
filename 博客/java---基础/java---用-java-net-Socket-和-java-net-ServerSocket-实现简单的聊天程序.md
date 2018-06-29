![](http://upload-images.jianshu.io/upload_images/7460499-8a8d1d9935b72069.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
用`java.net.Socket`和`java.net.ServerSocket`实现简单的聊天程序

>思路是这样的：
假设用户A与用户B进行聊天，用户B端使用ServerSocket作为服务端，而用户A使用Socket与用户B进行通信。

- 这两个小程序需要运行在同一台电脑上，才能正常通信。
- 运行时一定要先运行`Server.class`再运行`Client.class`。
- 通过异常来控制程序逻辑不是首选方式，但想了很久也没想出其它方法 **`>.<`**

##### 用户A的代码如下（`Client.java`）
``` java
import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Created by DuanJiaNing on 2017/3/16.
 */
public class Client {

    public static void main(String[] args) {
        try {
            //创建Socket连接到本机的2003端口
            Socket socket = new Socket(InetAddress.getLocalHost(), 2003);
            println("**********您正在和：" + InetAddress.getLocalHost().getHostAddress() + " 聊天***********\n" + "输入信息，按回车即可发送!输入quit可结束会话并关闭程序。");
            OutputStream out = socket.getOutputStream();
            out.write("Hello 在么？".getBytes());

            //创建新线程负责打印接收到的消息
            new Thread(new ClientThread(socket.getInputStream())).start();

            //接收控制台输入
            BufferedReader bufr = new BufferedReader(new InputStreamReader(System.in));
            //读取控制台输入，有输入且不为quit时进行发送
            String line;
            while ((line = bufr.readLine()) != null && !line.equals("quit")) {
                out.write(line.getBytes());
            }
            //关闭套接字
            //程序结束 这将导致ClientThread抛java.net.SocketException: Socket closed异常而结束（暂且就这样关闭这个线程）
            socket.close();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    static class ClientThread implements Runnable {
        InputStream input;

        public ClientThread(InputStream in) {
            this.input = in;
        }

        public void run() {
            byte[] bytes = new byte[1024];
            int len;
            try {
                //当收到消息时把消息打印到控制台
                while ((len = input.read(bytes)) != -1) {
                    println(new String(bytes, 0, len));
                }
            } catch (Exception e) {
                //当服务端或客户端结束会话时（输入quit时）会关闭socket从而抛出java.net.SocketException: Socket closed异常
                //隐藏此异常不做处理让程序继续“正常”运行(正常结束)
                if (!(e instanceof SocketException))
                    e.printStackTrace();
            }
            println("**********此次会话结束**********");
            System.exit(0);
        }
    }

    private static void println(String msg) {
        Date date = new Date();
        DateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss ");
        System.out.println(format.format(date) + msg);
    }

}
```
-----
##### 用户B代码（服务端）（`Server.java`）
``` java
import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Created by DuanJiaNing on 2017/3/16.
 */
public class Server {

    private static BufferedReader reader;

    public static void main(String[] args) {

        //监听本机（服务器）的2003端口
        ServerSocket serverSocket = null;
        try {
            serverSocket = new ServerSocket(2003);
        } catch (IOException e) {
            e.printStackTrace();
        }
        while (true) {

                try {
                    println("************当前没有正在进行的会话***********");

                    //accept为阻塞时方法，当有连接时会返回连接着的socket对象
                    Socket socket = serverSocket.accept();
                    println("当前聊天对象来自：" + socket.getLocalAddress().getHostAddress() + "\n" + "输入信息，按回车即可发送!输入quit可结束此次会话。");

                    //创建新线程负责打印接收到的消息
                    InputStream in = socket.getInputStream();
                    new Thread(new ServerThread(in)).start();

                    //接收输入并发送给连接者
                    reader = new BufferedReader(new InputStreamReader(System.in));
                    String line;
                    OutputStream out = socket.getOutputStream();
                    while ((line = reader.readLine()) != null && !line.equals("quit")) {
                        out.write(line.getBytes());
                    }
                    //服务端主动结束会话
                    out.write("先不聊了。bye~".getBytes());

                    //关闭套接字
                    //程序结束 这将导致ServerThread抛java.net.SocketException: Socket closed异常而结束（暂且就这样关闭此次会话对应的线程）
                    socket.close();

                } catch (Exception e) {
                    //当客户端主动结束会话（输入quit时），而服务端试图发送信息时会抛出java.net.SocketException: Software caused connection abort: socket write error
                    //借这个异常让服务端回到等待客户端接入状态
                    if (e instanceof SocketException) {
                        println("您的聊天对象已离线\n");
                        continue;
                    }
                    else
                        e.printStackTrace();
                }
            }
    }

    static class ServerThread implements Runnable {
        InputStream input;

        public ServerThread(InputStream in) {
            this.input = in;
        }

        public void run() {
            byte[] bytes = new byte[1024];
            int len;
            try {
                //当收到消息时把消息打印到控制台
                while ((len = input.read(bytes)) != -1) {
                    println(new String(bytes, 0, len));
                }
            } catch (Exception e) {
                //当服务端或（输入quit时）会关闭socket从而抛出java.net.SocketException: Socket closed异常
                //当服务端输入quit时隐藏此异常不做处理让程序继续运行（回到等待客户端接入的状态）
                if (!(e instanceof SocketException)) {
                    e.printStackTrace();
                }
            }

        }
    }

    private static void println(String msg) {
        Date date = new Date();
        DateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss ");
        System.out.println(format.format(date) + msg);
    }
}

```
----
##### 编译运行
- 分别编译Server.java，Client.java文件
``` dos
javac Server.java
javac Client.java
```
- **先后**运行Server.class和Client.class
``` dos
java Server
java Client
```
- 此时可看到：
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-95237bd2ac5f1023?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
<br>
- 接下来进行简单对话后**在服务端输入`quit`**可看到：
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-3f3ead195c84cdc2?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
<br>
- 此时客户端已经关闭，而服务端会循环阻塞在accept方法上。此时只需**再次运行Client.class**（在dos终端切换到Client.class文件所在路径后输入`java Client`）就能再次连接到服务端
<br>
- 服务端主动结束会话，此时时这样的：
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-ac1e41a65687ee44?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
 ---------
