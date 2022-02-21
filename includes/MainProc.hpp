#pragma once

#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <string>
#include <csignal>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <stack>
#include <utility>
#include <algorithm>
#include "Socket.hpp"
#include "Config.hpp"
#include "Worker.hpp"
#include "utils.hpp"

#define SOCKET_ERROR -1

#define READ_BUFF_SIZE 16384
#define WRITE_BUFF_SIZE 65536


using std::string;

bool operator==(struct pollfd &fd1, struct pollfd &fd2) {
    return fd1.fd == fd2.fd;
}

class MainProc {
public:
    MainProc(Config &conf, char **env): fd_closed(false), config(conf), sockets(), fds(), worker(conf, env) {}

    bool adress_in_serv(std::vector<Server> &tmp, std::vector<Server>::iterator &it_find) {
        std::vector<Server>::iterator it = tmp.begin();
        for ( ; it != tmp.end(); ++it) {
            if (it->getAddress() == it_find->getAddress() && it->getPort() == it_find->getPort())
                return true;
        }
        return false;
    }

    void serv_init()
    {  
        std::vector<Server> tmp;
        std::vector<Server>::iterator it = config.servers.begin();
        for ( ; it != config.servers.end(); ++it) {
            if (adress_in_serv(tmp, it))
                continue;
            tmp.push_back(*it);
            int listen_socket, result;
            struct sockaddr_in addr;

            listen_socket = socket(PF_INET, SOCK_STREAM, 0);
            
            nonblock(listen_socket);

            addr.sin_family = AF_INET;
            addr.sin_port = htons(it->getPort());     // short, network byte order
            addr.sin_addr.s_addr = it->getAddress();
            memset(addr.sin_zero, '\0', sizeof addr.sin_zero);

            int opt = 1;
            setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt)); /* setsockopt */

            // Привязываем сокет к IP-адресу
            result = bind(listen_socket, (struct sockaddr *)&addr, sizeof addr);
            if (result == SOCKET_ERROR) {
                std::cerr << "bind failed with error: " << "\n";
                close(listen_socket);
                continue;
            }

            // Инициализируем слушающий сокет SOMAXCONN=128
            if (listen(listen_socket, 256) == SOCKET_ERROR) {
                std::cerr << "listen failed with error: " << "\n";
                close(listen_socket);
                continue;
            }
            std::cout << "start listen on " << listen_socket << "for " ;
            std::cout << it->getAddressStr() << ":" << it->getPort() << "\n";

            Socket tmp("listen", listen_socket);
            sockets.push_back(tmp);
        }
    }

    void sockets_close() {
        std::vector<Socket>::iterator it = sockets.begin();
        for (; it != sockets.end(); ++it) {
            close(it->get_fd());
        }
    }

    void main_loop(int &run_flag)
    {
        if (sockets.empty()) {
            std::cerr << "No listen socket started\n";
            return;
        }
        set_pollfd(sockets);
        while (run_flag) {
            int size = fds.size();
            int ret = poll(fds.data(), size, -1);  // -1 - without timeout
            if (ret == -1){
                std::cerr << "POLL error\n";
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < size; ++i) {
                if (fds[i].revents == 0)
                    continue;
                if (fds[i].revents & POLLIN) {
                    if (sockets[i].get_type() == "listen")
                        new_in_connect(sockets[i], i);
                    else {
                        // запускаем обработку если полностью готов request
                        if (read_sock(sockets[i], i) == 1) {
                            if (worker.parsing(sockets[i])) {
                                sockets[i].clear_read_buf();
                                worker.processing(sockets[i]);
                            } 
                            // else
                            //     fds[i].events = POLLIN;
                        }
                    }
                    fds[i].revents = 0;
                }
                if (fds[i].revents & POLLOUT) {
                    if (sockets[i].get_type() == "listen")
                        perror("POLL error");
                    else
                        send_buf_in_sock(sockets[i], i);
                    fds[i].revents = 0;
                }
            }
            if (fd_closed) {
                close_fds();
                fd_closed = false;
            }
        }
    }

private:
    bool fd_closed;
    std::stack<int> closed_fds;

    void close_fds() {
        while (!closed_fds.empty()) {
            int index = closed_fds.top();
            sockets.erase(sockets.begin() + index);
            fds.erase(fds.begin() + index);
            closed_fds.pop();
        }
    }

    void close_fd(Socket &socket, int index) {
        if (closed_fds.empty() || closed_fds.top() != index) {
            close(socket.get_fd());
            closed_fds.push(index);
        }
        fd_closed = true;
    }

    void new_in_connect(Socket &listen, int index)
    {
        int client_socket = SOCKET_ERROR;
        struct sockaddr_in clientaddr;
        socklen_t clientaddr_size = sizeof(clientaddr);

        client_socket = accept(listen.get_fd(), (struct sockaddr *)&clientaddr, &clientaddr_size);

        if (client_socket == SOCKET_ERROR) {
            std::cerr << "accept failed: " <<  "\n";
            close_fd(listen, index);
            return;
        }

        string res = inet_ntoa(clientaddr.sin_addr);
        res += ":" + ToString((int)ntohs(clientaddr.sin_port));
        
        nonblock(client_socket);  // check ?

        Socket new_in("client", client_socket);
        new_in.set_ip_adr(res);
        sockets.push_back(new_in);
        struct pollfd fd;
        fd.fd = client_socket;
        fd.events = POLLIN | POLLOUT; //  
        fd.revents = 0;
        fds.push_back(fd);
    }

    // if 1 - read complited start get request
    int read_sock(Socket &client, int index)
    {
        std::string all_buf;
        int count_end = 0;
        char buf[READ_BUFF_SIZE];
        int result;

        while (count_end < 4) {
            result = recv(client.get_fd(), buf, READ_BUFF_SIZE, 0);
            if (result > 0) {
                buf[result] = '\0';
                all_buf += buf;
                if (count_end == 0)
                    ++count_end;
            } else if (count_end == 0 && result == SOCKET_ERROR) {
                // ошибка получения данных
                // std::cerr << "recv failed: " << result << "\n";
                close_fd(client, index);
                // std::cerr << "fd closed " << index << "\n";
                return 0;
            }
            else
                ++count_end;
        }
        if (all_buf.empty() && client.read_buf_empty()) {
            // соединение закрыто клиентом
            // std::cerr << "connection closed...\n";
            close_fd(client, index);
            return 0;
        }
        if (!all_buf.empty()) {
            client.append_read_buf(all_buf.c_str());
        }
        // fds[index].events = POLLOUT;
        return 1;
    }

    int send_buf_in_sock(Socket &client, int index) {
        // Отправляем ответ клиенту с помощью функции send
        const string msg = client.get_write_buf();
        int size;
        if (client.write_buf_len - client.write_buf_seek > WRITE_BUFF_SIZE)
            size = WRITE_BUFF_SIZE;
        else
            size = client.write_buf_len - client.write_buf_seek;
        const char *buf = msg.c_str();
        int result = send(client.get_fd(), buf + client.write_buf_seek, size, 0);
        if (result == SOCKET_ERROR) {
            // произошла ошибка при отправле данных
            std::cerr << "send failed: " <<  "\n";
        } else
            client.write_buf_seek += result;
        if (client.write_buf_seek == client.write_buf_len) {
            // std::cout << "**** send " << client.write_buf_len << " byte ****\n";
            client.clear_write_buf();
            // fds[index].events = POLLIN;
        }
        if (!client.get_keep_alive())
            close_fd(client, index);
        return 1;
    }

    void set_pollfd(std::vector<Socket> &sockets)
    {
        size_t tmp_size = sockets.size();
        for (size_t i = 0; i < tmp_size; ++i) {
            struct pollfd fd;
            fd.fd = sockets[i].get_fd();
            fd.events = POLLIN | POLLOUT;
            fds.push_back(fd);
        }
    }

    Config config;
    std::vector<Socket> sockets;
    std::vector<struct pollfd> fds;
    Worker worker;
};