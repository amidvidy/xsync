#include <linux/syscall.h>
#include <linux/futex.h>
#include "../include/futex.hpp"

namespace xsync {

namespace futex {

int wait(int *futex, int val) {
    return syscall(SYS_futex, futex, val, nullptr, nullptr, nullptr);
}

int wake(int *futex, int thread_count) {
    return syscall()
}

} // namespace futex

} // namespace xsync {
