LIBRARYDIR=/Applications/MPW-GM/Interfaces\&Libraries/Libraries/Libraries
CLIBRARYDIR=/Applications/MPW-GM/Interfaces\&Libraries/Libraries/CLibraries

RINCLUDEDIR=/Applications/MPW-GM/Interfaces\&Libraries/Interfaces/RIncludes

LIBRARIES="StdCLib.o Stubs.o IntEnv.o MacRuntime.o Interface.o ToolLibs.o"

OUTPUTNAME=MPWTest
CFILES=MPWTest
RFILES=MPWTest

echo "Outputting to $CONFIGURATION_BUILD_DIR"
mkdir -p $CONFIGURATION_BUILD_DIR/.Intermediates/

pushd $CONFIGURATION_BUILD_DIR/.Intermediates/

mpw sc $SRCROOT/$CFILES.c -i /Applications/MPW-GM/Interfaces\&Libraries/Interfaces/CIncludes -o $CFILES.o

ln -fs $LIBRARYDIR/* $CONFIGURATION_BUILD_DIR/.Intermediates/
ln -fs $CLIBRARYDIR/* $CONFIGURATION_BUILD_DIR/.Intermediates/

mpw link -d -c 'HCC ' -t APPL -mf $CFILES.o $LIBRARIES -o $OUTPUTNAME
Rez -rd $SRCROOT/*.r -o $OUTPUTNAME -i $RINCLUDEDIR -append

cp $OUTPUTNAME $CONFIGURATION_BUILD_DIR/

open $CONFIGURATION_BUILD_DIR
