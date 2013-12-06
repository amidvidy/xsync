#include "../include/condition_variable.hpp"
#include "../include/futex.hpp"
#include <emmintrin.h>

namespace xsync {

void XCondVar::wait() {
    // Record the current value of the counter in case we are signaled before we go to sleep.
    int counter_val = cv_counter_;
    // Commit the transaction
    _xend();
    // Wait if there has been no intervening signal
    futex::wait(&futex_, counter_val);
}

void XCondVar::signal() {

}

} // namespace xsync {
