# SPDX-License-Identifier: GPL-2.0-or-later
# dpkg-parsechangelog is significantly slow
LIBNAME = libtbl
MAJOR = 3
MINOR = 0

SHLIB        = $(LIBNAME).so
SONAME       = $(SHLIB).$(MAJOR)
BUILD_SONAME = $(SONAME).$(MINOR)

CC ?= gcc
CPP = g++
ARCH = $(shell $(CC) -Q --help=target | grep -e -march | awk '{print $$2}')
CFLAGS = -fPIC -Wall -O2 -g -Iinclude/
GTEST_LDFLAGS = -pthread -lgtest_main -lgtest -lpthread
LDFLAGS = -shared -Wl,-soname,$(SONAME)

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

TSRC = $(wildcard test/*.c)
TOBJ = $(TSRC:.c=.o)

GTEST_SRC = $(wildcard test/*.cpp)
GTEST_OBJ = $(GTEST_SRC:.cpp=.o)

TARGET_LIB  = $(BUILD_SONAME)
TARGET_LINKS = $(SONAME) $(SHLIB)
TARGETS_TESTS = libtbl_example libtbl_regress
TARGETS_GTESTS = libtbl_unittests
TARGETS = $(TARGET_LIB) $(TARGET_LINKS) $(TARGETS_TESTS)

.PHONY: all
all: build

build: $(TARGET_LIB) $(TARGET_LINKS)

test: build $(TARGETS_TESTS)

# Stolen from:
# https://www.gnu.org/software/make/manual/html_node/Automatic-Prerequisites.html
src/%.d: src/%.c test/%.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

test/%.d: test/%.cpp
	@set -e; rm -f $@; \
	$(CPP) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

ifneq ($(MAKECMDGOALS),clean)
# do not include for 'clean' goal. make won’t create *.d only to
# immediately remove them again.
-include $(OBJ:.o=.d)
-include $(TOBJ:.o=.d)
-include $(GTEST_OBJ:.o=.d)
endif

LIBS=-L. -ltbl

$(TARGET_LIB): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

$(SHLIB): $(TARGET_LIB)
	ln -sf $(TARGET_LIB) $(SHLIB)

$(SONAME): $(TARGET_LIB)
	ln -sf $(TARGET_LIB) $(SONAME)

libtbl_example: $(TOBJ) $(SONAME)
	$(CC) -o $@ $^ $(LIBS)
	/bin/sh -c "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`; ./libtbl_example table"

libtbl_regress: libtbl_example
	/bin/sh -c "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`; cd test; python3 libtbl_test.py -p ../"

libtbl_unittests: $(GTEST_OBJ) $(OBJ)
	g++ $(GTEST_LDFLAGS) -o $@ $^
	./libtbl_unittests

.PHONY: install
install:
	mkdir -p $(DESTDIR)/usr/lib $(DESTDIR)/usr/include
	cp include/libtbl.h $(DESTDIR)/usr/include/
	cp -d $(LIBNAME).so* $(DESTDIR)/usr/lib

.PHONY: remove
remove:
	rm -f $(DESTDIR)/usr/lib/$(LIBNAME).so* $(DESTDIR)/usr/include/libtbl.h

.PHONY: clean
clean:
	rm -f *~ $(TARGETS) $(TARGETS_TESTS) $(TARGETS_GTESTS) $(OBJ) $(TOBJ) $(GTEST_OBJ) $(OBJ:.o=.d) $(TOBJ:.o=.d) $(GTEST_OBJ:.o=.d)
