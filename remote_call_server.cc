#include <stdio.h>
#include "Reactor.h"
#include "Common.h"
#include "LogWrite.h"
#include "Server.h"


int square(int line)
{
    return line * line;
}

class DataBase
{
    public:
        void set(const std::string& name, const int& age)
        {
            name_ = name;
            age_ = age;
        }

        void get(std::string& name, int& age)
        {
            name = name_;
            age = age_;
        }

    private:
        std::string name_;
        int age_;
};

void work(Fson& response, Fson& request, void* args)
{
    DataBase& dataBase = *(DataBase*)args; 
    std::string method;

    try
    {
        method = request.getStr("method");
    }
    catch(int ret)
    {
        response.setInt("code", 201);
        return;
    }

    response.setInt("code", 200);
    if(0 == method.compare("square"))
    {
        int v = request.getInt("line");
        response.setInt("result", v*v);
    }
    else if(0 == method.compare("set"))
    {
        dataBase.set(request.getStr("name"), request.getInt("age"));
        response.setStr("msg", "ok");
    }
    else if(0 == method.compare("get"))
    {
        std::string name;
        int age;
        dataBase.get(name, age);
        response.setStr("name", name);
        response.setInt("age", age);
    }
}

int main(int argc,char*argv[])
{
    DataBase dataBase;
    std::string ip = "127.0.0.1";
    std::string port = "12345";
    Server server;
    server.setCallBack(work, (void*)&dataBase);
    server.startRun(ip, port);
    return 0;
}




