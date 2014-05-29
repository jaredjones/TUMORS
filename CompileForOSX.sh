#!/bin/bash
BASEDIR=$(dirname $0)
cd $BASEDIR
rm -f CMakeCache.txt
rm -rf CMakeFiles
rm -rf BIN
mkdir BIN
cd BIN
cmake -G Xcode ../  -DWITH_WARNINGS=0 -DUSE_PCH=1
cd ../
echo ""
echo "Success! Check the BIN Folder for the Xcode Project!"
rm -f CMakeCache.txt
rm -rf CMakeFiles
