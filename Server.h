#ifndef __SERVER_H
#define __SERVER_H

#include <stdio.h>
#include "Reactor.h"
#include "Common.h"
#include "LogWrite.h"


class Server
{
    public:
        void setCallBack(std::function<void(Fson& response, Fson& request, void* args)> cb, void* args)
        {
            cb_ = cb;
            args_ = args;
        }

        int processHandler(int cfd, void* args)
        {
            auto work = [&]()->int {
                Fson request;
                Fson response;

                int res = recvRequest(cfd, request);
                if(res <= 0)
                    return SOCKET_EXCEPTION;

                cb_(response, request, args);

                res = sendRequest(cfd, response);
                if(res < 0)
                {
                    logWrite("reply fail\n");
                }

                return SOCKET_CLOSE;
            };
            return work();
        }

        void startRun(const std::string& ip, const std::string& port)
        {
            Reactor reactor(10);
            reactor.runEpollServer(ip.c_str(), port.c_str(), std::bind(&Server::processHandler, this,  _1, args_));
        }

    private:
        std::function<void(Fson& response, Fson& request, void* args)> cb_;
        void* args_;
};

#endif


