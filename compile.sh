#! /bin/bash
if [ -z "$HFS" ]; then
    export HFS=/opt/hfs16.0.600
    export PATH=$HFS/bin:$PATH
    export CUR=`pwd`
    cd $HFS
    source houdini_setup
    cd $CUR
fi

# HDK operators will break/crash in unexpected ways if the compiler version is not
# the same as used for Houdini itself (GCC 4.8 for Houdini 15 and 16.)
# The correct version can be seen in Help > About Houdini
export CC=gcc-4.8
export CXX=g++-4.8

export HCUSTOM_CFLAGS="-fopenmp" # much faster

# without debug info
hcustom -e -i ./dso SOP_principalcurvature.C -I eigen -I libigl/include

# with debug info
# hcustom -e -i ./dso -g SOP_principalcurvature.C -I eigen -I libigl/include
