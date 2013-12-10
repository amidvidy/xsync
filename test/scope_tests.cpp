// GTest
#include "gtest/gtest.h"

// PThreads
#include <pthread.h>

// Intel intrinsics
#include <immintrin.h>

// xsync
#include "../include/scope.hpp"

TEST(ScopeTest, ExecutesTransactionally) {
    pthread_mutex_t fallback;
    pthread_mutex_init(&fallback, nullptr);
    unsigned char wasTransactional = 0;
    {
        xsync::XScope<pthread_mutex_t> scope(fallback);
        wasTransactional = _xtest();
    }

    EXPECT_TRUE(wasTransactional);
    pthread_mutex_destroy(&fallback);
}

TEST(ScopeTest, MakesForwardProgress) {
    pthread_mutex_t fallback;
    pthread_mutex_init(&fallback, nullptr);

    // This test will only finish if fallback is eventually taken
    {
        xsync::XScope<pthread_mutex_t> scope(fallback);
        if (_xtest()) _xabort(0);
    }

    EXPECT_TRUE(true);
    pthread_mutex_destroy(&fallback);
}

TEST(ScopeTest, ExecutesCommitCallback) {
    pthread_mutex_t fallback;
    pthread_mutex_init(&fallback, nullptr);

    bool callback_ran = false;

    {
        xsync::XScope<pthread_mutex_t> scope(fallback);
        scope.registerCommitCallback([&callback_ran]() { callback_ran = true; });
    }

    EXPECT_TRUE(callback_ran);
    pthread_mutex_destroy(&fallback);
}
