#ifndef CPP_POOL_H
#define CPP_POOL_H

#include <array>
#include <boost/assert.hpp>
#include <boost/pool/object_pool.hpp>
#include <unordered_set>

namespace pool {
/**
 * AdaptivePobjectPool is an object pool designed for fast object allocation and deallocation.
 *
 * The pool maintains an internal free list of objects for quick allocation. The list is pre-allocated
 * with a fixed size (N) to minimize runtime allocations and deallocations. This is intended to provide
 * a performance advantage in scenarios where the cost of dynamic memory allocation is critical.
 *
 * If the pool's free list is exhausted, the pool will dynamically allocate new objects. These "extras"
 * are tracked separately and are properly deallocated when released back to the pool. The idea is that
 * these extras will introduce some allocation penalty but will enable the pool to continue functioning.
 *
 * The release method includes a runtime check to ensure that objects are not released into a full pool.
 *
 * Note: This implementation is not thread-safe. If thread-safe behavior is required,
 * additional synchronization mechanisms must be added.
 *
 * Usage:
 * AdaptivePobjectPool<MyObject, 100> pool;
 * MyObject* obj = pool.acquire();
 * // Use obj...
 * pool.release(obj);
 *
 * Template Parameters:
 * T - The type of object the pool will manage.
 * N - The fixed size of the internal free list.
 */
template <typename T, std::size_t N>
class AdaptivePobjectPool {
   public:
    AdaptivePobjectPool() : pool_(N, N) {
        for (std::size_t i = 0; i < N; ++i) {
            auto obj = pool_.construct();
            free_list_[i] = obj;
        }
        index_ = N;
    }

    T* acquire() {
        if (index_ == 0) {
            T* t = new T();
            extras_set_.insert(t);
            return t;
        }
        return free_list_[--index_];
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

        BOOST_ASSERT(index_ < N);

        pool_.destroy(obj);
        free_list_[index_++] = pool_.construct();

        return true;
    }

    ~AdaptivePobjectPool() {
        for (std::size_t i = 0; i < index_; ++i) {
            pool_.destroy(free_list_[i]);
        }

        for (auto iter = extras_set_.begin(); iter != extras_set_.end(); ++iter) {
            delete *iter;
        }

        extras_set_.clear();
    }

   private:
    boost::object_pool<T> pool_;
    std::array<T*, N> free_list_;
    std::unordered_set<T*> extras_set_;
    std::size_t index_{0};
};

}  // namespace pool

#endif  // CPP_POOL_H
