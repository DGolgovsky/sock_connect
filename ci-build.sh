#!/usr/bin/env bash

src_dir=$(pwd)
TESTS=FALSE
DBG=FALSE
BUILD=5

while [[ $# -gt 0 ]]; do
	case "$1" in
	-t | --tests)
		do_tests=1
		shift
		;;
	-d | --debug)
		do_debug=1
		shift
		;;
	-b | --build)
		do_build=1
		shift
		;;
	-p | --package)
		case "$2" in
		deb | DEB)
			do_pkg=DEB
			;;
		rpm | RPM)
			do_pkg=RPM
			;;
		tgz | TGZ)
			do_pkg=TGZ
			;;
		*)
			echo "Wrong number of arguments in package:"
			echo "  --package [PKG_EXTENSION]"
			echo "    deb|DEB - Debian/Ubuntu package"
			echo "    rpm|RPM - openSUSE/Fedora package"
			echo "    tgz|TGZ - other distributives"
			exit 1
			;;
		esac
		shift
		;;
	-v | --version)
		BUILD=$2
		shift
		;;
	--docs)
		do_docs=1
		shift
		;;
	-h | --help | *)
		echo "Usage:"
		echo "  -p|--package [PKG]: Type of generated package"
		echo "  -t|--tests:         Fills build tests"
		echo "  -d|--debug:         Build package with debug information"
		echo "  -b|--build:         Build package without debug information"
		echo "    |--docs:          Generate doxygen documentation"
		echo "  -h|--help:          Print this message"
		exit 1
		;;
	esac
	shift
done
if [[ ! $do_pkg ]]; then
	distroname=$(grep PRETTY_NAME /etc/os-release | sed 's/PRETTY_NAME=//g' | tr -d '="' | awk -F' ' '{print $1}')
	if [[ $distroname == "Ubuntu" || $distroname == "Debian" ]]; then
		PKG=DEB
	elif [[ $distroname == "openSUSE" || $distroname == "Fedora" ]]; then
		PKG=RPM
	else
		PKG=TGZ
	fi
else
	PKG=$do_pkg
fi
if [[ $do_docs ]]; then
	if [[ -e docs ]]; then
		rm -rf docs
	fi
	doxygen Doxyfile
fi
if [[ $do_debug ]]; then
	DBG=TRUE
fi
if [[ $do_build ]]; then
	DBG=FALSE
fi
if [[ $do_tests ]]; then
	TESTS=TRUE
fi
echo "* Starting CI building script with DEBUG = $DBG"
if [[ -e build ]]; then
	rm -rf build
fi
mkdir build && cd build || exit
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr/ -DDEBUG:BOOL=$DBG -DBUILD_TESTING:BOOL=$TESTS -DGEN_PKG=$PKG -DBUILD_NUMBER="$BUILD" ..
cmake --build . -- -j2
if [[ $do_tests ]]; then
	ctest -j2
fi
if [[ $do_pkg ]]; then
	cpack .
	PACK_TYPE="build"
	if [[ $do_debug ]]; then
		PACK_TYPE="debug"
	fi
	mkdir -p "$src_dir"/packages/$PACK_TYPE/
	mv libsock_connect-* "$src_dir"/packages/$PACK_TYPE/
		if [[ $PKG != "DEB" ]]; then
		exit 0
	fi
	if [ -f "/etc/arch-release" ]; then
		cd "$src_dir"/packages/$PACK_TYPE || exit
		echo "# Maintainer: Dmitry Golgovsky
# e-mail: d.westcoast@aol.com

pkgname=libsock_connect
pkgver=0.$BUILD
pkgrel=1
pkgdesc=\"Prototype shared library of Socket Connector.\"
conflicts=(libsock_connect_devel)
arch=('x86_64')
url=\"https://github.com/DGolgovsky/sock_connect\"
license=('MIT')
groups=('')
options=('!strip' '!emptydirs')
depends=(gcc glibc)
source_x86_64=(\"libsock_connect-0.$BUILD-1-x86_64.deb\")

package() {
    # Extract package data
	tar xzf data.tar.gz -C \"\${pkgdir}\"
}" >PKGBUILD
		CHROOT=/tmp/chroot
		mkdir $CHROOT
		mkarchroot -C /etc/pacman.conf -M /etc/makepkg.conf $CHROOT/root base-devel
		arch-nspawn $CHROOT/root pacman -Syu
		makechrootpkg -c -r $CHROOT -- --skipinteg
		rm PKGBUILD
		rm ./*.log
		sudo rm -rf $CHROOT
	fi
	cd "$src_dir" || exit
fi
exit 0
