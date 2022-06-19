#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "crc16.h"
#include <cstring>
//#include <malloc.h>
using namespace std;

#define MAXTABLESIZE 20000 //允许开辟的最大散列表长度
typedef char* ElementType;
typedef enum
{
    Legitimate,
    Empty,
    Deleted
} EntryType; //散列单元的状态类型

struct HashEntry
{
    ElementType data; //存放的元素
    EntryType state;  //单元状态
};

typedef struct HashEntry Cell;

struct TblNode
{
    int tablesize; //表的最大长度
    Cell* cells;   //存放散列单元数据的数组
};
typedef struct TblNode* HashTable;

class HashTableClass
{
public:
    HashTableClass();
    //~HashTableClass();
    int NextPrime(int);
    int table_size;
    int Find(HashTable, ElementType);
    bool Insert(HashTable, ElementType);
    HashTable HashTable_h = (HashTable)malloc(sizeof(TblNode));

protected:
    unsigned int keyHashSlot(char*);
};


HashTableClass::HashTableClass()
{
    //HashTable HashTable_h = (HashTable)malloc(sizeof(TblNode));
    HashTable_h->tablesize = NextPrime(table_size);
    HashTable_h->cells = (Cell*)malloc(HashTable_h->tablesize * sizeof(Cell));
    //初始化哈希表状态为空单元
    for (int i = 0; i < HashTable_h->tablesize; i++)
    {
        HashTable_h->cells[i].state = Empty;
    }
}


/*返回大于n且不超过MAXTABLESIZE的最小素数*/
int HashTableClass::NextPrime(int Hash_n)
{
    int Hash_p = (Hash_n % 2) ? Hash_n + 2 : Hash_n + 1; //从大于n的下一个奇数开始
    int Hash_i;
    while (Hash_p <= MAXTABLESIZE)
    {
        for (Hash_i = (int)sqrt(Hash_p); Hash_i > 2; Hash_i--)
        {
            if ((Hash_p % Hash_i) == 0)
                break;
        }
        if (Hash_i == 2)
            break; //说明是素数，结束
        else
            Hash_p += 2;
    }
    return Hash_p;
}


/* We have 16384 hash slots. The hash slot of a given key is obtained
 * as the least significant 14 bits of the crc16 of the key.
 *
 * However if the key contains the {...} pattern, only the part between
 * { and } is hashed. This may be useful in the future to force certain
 * keys to be in the same node (assuming no resharding is in progress). */
unsigned int HashTableClass::keyHashSlot(char* key) {
    int s, e; /* start-end indexes of { and } */
    int keylen = strlen(key);
    for (s = 0; s < keylen; s++)
    {
        if (key[s] == '{')
        {
            break;
        }         
    }
    /* No '{' ? Hash the whole key. This is the base case. */
    if (s == keylen)
    {
        return crc16(key, keylen) & 0x3FFF;
    }
    /* '{' found? Check if we have the corresponding '}'. */
    for (e = s + 1; e < keylen; e++)
    {
        if (key[e] == '}')
        {
            break;
        }
    }
    /* No '}' or nothing between {} ? Hash the whole key. */
    if (e == keylen || e == s + 1)
    {
        return crc16(key, keylen) & 0x3FFF;
    }
    /* If we are here there is both a { and a } on its right. Hash
     * what is in the middle between { and }. */
    return crc16(key + s + 1, e - s - 1) & 0x3FFF;
}


/*查找元素位置*/
int HashTableClass::Find(HashTable Find_h, ElementType Find_key)
{
    int current_pos, new_pos;
    int collision_num = 0; //记录冲突次数

    current_pos = keyHashSlot((char*)Find_key); //初始散列位置
    new_pos = current_pos;
    //当该位置元素为非空并且不是要找的元素的时候，发生冲突
    while (Find_h->cells[new_pos].state != Empty && Find_h->cells[new_pos].data != Find_key)
    {
        collision_num++;
        if (collision_num % 2) //处理奇数冲突
        {
            new_pos = current_pos + (collision_num + 1) * (collision_num + 1) / 4;
            if (new_pos >= Find_h->tablesize)
                new_pos = new_pos % Find_h->tablesize;
        }
        else //处理偶数冲突
        {
            new_pos = current_pos - (collision_num) * (collision_num) / 4;
            while (new_pos < 0)
                new_pos += Find_h->tablesize; //调整到合适大小
        }
    }
    return new_pos;
}


/*插入新的元素*/
bool HashTableClass::Insert(HashTable Insert_h, ElementType Insert_key)
{
    int pos = Find(Insert_h, Insert_key);  //先查找key是否存在
    if (Insert_h->cells[pos].state != Legitimate)
    {
        Insert_h->cells[pos].state = Legitimate;
        Insert_h->cells[pos].data = Insert_key;
        return true;
    }
    else
    {
        cout << "hey exist" << endl;
        return false;
    }
    cout << "insert has done" << endl;
}


