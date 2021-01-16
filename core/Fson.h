#ifndef __FSON_H
#define __FSON_H
#include <stdio.h>
#include <map>
#include <string.h>

/*
   exp.  { "key1":"value1",  "key23": 123, "wav":0xff0xmm}
   | 0 1 | 2 3 | 4 5 | 6 7 | 8 9 | 10 11 | 12 13 | 14 15 16 17| 18 |19|20|21|22|23|24|25|26|27|28|29| 30 |31|32|33|34|35|36|37| 38 |39 |40|
   |  2  |  4  |  7  |  5  |  5  |   3   |  3    | k  e  y  1 | s  |v  a  l  u   e 1 | k  e y  2  3 |int |   123     |w  a  v |byte| ff mm|
   |kvnum|ksize|vsize|                                        |type|                                |type|                    |type| 
   |-num-|-------------------head----------------|------------------------------------------body------------------------------------------| 
   */


enum FTYPE
{
    VBYTE = 0,
    VCHAR,
    VSHORT,
    VINT,
    VLONG,
    VDOUBLE,
    VSTRING = 10,
    
    VFSON = 99,
    VEND = 255
};

class Fson
{
    public:
        std::string serializer()
        {
            uint16_t num = data_.size();
            uint16_t headsize = 4 * num;
            uint16_t bodyOffset = sizeof(num) + headsize;
            uint16_t bodysize = 0;
            for(auto&& item : data_)
                bodysize += item.first.size() + item.second.size();

            uint16_t memSize = sizeof(num) + headsize + bodysize;

            //printf("%d", num);
            std::string mem;
            mem.resize(memSize);

            uint16_t offset = 0;
            memcpy(&mem[offset], (char*)&num, sizeof(num));
            offset += 2; 

            std::string key, value;
            uint16_t kSize, vSize;
            for(auto&& item : data_)
            {
                key = item.first;
                value = item.second;
                kSize = key.size();
                vSize = value.size();

                memcpy(&mem[offset], (char*)&kSize, sizeof(kSize));
                offset += 2; 
                //printf("%d", kSize);

                memcpy((char*)&mem[bodyOffset], (char*)&key[0], key.size());
                bodyOffset += kSize;

                memcpy(&mem[offset], (char*)&vSize, sizeof(vSize));
                offset += 2; 
                //printf("%d", vSize);

                memcpy((char*)&mem[bodyOffset], (char*)&value[0], value.size());
                bodyOffset += vSize;

                //kvPrint(key, value);
            }

            //printf("\n");
            return mem;
        }

        bool deserializer(const char* fsonstr, uint16_t size)
        {
            uint16_t offset = 0;
            uint16_t num = *(uint16_t*)fsonstr;
            uint16_t headsize = 4 * num;
            uint16_t bodyOffset = sizeof(num) + headsize;
            uint16_t memSize = sizeof(num) + headsize;
            offset += 2;
            std::string key, value;
            uint16_t kSize, vSize;
            for(uint16_t i = 0; i < num; i ++)
            {
                kSize = *(uint16_t*)(fsonstr + offset);
                offset += 2;
                vSize = *(uint16_t*)(fsonstr + offset); 
                offset += 2;
                key.resize(kSize);
                value.resize(vSize); 
                memcpy((char*)&key[0], (char*)(fsonstr + bodyOffset), kSize);
                bodyOffset += kSize;
                memcpy((char*)&value[0], (char*)(fsonstr + bodyOffset), vSize);
                bodyOffset += vSize;
                data_[key] = value;
                memSize = memSize + kSize + vSize;
            }
            if(memSize == size)
                return true;
            else
                return false;
        }

        bool deserializer(const std::string& fonstr)
        {
            return deserializer(fonstr.c_str(), fonstr.size());
        }

        void setMem(const std::string& key, const std::string& v)
        {
            data_[key] = v;
        }

        void setFson(const std::string& key, Fson& fson)
        {
            setMem(key, fson.serializer());
        }

        void setStr(const std::string& key, const std::string& v)
        {
            std::string value;
            value.resize(1 + v.size());
            value[0] = VSTRING;
            memcpy((char*)&value[1], (char*)&v[0], v.size());
            data_[key] = value;
        }

        void setStr(const std::string& key, const char* v)
        {
            setStr(key, std::string(v));
        }

        void setInt(const std::string& key, int v)
        {
            set(key, v);
        }

        void setDouble(const std::string& key, double v)
        {
            set(key, v);
        }

        template<typename Type> void set(const std::string& key, Type v)
        {
            std::string value;
            if(typeid(value) == typeid(Type))
            {
                logWrite("set error\n");
                throw -1;
            }
            else
            {
                value.resize(1 + sizeof(v));
                if(typeid(int) == typeid(v))
                    value[0] = VINT;
                else if(typeid(double) == typeid(v))
                    value[0] = VDOUBLE;
                else if(typeid(short) == typeid(v))
                    value[0] = VSHORT;
                else if(typeid(char*) == typeid(v))
                    value[0] = VCHAR; 
                memcpy((char*)&value[1], (char*)&v, sizeof(v));
                data_[key] = value;
            }
        }

        std::string getMem(const std::string& key)
        {
            auto&& item = data_.find(key);
            if(item == data_.end())
            {
                logWrite("getMem error, no %s in Fson\n", key.c_str());
                throw -1;
            }
            return item->second;
        }

        Fson getFson(const std::string& key)
        {
            Fson fson;
            fson.deserializer(getMem(key));
            return fson; 
        }

        std::string getStr(const std::string& key)
        {
            auto&& item = data_.find(key);
            if(item == data_.end())
            {
                logWrite("getStr error, no %s in Fson\n", key.c_str());
                throw -1;
            }
            return item->second.substr(1);
        }

        int getInt(const std::string& key)
        {
            return get<int>(key);
        }

        double getDouble(const std::string& key)
        {
            return get<double>(key);
        }

        template<typename Type> Type get(const std::string& key)
        {
            std::string value;
            auto&& item = data_.find(key);
            if(item == data_.end() || typeid(value) == typeid(Type))
            {
                logWrite("get error, no %s in Fson\n", key.c_str());
                throw -1;
            }
            Type ret;
            value = item->second;
            memcpy((char*)&ret, (char*)&value[1], sizeof(ret));
            return ret;
        }

        void kvPrint(const std::string& key, const std::string& value)
        {
            std::string svalue;
            int ivalue;
            double dvalue;

            FTYPE ftype = (FTYPE)value[0];
            if(ftype == VSTRING)
            {
                svalue = value.substr(1);
                printf("<%s, %s>", key.c_str(), svalue.c_str());
            }
            else if(ftype == VDOUBLE)
            {
                memcpy((char*)&dvalue, (char*)&value[1], sizeof(dvalue));
                printf("<%s, %f>", key.c_str(), dvalue);
            }
            else
            {
                memcpy((char*)&ivalue, (char*)&value[1], sizeof(ivalue));
                printf("<%s, %d>", key.c_str(), ivalue);
            }    
        }

        void clear()
        {
            data_.clear();
        }

        void print()
        {
            printf("============= Fson begin ============\n");
            for(auto&& item : data_)
            {
                kvPrint(item.first, item.second);
                printf("\n"); 
            }
            printf("============= Fson end ============\n");
        }

    private:
        std::map<std::string, std::string> data_;
};




#endif




