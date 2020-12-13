#ifndef __MemBlock_H
#define __MemBlock_H

#include <stdio.h>
#include <map>
#include <memory>
#include <mutex>
#include "LogWrite.h"

struct MemUnit
{
    size_t size;
    const char* func;
    const char* file;
    size_t line;
};

class MemBlock
{
    private:
        MemBlock():mid_(0){};
    public:
        static MemBlock& Instance()
        {
            static MemBlock memBlock;
            return memBlock;
        }

        ~MemBlock()
        {
            std::lock_guard<std::mutex> lock(mx_);
            if(memUnits_.size()) 
            {
                logWrite("\n!!! leak memory is shown as blow:\n");
                for(auto&& item : memUnits_)
                {
                    logWrite("  ---[addr, %p] (size, %lu), location(%s, %s, %lu)\n\n\n", 
                            item.first,
                            item.second.size,
                            item.second.func, 
                            item.second.file, 
                            item.second.line
                          );
                }
                exit(1);
            }
        }

        void* openMem(size_t s, const char* func, const char* file, size_t line)
        {
            std::lock_guard<std::mutex> lock(mx_);
            void* data = malloc(s);
            if(!data)
                logWrite("error in malloc in %s, %s: %d\n", __FUNCTION__, __FILE__,  __LINE__); 
            MemUnit memUnit = {s, func, file, line};
            memUnits_[data] = memUnit;
            //logWrite("openMem, %lu, [addr, %p], location(%s, %s, %lu)\n", ++mid_, data, func, file, line);
            return data;
        }

        void closeMem(void* data, const char* func, const char* file, size_t line)
        {
            std::lock_guard<std::mutex> lock(mx_);
            auto&& item = memUnits_.find(data);
            if(item == memUnits_.end())
            {
                logWrite("closeMem, not found\n");
                logWrite("[%p], location(%s, %s, %lu)\n", data, func, file, line);
                exit(1); 
            }
            free(data);
            memUnits_.erase(item);
            //logWrite("closeMem, %lu, [addr, %p], location(%s, %s, %lu)\n", --mid_, data, func, file, line);
        }

        void* modifyMem(void* data, size_t s, const char* func, const char* file, size_t line)
        {
            std::lock_guard<std::mutex> lock(mx_);
            auto&& item = memUnits_.find(data);
            if(item == memUnits_.end())
            {
                logWrite("ReMalloc, not found\n");
                logWrite("[%p], location(%s, %s, %lu)\n", data, func, file, line);
                exit(1); 
            }
            memUnits_.erase(item);
            void* newdata = realloc(data, s);
            MemUnit memUnit = {s, func, file, line};
            memUnits_[newdata] = memUnit;
            //logWrite("modifyMem, %lu, [addr, %p], location(%s, %s, %lu)\n", mid_, newdata, func, file, line);
            return newdata;
        }

        template<typename Object, typename... Args> Object* openMem(const char* func, const char* file, size_t line, Args&&... args)
        {
            std::lock_guard<std::mutex> lock(mx_);
            Object* object = new Object(args...);
            if(!object)
                logWrite("error in new in %s, %s: %d\n", __FUNCTION__, __FILE__,  __LINE__); 
            MemUnit memUnit = {sizeof(Object), func, file, line};
            memUnits_[object] = memUnit;
            //logWrite("openMem, %lu, [addr, %p], location(%s, %s, %lu)\n", ++mid_, object, func, file, line);
            return object;
        }

        template<typename Object> void closeMem(const char* func, const char* file, size_t line, Object* object)
        {
            std::lock_guard<std::mutex> lock(mx_);
            auto&& item = memUnits_.find(object);
            if(item == memUnits_.end())
            {
                logWrite("closeMem, not found\n");
                logWrite("[%p], location(%s, %s, %lu)\n", object, func, file, line);
                exit(1); 
            }
            delete object;
            memUnits_.erase(item);
            //logWrite("closeMem, %lu, [addr, %p], location(%s, %s, %lu)\n", --mid_, object, func, file, line);
        }

        void showMem()
        {
            std::lock_guard<std::mutex> lock(mx_);
            logWrite("memory used is shown as blow:\n");
            for(auto&& item : memUnits_)
            {
                logWrite("[%p] (%lu), location(%s, %s, %lu)\n", 
                        item.first,
                        item.second.size, 
                        item.second.func, 
                        item.second.file,
                        item.second.line
                      );
            }
        }

    private:
        std::map<void*, MemUnit> memUnits_;
        std::mutex mx_;
        size_t mid_;
};


#define Malloc(size)       MemBlock::Instance().openMem  ((size),          __func__, __FILE__, __LINE__)
#define Free(data)      MemBlock::Instance().closeMem ((data),          __func__, __FILE__, __LINE__)
#define Realloc(data, s)  MemBlock::Instance().modifyMem((data), (size),  __func__, __FILE__, __LINE__)
#define New(Object)   MemBlock::Instance().openMem<Object>  (__func__, __FILE__, __LINE__)
#define New2(Object, Args, ...)   MemBlock::Instance().openMem<Object>  (__func__, __FILE__, __LINE__, Args, ##__VA_ARGS__)
#define Delete(Object, object)   MemBlock::Instance().closeMem<Object> (__func__, __FILE__, __LINE__, (object))
#define InfoMem()           MemBlock::Instance().showMem()



#endif







