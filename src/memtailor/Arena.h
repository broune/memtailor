/* Copyright (C) 2011 Bjarke Hammersholt Roune (www.broune.com)
   MemTailor is distributed under the Modified BSD License. See license.txt. */
#ifndef MEMT_ARENA_GUARD
#define MEMT_ARENA_GUARD

#include "MemoryBlocks.h"
#include "stdinc.h"

#include <new>
#include <cstddef>
#include <utility>
#include <vector>

#ifdef MEMT_DEBUG
#include <vector>
#endif

namespace memt {
  /** This is an arena allocator. Arena allocators are very fast at the
      cost of imposing limitations on how memory can be deallocated.

      Allocation and deallocation must occur in stack order (LIFO). In
      other words, only the most recently allocated buffer that has not
      been deallocated yet can be deallocated. It is also possible to
      deallocate all buffers that were deallocated after a given buffer. In
      DEBUG mode stack order is enforced by ASSERTs.

      Arena satisfies allocation requests out of a larger block of
      memory. When a block is exhausted another block must be allocated
      using new. This new block is at least twice the size of the previous
      block. Old blocks are never re-used though they will be deallocated
      if they become old. So the current block is replaced if and only if
      it becomes exhausted.

      The scheme of geometric block growth is used because it allows a
      very fast implementation with excellent locality of
      reference. This can consume memory beyond that which the user of
      the Arena needs - all allocators have memory overhead. Optimal
      performance on both speed and memory consumption can usully be
      reached by all code using the same Arena object when that is
      possible given the stack-order limitation on deallocation.

      All methods throw bad_alloc if backing memory allocation using
      new fails.
  */
  class Arena {
  public:
    Arena();
    ~Arena();

    // ***** Basic void* interface *****

    /** Returns a pointer to a buffer of size bytes. Throws bad_alloc if
        that is not possible. All allocated and not freed buffers have
        unique addresses even when size is zero. */
    void* alloc(size_t size);

    /** Frees the buffer pointed to by ptr. That buffer must be the most
        recently allocated buffer from this Arena that has not yet been
        freed. Double frees are not allowed. ptr must not be null. */
    void freeTop(void* ptr);

    /** Frees the buffer pointed to by ptr and all not yet freed
        allocations that have happened since that buffer was allocated. ptr
        must not be null. */
    void freeAndAllAfter(void* ptr);

    /** Marks all previous allocations as freed. Does not deallocate
        all the backing memory. */
    void freeAllAllocs();

    /** Marks all previous allocations as freed and deallocates all
        backing memory. */
    void freeAllAllocsAndBackingMemory();

    // ***** Object interface *****

    /** Allocates and default constructs an instance of T.
        Only default construction supported. */
    template<class T>
      T* allocObject() {
      return new (allocObjectNoCon<T>()) T();
    }

    /** Allocates memory for an instance of T. No construction
        is performed. */
    template<class T>
    T* allocObjectNoCon() {
      return static_cast<T*>(alloc(sizeof(T)));
    }

    /** Destructs *ptr and then frees it as a memory buffer.
        That buffer must be the most recently allocated buffer from
        this Arena that has not yet been freed. Double frees are not
        allowed. ptr must not be null. */
    template<class T>
      void freeTopObject(T* ptr) {
      ptr->~T();
      freeTop(ptr);
    }

    /** Destructs *ptr and then frees it as a memory buffer
        along with all not yet freed allocations that have happened
        since that buffer was allocated. ptr must not be null. */
    template<class T>
      void freeObjectAndAllAfter(T* ptr) {
      ptr->~T();
      freeAndAllAfter(ptr);
    }

    // ***** Array interface *****

    /** As alloc(elementCount * sizeof(T)). Constructors for the
        elements of the array are not called. */
    template<class T>
      std::pair<T*, T*> allocArrayNoCon(size_t elementCount);

    /** As allocArrayNoCon except that constructors for the elements of
        the array are called. The constructors are called in increasing
        order of index. Constructed objects are destructed in reverse
        order if a constructor throws an exception. */
    template<class T>
      std::pair<T*, T*> allocArray(size_t elementCount);

    /** As freeTop(array) except that the elements of the array in the
        range (array, arrayEnd] are deconstructed in decreasing order of
        index. The destructors must not throw exceptions.

        array and arrayEnd must not be zero. */
    template<class T>
      void freeTopArray(T* array, T* arrayEnd);

    /** As freeTopArray(p.first, p.second). */
    template<class T>
      void freeTopArray(std::pair<T*, T*> p) {freeTopArray(p.first, p.second);}

    /** As freeAndAllAfter(array) except that the elements of the array
        in the range (array, arrayEnd] are deconstructed in decreasing
        order of index. The destructors must not throw exceptions. */
    template<class T>
      void freeArrayAndAllAfter(T* array, T* arrayEnd);

    /** As freeTopArrayAndAllAfter(p.first, p.second). */
    template<class T>
      void freeArrayAndAllAfter(std::pair<T*, T*> p) {
      freeArrayAndAllAfter(p.first, p.second);
    }

    // ***** Miscellaneous *****

    /** Returns true if there are no live allocations for this Arena. */
    inline bool isEmpty() const;

    /** Returns the total amount of memory allocated by this object. Includes
        excess capacity that has not been allocated by a client yet. Does NOT
        include memory for a DEBUG-only mechanism to catch bugs. */
    size_t getMemoryUse() const {return _blocks.getMemoryUse();}

    /** Returns the total amount of memory allocated by this object to
        clients. Does not include excess capacity that is not currently
        allocated by a client. Does not include memory for a DEBUG-only
        mechanism to catch bugs. */
    size_t getAllocatedMemoryUse() const {return _blocks.getMemoryUseToLeft();}

    /** Returns an arena object that can be used for non-thread safe
        scratch memory after static objects have been initialized. The
        default contract is that each function leaves this arena with the
        exact same objects allocated as before the function was entered. It
        is fine for functions to collaborate for example by using the arena
        to return variable size objects without calling new, though care
        should be used in such cases. */
    static Arena& getArena() {return _scratchArena;}

  private:
    typedef MemoryBlocks::Block Block;
    Block& block() {return _blocks.getFrontBlock();}
    const Block& block() const {return _blocks.getFrontBlock();}

    /** Allocate a new block with at least needed bytes and at least
        double the capacity of the current block. */
    void growCapacity(size_t needed);

    /** As freeTop where ptr was allocated from an old block. */
    void freeTopFromOldBlock(void* ptr);

    /** As freeAndAllAfter where ptr was allocated from an old block. */
    void freeAndAllAfterFromOldBlock(void* ptr);

    MemoryBlocks _blocks;
#ifdef MEMT_DEBUG
    std::vector<void*> _debugAllocs;
#endif

    static Arena _scratchArena;
  };

  inline bool Arena::isEmpty() const {
    return !block().hasPreviousBlock() && block().empty();
  }


  inline void* Arena::alloc(size_t size) {
    // It is OK to check capacity before aligning size as capacity is aligned.
    // This single if checks for three different special circumstances:
    //  * size is 0 (size - 1 will overflow)
    //  * there is not enough capacity (size > capacity)
    //  * aligning size would cause an overflow (capacity is aligned)
    const size_t capacity = block().getBytesToRight();
    MEMT_ASSERT(capacity % MemoryAlignment == 0);
    if (size - 1 >= capacity) {
      MEMT_ASSERT(size == 0 || size > capacity);
      if (size == 0) {
        size = 1;
        if (capacity > 0)
          goto capacityOK;
      }
      growCapacity(size);
    }
  capacityOK:
    MEMT_ASSERT(0 < size);
    MEMT_ASSERT(size <= block().getBytesToRight());
    MEMT_ASSERT(MemoryBlocks::alignNoOverflow(size) <= block().getBytesToRight());

    char* ptr = block().position();
    block().setPosition(ptr + MemoryBlocks::alignNoOverflow(size));

#ifdef MEMT_DEBUG
    _debugAllocs.push_back(ptr);
#endif
    return ptr;
  }

  inline void Arena::freeTop(void* ptr) {
    MEMT_ASSERT(ptr != 0);
#ifdef MEMT_DEBUG
    MEMT_ASSERT(!_debugAllocs.empty());
    MEMT_ASSERT(_debugAllocs.back() == ptr);
    _debugAllocs.pop_back();
#endif

    if (!block().empty())
      block().setPosition(ptr);
    else
      freeTopFromOldBlock(ptr);
  }

  inline void Arena::freeAndAllAfter(void* ptr) {
    MEMT_ASSERT(ptr != 0);
#ifdef MEMT_DEBUG
    while (!_debugAllocs.empty() && ptr != _debugAllocs.back())
      _debugAllocs.pop_back();
    MEMT_ASSERT(!_debugAllocs.empty());
    MEMT_ASSERT(_debugAllocs.back() == ptr);
    _debugAllocs.pop_back();
#endif

    if (block().isInBlock(ptr))
      block().setPosition(ptr);
    else
      freeAndAllAfterFromOldBlock(ptr);
  }

  template<class T>
    std::pair<T*, T*> Arena::allocArrayNoCon(size_t elementCount) {
    if (elementCount > static_cast<size_t>(-1) / sizeof(T))
      throw std::bad_alloc();
    const size_t size = elementCount * sizeof(T);
    MEMT_ASSERT(size / sizeof(T) == elementCount);
    char* buffer = static_cast<char*>(alloc(size));
    T* array = reinterpret_cast<T*>(buffer);
    T* arrayEnd = reinterpret_cast<T*>(buffer + size);
    return std::make_pair(array, arrayEnd);
  }

  template<class T>
    std::pair<T*, T*> Arena::allocArray(size_t elementCount) {
    std::pair<T*, T*> p = allocArrayNoCon<T>(elementCount);
    T* it = p.first;
    try {
      for (; it != p.second; ++it) {
        new (it) T();
      }
    } catch (...) {
      freeTopArray<T>(p.first, it);
      throw;
    }
    return p;
  }

  template<class T>
    void Arena::freeTopArray(T* array, T* arrayEnd) {
    MEMT_ASSERT(array != 0);
    MEMT_ASSERT(array <= arrayEnd);

    while (arrayEnd != array) {
      --arrayEnd;
      arrayEnd->~T();
    }
    freeTop(array);
  }

  template<class T>
    void Arena::freeArrayAndAllAfter(T* array, T* arrayEnd) {
    MEMT_ASSERT(array != 0);
    MEMT_ASSERT(array <= arrayEnd);

    while (arrayEnd != array) {
      --arrayEnd;
      arrayEnd->~T();
    }
    freeAndAllAfter(array);
  }
}

#endif
