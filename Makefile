########################################################################
#                             T R O K A M
#                          Fair Search Engine
#
#  Copyright (C) 2018, Nicolas Slusarenko
#                      nicolas.slusarenko@trokam.com
#
#  This file is part of Trokam.
#
#  Trokam is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  Trokam is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with Trokam. If not, see <http://www.gnu.org/licenses/>.
########################################################################

CC=c++

CPPFLAGS=-c -std=c++14 -Wall -Wcpp -I./include

LDFLAGS=-g

LDLIBS= -lboost_system \
        -lboost_program_options \
        -lpqxx

BIN=./bin

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# source
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SOURCES_CRUNCHER= src/mainCruncher.cpp \
                  src/cruncher.cpp \
                  src/fileOps.cpp \
                  src/infoStore.cpp \
                  src/options.cpp \
                  src/postgresql.cpp \
                  src/textProcessing.cpp \
                  src/textStore.cpp \
                  src/web.cpp

OBJECTS_CRUNCHER=$(SOURCES_CRUNCHER:.cpp=.o)

SOURCES_INITIALISER= src/mainInitialiser.cpp \
                     src/fileOps.cpp \
                     src/infoStore.cpp \
                     src/initialiser.cpp \
                     src/options.cpp \
                     src/postgresql.cpp \
                     src/textProcessing.cpp \
                     src/textStore.cpp

OBJECTS_INITIALISER=$(SOURCES_INITIALISER:.cpp=.o)


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# targets
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

all: cruncher initialiser

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# individual recipes
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

cruncher: $(OBJECTS_CRUNCHER)
	$(CC) $(LDFLAGS) $(OBJECTS_CRUNCHER) -o $(BIN)/$@ $(LDLIBS)

initialiser: $(OBJECTS_INITIALISER)
	$(CC) $(LDFLAGS) $(OBJECTS_INITIALISER) -o $(BIN)/$@ $(LDLIBS)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# These are the list of files and the automatically
# generated dependencies to build them
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
DEPENDENCIES= $(OBJECTS:.o=.d)

include $(DEPENDENCIES:.cpp=.d)

%.d: %.cpp
		@set -e; rm -f $@; \
		$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.PHONY : clean
clean :
	-rm -f bin/*
	-rm -f src/*.o
