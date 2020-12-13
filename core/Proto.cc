#include "Proto.h"
#include "Socket.h"
#include <iostream>
#define MAGIC 0x5a


/*   0      1  ~ 
 * magic   Fson 
 */


int sendRequest(int sfd, Fson& fson)
{
    uint8_t magic = MAGIC;   // crc
    int res = nbSafeSend(sfd, (char*)&magic, sizeof(magic));
    if(res < 0)
    {
        logWrite("send fail1\n");
        return -1;
    }

    std::string body = fson.serializer();
    res = nbSafeSend(sfd, (char*)&body[0], body.size());
    if(res < 0)
    {
        logWrite("send fail2\n");
        return -1;
    }
    return 0;
}

int recvRequest(int cfd, Fson& fson)
{
    std::string mem;
    uint8_t magic;   // crc
    int res = nbSafeRecv(cfd, (char*)&magic, sizeof(magic));
    if(res < 0) return -1;

    if(magic != MAGIC) 
    {
        logWrite("recvRequest, magic not match\n");
        close(cfd);
        return -1;
    }

    uint16_t num;
    res = nbSafeRecv(cfd, (char*)&num, sizeof(num));
    if(res < 0) return -1;

    uint16_t headsize = 4 * num;
    std::string head;
    head.resize(headsize);
    res = nbSafeRecv(cfd, (char*)&head[0], headsize);
    if(res < 0) return -1;

    uint16_t bodysize = 0;
    uint16_t itemSize = 0;

    for(size_t i = 0; i < headsize; i += 2)
    {
        memcpy((char*)&itemSize, (char*)&head[i], sizeof(itemSize));
        bodysize += itemSize;
    }

    std::string body;
    body.resize(bodysize);
    res = nbSafeRecv(cfd, (char*)&body[0], bodysize);
    if(res < 0) return -1;

    mem.resize(sizeof(num) + headsize + bodysize);
    memcpy((char*)&mem[0], (char*)&num, sizeof(num)); 
    memcpy((char*)&mem[0] + sizeof(num), (char*)&head[0], head.size()); 
    memcpy((char*)&mem[0] + sizeof(num) + head.size(), (char*)&body[0], body.size()); 

    fson.deserializer(mem);

    return 1;
}









