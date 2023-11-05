#ifndef CPP_POOL_H
#define CPP_POOL_H

#include <boost/assert.hpp>
#include <boost/pool/object_pool.hpp>
#include <unordered_set>

namespace pool {
/**
 * AdaptiveObjectPool is an object pool designed for fast object construction and destruction.
 *
 * The pool uses a fixed sized boost object_pool for construction / destruction. This is intended to provide
 * a performance advantage in scenarios where the cost of dynamic memory allocation is critical.
 *
 * If the object_pool is exhausted, the pool will dynamically allocate new objects. These "extras"
 * are tracked separately and are properly deallocated when released back to the pool. The idea is that
 * these extras will introduce some allocation penalty but will enable the pool to continue functioning.
 *
 * By default the pool works with Safe = true which will keep a track of objects created by the pool
 * and only destroy those and not other objects sent to release. Additionally it also keeps track of
 * objects created by the pool and safely destoys them when the pool itself gets destroyed.
 *
 * If Safe = false is used, care should be taken to always call release() correctly to avoid leaking
 * memory, and to avoid destroying objects not owned by the pool.
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
 * N - The fixed size of the internal fixed size pool.
 * Safe - Whether to add additional safety to objects allocated after overflowing the size of the fixed pool.
 */
template <typename T, bool Safe = true>
class AdaptiveObjectPool {
   public:
    AdaptiveObjectPool(size_t fixed_size) : pool_(fixed_size, fixed_size){};

    template <typename... Args>
    T* acquire(Args&&... args) {
        T* t = static_cast<T*>(pool_.malloc());
        if (t) {
            new (t) T(std::forward<Args>(args)...);
        } else {
            t = new T(std::forward<Args>(args)...);
            if constexpr (Safe) {
                overflow_.insert(t);
            }
        }
        return t;
    }

    bool release(T* obj) {
        if (!pool_.is_from(obj)) {
            if constexpr (Safe) {
                if (auto iter = overflow_.find(obj); iter != overflow_.end()) {
                    overflow_.erase(obj);
                    delete obj;

                    return true;
                }
            } else {
                delete obj;
                return true;
            }
            return false;
        }

        pool_.destroy(obj);
        return true;
    }

    ~AdaptiveObjectPool() {
        for (auto iter = overflow_.begin(); iter != overflow_.end(); ++iter) {
            delete *iter;
        }

        overflow_.clear();
    }

   private:
    boost::object_pool<T> pool_;
    std::unordered_set<T*> overflow_ = {};
};

}  // namespace pool

#endif  // CPP_POOL_H
