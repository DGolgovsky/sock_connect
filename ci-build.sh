#!/bin/bash

TESTS=FALSE
DBG=$1

echo "* Starting CI building script"

if [[ -e build ]]; then
    rm -rf build
fi

if [[ -e docs ]]; then
    rm -rf docs
    doxygen Doxyfile
fi

mkdir build && cd build

cmake -DDEBUG:BOOL=${DBG} -DBUILD_TESTING:BOOL=${TESTS} ..
cmake --build . -- -j2

case ${TESTS} in
    TRUE) ctest -j2
esac

cpack .

if [[ ${1} ]]; then
    mv *.deb ../packages/devel/
    ## Archlinux automake
    if [ -f "/etc/arch-release" ]; then
        cd ../packages/devel
        makepkg -csf --skipinteg
        exit 0
    fi
fi

mv *.deb ../packages/build/

if [ -f "/etc/arch-release" ]; then
    cd ../packages/build
    makepkg -csf --skipinteg
    exit 0
fi

exit 0
