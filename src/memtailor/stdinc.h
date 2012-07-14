/* Copyright (C) 2011 Bjarke Hammersholt Roune (www.broune.com)
   MemTailor is distributed under the Modified BSD License. See license.txt. */

#ifndef MEMT_STDINC_GUARD
#define MEMT_STDINC_GUARD

#if (defined MEMT_DEBUG) && (defined MEMT_NDEBUG)
#error Both MEMT_DEBUG and MEMT_NDEBUG defined
#endif

// If MemTailor has not been instructed to enable or disable debugging
// specifically, inherit behavior based on DEBUG
#if (!defined MEMT_DEBUG) && (!defined MEMT_NDEBUG)
#if (defined DEBUG) || (defined _DEBUG)
#define MEMT_DEBUG
#endif
#endif

#ifdef MEMT_DEBUG
#include <cassert>
#define MEMT_ASSERT(X) assert(X)
#else
#define MEMT_ASSERT(X)
#endif

namespace memt {
  /// The alignment that memory allocators must ensure. In other words
  /// allocators must return pointer addresses that are divisible by
  /// MemoryAlignment. MemoryAlignment must be a power of 2.
  static const unsigned int MemoryAlignment = sizeof(void*);

  static const unsigned int BitsPerByte = 8;
}

#endif
