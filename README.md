# cpp-pool

AdaptivePobjectPool is an object pool designed for fast object allocation and deallocation.

The pool maintains an internal free list of objects for quick allocation. The list is pre-allocated
with a fixed size (N) to minimize runtime allocations and deallocations. This is intended to provide
a performance advantage in scenarios where the cost of dynamic memory allocation is critical.

If the pool's free list is exhausted, the pool will dynamically allocate new objects. These "extras"
are tracked separately and are properly deallocated when released back to the pool. The idea is that
these extras will introduce some allocation penalty but will enable the pool to continue functioning.

The release method includes a runtime check to ensure that objects are not released into a full pool.
If an attempt is made to release an object into a full pool, a std::runtime_error will be thrown.

Note: This implementation is not thread-safe. If thread-safe behavior is required,
additional synchronization mechanisms must be added.

Exception Handling:
- The release() method may throw a std::runtime_error if it detects an attempt to release an object into a full pool.

Usage:
```cpp
AdaptivePobjectPool<MyObject, 100> pool;
MyObject* obj = pool.acquire();
// Use obj...
pool.release(obj);  // May throw std::runtime_error if pool is full.
```
