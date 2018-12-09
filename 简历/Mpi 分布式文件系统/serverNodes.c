
/*
如果fork执行成功，就向父进程返回子进程的PID，并向子进程返回0。这就一起这即使你只调用fork一次，他也会返回两次。
 */
//pid_t fork(void);
/*
1、PID（process ID）：
	PID是程序被操作系统加载到内存成为进程后动态分配的资源。
	每次程序执行的时候，操作系统都会重新加载，PID在每次加载的时候都是不同的。
2、PPID（parent process ID）：PPID是程序的父进程号。
3、PID和PPID都是非零的整数。
4、PID是唯一的，一个PID只标识一个进程。
5、一个进程创建的另一个新进程称为子进程。相反地，创建子进程的进程称为父进程。
6、对于一个普通的用户进程，它的父进程就是执行它的哪个Shell，对于Linux而言，Shell就是bash。
7、bash所在的目录：[negivup@localhost bin]$ cd /bin | ls bash

GID为GroupId，即组ID，用来标识用户组的唯一标识符
UID为UserId，即用户ID，用来标识每个用户的唯一标示符

实际上，当计算机开机的时候，内核(kernel)只建立了一个init进程。Linux内核并不提供直接建立新进程的系统调用。
剩下的所有进程都是init进程通过fork机制建立的。新的进程要通过老的进程复制自身得到，这就是fork。fork是一个系统调用。
进程存活于内存中。每个进程都在内存中分配有属于自己的一片空间 (address space)。当进程fork的时候，Linux在内存中开辟
出一片新的内存空间给新的进程，并将老的进程空间中的内容复制到新的空间中，此后两个进程同时运行。

老进程成为新进程的父进程(parent process)，而相应的，新进程就是老的进程的子进程(child process)。一个进程除了有一个PID之外，
还会有一个PPID(parent PID)来存储的父进程PID。如果我们循着PPID不断向上追溯的话，总会发现其源头是init进程。所以说，所有的进
程也构成一个以init为根的树状结构

fork通常作为一个函数被调用。这个函数会有两次返回，将子进程的PID返回给父进程，0返回给子进程。实际上，子进程总可以查询自己的PPID
来知道自己的父进程是谁，这样，一对父进程和子进程就可以随时查询对方。

通常在调用fork函数之后，程序会设计一个if选择结构。当PID等于0时，说明该进程为子进程，那么让它执行某些指令,比如说使用exec库函数
(library function)读取另一个程序文件，并在当前的进程空间执行 (这实际上是我们使用fork的一大目的: 为某一程序创建进程)；而当PID
为一个正整数时，说明为父进程，则执行另外一些指令。由此，就可以在子进程建立之后，让它执行与父进程不同的功能。

当子进程终结时，它会通知父进程，并清空自己所占据的内存，并在内核里留下自己的退出信息(exit code，如果顺利运行，为0；如果有错误或异常状况，
为>0的整数)。在这个信息里，会解释该进程为什么退出。父进程在得知子进程终结时，有责任对该子进程使用wait系统调用。这个wait函数能从内核中取出
子进程的退出信息，并清空该信息在内核中所占据的空间。但是，如果父进程早于子进程终结，子进程就会成为一个孤儿(orphand)进程。孤儿进程会被过继
给init进程，init进程也就成了该进程的父进程。init进程负责该子进程终结时调用wait函数。

当然，一个糟糕的程序也完全可能造成子进程的退出信息滞留在内核中的状况（父进程不对子进程调用wait函数），这样的情况下，子进程成为僵尸（zombie）
进程。当大量僵尸进程积累时，内存空间会被挤占。

exec函数族
与fork函数一样，exec也在<unistd.h>中声明。它的原型为：
int execl(const char *path, const char* arg, ...);
int execlp(const char *file, const char* arg, ...);
int execle(const char *path, const char* arg , ..., char * const envp[]);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execve(const char *path, char *const argv[], char *const envp[]);
exec用被执行的程序完全替换了调用进程的映像。Fork创建了一个新进程就产生了一个新的PID，exec启动一个新程序，替换原有进程。因此被执行的进程的PID不会改变。
例如：
int execve(const char *path, char *const argv[], char *const envp[]);
接收三个参数：
path是要执行的二进制文件或脚本的完整路径。
argv是要传递给程序的完整参数列表，包括argv[0]，它一般是执行程序的名字，
envp是指向用于执行execed程序的专门环境的指针。
这几个函数可以简单讨论如下：
名字中含有l的函数：希望接受以逗号分隔的参数列表，列表以NULL指针作为结束标志，这些参数将传递给被执行的程序。
名字中包v的函数：则接受一个向量，也就是以空结尾的字符串的指针数组。这个数组必须以一个NULL指针作为结束标志。
不过，需要注意的是，有时候可能那个NULL，需要写成(char *)0。


线程

在编译的时候需要注意，由于线程创建函数在libpthread.so库中，所以在编译命令中需要将该库导入。命令如下：
gcc –o createthread –lpthread createthread.c

如果想传递参数给线程函数，可以通过其参数arg，其类型是void *。如果你需要传递多个参数的话，可以考虑将这些参数组成一个结构体来传递。另外，
由于类型是void *，所以你的参数不可以被提前释放掉。

下面一个问题和前面创建线程类似，不过带来的问题回避进程要严重得多。如果你的主线程，也就是main函数执行的那个线程，在你其他线程推出之前就已经退出，
那么带来的bug则不可估量。通过pthread_join函数会让主线程阻塞，直到所有线程都已经退出。
pthread_join:使一个线程等待另一个线程结束。
代码中如果没有pthread_join主线程会很快结束从而使整个进程结束，从而使创建的线程没有机会开始执行就结束了。加入pthread_join后，主线程会一直等待直
到等待的线程结束自己才结束，使创建的线程有机会执行。
thread：等待退出线程的线程号。
value_ptr：退出线程的返回值。


int pthread_create(pthread_t *restrict thread,const pthread_attr_t *restrict attr,void *(*start_routine)(void*), void *restrict arg);
thread：所创建的线程号。
attr：所创建的线程属性，这个将在后面详细说明。
start_routine：即将运行的线程函数。
art：传递给线程函数的参数。

 */
#include <sys/types.h> 
#include <stdio.h> 
#include <stdlib.h> 

#include <string.h> 
#include <errno.h> 

#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#include <unistd.h> 
#include <sys/time.h> 
#include <pthread.h>

#include <sys/stat.h> //文件读写
#include <fcntl.h>

#include <time.h>
  
#define BUFLEN 1024 
#define PORT 6666
#define LISTNUM 20
#define MAX_CONNECT_COUNT 50

#define DIR "mpi_file_dir"
#define LOGFILE "log_file.txt"

/*
此方法在新的线程中运行，传入“已连接套接字描述符”线程执行后可与发送
端（客户端）配合，客户端每次发送一定大小的数据，该线程则每次接收相
应大小的数据，并实时把数据写入到本地。接收完成且写入完成时关闭线程
和“已连接套接字描述符”
 */
void handData();
/**
 * 把buf中长度为len的数据写入到本地
 */
void writeDownData(char buf[],int len);

/*
创建服务器运行日志文件到本地
*/
void catlog(char *where,char *what);

/*
服务器启动时检查是否存在需要的文件夹和日志文件
返回-1服务器启动失败
*/
int init();


int main() 
{ 
    int sockfd, newfd; 
    
    struct sockaddr_in s_addr, c_addr; 
    
    char buf[BUFLEN]; 
    
    socklen_t len; 
    
	//listnum为listener函数第二个参数决定了未完成队列和已完成队列中连接数目之和的最大值
    unsigned int port, listnum; 
    
    //struct timeval是一个大家常用的结构，用来代表时间值，有两个成员，一个是秒数，另一个是微妙数。
    struct timeval tv; 

    //记录有多少连接创立，主要用于连接建立并创建线程处理时 在服务器（主线程）关闭时避免子线程还未关闭而可能导致的bug
    int connect_count[MAX_CONNECT_COUNT]; 
    pthread_t thread_id;
    int tc,i;
      
	 if(init() == -1)
	{
		 printf("server start fail,check the server log for help\n");
		 return;
	}

    /*1.    建立socket*/ 
    if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1){ 
        //perror(s) 用来将上一个函数发生错误的原因输出到标准设备(stderr)
        perror("socket"); 
        exit(errno); 
    }else 
        printf("socket create success!\n"); 
    
    


    /*2.    绑定地址族（底层的通信协议TCP/IPv4...），端口，网络接口*/
    //将s_addr指向内存空间的前sizeof(s_addr)个字节的内容全置为第二个参数指定的值（这里为0）
    //这个函数通常为新申请的内存做初始化工作， 其返回值为指向s的指针。该函数对数组操作时只能用于数组的置0或-1，其他值无效。
    memset(&s_addr,0,sizeof(s_addr)); 
    s_addr.sin_family = AF_INET; 
    s_addr.sin_port = htons(PORT); 
    s_addr.sin_addr.s_addr = htons(INADDR_ANY); 
    

    

    /*3.    把地址和端口结构体帮定到套接字上（把一个本地协议地址（s_addr）赋予给一个套接字）*/ 
    if((bind(sockfd, (struct sockaddr*) &s_addr,sizeof(struct sockaddr))) == -1){ 
        perror("bind"); 
        exit(errno); 
    }else 
        printf("bind success!\n"); 
    



    /*4.    由TCP服务器调用侦听本地端口（默认的主动套接字转换为监听套接字）*/ 
    if(listen(sockfd,listnum) == -1){ 
        perror("listen"); 
        exit(errno); 
    }else 
        printf("the server is listening!\n"); 
    printf("*****************server start***************\n"); 




    /*5.    无限循环阻塞式不断检测连接*/
    tc = 0;
    while(1){  //不断从已完成连接队列中取得已成功连接的描述符
    	printf("check connection....\n");

        /*5.1   accept函数由TCP服务器调用，用于从已完成连接（完成三路握手的连接）
		队列的队头返回下一
        个已完成的连接。如果已完成的队列为空，那么就投入睡眠*/
        len = sizeof(struct sockaddr); 
        if((newfd = accept(sockfd,(struct sockaddr*) &c_addr, &len)) == -1){ 
            perror("accept"); 
            exit(errno); 
        }else 
        {
            //Test
           printf("successfully connect with ：%s: %d newfd = %d\n",inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port),newfd); 
        }
        
        /*5.2   创建线程接收,并处理接收开启、接收、接收关闭*/
        if(pthread_create(&thread_id, NULL,(void *)handData, &newfd) == 0)  //线程创建成功，则返回0。若线程创建失败，则返回出错编号，
            connect_count[tc++] = thread_id;
        //else
        //{
            //Test
          //  printf("create thread fail\n");
            //break;
        //}
    }



    /*6.    确保服务器退出时所有子线程都关闭*/
    for (i = 0; i < tc; ++i)
    {
       pthread_join(connect_count[i],NULL);
    }

    /*7.    关闭服务器的套接字（停止服务器）*/ 
    close(sockfd); 

    //Test
    printf("*********************server shutdown*********************\n");

    return 0; 

}


void handData(void *args)
{
    int newfd = (int)(*(int *)args);
    /*
    fd_set实质为long类型的数组
    每一个数组元素都能与一打开的文件句柄（不管是socket句柄，还是其他文件或命名
	管道或设备句柄）建立联系，
     */ 
    fd_set rfds;
    int retval;
    int maxfd;
    struct timeval tv;
    char buf[BUFLEN]; 
    int len;


    //Test
    printf("thread %u create success\n",pthread_self());

    /* 不断循环接收数据直到数据接收完成*/
    while(1)
    {
        FD_ZERO(&rfds);
        FD_SET(0,&rfds); //0是默认的本机监听符
        FD_SET(newfd,&rfds);//监听本机端socket描述符以及客户端socket描述符
        maxfd = 0;


        /*1
        检测文件描述符集中有多少元素
         */
        if (maxfd < newfd)
            maxfd = newfd;


        /*2
        设置等待超时时间
         */
        tv.tv_sec = 6;
        tv.tv_usec = 0;


        /*3
        监测文件描述符集中哪些元素准备好（收到数据）可以被读取
        
        int select(int maxfd, fd_set *readfds, fd_set *writefds, fe_set *exceptfds, const struct timeval *timeout);
        select的第一个参数是文件描述符集中要被检测的比特数，这个值必须至少比待检测的最大文件描述符大1；
        参数readfds指定了被读监控的文件描述符集；
        参数writefds指定了被写监控的文件描述符集；
        而参数exceptfds指定了被例外条件监控的文件描述符集。
        参数timeout起了定时器的作用：  到了指定的时间，无论是否有设备准备好，都返回调用。
         */
        retval = select(maxfd+1, &rfds, NULL, NULL, &tv); 
        if(retval == -1){ //出错
            printf("%u select fail this connect will be close\n",pthread_self()); 
            break; 
        }else if(retval == 0){  //等待超时（时长取决于设置的超时时间），没有可读（该select只监听了读）的文件
            printf("%u overtime and waiting...\n",pthread_self()); 
            continue; 
        }else{  //select成功
            if (FD_ISSET(0,&rfds)) //服务端准备好（空闲）代表服务端以接收完数据，应提醒客户端开始下一段数据传送
            {
                memset(buf,0,sizeof(buf));  
                strcpy(buf,"true");
                len = send(newfd,buf,strlen(buf),0); 
                if(len > 0) ;
                    //printf("",buf); 
                else{ 
                        printf("%u send feedback fail\n",pthread_self()); 
                        break; 
                    } 
            } 
            if(FD_ISSET(newfd, &rfds)){  //客户端准备好（空闲）表示客户端发送数据结束，服务端可以开始读取
                /******接收数据*******/ 
               
               /*
               把数据追加写入到本地
                */
                memset(buf,0,sizeof(buf));
                len = recv(newfd,buf,BUFLEN,0);
                if (len > 0)
                {
                    //Test
                    //printf("%u is writedown data.. %s\n",pthread_self(),buf);
                    writeDownData(buf,len);
                }
                else
                {
                    if (len < 0)
                        printf("%u recevier data fail\n",pthread_self());
                    else
                        //Test
                        printf("%u client is quit and transport complete\n",pthread_self());
                    break;
                }
            }
        }
    }
    close(newfd);
    //Test
    printf("%u client is shutdown\n",pthread_self());
    return;

}

void writeDownData(char buf[],int len)
{
    //char re[len];

    char dir[] = DIR;

    char file_name[10];

    char des_path;

    int file_fd;

    ssize_t ss_len;

    size_t s_len = len;


    //strncpy(re,buf,len);
    //printf("%s\t",buf);
    /*
    strncat()用于将n个字符追加到字符串的结尾，其原型为：
    char * strncat(char *dest, const char *src, size_t n);
    path[100] = "/mpi_receive_file_dir/"+pthread_self()+".txt";

    itoa(pid, &namebuf[length], 10);     
    Unix版本：itoa()在头文件<stdlib.h>中不存在  
    使用sprintf将整数转换成字符串
    sprintf(namebuf, "%d", pid);

    原型：itoa (int,char*,int)
    功能：把整数i转换成字符串
    第一个参数是要转换的数字
    第二个参数是要写入转换结果的目标字符串
    第三个参数是转移数字时所用的基数。10：十进制；2：二进制...

    函数作用：获得线程自身的ID。pthread_t的类型为unsigned long int，所以在打印的时候要使用%lu方式，否则显示结果出问题。
    */
   //itoa(pthread_self(),temp,10);
   sprintf(file_name,"%lu",pthread_self());
   strncat(des_path,"/",1);
   strncat(des_path,dir,sizeof(dir));
   strncat(des_path,file_name,10);
   strncat(des_path,".txt",4);

   //Test
   printf("-------%s\n", des_path);

   /*access(const char *pathname,int mode);
    4.判断文件是否存在和是否可读可写


    int access(const char *pathname,int mode);
    pathname:是文件名称
    mode是我们要判断的属性.可以取以下值或者是他们的组合:
    R_OK文件可以读
    W_OK文件可以写
    X_OK文件可以执行
    F_OK文件存在.
    当我们测试成功时,函数返回0,否则如果有一个条件不符时,返回-1.
    */
   // if (access())
   // {
   //     /* code */
   // }

   /*
    int open( const char * pathname, int flags);
    int open( const char * pathname,int flags, mode_t mode);
    参数pathname 指向欲打开的文件路径字符串。下列是参数flags 所能使用的旗标:
    O_RDONLY 以只读方式打开文件
    O_WRONLY 以只写方式打开文件
    O_RDWR 以可读写方式打开文件。
    上述三种旗标是互斥的，也就是不可同时使用，但可与下列的旗标利用OR(|)运算符组合。
    O_CREAT 若欲打开的文件不存在则自动建立该文件。
    .............
    -----------O_CREAT:如果改文件不存在，就创建一个新的文件，并用第三个参数为其设置权限-----------
    O_APPEND 当读写文件时会从文件尾开始移动，也就是所写入的数据会以附加的方式加入到文件后面。
    O_SYNC 以同步的方式打开文件。
    S_IRUSR:S_IRUSR 或S_IREAD，00400权限，代表该文件所有者具有可读取的权限。
    S_IWUSR:S_IWUSR 或S_IWRITE，00200 权限，代表该文件所有者具有可写入的权限。

    若所有欲核查的权限都通过了检查则返回0值(文件描述符)，表示成功，只要有一个权限被禁止则返回-1。

    S_IRUSR,
    S_IWUSR,
    S_IXUSR,
    S_IRGRP,
    S_IWGRP,
    S_IXGRP,
    S_IROTH,
    S_IWOTH,
    S_IXOTH.
    其中
    R：读，W：写，
    X：执行，USR：文件所属的用户，
    GRP：文件所属的组，OTH：其他用户。
    注：第三个参数是在第二个参数中有O_CREAT时才用作用。若没有，则第三个参数可以忽略。
     */
    	//open file for write
     file_fd = open(des_path,O_RDWR|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR);
     if (file_fd == -1) {  
        fprintf(stderr, "Open %s Error：%s\n", des_path, strerror(errno));  
        return;  
     }
    /*
    表头文件
    #include<unistd.h>
    定义函数
    ssize_t write (int fd,const void * buf,size_t count);
    函数说明
    write()会把参数buf所指的内存写入count个字节到参数fd所指的文件内。当然，文件读写位置也会随之移动。
    返回值
    如果顺利write()会返回实际写入的字节数。当有错误发生时则返回-1，错误代码存入errno中。
    错误代码
    EINTR 此调用被信号所中断。
    */
   if (write(file_fd, buf, s_len) == -1)
   {
        fprintf(stderr, "Write %s Error：%s\n", file_name, strerror(errno));  
        return;  
   }
   else
    printf("write file success\n");

   /*
   功能描述：用于关闭一个被打开的的文件
    所需头文件： #include <unistd.h>
    函数原型:int close(int fd)
    参数：fd文件描述符
    函数返回值：0成功，-1出错
    */
   if (close(file_fd) == -1)
        printf("close file fail");
    else
        printf("file close success\n");

   
}

void catLog(char *where,char *what)
{

}

int init()
{
	/*
    int mkdir(const char *pathname,mode_t mode);open

    其中参数pathname是新创建目录的目录名，mode指定该目录的访问权限，这些位将受到文件创建方式屏蔽（umask）的修正。

    该函数创建一个名为pathname的空目录，此目录自动含有“.”和“..”2个登记项。这个新创建目录的用户ID被设置为调用进程的有效用户ID，其组则为父目录的组ID或者进程的有效组ID。

    若调用成功，mkdir将更新该目录的st_atime、st_ctime和st_mtime，同时更新其父目录的st_ctime和st_mtime，然后返回0。若调用失败，mkdir将返回-1.

    由pathname指定的新目录的父目录必须存在，并且调用进程必须具有该父目录的写权限以及pathname涉及的各个分路径目录的搜寻权限。
    定义函数
    */
   char dir[20] = "/";
   strncat(dir,DIR,sizeof(DIR));

   mkdir(dir,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1;
   catlog("init",strerror(errno));
}

void catlog(char *where,char *what)
{
	  int log_fd;

	  char log_file_path[100];
	  char down[300];
	  char local_time[30];

	  char current[1024];

	  time_t now_t;
	  struct tm *time_now;

	   strncat(log_file_path,"/",1);
	   strncat(log_file_path,DIR,sizeof(DIR));
	   strncat(log_file_path,"/",1);
       strncat(log_file_path,LOGFILE,sizeof(LOGFILE));
		//open file for write
       log_fd = open(log_file_path,O_RDWR|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR);
       if (log_fd == -1) 
		 return;

	//获得当前时间（国际标准时间非北京时间）
	 time(&now_t);
	 //转换为所在地区时间
	 time_now = localtime(&now_t);
	 sprintf(local_time,"%s",asctime(time_now));


	 //timespec time;
     //clock_gettime(CLOCK_REALTIME, &time);  //获取相对于1970到现在的秒数
     //tm nowTime;
    // localtime_r(&time.tv_sec, &nowtime);
     //sprintf(current, "%04d%02d%02d%02d:%02d:%02d", nowTime.tm_year + 1900, nowTime.tm_mon, nowTime.tm_mday, 
     //nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);

	 strncat(down,local_time,sizeof(local_time));
	 strncat(down," : ",3);
	 strncat(down,where,sizeof(where));
	 strncat(down,"  ",2);
	 strncat(down,what,sizeof(what));
	 strncat(down,"\n",2);

	write(log_fd,down,sizeof(down));

	close(log_fd);
}
