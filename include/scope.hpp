#include "lock.hpp"

namespace xsync {

/*
 * Provides an RAII style wrapper for transactional execution.
 */
template <typename LockType>
class Scope {
public:
    Scope(LockType& fallback, bool writeLock = false);
    ~Scope();
private :
    LockType &fallback_;
};

}
