#ifndef CPP_POOL_H
#define CPP_POOL_H

#include <array>
#include <boost/assert.hpp>
#include <boost/pool/object_pool.hpp>
#include <unordered_set>

namespace pool {
/**
 * AdaptiveObjectPool is an object pool designed for fast object allocation and deallocation.
 *
 * The pool uses a fixed sized boost object_pool for construction / destruction. This is intended to provide
 * a performance advantage in scenarios where the cost of dynamic memory allocation is critical.
 *
 * If the object_pool is exhausted, the pool will dynamically allocate new objects. These "extras"
 * are tracked separately and are properly deallocated when released back to the pool. The idea is that
 * these extras will introduce some allocation penalty but will enable the pool to continue functioning.
 *
 * The release method includes a runtime check to ensure that objects are not released into a full pool.
 *
 * Note: This implementation is not thread-safe. If thread-safe behavior is required,
 * additional synchronization mechanisms must be added.
 *
 * Usage:
 * AdaptiveObjectPool<MyObject, 100> pool;
 * MyObject* obj = pool.acquire();
 * // Use obj...
 * pool.release(obj);
 *
 * Template Parameters:
 * T - The type of object the pool will manage.
 * N - The fixed size of the internal free list.
 */
template <typename T, std::size_t N>
class AdaptiveObjectPool {
   public:
    AdaptiveObjectPool() : pool_(N, N) {}

    T* acquire() {
        T* t = pool_.construct();
        if (!t) {
            t = new T();
            extras_set_.insert(t);
        }
        return t;
    }

    bool release(T* obj) {
        if (!pool_.is_from(obj)) {
            if (auto iter = extras_set_.find(obj); iter != extras_set_.end()) {
                extras_set_.erase(obj);
                delete obj;

                return true;
            }
            return false;
        }

        pool_.destroy(obj);
        return true;
    }

    ~AdaptiveObjectPool() {
        for (auto iter = extras_set_.begin(); iter != extras_set_.end(); ++iter) {
            delete *iter;
        }

        extras_set_.clear();
    }

   private:
    boost::object_pool<T> pool_;
    std::unordered_set<T*> extras_set_;
};

}  // namespace pool

#endif  // CPP_POOL_H
