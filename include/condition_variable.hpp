#include <linux/futex.h>
#include "lock.hpp"

namespace xsync {

/*
 * Transactional condition variable implementation.
 * Based on the deferred-signal design from a paper by Dudnik & Swift.
 */
class XCondVar {
public:
  XCondVar() : cv_counter_(0), futex_(0) {}
  void prepareWait();
  void completeWait();
  void signal();
  void broadcast();
private:
  int cv_counter_;
  int futex_;
};

} // namespace xsync
