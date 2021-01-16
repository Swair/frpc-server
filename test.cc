#include <stdio.h>
#include "Socket.h"
#include "Common.h"
#include "LogWrite.h"





int main(int argc,char*argv[])
{   

    Fson fatherFson;
    Fson childFson;
    fatherFson.setStr("fa", "123");
    fatherFson.setInt("line", 56);


    childFson.setStr("mm", "value");
    childFson.setDouble("nn", 23.43);

    fatherFson.setFson("child", childFson);

    std::string codestr = fatherFson.serializer();


    Fson decodeFson;
    decodeFson.deserializer(codestr);

    Fson cFson = decodeFson.getFson("child");

    printf("%s, %f\n", cFson.getStr("mm").c_str(), cFson.getDouble("nn"));


    return 0;
}


















