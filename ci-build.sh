#!/usr/bin/env bash

src_dir=$(pwd)
TESTS=FALSE
BUILD_TYPE="Debug"
BUILD_VERSION=1

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
	-i | --install)
		do_install=1
		;;
	-u | --uninstall)
		do_uninstall=1
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
		pkg | PKG)
			do_pkg=PKG
			;;
		tgz | TGZ)
			do_pkg=TGZ
			;;
		all | ALL)
			do_pkg=ALL
			;;
		*)
			echo "Wrong number of arguments in package:"
			echo "  --package [PKG_EXTENSION]"
			echo "    deb|DEB - Debian/Ubuntu package"
			echo "    rpm|RPM - openSUSE/Fedora package"
			echo "    pkg|PKG - ArchLinux package"
			echo "    tgz|TGZ - other distributive"
			echo "    all|ALL - all available packages"
			exit 1
			;;
		esac
		;;
	-v | --version)
		BUILD_VERSION=$2
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
		echo "  -i|--install:       Install after build. May be need root access"
		echo "  -u|--uninstall:     Uninstall previous build (if build dir still exists)"		
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

if [[ $do_docs ]]; then
	if [[ -e docs ]]; then
		rm -rf doxy_docs
	fi
	doxygen Doxyfile
fi
if [[ $do_debug ]]; then
	BUILD_TYPE="Debug"
fi
if [[ $do_release ]]; then
	BUILD_TYPE="Release"
fi
if [[ $do_tests ]]; then
	TESTS=TRUE
fi

if [[ ${do_uninstall} ]]; then
  cd build || exit 1
	sudo make uninstall
	cd "$src_dir" || exit 1
	exit 0
fi

echo "* Start CI building script with BUILD_TYPE = $BUILD_TYPE"
if [[ -e build ]]; then
	rm -rf build
fi
mkdir build 2> /dev/null && cd build || exit
cmake .. \
	-GNinja \
	-DCMAKE_INSTALL_PREFIX:PATH=/usr/ \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DTESTS:BOOL=$TESTS \
	-DBUILD_VERSION="$BUILD_VERSION" \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=${TESTS} \
	-DCMAKE_CXX_CPPCHECK=/usr/bin/cppcheck
#cmake --build . -- -j2
ninja

if [[ $do_tests ]]; then
	ctest -j2
fi

if [[ ${do_install} ]]; then
	sudo make install
fi

if [[ $do_pkg ]]; then
	if [[ $PKG == "ALL" ]]; then
		cpack .
	elif [[ $PKG == "PKG" ]]; then
	    cpack -G DEB
	    debtap -Q libsock_connect-0."$MINOR"."$BUILD_VERSION"-1-x86_64.deb
	    rm libsock_connect-0."$MINOR"."$BUILD_VERSION"-1-x86_64.deb*
	else
		cpack -G $PKG
	fi
	PACK_TYPE="release"
	if [[ $do_debug ]]; then
		PACK_TYPE="debug"
	fi
	mkdir -p "$src_dir"/packages/$PACK_TYPE/
	mv ./*-0."$MINOR"."$BUILD_VERSION"-* "$src_dir"/packages/$PACK_TYPE/ 2> /dev/null
fi

cd "$src_dir" || exit

exit 0
