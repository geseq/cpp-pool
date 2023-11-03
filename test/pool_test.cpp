#include "pool.hpp"

#include <gtest/gtest.h>

static int allocatedCounter = 0;

class MyClass {
   public:
    MyClass() { ++allocatedCounter; }

    ~MyClass() { --allocatedCounter; }
};

TEST(AdaptiveObjectPool, AcquireRelease) {
    pool::AdaptiveObjectPool<MyClass, 5> pool;

    MyClass* obj1 = pool.acquire();
    ASSERT_NE(obj1, nullptr);

    MyClass* obj2 = pool.acquire();
    ASSERT_NE(obj2, nullptr);

    ASSERT_TRUE(pool.release(obj1));
    ASSERT_TRUE(pool.release(obj2));
}

TEST(AdaptiveObjectPool, ExtraAllocation) {
    pool::AdaptiveObjectPool<MyClass, 1> pool;

    MyClass* obj1 = pool.acquire();
    ASSERT_NE(obj1, nullptr);

    MyClass* obj2 = pool.acquire();  // This should trigger extra allocation
    ASSERT_NE(obj2, nullptr);

    ASSERT_NE(obj1, obj2);

    ASSERT_TRUE(pool.release(obj2));
    ASSERT_TRUE(pool.release(obj1));
}

TEST(AdaptiveObjectPool, InvalidRelease) {
    pool::AdaptiveObjectPool<MyClass, 5> pool;

    MyClass* obj1 = new MyClass();

    ASSERT_FALSE(pool.release(obj1));

    delete obj1;
}

TEST(AdaptiveObjectPoolTest, ReleaseExtraObjectsMemory) {
    allocatedCounter = 0;

    pool::AdaptiveObjectPool<MyClass, 2> pool;

    int initialCount = allocatedCounter;

    MyClass* obj1 = pool.acquire();
    MyClass* obj2 = pool.acquire();

    MyClass* extra1 = pool.acquire();
    MyClass* extra2 = pool.acquire();

    ASSERT_TRUE(pool.release(obj1));
    ASSERT_TRUE(pool.release(obj2));

    ASSERT_EQ(allocatedCounter, initialCount + 2);

    ASSERT_TRUE(pool.release(extra1));
    ASSERT_TRUE(pool.release(extra2));

    ASSERT_EQ(allocatedCounter, initialCount);

    {
        pool::AdaptiveObjectPool<MyClass, 2> temp_pool;
        for (int i = 0; i < 10; ++i) {
            temp_pool.acquire();
        }

        ASSERT_EQ(allocatedCounter, initialCount + 10);
    }

    ASSERT_EQ(allocatedCounter, initialCount);
}

TEST(AdaptiveObjectPool, Unsafe_AcquireRelease) {
    pool::AdaptiveObjectPool<MyClass, 5, false> pool;

    MyClass* obj1 = pool.acquire();
    ASSERT_NE(obj1, nullptr);

    MyClass* obj2 = pool.acquire();
    ASSERT_NE(obj2, nullptr);

    ASSERT_TRUE(pool.release(obj1));
    ASSERT_TRUE(pool.release(obj2));
}

TEST(AdaptiveObjectPool, Unsafe_ExtraAllocation) {
    pool::AdaptiveObjectPool<MyClass, 1, false> pool;

    MyClass* obj1 = pool.acquire();
    ASSERT_NE(obj1, nullptr);

    MyClass* obj2 = pool.acquire();  // This should trigger extra allocation
    ASSERT_NE(obj2, nullptr);

    ASSERT_NE(obj1, obj2);

    ASSERT_TRUE(pool.release(obj2));
    ASSERT_TRUE(pool.release(obj1));
}

TEST(AdaptiveObjectPool, Unsafe_InvalidRelease) {
    pool::AdaptiveObjectPool<MyClass, 5, false> pool;

    MyClass* obj1 = new MyClass();

    // Unsafe requires only releasing valid
    ASSERT_TRUE(pool.release(obj1));

    // deleted above
}

TEST(AdaptiveObjectPool, Unsafe_ReleaseExtraObjectsMemory) {
    allocatedCounter = 0;

    pool::AdaptiveObjectPool<MyClass, 2, false> pool;

    int initialCount = allocatedCounter;

    MyClass* obj1 = pool.acquire();
    MyClass* obj2 = pool.acquire();

    MyClass* extra1 = pool.acquire();
    MyClass* extra2 = pool.acquire();

    ASSERT_TRUE(pool.release(obj1));
    ASSERT_TRUE(pool.release(obj2));

    ASSERT_EQ(allocatedCounter, initialCount + 2);

    ASSERT_TRUE(pool.release(extra1));
    ASSERT_TRUE(pool.release(extra2));

    ASSERT_EQ(allocatedCounter, initialCount);

    {
        pool::AdaptiveObjectPool<MyClass, 2> temp_pool;
        for (int i = 0; i < 10; ++i) {
            temp_pool.acquire();
        }

        ASSERT_EQ(allocatedCounter, initialCount + 10);
    }

    ASSERT_EQ(allocatedCounter, initialCount);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

