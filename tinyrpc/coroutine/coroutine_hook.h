/*************************** 
该类的hook和windows的全局钩子函数不一样，windows的全局钩子函数是全局的，比如我hook了CreateFile这个函数的话，
以后只要我调用到了CreateFile的话就会先走我hook之后的逻辑，然后再调用系统的CreateFile。
该类说到底只是对一些系统的API进行封装，我通过一个宏定义可以根据名字拿到系统的API指针，比如accept_hook，这里我拿到了accept的指针，
我在该程序的开发的时候就调用accept_hook,而不去调用accept，这个会先执行我的逻辑，然后因为我拿到了accept的指针，所以我在后面还会调用系统的api
****************************/

#ifndef TINYRPC_COROUTINE_COUROUTINE_HOOK_H
#define TINYRPC_COROUTINE_COUROUTINE_HOOK_H

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

typedef ssize_t (*read_fun_ptr_t)(int fd, void *buf, size_t count);

typedef ssize_t (*write_fun_ptr_t)(int fd, const void *buf, size_t count);

typedef int (*connect_fun_ptr_t)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

typedef int (*accept_fun_ptr_t)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

typedef int (*socket_fun_ptr_t)(int domain, int type, int protocol);

typedef int (*sleep_fun_ptr_t)(unsigned int seconds);


namespace tinyrpc {

static int accept_hook(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

static ssize_t read_hook(int fd, void *buf, size_t count);

static ssize_t write_hook(int fd, const void *buf, size_t count);

static int connect_hook(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

static unsigned int sleep_hook(unsigned int seconds);

// 暴露出去给用户设置是否需要hook
void SetHook(bool);
}

extern "C" {

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

ssize_t read(int fd, void *buf, size_t count);

ssize_t write(int fd, const void *buf, size_t count);

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

unsigned int sleep(unsigned int seconds);

}

#endif