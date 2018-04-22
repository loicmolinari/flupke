// Copyright © 2017-2018 Loïc Molinari <loicm@loicm.fr>
// Copyright © 2016 Canonical Ltd.
//
// This file is part of Quicken, licensed under the MIT license. See the license
// file at project root for full information.

#include "quickenglobal_p.h"

void* alignedAlloc(size_t alignment, size_t size)
{
    DASSERT(IS_POWER_OF_TWO(alignment));
    // DASSERT(((size % alignment) == 0));  // FIXME(loicm) ASSERT doesn't support '%'...

#if defined(__APPLE__)
    void* pointer;
    posix_memalign(&pointer, alignment, size);
    return pointer;
#elif defined(_WIN32)
    return _aligned_malloc(size, alignment);
#else
    return aligned_alloc(alignment, size);
#endif
}
