# boost.asio-wrapper, A C++ library, wrapping boost.asio, to be used for client or server applications
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
CC = g++ -std=c++11 -Wall -I$(SRCDIR)

LIBRARY = libnetwork.a

# Default target
default: $(LIBRARY)

####################################################
#        Other prerequisites / dependencies        #
####################################################

# network

wrapper/network/Connection.o: \
	wrapper/network/Connection.h
wrapper/network/Connection.h: \
	wrapper/network/events.h

wrapper/network/utils/Acceptor.o: \
	wrapper/network/utils/Acceptor.h
wrapper/network/utils/Acceptor.h: \
	wrapper/network/Connection.h \
	wrapper/network/events.h

wrapper/network/utils/Connector.o: \
	wrapper/network/utils/Connector.h
wrapper/network/utils/Connector.h: \
	wrapper/network/Connection.h \
	wrapper/network/events.h

wrapper/network/Connection_handler.o: \
	wrapper/network/Connection_handler.h
wrapper/network/Connection_handler.h: \
	wrapper/network/Connection.h \
	wrapper/network/utils/Acceptor.h \
	wrapper/network/utils/Connector.h \
	wrapper/network/events.h

#commands

wrapper/commands/interfaces/Command.h: \
	wrapper/network/events.h \
	wrapper/network/Connection.h \
	wrapper/network/Connection_handler.h

wrapper/commands/Command_hub.o: \
	wrapper/commands/Command_hub.h
wrapper/commands/Command_hub.h: \
	wrapper/commands/interfaces/Command.h

wrapper/commands/Execution_node.o: \
	wrapper/commands/Execution_node.h
wrapper/commands/Execution_node.h: \
	wrapper/commands/interfaces/Command.h

wrapper/commands/Filter_node.o: \
	wrapper/commands/Filter_node.h
wrapper/commands/Filter_node.h: \
	wrapper/commands/interfaces/Command.h

####################################################
#         Application definitions                  #
####################################################
OBJS = wrapper/network/Connection.o \
		wrapper/network/utils/Acceptor.o \
		wrapper/network/utils/Connector.o \
		wrapper/network/Connection_handler.o \
		wrapper/commands/Command_hub.o \
		wrapper/commands/Execution_node.o \
		wrapper/commands/Filter_node.o

$(LIBRARY): make_dirs $(OBJS)
	ar rcs $@ $(addprefix $(OBJDIR), $(OBJS))
	@echo Done linking $@.

####################################################
#        Generated Variables                       #
####################################################
OBJDIRS = $(subst $(SRCDIR),$(OBJDIR),$(shell find $(SRCDIR) -type d))

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
	-rm -r bin $(LIBRARY)

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

