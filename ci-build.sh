#!/bin/bash

TESTS=FALSE
DBG=$1

echo "* Starting CI building script"

if [[ -e build ]]; then
    rm -rf build
fi

if [[ -e docs ]]; then
    rm -rf docs
fi

doxygen Doxyfile

mkdir build
cd build

cmake -DDEBUG:BOOL=${DBG} -DBUILD_TESTING:BOOL=${TESTS} ..

cmake --build . -- -j2

case ${TESTS} in
    TRUE) ctest -j2
esac

cpack .

if [[ ${1} ]]; then
    mv *.deb ../packages/devel/
    cd ../packages/devel
    makepkg -csf --skipinteg
    exit 0
fi

mv *.deb ../packages/build/

## Archlinux automake
cd ../packages/build
makepkg -csf --skipinteg

exit 0
