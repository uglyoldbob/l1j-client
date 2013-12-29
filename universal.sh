#!/bin/bash

rm -rf buildppc
rm -rf buildi386
rm -rf buildx86_64
rm -rf builduniv

#use xcode 3.1.4 or earlier to build this for powerpc
mkdir -p buildppc
cd buildppc
../configure --host=powerpc-apple-darwin OBJCFLAGS="-arch ppc" CFLAGS="-F../libs -arch ppc" CXXFLAGS="-F../libs -arch ppc" LDFLAGS="-arch ppc"
make clean
make mac_universal mac_strip
cd ..

mkdir -p buildi386
cd buildi386
../configure --host=i386-apple-darwin OBJCFLAGS="-arch i386" CFLAGS="-F../libs -arch i386" CXXFLAGS="-F../libs -arch i386" LDFLAGS="-arch i386"
make clean
make mac_universal mac_strip
cd ..

mkdir -p buildx86_64
cd buildx86_64
../configure --host=x86_64-apple-darwin OBJCFLAGS="-arch x86_64" CFLAGS="-F../libs -arch x86_64" CXXFLAGS="-F../libs -arch x86_64" LDFLAGS="-arch x86_64"
make clean
make mac_universal mac_strip installer
cd ..

mkdir -p builduniv
cp -vr ./buildx86_64/release_client.app ./builduniv/

lipo buildppc/Lineage buildi386/Lineage buildx86_64/Lineage -create -output ./builduniv/release_client.app/Contents/MacOS/Lineage
lipo buildppc/Admin buildi386/Admin buildx86_64/Admin -create -output ./builduniv/release_client.app/Contents/MacOS/Admin

filename="lineage_osx_universal_beta_"
filename+=`date +%d%B%G`
filename+=".dmg"
rm -f "$filename"
hdiutil create "builduniv/$filename" -volname "$filename" -fs HFS+ -srcfolder "./builduniv/release_client.app"
scp "builduniv/$filename" www-data@doors-software.com:/var/doors-software/download/lineage_client/
exit $?
