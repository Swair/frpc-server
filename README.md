The writer is swair. if you have questions, please contact me with email: swair_fang@126.com


frpc特点：

1、Fson数据通信协议
  exp.  { "key1":"value1",  "key23": 123, "wav":0xff0xmm}
     | 0 1 | 2 3 | 4 5 | 6 7 | 8 9 | 10 11 | 12 13 | 14 15 16 17| 18 |19|20|21|22|23|24|25|26|27|28|29| 30 |31|32|33|34|35|36|37| 38 |39 |40|
     |  2  |  4  |  7  |  5  |  5  |   3   |  3    | k  e  y  1 | s  |v  a  l  u   e 1 | k  e y  2  3 |int |   123     |w  a  v |byte| ff mm|
     |kvnum|ksize|vsize|                                        |type|                                |type|                    |type| 
     |-num-|-------------------head----------------|------------------------------------------body------------------------------------------| 



2、无锁线程池
3、内存管理
4、Reactor网络模型











