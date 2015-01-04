#
# Requires https://github.com/ksherlock/mpw and an
# install of MPW in the emulator's root (~/mpw)
# Uses modern (Xcode) version of Rez
#

RINCLUDES=/Applications/MPW-GM/Interfaces\&Libraries/Interfaces/RIncludes

LDFLAGS = -w -c 'MPS ' -t AAPL -model far \
	-sn STDIO=Main -sn INTENV=Main -sn %A5Init=Main

LIBRARIES={Libraries}Stubs.o \
	{Libraries}MacRuntime.o \
	{Libraries}IntEnv.o \
	{Libraries}Interface.o \
	{Libraries}ToolLibs.o \
	{CLibraries}StdCLib.o \

PPC_LIBRARIES={SharedLibraries}InterfaceLib \
	{SharedLibraries}StdCLib \
	{PPCLibraries}StdCRuntime.o \
	{PPCLibraries}PPCCRuntime.o

SOURCES=MPWTest.c
OBJECTS=$(SOURCES:.c=.68k.o)
PPC_OBJECTS=$(SOURCES:.c=.ppc.o)
RFILES=MPWTest.r
EXECUTABLE=MPWTest

all: $(EXECUTABLE).ppc $(EXECUTABLE).68k 
	open .

$(EXECUTABLE).ppc: $(PPC_OBJECTS)
	mpw PPCLink $(PPC_LDFLAGS) $(PPC_OBJECTS) $(PPC_LIBRARIES) -o $@
	Rez -rd $(RFILES) -o $@ -i $(RINCLUDES) -append

$(EXECUTABLE).68k: $(OBJECTS)
	mpw link $(LFLAGS) $(OBJECTS) $(LIBRARIES) -o $@
	Rez -rd $(RFILES) -o $@ -i $(RINCLUDES) -append

%.68k.o : %.c
	mpw SC $(SCFLAGS) $< -o $@

%.ppc.o : %.c
	mpw MrC $(MRCFLAGS) $< -o $@
	
clean:
	rm -rf *o $(EXECUTABLE).ppc $(EXECUTABLE).68k