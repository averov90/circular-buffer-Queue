# circular buffer Queue
[![License](https://img.shields.io/badge/LICENSE-The%20Unlicense-green?style=flat-square)](/LICENSE)  [![Version](https://img.shields.io/badge/VERSION-STABLE-green?style=flat-square)](https://github.com/averov90/circular-buffer-Queue/releases)
### :small_orange_diamond: [Russian version](/README.md)

This repository provides a ring buffer implementation of a queue.
The use of a circular buffer allows this data structure to be reasonably efficient in its intended mode of operation (add and remove equally on average).
The performance of the presented implementation is similar to the performance of the STL version compiled in Release mode (testing was carried out on x64 bitness).

Depending on the specifics of use, additional optimizations of some scenarios are possible, for example, additions.
If it turns out that the time delays for expanding the structure are too long, a higher-level ring is possible: a ring containing data rings.
This solution will avoid using the potentially expensive *memcpy* for a large number of objects, and will also avoid moving elements while they is in the structure (the address of any element will not change).

It is also possible to optimize the reversal of the direction of the queue.
This can be done by swapping the start and end pointers, as well as adding a new field that will store the direction of traversing the elements of the ring.

These are all pretty useful and interesting tasks on the topic of coding.