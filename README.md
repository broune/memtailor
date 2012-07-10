memtailor
=========

Memtailor is a C++ library of special purpose memory allocators. It
currently offers an arena allocator and a memory pool.

The main motivation to use a memtailor allocator is better and more
predictable performance than you get with new/delete. Sometimes a
memtailor allocator can also be more convenient due to the ability to
free many allocations at one time.

The Memtailor memory pool is useful if you need to do many allocations
of a fixed size. For example a memory pool is well suited to allocate
the nodes in a linked list.

You can think of the Memtailor arena allocator as being similar to
stack allocation. Both kinds of allocation are very fast and require
you to allocate/deallocate memory in last-in-first-out order. Arena
allocation has the further benefits that it stays within the C++
standard, it will not cause a stack overflow, you can have multiple
arena allocators at the same time and allocation is not tied to a
function invocation.
