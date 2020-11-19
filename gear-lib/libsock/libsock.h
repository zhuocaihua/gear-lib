/******************************************************************************
 * Copyright (C) 2014-2020 Zhifeng Gong <gozfree@163.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/
#ifndef LIBSOCK_H
#define LIBSOCK_H

#include <libposix.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#if defined (OS_LINUX)
#include <unistd.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#define INET_ADDRSTRLEN 16
#endif

#define LIBSOCK_VERSION "0.1.0"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_ADDR_STRING (65)

//socket structs

typedef struct sock_addr {
    char ip_str[INET_ADDRSTRLEN];
    uint32_t ip;
    uint16_t port;
} sock_addr_t;

typedef struct sock_addr_list {
    sock_addr_t addr;
    struct sock_addr_list *next;
} sock_addr_list_t;

typedef struct sock_connection {
    int fd;
    int type;
    struct sock_addr local;
    struct sock_addr remote;
} sock_connection_t;

//socket tcp apis
struct sock_connection *sock_tcp_connect(const char *host, uint16_t port);
int sock_tcp_bind_listen(const char *host, uint16_t port);
int sock_accept(int fd, uint32_t *ip, uint16_t *port);

//socket udp apis
struct sock_connection *sock_udp_connect(const char *host, uint16_t port);
int sock_udp_bind(const char *host, uint16_t port);

//socket unix domain apis
struct sock_connection *sock_unix_connect(const char *host, uint16_t port);
int sock_unix_bind_listen(const char *host, uint16_t port);

//socket common apis
void sock_close(int fd);
void sock_destory();

int sock_send(int fd, const void *buf, size_t len);
int sock_sendto(int fd, const char *ip, uint16_t port,
                const void *buf, size_t len);
int sock_send_sync_recv(int fd, const void *sbuf, size_t slen,
                void *rbuf, size_t rlen, int timeout);
int sock_recv(int fd, void *buf, size_t len);
int sock_recvfrom(int fd, uint32_t *ip, uint16_t *port,
                void *buf, size_t len);

uint32_t sock_addr_pton(const char *ip);
int sock_addr_ntop(char *str, uint32_t ip);

int sock_set_noblk(int fd, int enable);
int sock_set_block(int fd);
int sock_set_nonblock(int fd);
int sock_set_reuse(int fd, int enable);
int sock_set_tcp_keepalive(int fd, int enable);
int sock_set_buflen(int fd, int len);

int sock_get_tcp_info(int fd, struct tcp_info *ti);
int sock_get_local_list(struct sock_addr_list **list, int loopback);
int sock_gethostbyname(struct sock_addr_list **list, const char *name);
int sock_getaddrinfo(sock_addr_list_t **list,
                const char *domain, const char *port);
int sock_getaddr_by_fd(int fd, struct sock_addr *addr);
int sock_get_remote_addr_by_fd(int fd, struct sock_addr *addr);
int sock_get_local_info(void);

#ifdef __cplusplus
}
#endif
#endif
