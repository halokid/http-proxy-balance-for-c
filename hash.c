#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define HASH_MAX_SIZE 100

typedef struct hash_node hash_n;
struct hash_node
{
    char* key;
    char* value;
    //节点的存在时间
    int end_time;
    hash_n* next;
};

//定义一个hash表
hash_n* hash_table[HASH_MAX_SIZE];

//哈希表的实际长度
int hash_table_size;


/**
@desc:  初始化哈希表
@param: void
@return:    void   
*/
void init_hash_table()
{
    hash_table_size = 0;
    //用0来填满哈希表
    memset(hash_table, 0, sizeof(hash_n*) * HASH_MAX_SIZE );
}


/**
@desc:  插入哈希表
@param: char* key, char* value
@return: void
*/
void insert_hash_table(char *key, char *value, int end_time)
{
    if( hash_table_size >= HASH_MAX_SIZE )
    {
        printf("out of hash table memory\n");
        return;
    }
    
    //检查哈希表里面是否已经存在该key, 从0开始检查
    hash_n* node_head = hash_table[0];
    while(node_head)
    {
        if( strcmp(node_head->key, key) == 0)
        {
            printf("%s already exists!\n", key);
            return;
        }
        node_head = node_head->next;
    }
    
    hash_n *new_node = (hash_n*)malloc(sizeof(hash_n));
    memset(new_node, 0, sizeof(hash_n));
    
    new_node->key = (char*)malloc(sizeof(char)*(strlen(key) + 1));
    strcpy(new_node->key, key);
    
    new_node->value = (char*)malloc(sizeof(char)*(strlen(value) + 1));
    strcpy(new_node->value, value);
    
    new_node->end_time = end_time;
    
    hash_table[hash_table_size] = new_node;
    
    printf("insert node +----------------+\npos %d\nkey: %s\nvalue: %s\nend_time: %d\n", hash_table_size, key, value, end_time);
    
    hash_table_size++;
}



/**
@desc:  检查缓存时间，建立缓存， 为了阐述缓存的原理，这里就不做哈希表的查询等多余代码，直接取得缓存，再匹配时间进行相关的处理
@param: end_time
@return:    hash_node value     
*/
char *get_cache()
{
    //建立缓存
    struct timeval now_time;
    gettimeofday(&now_time, NULL);
    //取得现在的时间，精确到秒
    int now_time_sec;
    now_time_sec = (int)now_time.tv_sec;
    
    // printf("sizeof(hash_table) %d\n", sizeof(hash_table));
    // printf("strlen(hash_table)%d\n", strlen(hash_table));
    
    if( hash_table[0] )
    {
    
    if( now_time_sec > (hash_table[0]->end_time + 60) )
    {
        //如果缓存时间超过60秒，则返回null
        return NULL;
    }
    else if( strcmp(hash_table[0]->value, NULL) != 0 )
    {
        //如果没超过60秒，则直接取缓存
        printf("use cache!!\n");
        return hash_table[0]->value;
    }
    else
    {
        return NULL;
    }
    }
    else
    {
        
        return NULL;
    }
    

}



/**
void main()
{
    struct timeval run_time;
    gettimeofday(&run_time, NULL);
    
    int sec_time;
    sec_time = (int)run_time.tv_sec;
    printf("time is %d\n", sec_time);
    int end_time;
    end_time = sec_time + 60;
    
    init_hash_table();
    
    char *key1 = "sky";
    char *value1 = "blue";
    
    
    char *key2 = "banana";
    char *value2 = "yellow";
    
    insert_hash_table(key1, value1, end_time);
    insert_hash_table(key2, value2, end_time);
}
**/
















