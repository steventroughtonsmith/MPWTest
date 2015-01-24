#
# Requires https://github.com/ksherlock/mpw and an
# install of MPW in the emulator's root (~/mpw)
# Uses modern (Xcode) version of Rez
#

# Location of your mpw binary
MPW=~/bin/mpw
HFSDM=/Applications/HFS\ Disk\ Maker.app/Contents/MacOS/hfs_disk_maker_cli

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

I386_LIBRARIES=-framework ApplicationServices -framework Carbon

CARBON_LIBRARIES={SharedLibraries}CarbonLib

CARBONFLAGS=-d TARGET_API_MAC_CARBON
TOOLBOXFLAGS=-d OLDROUTINENAMES=1
I386_CFLAGS=-D TARGET_API_MAC_CARBON=1 -arch i386 -w -c -fwritable-strings -fpascal-strings
I386_LDFLAGS=-arch i386

SOURCES=MPWTest.c

OBJECTS=$(SOURCES:%.c=obj/%.68k.o)
PPC_OBJECTS=$(SOURCES:%.c=obj/%.ppc.o)
I386_OBJECTS=$(SOURCES:%.c=obj/%.i386.o)

RFILES=MPWTest.r Carbon.r Size.r
EXECUTABLE=MPWTest

all: prepass bin/$(EXECUTABLE).i386 bin/$(EXECUTABLE).ppc bin/$(EXECUTABLE).68k package

prepass:
	mkdir -p obj bin disk

package:
	rm -f disk/$(EXECUTABLE).img.hqx
	$(HFSDM) bin disk/$(EXECUTABLE).img &> /dev/null
	SetFile -t dimg -c ddsk disk/$(EXECUTABLE).img
	binhex encode disk/$(EXECUTABLE).img

bin/$(EXECUTABLE).ppc: $(PPC_OBJECTS)
	if [ $(USE_CARBON) -eq 1 ]; then \
		$(MPW) PPCLink $(PPC_LDFLAGS) $(PPC_OBJECTS) $(CARBON_LIBRARIES) -o $@; \
	else \
		$(MPW) PPCLink $(PPC_LDFLAGS) $(PPC_OBJECTS) $(PPC_LIBRARIES) -o $@; \
	fi;
	Rez -rd $(RFILES) -o $@ -i $(RINCLUDES) -append

bin/$(EXECUTABLE).68k: $(OBJECTS)
	$(MPW) link $(LDFLAGS) $(OBJECTS) $(LIBRARIES) -o $@
	Rez -rd $(RFILES) -o $@ -i $(RINCLUDES) -append

bin/$(EXECUTABLE).i386: $(I386_OBJECTS)
	mkdir -p bin/$(EXECUTABLE).i386.app/Contents/MacOS
	mkdir -p bin/$(EXECUTABLE).i386.app/Contents/Resources
	gcc $(I386_LDFLAGS) $(I386_OBJECTS) $(I386_LIBRARIES) -o bin/$(EXECUTABLE).i386.app/Contents/MacOS/$(EXECUTABLE).i386
	Rez -rd $(RFILES) -useDF -o bin/$(EXECUTABLE).i386.app/Contents/Resources/$(EXECUTABLE).i386.rsrc -i $(RINCLUDES) -append

obj/%.68k.o : %.c
		$(MPW) SC $(TOOLBOXFLAGS) $< -o $@

obj/%.ppc.o : %.c
	if [ $(USE_CARBON) -eq 1 ]; then \
		$(MPW) MrC $(CARBONFLAGS) $< -o $@; \
	else \
		$(MPW) MrC $(TOOLBOXFLAGS) $< -o $@; \
	fi;

obj/%.i386.o : %.c
	gcc $(I386_CFLAGS) $< -o $@;

clean:
	rm -rf disk bin obj