#include "HashTableClass.hpp"

int main(int argc, char const* argv[])
{
    int n = 16384;
    HashTableClass hashtable_1;
    hashtable_1.Insert(hashtable_1.HashTable_h, (char*)"123");
    hashtable_1.Insert(hashtable_1.HashTable_h, (char*)"234");
    hashtable_1.Insert(hashtable_1.HashTable_h, (char*)"456");
    cout << hashtable_1.Find(hashtable_1.HashTable_h, (char*)"123") << " ";
    cout << hashtable_1.Find(hashtable_1.HashTable_h, (char*)"234") << " ";
    cout << hashtable_1.Find(hashtable_1.HashTable_h, (char*)"456") << " ";
    cout << "end" << endl;
    return 0;
}