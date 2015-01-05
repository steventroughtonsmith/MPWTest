#
# Requires https://github.com/ksherlock/mpw and an
# install of MPW in the emulator's root (~/mpw)
# Uses modern (Xcode) version of Rez
#

# Location of your mpw binary
MPW=~/bin/mpw

RINCLUDES=/Applications/MPW-GM/Interfaces\&Libraries/Interfaces/RIncludes

LDFLAGS =-w -c 'MPS ' -t APPL \
	-sn STDIO=Main -sn INTENV=Main -sn %A5Init=Main

PPC_LDFLAGS =-m main -w -c 'MPS ' -t APPL

USE_CARBON=0

LIBRARIES={Libraries}Stubs.o \
	{Libraries}MacRuntime.o \
	{Libraries}IntEnv.o \
	{Libraries}Interface.o \
	{Libraries}ToolLibs.o \
	{CLibraries}StdCLib.o

PPC_LIBRARIES={SharedLibraries}InterfaceLib \
	{SharedLibraries}StdCLib \
	{PPCLibraries}StdCRuntime.o \
	{PPCLibraries}PPCCRuntime.o

CARBON_LIBRARIES={SharedLibraries}CarbonLib

CARBONFLAGS=-d TARGET_API_MAC_CARBON
TOOLBOXFLAGS=

SOURCES=MPWTest.c

OBJECTS=$(SOURCES:.c=.68k.o)
PPC_OBJECTS=$(SOURCES:.c=.ppc.o)

RFILES=MPWTest.r Carbon.r Size.r
EXECUTABLE=MPWTest

all: $(EXECUTABLE).ppc $(EXECUTABLE).68k
	open .

$(EXECUTABLE).ppc: $(PPC_OBJECTS)
	if [ $(USE_CARBON) -eq 1 ]; then \
		$(MPW) PPCLink $(PPC_LDFLAGS) $(PPC_OBJECTS) $(CARBON_LIBRARIES) -o $@; \
	else \
		$(MPW) PPCLink $(PPC_LDFLAGS) $(PPC_OBJECTS) $(PPC_LIBRARIES) -o $@; \
	fi;
	Rez -rd $(RFILES) -o $@ -i $(RINCLUDES) -append

$(EXECUTABLE).68k: $(OBJECTS)
	if [ $(USE_CARBON) -eq 1 ]; then \
		$(MPW) link $(LDFLAGS) $(OBJECTS) $(CARBON_LIBRARIES) -o $@; \
	else \
		$(MPW) link $(LDFLAGS) $(OBJECTS) $(LIBRARIES) -o $@; \
	fi;
	Rez -rd $(RFILES) -o $@ -i $(RINCLUDES) -append

%.68k.o : %.c
	if [ $(USE_CARBON) -eq 1 ]; then \
		$(MPW) SC $(CARBONFLAGS) $< -o $@; \
	else \
		$(MPW) SC $(TOOLBOXFLAGS) $< -o $@; \
	fi;

%.ppc.o : %.c
	if [ $(USE_CARBON) -eq 1 ]; then \
		$(MPW) MrC $(CARBONFLAGS) $< -o $@; \
	else \
		$(MPW) MrC $(TOOLBOXFLAGS) $< -o $@; \
	fi;

clean:
	rm -rf *o $(EXECUTABLE).ppc $(EXECUTABLE).68k