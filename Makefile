#
# Requires https://github.com/ksherlock/mpw and an
# install of MPW in the emulator's root (~/mpw)
# - I symlink to my MPW install in /Applications
#

MPWROOT=/Applications/MPW-GM

LIBRARYDIR=$(MPWROOT)/Interfaces\&Libraries/Libraries/Libraries
CLIBRARYDIR=$(MPWROOT)/Interfaces\&Libraries/Libraries/CLibraries
RINCLUDES=$(MPWROOT)/Interfaces\&Libraries/Interfaces/RIncludes

CC=~/bin/mpw sc
LINKER=~/bin/mpw link

CFLAGS=-i $(MPWROOT)/Interfaces\&Libraries/Interfaces/CIncludes
LFLAGS=-d -c 'HCC ' -t APPL -mf

LIBRARIES=lib/Stubs.o lib/MacRuntime.o lib/IntEnv.o lib/Interface.o lib/ToolLibs.o
CLIBRARIES=clib/StdCLib.o

SOURCES=MPWTest.c
OBJECTS=$(SOURCES:.c=.o)
RFILES=MPWTest.r
EXECUTABLE=MPWTest.68k

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	ln -fs $(LIBRARYDIR) lib
	ln -fs $(CLIBRARYDIR) clib
	$(LINKER) $(LFLAGS) $(OBJECTS) $(LIBRARIES) $(CLIBRARIES) -o $@
	Rez -rd $(RFILES) -o $@ -i $(RINCLUDES) -append
	open .

.c.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -rf *o $(EXECUTABLE)