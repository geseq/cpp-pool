#include "pool.hpp"

#include <gtest/gtest.h>

class MyClass {
   public:
    MyClass() : x(0) {}
    int x;
};

TEST(AdaptivePobjectPool, AcquireRelease) {
    pool::AdaptivePobjectPool<MyClass, 5> pool;

    MyClass* obj1 = pool.acquire();
    ASSERT_NE(obj1, nullptr);

    MyClass* obj2 = pool.acquire();
    ASSERT_NE(obj2, nullptr);

    ASSERT_TRUE(pool.release(obj1));
    ASSERT_TRUE(pool.release(obj2));
}

TEST(AdaptivePobjectPool, ExtraAllocation) {
    pool::AdaptivePobjectPool<MyClass, 1> pool;

    MyClass* obj1 = pool.acquire();
    ASSERT_NE(obj1, nullptr);

    MyClass* obj2 = pool.acquire();  // This should trigger extra allocation
    ASSERT_NE(obj2, nullptr);

    ASSERT_NE(obj1, obj2);

    ASSERT_TRUE(pool.release(obj2));
    ASSERT_TRUE(pool.release(obj1));
}

TEST(AdaptivePobjectPool, InvalidRelease) {
    pool::AdaptivePobjectPool<MyClass, 5> pool;

    MyClass* obj1 = new MyClass();

    ASSERT_FALSE(pool.release(obj1));

    delete obj1;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

