# cpp-pool

AdaptiveObjectPool is an object pool designed for fast object construction and destruction.

The pool uses a fixed sized boost object_pool for construction / destruction. This is intended to provide
a performance advantage in scenarios where the cost of dynamic memory allocation is critical.

If the object_pool is exhausted, the pool will dynamically allocate new objects. These "extras"
are tracked separately and are properly deallocated when released back to the pool. The idea is that
these extras will introduce some allocation penalty but will enable the pool to continue functioning.

By default the pool works with Safe = true which will keep a track of objects created by the pool
and only destroy those and not other objects sent to release. Additionally it also keeps track of
objects created by the pool and safely destoys them when the pool itself gets destroyed.

If Safe = false is used, care should be taken to always call release() correctly to avoid leaking
memory, and to avoid destroying objects not owned by the pool.

Note: This implementation is not thread-safe. If thread-safe behavior is required,
additional synchronization mechanisms must be added.

Usage:
```cpp
AdaptiveObjectPool<MyObject, 100> pool;
MyObject* obj = pool.acquire();
// Use obj...
pool.release(obj);
```
