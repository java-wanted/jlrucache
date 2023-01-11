# jlrucache
A demonstration of the last recently used cache.

## TASK

  The task is stated by a Commerce Java Senior Programmer.

```java
Design a data structure that follows the constraints of a Least Recently
Used (LRU) cache.

Implement the LRUCache class:
- LRUCache(int capacity) Initialize the LRU cache with positive size
  capacity
- int get(int key) Return the value of the key if the key exists,
  otherwise return -1
- void put(int key, int value) Update the value of the key if the key
  exists. Otherwise, add the key-value pair ot the cache.
  If the number of keys exceeds the capacity from this operation, evict
  the least recently used key.

The functions get and put must each run in O(1) average time complexity.

Example 1:
Input
["LRUCache", "put", "put", "get", "put", "get", "put", "get", "get", "get"]
[[2], [1, 1], [2, 2], [1], [3, 3], [2], [4, 4], [1], [3], [4]]

Output
[null, null, null, 1, null, -1, null, -1, 3, 4]
```

## AUTHOR

  Boris Stankevich <microsoft-wanted@yandex.ru>.

## LICENSE

  This project can be used in accordance with rules and limitation of
  License GPLv3+: GNU GPL version 3 or later
  <https://gnu.org/licenses/gpl.html>.

  This is free software: you are free to change and redistribute it.
  There is NO WARRANTY, to the extent permitted by law.
