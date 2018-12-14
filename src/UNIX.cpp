#include "UNIX.h"

#ifdef NDEBUG
#include <iostream>
#endif

UNIX::UNIX(uint32_t &path)
: Connection(_UNIX, path, 0)
{

}

UNIX::~UNIX() {

}
