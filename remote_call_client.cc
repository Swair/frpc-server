#include <stdio.h>
#include <string>
#include "Socket.h"
#include "Common.h"
#include "LogWrite.h"





int main(int argc,char*argv[])
{   
    auto work = [](int sfd)->void{

        srand(time(0));
        int randnum = rand() % 10;

        Fson request;
        request.setStr("method", "square");
        request.setInt("line", randnum);
        sendRequest(sfd, request);
        printf("call server the function of square with %d line\n", randnum);


        Fson response;
        recvRequest(sfd, response);

        int result = response.getInt("result"); 
        printf("the square result is %d\n", result);
    };

    nbSendToServer("127.0.0.1", "12345", work);


    auto set = [](int sfd)->void{
        Fson request;
        request.setStr("method", "set");
        request.setStr("name", "jack");
        request.setInt("age", 20);
        sendRequest(sfd, request);

        Fson response;
        recvRequest(sfd, response);
        response.print();
    };

    nbSendToServer("127.0.0.1", "12345", set);


    auto get = [](int sfd)->void{
        Fson request;
        request.setStr("method", "get");
        sendRequest(sfd, request);

        Fson response;
        recvRequest(sfd, response);

        printf("name = %s, age = %d\n", response.getStr("name").c_str(), response.getInt("age"));
    };

    nbSendToServer("127.0.0.1", "12345", get);


    return 0;
}


















