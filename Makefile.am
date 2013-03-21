# options passed to aclocal, which is a tool for making macroes visible to
# autoconf. We use -I to tell aclocal where we put the local macros.
ACLOCAL_AMFLAGS = -I build/autotools/m4

# Options passed to the C PreProcessor (CPP), NOT the C Plus Plus compiler.
AM_CPPFLAGS = -I${top_srcdir}/

# tell Libtool what the name of the library is.
lib_LTLIBRARIES = libmemtailor.la

# set the C++ compiler to include src/
AM_CXXFLAGS=-I$(top_srcdir)/src/ -std=gnu++0x

# the sources that are built to make the library
libmemtailor_la_SOURCES =		\
  src/memtailor/BufferPool.cpp src/memtailor/Arena.cpp	\
  src/memtailor/MemoryBlocks.cpp

# The headers that are installed.
memtailorA_includedir = $(includedir)/memtailor
memtailorA_include_HEADERS = src/memtailor.h
# install remaining headers into a memtailor subdirectory of the include dir
memtailorB_includedir = \
  $(includedir)/memtailor/memtailor
memtailorB_include_HEADERS = src/memtailor/stdinc.h			\
  src/memtailor/BufferPool.h src/memtailor/Arena.h			\
  src/memtailor/ArenaVector.h src/memtailor/MemoryBlocks.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = build/autotools/memtailor.pc

# When making a distribution file, Automake knows to include all files
# that are necessary to build the project. EXTRA_DIST specifies files
# to include beyond those used in the build process.
EXTRA_DIST = autogen.sh

# set up tests to run on "make check"
if with_gtest

TESTS=unittest
check_PROGRAMS=$(TESTS)

unittest_CXXFLAGS = -I$(top_srcdir)/src/ -std=gnu++0x
unittest_LDFLAGS = $(top_builddir)/libmemtailor.la

# test_LIBS=
unittest_SOURCES=src/test/ArenaTest.cpp src/test/BufferPoolTest.cpp	\
  src/test/MemoryBlocksTest.cpp src/test/testMain.cpp				\
  src/test/gtestInclude.cpp

else

check:
	@echo
	@echo "Configure did not locate gtest, so unittests cannot be run."

endif
