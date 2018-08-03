########################################################################
#                             T R O K A M
#                        Internet Search Engine
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

CPPFLAGS=-c -g -pg -std=c++14 -O2 -Wall -Wcpp -I./include \
                                              -I/usr/local/include \

LDFLAGS=-g -pg -pthread -L/usr/local/lib

LDLIBS= -lboost_system \
        -lboost_program_options \
        -lboost_serialization \
        -lpqxx \
        -lmagic

LDLIBS_WEB= -lboost_program_options \
            -lboost_regex \
            -lboost_signals \
            -lboost_system \
            -lboost_thread \
            -lpqxx \
            -lmagic \
            -lwt \
            -lwtfcgi

BIN=./bin

WEB_SERVER_BIN_DIR=/var/www/html/bin
WEB_SERVER_APP_ROOT=/usr/local/etc
WEB_SERVER_DOC_ROOT=/var/www/html
WT_RUN_TIME=/usr/local/run/wt

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# source
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SOURCES_CRUNCHER= src/mainCruncher.cpp \
                  src/cruncher.cpp \
                  src/control.cpp \
                  src/differentStrings.cpp \
                  src/exception.cpp \
                  src/fileOps.cpp \
                  src/infoStore.cpp \
                  src/options.cpp \
                  src/pageInfo.cpp \
                  src/pageProcessing.cpp \
                  src/postgresql.cpp \
                  src/reporting.cpp \
                  src/textProcessing.cpp \
                  src/textStore.cpp \
                  src/web.cpp

OBJECTS_CRUNCHER=$(SOURCES_CRUNCHER:.cpp=.o)

SOURCES_INITIALISER= src/mainInitialiser.cpp \
                     src/differentStrings.cpp \
                     src/exception.cpp \
                     src/fileOps.cpp \
                     src/infoStore.cpp \
                     src/initialiser.cpp \
                     src/options.cpp \
                     src/pageInfo.cpp \
                     src/pageProcessing.cpp \
                     src/postgresql.cpp \
					 src/reporting.cpp \
                     src/textProcessing.cpp \
                     src/textStore.cpp

OBJECTS_INITIALISER=$(SOURCES_INITIALISER:.cpp=.o)

SOURCES_FEEDER= src/mainFeeder.cpp \
                src/differentStrings.cpp \
                src/exception.cpp \
                src/fileOps.cpp \
                src/infoCopier.cpp \
                src/feeder.cpp \
                src/options.cpp \
                src/pageInfo.cpp \
                src/pageProcessing.cpp \
                src/postgresql.cpp \
                src/reporting.cpp \
                src/textProcessing.cpp \
                src/textStore.cpp

OBJECTS_FEEDER=$(SOURCES_FEEDER:.cpp=.o)

SOURCES_WEBSEARCH= src/mainWebSearch.cpp \
                   src/searchWidget.cpp \
                   src/aboutWidget.cpp \
                   src/ackWidget.cpp \
                   src/donWidget.cpp \
                   src/pageWidget.cpp \
                   src/appGenerator.cpp \
                   src/differentStrings.cpp \
                   src/exception.cpp \
                   src/fileOps.cpp \
                   src/infoStore.cpp \
                   src/options.cpp \
                   src/pageInfo.cpp \
                   src/postgresql.cpp \
                   src/reporting.cpp \
                   src/sharedResources.cpp \
                   src/textProcessing.cpp \
                   src/textStore.cpp \
                   src/topWindow.cpp

OBJECTS_WEBSEARCH=$(SOURCES_WEBSEARCH:.cpp=.o)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# targets
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

all: cruncher initialiser feeder search.wt

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# individual recipes
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

cruncher: $(OBJECTS_CRUNCHER)
	$(CC) $(LDFLAGS) $(OBJECTS_CRUNCHER) -o $(BIN)/$@ $(LDLIBS)

initialiser: $(OBJECTS_INITIALISER)
	$(CC) $(LDFLAGS) $(OBJECTS_INITIALISER) -o $(BIN)/$@ $(LDLIBS)

feeder: $(OBJECTS_FEEDER)
	$(CC) $(LDFLAGS) $(OBJECTS_FEEDER) -o $(BIN)/$@ $(LDLIBS)

search.wt: $(OBJECTS_WEBSEARCH)
	$(CC) $(LDFLAGS) $(OBJECTS_WEBSEARCH) -o $(BIN)/$@ $(LDLIBS_WEB)

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

.PHONY : install
install :
		sudo mkdir -p $(WEB_SERVER_BIN_DIR)
		sudo cp $(BIN)/search.wt $(WEB_SERVER_BIN_DIR)

		sudo cp -r -a page/* $(WEB_SERVER_DOC_ROOT)
		sudo mkdir -p $(WEB_SERVER_DOC_ROOT)/style
		sudo cp -r -a docroot/style/* $(WEB_SERVER_DOC_ROOT)/style
		sudo mkdir -p $(WEB_SERVER_DOC_ROOT)/image
		sudo cp -r -a image/* $(WEB_SERVER_DOC_ROOT)/image
		sudo chown www-data:www-data -R $(WEB_SERVER_DOC_ROOT)

		sudo cp -r -a approot/* $(WEB_SERVER_APP_ROOT)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.PHONY : clean
clean :
	-rm -f bin/*
	-rm -f src/*.o
