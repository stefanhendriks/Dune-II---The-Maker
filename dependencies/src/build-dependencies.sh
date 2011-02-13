#!/bin/sh

build_alfont()
{
	cd alfont
	rm -rf build
	mkdir build
	cd build
	unzip ../alfont.zip
	sed -i "s/^TARGET=/#TARGET=/g" Makefile
	sed -i "s/^#TARGET=LINUX_STATIC/TARGET=LINUX_STATIC/g" Makefile
	make
}

build_almp3()
{
	cd almp3
	rm -rf build
	mkdir build
	cd build
	unzip ../almp3.zip
	sed -i "s/^TARGET=/#TARGET=/g" Makefile
	sed -i "s/^#TARGET=LINUX_STATIC/TARGET=LINUX_STATIC/g" Makefile
	sed -i "s/(char \*)data += data_start_skipped/data += data_start_skipped/g" src/almp3.c
	sed -i "s/(char \*)data += skipbytes/data += skipbytes/g" src/almp3.c
	make
}

build_fblend()
{
	cd fblend0.5
	rm -rf build
	mkdir build
	cd build
	unzip ../fblend-0.5-beta01.zip
	cd fblend
	sed -i "s/makefile.mgw/makefile.unx/g" makefile
	sed -i "7iSRC_DIR_U=\$(SRC_DIR)" make/makefile.unx
	make
}

machine=`uname -m`
if [ "$machine" != "i686" ]; then
	echo "incompatible cpu - expected i686 - was $machine"
	exit 1
fi

echo "building alfont..."
build_alfont > alfont_build.log 2>&1

echo "building almp3..."
build_almp3 > almp3_build.log 2>&1

echo "building fblend..."
build_fblend > fblend_build.log 2>&1

echo "finished!"

