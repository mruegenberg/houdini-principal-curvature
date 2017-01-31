#! /bin/bash
if [ -z "$HFS" ]; then
    export HFS=/opt/hfs15.5.673
    export PATH=$HFS/bin:$PATH
    export CUR=`pwd`
    cd $HFS
    source houdini_setup
    cd $CUR
fi

# note: for Houdini 15.5, you need GCC 4.8!

# without debug info
# hcustom -e -i ./dso SOP_principalcurvature.C -I eigen -I libigl/include

# with debug info
hcustom -e -i ./dso -g SOP_principalcurvature.C -I eigen -I libigl/include

