#include <stdio.h>
#include "Reactor.h"
#include "Common.h"
#include "LogWrite.h"


int square(int line)
{
    return line * line;
}

void nbServer(const std::string& ip, const std::string& port)
{
    auto userHandler = [&](int cfd)->int {
        Fson request;
        Fson response;
        response.set("code", 200);

        auto work = [&]()->void{
            std::string method;
            int res = 0;
            try
            {
                method = request.getStr("method");
            }
            catch(int ret)
            {
                res = ret;
            }

            if(res == 0)
            {
                if(0 == method.compare("square"))
                {
                    int v = request.getInt("line");
                    response.setInt("result", v*v);
                }
            }
            else
                response.setInt("code", 201);
        };

        int res = recvRequest(cfd, request);
        if(res <= 0)
            return SOCKET_EXCEPTION;

        work();
        res = sendRequest(cfd, response);
        if(res < 0)
        {
            logWrite("reply fail\n");
        }

        return SOCKET_CLOSE;
    };

    Reactor reactor(10);
    reactor.runEpollServer(ip.c_str(), port.c_str(), userHandler);
}

int main(int argc,char*argv[])
{
    std::string ip = "127.0.0.1";
    std::string port = "12345";
    nbServer(ip, port);
    return 0;
}




