#! /bin/bash
if [ -z "$HFS" ]; then
    export HFS=/opt/hfs15.5.480
    export PATH=$HFS\bin:$PATH
    export CUR=`pwd`
    cd $HFS
    source houdini_setup
    cd $CUR
fi

# note: for Houdini 15.5, you need GCC 4.8!

# hcustom -e -i ./dso SOP_xfomatcher.C -I eigen -I libigl/include
hcustom -e -i ./dso -g SOP_xfomatcher.C -I eigen -I libigl/include

