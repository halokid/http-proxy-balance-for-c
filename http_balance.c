#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include "hash.h"

#define BUFFER_SIZE 4096

int tcpSrv(int listen_port);


/**
//+----------- for hash ------------------------+
//定义hash表的最大节点数(容量)
#define HASH_MAX_SIZE 100

typedef struct hash_node hash_n;

struct hash_node
{
    char* key;
    char* value;
    //节点的存在时间
    int end_time;
    hash_n* next;
};

//定义一个hash表
hash_n* hash_table[HASH_MAX_SIZE];

//哈希表的实际长度
extern int hash_table_size;
//+----------- for hash ------------------------+
**/




int main(int argc, char** argv) {
    // if (argc < 2)
    // {
        // printf("Usage: real server ip. like ./http_balance x.x.x.x");
    // }
    tcpSrv(8888);
    return 0;
}

int tcpSrv(int listen_port) {
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    char buff[BUFFER_SIZE+1];
    int n;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(listen_port);

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        close(listenfd);
        return -1;
    }

    if (listen(listenfd, 2048) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        close(listenfd);
        return -1;
    }
    
    //初始化哈希表
    init_hash_table();

    printf("-----waiting for client's request-----\n");
    while (1) {
        if ((connfd = accept(listenfd, (struct sockaddr *)NULL, NULL)) == -1) {
            printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
            close(listenfd);
            return -1;
        }
        //读取客户端的请求
        memset(&buff, 0, sizeof(buff));
        recv(connfd, buff, BUFFER_SIZE, 0);
        //printf("recv msg:\n%s\n", buff);
        // char resp[] = "HTTP/1.0 200 OK\r\n\r\n<html>hello</html>";
        
        
        //定义返回给客户端的内容
        char reverse_buf[100];
        
        struct timeval now_time;
        gettimeofday(&now_time, NULL);
        //取得现在的时间，精确到秒
        int now_time_sec;
        now_time_sec = (int)now_time.tv_sec;
        
        //初始化哈希表
        // init_hash_table();
        
        //检查缓存
        char *cache = get_cache(now_time_sec);
        
        //缓存为空
        // if( strcmp(cache, NULL) == 0 )
        if( cache == NULL )
        {
        
        //开始请求真实服务器
        int reverse_fd, reverse_bytes;
        // char reverse_buf[100];
        struct sockaddr_in local_serv_addr;

        if (( reverse_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket error!");
            exit(1);
        }
        bzero(&local_serv_addr,sizeof(local_serv_addr));
        local_serv_addr.sin_family    = AF_INET;
        //真实服务器的端口
        local_serv_addr.sin_port      = htons(8080);
        //真实服务器的IP
        local_serv_addr.sin_addr.s_addr= inet_addr("127.0.0.1");

        if (connect(reverse_fd, (struct sockaddr *)&local_serv_addr,sizeof(struct sockaddr)) == -1) {
            perror("connect error!");
            exit(1);
        }

        //发送客户端的请求给真实服务器
        write(reverse_fd,buff, sizeof(buff));

        if ((reverse_bytes = recv(reverse_fd, reverse_buf, 100,0)) == -1) {
            perror("recv error!");
            exit(1);
        }

        reverse_buf[reverse_bytes] = '\0';
        printf("Received: %s\n",reverse_buf);
        printf("+----------------------------------+\n");
        close(reverse_fd);
        //结束请求真实服务器
        
        
        //+-------------------- for cache ----------------+
        //建立缓存
        
        //设置缓存的时间为60秒
        int cache_time;
        cache_time = now_time_sec + 60;
        
        //初始化哈希表
        // init_hash_table();
        
        //定义一个key
        char *key = "key-cache1";
        //缓存的值为真实服务器返回的内容, 这样下次就不用再去请求真实服务器
        char *value = reverse_buf;
        
        insert_hash_table(key, value, cache_time);
        //+-------------------- for cache ----------------+
        
        }
        else
        {
            //如果缓存不为空
            strcpy(reverse_buf, cache);
            
        }
        
        
        //把真实服务器的返回发送给客户端
        send(connfd, reverse_buf, strlen(reverse_buf), 0);
        close(connfd);
    }

    close(listenfd);
    return 0;
}


































