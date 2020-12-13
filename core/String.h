#ifndef __STRING_H
#define __STRING_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <memory>
#include <vector>
#include "MemBlock.h"



#define COPY(data, s)  {          \
    if(data_ != nullptr)          \
    Free(data_);                  \
    data_ = (char*)Malloc(s + 1); \
    memcpy(data_, data, s);       \
    data_[s] = 0;                 \
    size_ = s;                    \
}

#define RESET(s)  {               \
    if(data_ != nullptr)          \
    Free(data_);                  \
    data_ = (char*)Malloc(s + 1); \
    memset(data_, 0, s);          \
    data_[s] = 0;                 \
    size_ = s;                    \
}


class String
{
    public:

        explicit String()
        {
            data_ = nullptr;
            size_ = 0;
            capacity_ = 0;
        }

        ~String()
        {
            if(data_ != nullptr)
            {
                Free(data_);
                size_ = 0;
            }
        }

        explicit String(size_t s)
        {
            data_ = (char*)Malloc(s + 1);
            data_[s] = 0;
            size_ = s;
        }

        explicit String(const String& l)
        {
            if(this == &l)
                return;
            COPY(l.data_, l.size_); 
        }

        explicit String(const void* d, size_t s)
        {
            if(!d) return; 
            COPY(d, s);
        }

        explicit String(const char* d)
        {
            if(!d) return;
            COPY(d, strlen(d) + 1);
        }

        explicit String(const std::string& l)
        {
            COPY(&l[0], l.size()); 
        }

        String& operator=(const String& l)
        {
            if(this == &l)
                return *this;
            COPY(l.data_, l.size_);
            return *this;
        }

        String& operator=(const char* d)
        {
            if(!d) return *this; 
            COPY(d, strlen(d) + 1);
            return *this;
        }


        String(String&& r)  
        {
            if(this == &r)
                return;
            data_ = r.data_;
            size_ = r.size_;
            r.data_ = nullptr;
            r.size_ = 0;
        }

        String& operator=(String&& r)
        {
            if(this == &r)
                return *this;
            if(data_ != nullptr)
                Free(data_);
            data_ = r.data_;
            size_ = r.size_;
            r.data_ = nullptr;
            r.size_ = 0;
            return *this;
        }


        String operator+(const String& l)
        {
            String newString;
            newString.size_ = size_ + l.size_;
            newString.data_ = (char*)Malloc(newString.size_ + 1);
            memcpy(newString.data_, data_, size_);
            memcpy(newString.data_ + size_, l.data_, l.size_);
            newString.data_[newString.size_] = 0;
            return newString;  
        }

        String& operator+=(const String& l)
        {
            if(data_ != nullptr)
                Free(data_);
            size_t size = size_ + l.size_;
            char* data = (char*)Malloc(size + 1);
            memcpy(data, data_, size_);
            memcpy(data + size_, l.data_, l.size_);
            data[size] = 0;
            data_ = data;
            size_ = size;
            return *this;
        }

        char& operator[](size_t i) const
        {
            if(i < 0)
                return data_[0];
            else if(i >= size_)
                return data_[size_ - 1];
            else
                return data_[i];
        }

        bool operator==(const String& l) const
        {
            if(size_ != l.size_)
                return false;
            return strcmp(data_, l.data_) ? false : true;
        }
        
        void resize(size_t s)
        {
            RESET(s);
        }

        size_t size()
        {
            return size_;
        }

        const char* c_str()
        {
            return data_;
        }

    public:
        std::vector<char> toByteStream()
        {
            std::vector<char> outBuf;
            outBuf.assign(data_, data_ + size_);
            return outBuf;
        }





    private:
        char* data_ = nullptr;
        size_t size_ = 0;
        size_t capacity_ = 0;
};


#endif



