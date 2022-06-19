#ifndef LRU_HPP
#define LRU_HPP
#include "unordered_map"
#include "cstring"
using namespace std;
struct DLinkedNode {
    string key, value;
    DLinkedNode* prev;
    DLinkedNode* next;
    DLinkedNode(): key(20,' '), value(200,' '), prev(nullptr), next(nullptr) {}
    DLinkedNode(string _key, string _value): key(_key), value(_value), prev(nullptr), next(nullptr) {}
};
class LRUCache {
private:
    unordered_map<string, DLinkedNode*> cache;
    DLinkedNode* head;
    DLinkedNode* tail;
    int size;
    int capacity;
public:
    LRUCache(int _capacity);
    string get(string key);
    void put(string key, string value);
    void addToHead(DLinkedNode* node);
    void removeNode(DLinkedNode* node);
    void moveToHead(DLinkedNode* node);
    DLinkedNode* removeTail();
};

LRUCache::LRUCache(int _capacity): capacity(_capacity), size(0) {
    head = new DLinkedNode();
    tail = new DLinkedNode();
    head->next = tail;
    tail->prev = head;
}
string LRUCache::get(string key) {
    if (!cache.count(key)) {
        return "";
    }
    DLinkedNode* node = cache[key];
    moveToHead(node);
    return node->value;
}
void LRUCache::put(string key, string value) {
    if (!cache.count(key)) {
        DLinkedNode* node = new DLinkedNode(key, value);
        cache[key] = node;
        addToHead(node);
        ++size;
        if (size > capacity) {
            DLinkedNode* removed = removeTail();
            cache.erase(removed->key);
            delete removed;
            --size;
        }
    }
    else {
        DLinkedNode* node = cache[key];
        node->value = value;
        moveToHead(node);
    }
}
void LRUCache::addToHead(DLinkedNode* node) {
    node->prev = head;
    node->next = head->next;
    head->next->prev = node;
    head->next = node;
}
void LRUCache::removeNode(DLinkedNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}
void LRUCache::moveToHead(DLinkedNode* node) {
    removeNode(node);
    addToHead(node);
}
DLinkedNode* LRUCache::removeTail() {
    DLinkedNode* node = tail->prev;
    removeNode(node);
    return node;
}

#endif /* LRU_HPP */
