#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>

#define BUFLEN 1024
#define PORT 6666

int main(int argc, char **argv)
{
    int sockfd;
    
    struct sockaddr_in s_addr;
    
    socklen_t len;
    
    unsigned int port;
    
    char buf[BUFLEN];
    
    fd_set rfds;
    
    struct timeval tv;
    
    int retval, maxfd; 
    



    /*1.    建立socket*/
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(errno);
    }else
        printf("socket create success!\n");



    
    /*2.    设置服务器ip*/
    memset(&s_addr,0,sizeof(s_addr));
    s_addr.sin_family = AF_INET;
     s_addr.sin_port = htons(PORT);
     /*
     inet_aton是一个改进的方法来将一个字符串IP地址转换为一个32位的网络序列IP地址。
     如果这个函数成功，函数的返回值非零，如果输入地址不正确则会返回零。使用这个函数并没有错误码存放在errno中，所以它的值会被忽略。
      */
    if (inet_aton(argv[1], (struct in_addr *)&s_addr.sin_addr.s_addr) == 0) {
        perror(argv[1]);
        exit(errno);
    }
    



    /*3.    自动开始连接服务器*/ 
    if(connect(sockfd,(struct sockaddr*)&s_addr,sizeof(struct sockaddr)) == -1){
        perror("connect");
        exit(errno);
    }else
        printf("********************conncet success!**********************\n");



    /*4.    开始传送数据*/
    printf("trains data begin.......\n");
    while(1){

        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        FD_SET(sockfd, &rfds);

        maxfd = 0;
        if(maxfd < sockfd)
            maxfd = sockfd;
        
        tv.tv_sec = 6;
        tv.tv_usec = 0;
        
        retval = select(maxfd+1, &rfds, NULL, NULL, &tv);//监听读
        if(retval == -1){
            printf("select fail，client will exit\n");
            break;
        }else if(retval == 0){
            printf("waiting...\n");
            continue;
        }else{
            
            if(FD_ISSET(sockfd, &rfds)){  //服务器端准备好
                //Test
                printf("server has receive data(the feedback should be --true--)\n");
                /******接收服务器返回的回馈消息*******/
                /*服务器每接收完一段数据都会有此回馈*/
                bzero(buf,BUFLEN);
                len = recv(sockfd,buf,BUFLEN,0);
                if(len > 0)
                    printf("feedback the server is:%s\n",buf);
                else{
                    if(len < 0 )
                        printf("feedback recvier data fail\n");
                    else
                        printf("feedback server exit\n");
                break; 
                }
            }
             if(FD_ISSET(0,&rfds)){ //客户端准备好了（空闲），表示服务端正在读取数据。客户端以将一次数据发送完可以开始第二次传送
                //Test
                //******发送数据*******/ 
                bzero(buf,BUFLEN);
                /*模拟数据传送（通过键盘输入）*/
                fgets(buf,BUFLEN,stdin);
               
                if(!strncasecmp(buf,"quit",4)){
                    printf("client ask quit(data trains complete)!\n");
                    break;
                }

                /*发送数据*/
                len = send(sockfd,buf,strlen(buf),0);
                if(len > 0)
                {
                    //Test
                    printf("send data successfully：%s\n",buf); 
                }
                else{
                    //Test
                    printf("send data fail!\n");
                    break; 
                } 
            }
        }
    
    }
    printf("trains complete\n");
    
    /*关闭连接*/
    close(sockfd);
    printf("*********************client shutdown**************\n");

    return 0;
}