#pragma once

#include <cstdio>
#include <iostream>
#include <fcntl.h>
#include <string>
#include <cstring>
#include <vector>

#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdexcept>

#include "utils.hpp"

using std::string;

class CGIprocess
{
public:
    int fdStdInPipe[2];
    int fdStdOutPipe[2];
    std::string response_body;

    CGIprocess() {}

    void processing(string &strRequestBody, char **pszChildProcessArgs, char **pszChildProcessEnvVar) {
        fdStdInPipe[0] = fdStdInPipe[1] = fdStdOutPipe[0] = fdStdOutPipe[1] = -1;
        if (pipe(fdStdInPipe) != 0 || pipe(fdStdOutPipe) != 0)
            throw std::runtime_error("Cannot create CGI pipe");

        // Duplicate stdin and stdout file descriptors
        int fdOldStdIn = dup(fileno(stdin));
        int fdOldStdOut = dup(fileno(stdout));

        // Duplicate end of pipe to stdout and stdin file descriptors
        if ((dup2(fdStdOutPipe[1], fileno(stdout)) == -1) ||
                (dup2(fdStdInPipe[0], fileno(stdin)) == -1))
            throw std::runtime_error("Error Duplicate end of pipe to stdout and stdin file descriptors");

        // Close original end of pipe
        close(fdStdInPipe[0]);
        close(fdStdOutPipe[1]);

        //Запускаем дочерний процесс, отдаем ему переменные командной строки и окружения
        int nChildProcessID = spawn_process(pszChildProcessArgs, pszChildProcessEnvVar);

        // Duplicate copy of original stdin an stdout back into stdout
        dup2(fdOldStdIn, fileno(stdin));
        dup2(fdOldStdOut, fileno(stdout));

        // Close duplicate copy of original stdin and stdout
        close(fdOldStdIn);
        close(fdOldStdOut);
        
        
        nonblock(fdStdInPipe[1]);
        nonblock(fdStdOutPipe[0]);

        int status = 0;
        unsigned long buf_size = 65536;
        char bufferOut[buf_size + 1];
        unsigned long begin = 0;
        int chank_size = buf_size;
        bool flag = true;
        // int global_size = 0;
        // std::cout << "start write-read\n";
        while (1) {
            //Отдаем тело запроса дочернему процессу
            if (begin < (unsigned long)strRequestBody.size()) {
                if (strRequestBody.size() - begin < buf_size)
                    chank_size = strRequestBody.size() - begin;
                int k = write(fdStdInPipe[1], strRequestBody.c_str() + begin, chank_size);
                if (k >= 0) {
                    // std::cout << "write " << k << " byte\n";
                    begin += k;
                }
            } else if (flag) {
                close(fdStdInPipe[1]); //tmp
                // std::cout << "write end\n";
                flag = false;
            }
            //Читаем ответ от дочернего процесса
            int n = read(fdStdOutPipe[0], bufferOut, buf_size);
            if (n > 0) {
                // std::cout << "read " << n << " byte\n";
                // global_size += n;
                bufferOut[n] = '\0';
                response_body += std::string(bufferOut);
                // std::memset(bufferOut, 0, buf_size);
            }
            //Если дочерний процесс завершился, то завершаем и родительский процесс
            if (waitpid(nChildProcessID, &status, WNOHANG) > 0)
                break;
        }
        // std::cout << "read total " << global_size << " byte\n";
        close(fdStdOutPipe[0]);
    }

    ~CGIprocess() { 
    }

    int spawn_process(char **args, char **pEnv) {
        /* Create copy of current process */
        int pid = fork();
        
        /* The parent`s new pid will be 0 */
        if(pid == 0) {
            /* We are now in a child progress 
            Execute different process */
            execve(args[0], args, pEnv);
            /* This code will never be executed */
            exit(EXIT_SUCCESS);
        }

        /* We are still in the original process */
        return pid; 
    } 

};