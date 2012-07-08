# options passed to aclocal, which is a tool for making macroes visible to
# autoconf. We use -I to tell aclocal where we put the local macros.
ACLOCAL_AMFLAGS = -I build/autotools/m4

# Options passed to the C PreProcessor (CPP), NOT the C Plus Plus compiler.
AM_CPPFLAGS = -I${top_srcdir}/

# tell Libtool what the name of the library is.
lib_LTLIBRARIES = libmemtailor-@MEMTAILOR_API_VERSION@.la

# set the C++ compiler to include src/
AM_CXXFLAGS=-I$(top_srcdir)/src/

# the sources that are built to make the library
libmemtailor_@MEMTAILOR_API_VERSION@_la_SOURCES =		\
  src/memtailor/BufferPool.cpp src/memtailor/Arena.cpp	\
  src/memtailor/MemoryBlocks.cpp

# The headers that are installed.
memtailorA_includedir = $(includedir)/memtailor-$(MEMTAILOR_API_VERSION)
memtailorA_include_HEADERS = src/memtailor.h
# install remaining headers into a memtailor subdirectory of the include dir
memtailorB_includedir = \
  $(includedir)/memtailor-$(MEMTAILOR_API_VERSION)/memtailor
memtailorB_include_HEADERS = src/memtailor/stdinc.h			\
  src/memtailor/BufferPool.h src/memtailor/Arena.h			\
  src/memtailor/ArenaVector.h src/memtailor/MemoryBlocks.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = build/autotools/memtailor-$(MEMTAILOR_API_VERSION).pc

# When making a distribution file, Automake knows to include all files
# that are necessary to build the project. EXTRA_DIST specifies files
# to include beyond those used in the build process.
EXTRA_DIST = autogen.sh libs/gtest

# allow to download gtest in case of needing to run unit tests
GTEST_DIR = $(top_srcdir)/libs/
GTEST_TMP_DIR = $(top_srcdir)/libs/
GTEST_VERSION = 1.6.0
GTEST_DOWNLOAD_FILE = gtest-$(GTEST_VERSION).zip
gtest: $(GTEST_DIR)/gtest
$(GTEST_DIR)/gtest:
	mkdir -p $(GTEST_TMP_DIR);
	rm -rf $(GTEST_TMP_DIR)/$(GTEST_DOWNLOAD_FILE);
	rm -rf $(GTEST_TMP_DIR)/gtest-$(GTEST_VERSION);
	rm -rf $(GTEST_TMP_DIR)/gtest;
	(cd $(GTEST_TMP_DIR); \
	  wget http://googletest.googlecode.com/files/$(GTEST_DOWNLOAD_FILE); \
	  unzip $(GTEST_DOWNLOAD_FILE); \
	  rm $(GTEST_DOWNLOAD_FILE))
	rm -rf $(GTEST_DIR)/gtest;
	mv $(GTEST_TMP_DIR)/gtest-$(GTEST_VERSION) $(GTEST_DIR)/gtest;
.PHONY: gtest $(GTEST_DIR)/gtest

TESTS=unittest
check_PROGRAMS=$(TESTS)

unittest_CXXFLAGS=\
  -I$(top_srcdir)/libs/gtest/include\
  -I$(top_srcdir)/libs/gtest/\
  -I$(top_srcdir)/src/
unittest_LDFLAGS = $(top_builddir)/libmemtailor-$(MEMTAILOR_API_VERSION).la

# test_LIBS=
unittest_SOURCES=src/test/ArenaTest.cpp src/test/BufferPoolTest.cpp	\
  src/test/MemoryBlocksTest.cpp src/test/testMain.cpp				\
  src/test/gtestInclude.cpp
