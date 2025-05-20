#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>


int dealRequset(int net_fd, char *buf){

    // 解析HTTP请求
    int index = 0;
    while(buf[index] != '/'){
        index=index+1;
        if(index>1000){
            break;
        }
    }

    if(buf[index+1] == '0'){
        //解析HTTP和JSON, 存储信息:

        char parameter[200] = {0};
        //GET /0 HTTP/1.1 
        // 
        //{ "status": 0 , "stm32_msg": "parameter1:2000,parameter2: 1700"  }
        char *start = strstr(buf, "stm32_msg");
        start = strstr(start, "p");
        char *end = strchr(start, '"');
        ssize_t len = end-start;
        strncpy(parameter, start, len);

        // write file
        FILE *file = fopen("data.txt", "a");
        fprintf(file, "%s\n", parameter);
        fflush(file);
        fclose(file);

        return 0;
    }else if(buf[index+1]=='1'){
        // read file -> write to app

        char buf[409600] = {0};
        char *str1 = "HTTP/1.1 200 OK\r\n\r\n";
        strncpy(buf, str1, strlen(str1));

        FILE *file = fopen("data.txt", "r");

        buf[strlen(buf)]='[';

        while(1){
            char line[100] = {0};
            char * pRet = fgets(line, sizeof(line), file);
            if(pRet == NULL){
                break;
            }
            // 
            buf[strlen(buf)]='\"';
            strncpy(buf+strlen(buf), line, strlen(line));
            buf[strlen(buf)-1]='\"';
            buf[strlen(buf)]=',';
        }
        if(strlen(buf)>25){
            buf[strlen(buf)-1]=']';
        }else{
            char *str1 = "HTTP/1.1 200 OK\r\n\r\n[]";
            sprintf(buf, "%s", str1);
        }

        printf("-----\n%s\n-----\n", buf);
        send(net_fd, buf, strlen(buf), 0);

        return 1;
    }else if(buf[index+1] =='2'){
        // clean file -> write to app
        FILE *file = fopen("data.txt", "w");
        fclose(file);

        char *str1 = "HTTP/1.1 200 OK\r\n\r\n[]";
        send(net_fd, str1, strlen(str1), 0);
        return 2;
    }

    return 0;
}

int main() {

    char *port = "8000";
    char *ip = "0.0.0.0";

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(atoi(port));
    sockaddr.sin_addr.s_addr = inet_addr(ip);
    int reuse = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    bind(socket_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    listen(socket_fd, 10);

    int list[100] = {0};
    int size = 0;

    fd_set base_set;
    FD_ZERO(&base_set);
    FD_SET(socket_fd, &base_set);

    while (1) {
        fd_set set;
        memcpy(&set, &base_set, sizeof(base_set));

        select(100, &set, NULL, NULL, NULL);

        if(FD_ISSET(socket_fd, &set)){
            // 有新链接进来
            // 获取新连接, 记录新连接, 监听新连接
            int net_fd = accept(socket_fd, NULL, NULL);
            printf("新链接进来 \n");

            list[size] = net_fd;
            size++;

            FD_SET(net_fd, &base_set);
        }else{
            // 有人发信息过来
            // 判断是谁
            for(int i=0; i<size; i++){
                if(FD_ISSET(list[i], &set)){
                    char buf[4096] = {0};
                    ssize_t ret = recv(list[i], buf, sizeof(buf), 0);
                    printf("读取信息: \n %s \n", buf);
                    if(ret == 0){
                        // 对方断开链接: -> close, 移除监听,移动list删除, 
                        close(list[i]);

                        FD_CLR(list[i], &base_set);

                        for(int j=i; j<size; j++){
                            list[j] = list[j+1];
                        }
                        size--;
                        continue;
                    }

                    // 分析是谁发送数据过来, 根据URL资源路径: 存储, 获取, 删除?
                    ret = dealRequset(list[i], buf);
                    if(ret == 0){
                        // 信息上传
                    }else if(ret == 1){
                        // APP获取信息
                    }else if(ret == 2){
                        // APP删除信息
                    }

                    usleep(200);
                    // 回复完毕, 关闭连接, 移除监听, 移除记录 
                    close(list[i]);
                    FD_CLR(list[i], &base_set);
                    for(int j=i; j<size; j++){
                        list[j] = list[j+1];
                    }
                    size--;
                }
            }
        }
    }

    close(socket_fd);
    return 0;
}
