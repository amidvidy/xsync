#include "lock.hpp"
#include "scope.hpp"
#include <atomic>

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
    XCondVar(XScope<LockType>& scope) : cv_counter_(0), scope_(scope) {}
    ~XCondVar();
    void wait() {
        // Record the current value of the counter to prevent lost wakeups in case another thread signals before
        // we make it into the futex
        int counter_val = cv_counter_.load();
        // Commit partial results
        scope_.exit();
        // Wait if there has been no intervening signal
        futex::wait(&futex_, counter_val);
        // Resume transactional execution
        scope_.enter();
    };
    void signal() {
        scope_.registerCommitCallback(std::bind(&XCondVar::signalCommit, this));
    }
    void signalCommit() {
        // this increment is atomic
        ++cv_counter_;
        // wake up a waiter
        futex::wake(&futex_, 1);
    }
    void broadcast() {
        scope_.registerCommitCallback(std::bind(&XCondVar::broadcastCommit, this));
    }
    void broadcastCommit() {
        // atomic increment
        ++cv_counter_;
        // wake 'em allp
        futex::wake(&futex_, std::numeric_limits<int>::max());
    }
private:
    std::atomic<int> cv_counter_;
    XScope<LockType>& scope_;

};

} // namespace xsync
