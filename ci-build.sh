#!/usr/bin/env bash

src_dir=$(pwd)
TESTS=FALSE
DBG="Debug"
BUILD=1

if [[ -z "${MINOR_VERSION}" ]]; then
  MINOR=1
else
  MINOR="${MINOR_VERSION}"
fi

while [[ $# -gt 0 ]]; do
	case "$1" in
	-t | --tests)
		do_tests=1
		;;
	-d | --debug)
		do_debug=1
		;;
	-r | --release)
		do_release=1
		;;
	-p | --package)
		shift
		case "$1" in
		deb | DEB)
			do_pkg=DEB
			;;
		rpm | RPM)
			do_pkg=RPM
			;;
		tgz | TGZ)
			do_pkg=TGZ
			;;
		all)
			do_pkg=ALL
			;;
		*)
			echo "Wrong number of arguments in package:"
			echo "  --package [PKG_EXTENSION]"
			echo "    deb|DEB - Debian/Ubuntu package"
			echo "    rpm|RPM - openSUSE/Fedora package"
			echo "    tgz|TGZ - other distributive"
			echo "    all 	  - all available packages"
			exit 1
			;;
		esac
		;;
	-v | --version)
		BUILD=$2
		shift
		;;
	--docs)
		do_docs=1
		;;
	-h | --help | *)
		echo "Usage:"
		echo "  -p|--package [PKG]: Type of generated package"
		echo "  -t|--tests:         Fills build tests"
		echo "  -d|--debug:         Build package with debug information"
		echo "  -r|--release:       Build package without debug information"
		echo "  -v|--version:       Build package with custom version"
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
echo "do_pkg=$do_pkg | PKG=$PKG"
if [[ $do_docs ]]; then
	if [[ -e docs ]]; then
		rm -rf docs
	fi
	doxygen Doxyfile
fi
if [[ $do_debug ]]; then
	DBG="Debug"
fi
if [[ $do_release ]]; then
	DBG="Release"
fi
if [[ $do_tests ]]; then
	TESTS=TRUE
fi
echo "* Starting CI building script with BUILD_TYPE = $DBG"
if [[ -e build ]]; then
	rm -rf build
fi
mkdir build && cd build || exit
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr/ -DCMAKE_BUILD_TYPE=$DBG -DBUILD_TESTING:BOOL=$TESTS -DBUILD_NUMBER=$BUILD ..
cmake --build . -- -j2

if [[ $do_tests ]]; then
	ctest -j2
fi

if [[ $do_pkg ]]; then
	if [[ $PKG -eq "ALL" ]]; then
		cpack .
	else
		cpack . -G $PKG
	fi
	PACK_TYPE="release"
	if [[ $do_debug ]]; then
		PACK_TYPE="debug"
	fi
	mkdir -p "$src_dir"/packages/$PACK_TYPE/
	mv ./*-0."$MINOR"."$BUILD"-* "$src_dir"/packages/$PACK_TYPE/ 2> /dev/null
	if [[ $PKG -eq "DEB" || $PKG -eq "ALL" ]]; then
		cd "$src_dir"/packages/$PACK_TYPE/ || exit
		debtap -Q libsock_connect-0."$MINOR"."$BUILD"-1-x86_64.deb
		cd "$src_dir" || exit
	fi
fi
exit 0
