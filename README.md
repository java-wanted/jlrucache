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

## SOLUTION

  The following design is considered:

  - A `frames` object provides the storage for values cached.

  - An `hmap` object maps keys into the corresponding frames.

  - An `lrul` object keeps the history of access to elements of `hmap`.

  The following pseudo code describes the interaction of this objects:

```c
; A hash map - O(1) access time in average.
; NOTE: It will be exactly O(1) for UINT32_MAX buckets.
hmap
    backets []
        hmap_item: list of (key, frame_idx, lrul_item)
    [key]
        <- hmap_item for the key or None

; A list of least recently used hmap items
lrul
    lrul_item: list of hmap items

; An array of values of the given capacity
frames
    value: i32 []
    len: u32
    reserve()
        <- len++

; Add a value for a key.
put(key, value)
    hmap_item = hmap[key]
    if hmap_item
        lrul_item = lrul.rm(hmap_item.lrul_item)
    else
        if len(frames) < capacity(frames)
            hmap_item = (reserve(frames))
            lrul_item = (hmap_item)
        else
            lrul_item = lrul.rm_tail()
            hmap_item = lrul_item.hmap_item
            hmap.rm(hmap_item)
        hmap_item.key = key
        hmap.add(hmap_item)
    lrul.add_head(lru_item)
    frames[hmap_item.frame_idx] = value

; Get value for a key.
get(key)
    hmap_item = hmap[key]
    if not hmap_item
        <- -1
    lrul_item = lrul.rm(hmap_item.lrul_item)
    lrul.add_head(lrul_item)
    <- frames[hmap_item.frame_idx]
```

## AUTHOR

  Boris Stankevich <microsoft-wanted@yandex.ru>.

## LICENSE

  This project can be used in accordance with rules and limitation of
  License GPLv3+: GNU GPL version 3 or later
  <https://gnu.org/licenses/gpl.html>.

  This is free software: you are free to change and redistribute it.
  There is NO WARRANTY, to the extent permitted by law.
