blog: https://my.oschina.net/u/4936903

### 1. Fson协议
#### 类型标记
    目前有以下8种类型标记
```
​    byte        0
​    char        1
​    short       3
​    int           4
​    long         5
​    double    6
​    string    10
​    Fson     99
```

#### Fson协议格式
        Fson协议的最开始16位为<key, value>对的数量，紧接着以16位为单元依次存储key和value的大小，其中value需要存储类型，故+1
```
| 0 1 | 2 3 |  4  5 |...|koffset+ksize|valuetype|voffset+vsize|...|...|
|kvnum|ksize|vsize+1|...|     key     |  vtype  |     value   |...|...|
```

#### 举例
   exp.  { "k1":"v1", "k2": 123, "wav":0xff0xmm}
   
        由于一共3组键值对，所以kvnum值为3，| 0  1 |位置16个字节存储3；
		k1为2个字节，v1也是2个字节，所以| 2  3 |区间位置存储2，|  4  5 |区间位置存储2+1=3；
		k2为2个字节，v2是当做int存储，就是4个字节，所以| 6 7 |区间位置存储2，|  8 9 |区间位置存储4+1=5；
		wav为3个字节，v2是当做byte存储，就是2个字节，所以| 6 7 |区间位置存储3，|  8 9 |区间位置存储2+1=3;
		接着，就是存储键值对的实际值了，最终如下
```

   |-num-|-------------------head------------------|---------------body-----------------------------------| 
   |kvnum|k1size|v1size|k2size|v2size|k3size|v3size| k1  |type|  v1  |  k2 |type| 123 |   wav  |type| ffmm|
   | 0 1 | 2 3  | 4 5  | 6  7 | 8  9 | 10 11| 12 13|14 15| 16 | 17 18|19 20|21  |22|23|24|25|26|27  |28|29|
   |  3  |  2   |  3   |   2  |  5   |  3   |   3  | k 1 | 10 | v  1 | k 2 | 4  | 123 | w  a v |0   |ff mm|


```

### 2、无锁线程池
### 3、内存管理
### 4、Reactor网络模型
### 5、编译&运行

​    make

​    ./remote_call_server

​    ./remote_call_client









