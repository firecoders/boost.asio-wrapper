# A C++ library, wrapping boost.asio, to be used for client or server applications
# Copyright (C) 2014 firecoders
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
# OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

####################################################
#        Constants                                 #
####################################################
SRCDIR = src/
OBJDIR = bin/
CC = g++ -std=c++11 -Wall -I$(SRCDIR) -fdiagnostics-color=auto

LIBRARY = libnetwork.a

# Default target
default: $(LIBRARY)

####################################################
#        Other prerequisites / dependencies        #
####################################################

####################################################
#         Application definitions                  #
####################################################
OBJS = 

$(LIBRARY): make_dirs $(OBJS)
	ar rcs $@ $(addprefix $(OBJDIR), $(OBJS))
	@echo Done linking $@.

####################################################
#        Generated Variables                       #
####################################################
OBJDIRS = $(subst $(SRCDIR),$(OBJDIR),$(shell find $(SRCDIR) -type d))
TEST_OBJECTS = $(subst .cpp,.o,$(shell find $(TEST_DIR) -type f -regex .*.cpp))

####################################################
#               Boost integration                  #
####################################################

BOOST_DIR = boost_1_55_0
BOOST_URL = http://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.tar.gz/download
BOOST_DOWNLOAD_NAME = boost

$(BOOST_DOWNLOAD_NAME):
	@echo "Downloading boost from $(BOOST_URL)"
	@wget -O boost_1_55_0.tar.gz $(BOOST_URL)
	@tar xzvf boost_1_55_0.tar.gz
	@rm boost_1_55_0.tar.gz
	@echo "Done downloading boost"

####################################################
#          Googletest integration                  #
####################################################

GTEST_DIR = dep/gtest-1.7.0
GTEST_URL = https://googletest.googlecode.com/files/gtest-1.7.0.zip
TEST_DIR = tests
TEST_EXEC_NAME = test

$(TEST_EXEC_NAME): $(TEST_OBJECTS) libengine.a dep/bin/libgtest.a
	$(CC) -pthread $^ -o $@
	@echo Done linking $@

$(TEST_DIR)/%.o: $(TEST_DIR)/%.cpp libengine.a dep/bin/libgtest.a
	$(CC) -isystem $(GTEST_DIR)/include -Isrc -c $(filter %.cpp, $^) -o $@

$(GTEST_DIR)/%:
	@echo "Downloading googletest from $(GTEST_URL)"
	@mkdir -p dep/bin
	@curl $(GTEST_URL) > gtest.zip
	@unzip gtest.zip -d dep
	@rm gtest.zip
	@echo "Done downloading googletest"

dep/bin/gtest-all.o: $(GTEST_DIR)/src/gtest-all.cc
	$(CC) -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) \
		-pthread -c $^ -o $@

dep/bin/gtest_main.o: $(GTEST_DIR)/src/gtest_main.cc
	$(CC) -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) \
		-pthread -c $^ -o $@

dep/bin/libgtest.a: dep/bin/gtest-all.o dep/bin/gtest_main.o
	ar -rv $@ $^
	@echo Done linking $@

####################################################
#          Other targets                           #
####################################################
.PHONY : clean make_dirs all

%.h:
	@touch $(SRCDIR)$@

%.hpp:
	@touch $(SRCDIR)$@

%.o : %.cpp
	$(CC) -c $(filter %.cpp, $^) -o $(OBJDIR)$@

clean:
	-rm -r bin $(LIBRARY) $(TEST_OBJECTS) $(TEST_EXEC_NAME)

make_dirs:
	@mkdir -p $(OBJDIRS)

all: $(LIBRARY)


####################################################
#          vpath                                   #
####################################################
vpath %.cpp $(SRCDIR)
vpath %.o   $(OBJDIR)
vpath %.h   $(SRCDIR)
vpath %.hpp $(SRCDIR)

