////
//// Created by Chenglinli on 2022/4/26.
////
//#include <cstdio>
//#include <string>
//#include <cstdlib>
//#include <unistd.h>
//#include <arpa/inet.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <iostream>
//
//#define BUFFER_SIZE 100
//
//int main(){
//    //创建套接字
////    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//    int serv_sock = socket(AF_INET, SOCK_STREAM, 0); // AF_INET：表示IPv4
//                                                     // SOCK_STREAM：表示流套接字
//                                                     // 0：表示具体使用的协议由系统自动推演
//
//    //将套接字和IP、端口绑定
//    sockaddr_in serv_addr;
//    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
//    serv_addr.sin_family = AF_INET;  //使用IPv4地址
//    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
//    serv_addr.sin_port = htons(1234);  //端口
//    bind(serv_sock, (sockaddr*)&serv_addr, sizeof(serv_addr));
//
//    //进入监听状态，等待用户发起请求
//    listen(serv_sock, 20);
//
//    // 查看服务端缓冲区大小
//    unsigned optVal;
//    socklen_t optLen = sizeof(int);
//    getsockopt(serv_sock, SOL_SOCKET, SO_SNDBUF, (char*)&optVal, &optLen);
//    printf("Buffer length: %d\n", optVal); // 131072 == 128k
//
//    //接收客户端请求
//    sockaddr_in clnt_addr;
//    socklen_t clnt_addr_size = sizeof(clnt_addr);
//
//    // 持续监听
//    while(true){
//        int clnt_sock = accept(serv_sock, (sockaddr*)&clnt_addr, &clnt_addr_size);
//
//        // 读取客户端发送来的信息
//        char buffer[BUFFER_SIZE];
//        int strLen = read(clnt_sock, buffer, BUFFER_SIZE);
//        std::cout << "用户数据是：" << buffer << std::endl;
//
//        // 回传给用户信息
//        write(clnt_sock, buffer, strLen);
//
//        shutdown(clnt_sock, SHUT_WR);  //文件读取完毕，断开输出流，向客户端发送FIN包（保证输出缓冲区的数据能够正确被发送）
//        recv(clnt_sock, buffer, BUFFER_SIZE, 0);  //阻塞，等待客户端接收完毕,用于接受FIN包
//
//        close(clnt_sock);
//        memset(buffer, 0, BUFFER_SIZE);
//    }
//
//
//
//    //关闭套接字
//
//    close(serv_sock);
//
//    return 0;
//}
//
