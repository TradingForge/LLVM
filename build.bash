#!/bin/bash
rm -f /d/LLVM/build/bin/libclang.dll
pushd /d/LLVM/build || exit
make -j12
popd || exit
rm -f /c/Users/ellen/RiderProjects/mql/MqlNet/LibMqlSharp/ClangSharp/libclang.dll
cp /d/LLVM/build/bin/libclang.dll /c/Users/ellen/RiderProjects/mql/MqlNet/LibMqlSharp/ClangSharp/libclang.dll
