- This is a simulation of distributed master-cache server system that can store and retrieve data(key-value pair). The key and server address correspondence is achieved through consistent hash algorithm. LRU cache has been used on each cache server for fast retrieve of frequent data. The master server is responsible for monitoring the survival states of each server, and updating the key-address table when a new cache server is online or an existing cache server is down.

- Co-Authors: Shiming Li, Yingluo Li, Tianjian Gao, Wei Zhang
