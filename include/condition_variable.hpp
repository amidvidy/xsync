// STL
#include <atomic>

// xsync
#include "lock.hpp"
#include "scope.hpp"
#include "futex.hpp"


namespace xsync {

// Forward declaration
template <typename LockType>
class XScope;

/*
 * Transactional condition variable implementation.
 * Based on the deferred-signal design from a paper by Dudnik & Swift.
 */
template <typename LockType>
class XCondVar {
public:
    XCondVar() : cv_counter_(0) {}
    ~XCondVar() = default;

    void wait(XScope<LockType>& scope) {
        // Record the current value of the counter to prevent lost wakeups in case another thread signals before
        // we make it into the futex
        int counter_val = cv_counter_.load();
        // Commit partial results
        scope.exit();
        // Wait if there has been no intervening signal
        futex::wait(&cv_counter_, counter_val);
        // Resume transactional execution
        scope.enter();
    };

    void signal(XScope<LockType>& scope) {
        scope.registerCommitCallback(std::bind(&XCondVar::signalCommit, this));
    }

    void signalCommit() {
        // this increment is atomic
        ++cv_counter_;
        // wake up a waiter
        futex::wake(&cv_counter_, 1);
    }

    void broadcast(XScope<LockType>& scope) {
        scope.registerCommitCallback(std::bind(&XCondVar::broadcastCommit, this));
    }

    void broadcastCommit() {
        // atomic increment
        ++cv_counter_;
        // wake 'em all up
        futex::wake(&cv_counter_, std::numeric_limits<int>::max());
    }
private:
    std::atomic<int> cv_counter_;
};

} // namespace xsync
