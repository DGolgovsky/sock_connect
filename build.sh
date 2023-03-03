#!/usr/bin/env bash

COMPONENT_NAME=sock_connect
SRC_DIR=$(pwd)
TESTS=FALSE
BUILD_TYPE="Debug"

MAJOR=0
if [[ -z "${MINOR_VERSION}" ]]; then
    MINOR=1
else
    MINOR="${MINOR_VERSION}"
fi
BUILD_VERSION=6

function print_help_options {
cat << EOF
usage:  build.sh [options]
options:
  -p, --package [PKG]  Type of generated package
  -t, --tests          Fills build tests
  -d, --debug          Build with debug information (default)
  -r, --release        Build without debug information
  -i, --install        Install after build. May be need root access
  -u, --uninstall      Uninstall previous build (if build dir still exists)
  -v, --version [VER]  Build package with custom version
      --docs           Generate doxygen documentation
  -h, --help           Print this message
EOF
}

function print_help_packages {
cat << EOF
Wrong number of arguments in package
usage:  --package <extension>"
extension:
    deb|DEB - Debian/Ubuntu package"
    rpm|RPM - openSUSE/Fedora package"
    pkg|PKG - ArchLinux package"
    tgz|TGZ - Other distributive"
    all|ALL - All available packages"
EOF
}

while [[ $# -gt 0 ]]; do
  case $1 in
    -t | --tests)     do_tests=1;;
    -d | --debug)     do_debug=1;;
    -r | --release)   do_release=1;;
    -i | --install)   do_install=1;;
    -u | --uninstall) do_uninstall=1;;
         --docs)      do_docs=1;;
    -v | --version)   BUILD_VERSION=$2; shift;;
    -p | --package)   case $2 in
                        deb | DEB) do_pkg=DEB;;
                        rpm | RPM) do_pkg=RPM;;
                        pkg | PKG) do_pkg=PKG;;
                        tgz | TGZ) do_pkg=TGZ;;
                        all | ALL) do_pkg=ALL;;
                        *) print_help_packages; exit 1;;
                      esac
                      shift;;
    -h | --help | *)  print_help_options; exit 1;;
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
    BUILD_TYPE="Debug"
fi
if [[ $do_release ]]; then
    BUILD_TYPE="Release"
fi
if [[ $do_tests ]]; then
    TESTS=TRUE
fi

if [[ ${do_uninstall} ]]; then
  cd build/$BUILD_TYPE || exit 1
  sudo make uninstall
  cd "$SRC_DIR" || exit 1
  exit 0
fi

echo "* Start building script with BUILD_TYPE = $BUILD_TYPE"
if [[ -e build/$BUILD_TYPE ]]; then
    rm -rf build/$BUILD_TYPE/*
fi
mkdir -p build/$BUILD_TYPE 2>/dev/null && cd build/$BUILD_TYPE || exit
cmake "$SRC_DIR" \
  -DCMAKE_INSTALL_PREFIX:PATH=/usr/ \
  -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
  -DTESTS:BOOL=$TESTS \
  -DBUILD_VERSION="$BUILD_VERSION" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=${TESTS} \
  -DCMAKE_CXX_CPPCHECK=/usr/bin/cppcheck
cmake --build . -- -j $(nproc)

if [[ $do_tests ]]; then
  ctest -j2
fi
if [[ ${do_install} ]]; then
  sudo make install
fi

if [[ $do_pkg ]]; then
  if   [[ $PKG == "ALL" ]]; then
    cpack .
  elif [[ $PKG == "PKG" ]]; then
    cpack -G DEB
    debtap -Q lib$COMPONENT_NAME-$MAJOR.$MINOR.$BUILD_VERSION-1-x86_64.deb
    rm lib$COMPONENT_NAME-$MAJOR.$MINOR.$BUILD_VERSION-1-x86_64.deb*
  else
    cpack -G $PKG
  fi
  PACK_TYPE="release"
  if [[ $do_debug ]]; then
    PACK_TYPE="debug"
  fi
  mkdir -p "$SRC_DIR"/packages/$PACK_TYPE/
  mv ./*-$MAJOR.$MINOR.$BUILD_VERSION-* "$SRC_DIR"/packages/$PACK_TYPE/ 2>/dev/null
fi

cd "$SRC_DIR" || exit

exit 0
