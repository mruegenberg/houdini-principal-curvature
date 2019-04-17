#! /bin/bash
if [ -z "$HFS" ]; then
    export HFS=/opt/hfs17.5
    export PATH=$HFS/bin:$PATH
    export CUR=`pwd`
    cd $HFS
    source houdini_setup
    cd $CUR
fi

# HDK operators will break/crash in unexpected ways if the compiler version is not
# the same as used for Houdini itself (GCC 4.8 for Houdini 15 and 16.)
# The correct version can be seen in Help > About Houdini
# For Houdini 17.5: gcc-6.3.
# These older compiler versions should be available as a separate package for
# most Linux distributions.
# (Arch Linux: gcc48 and gcc6 in the AUR)
export CC=gcc-6
export CXX=g++-6

export HCUSTOM_CFLAGS="-fopenmp" # much faster
export HCUSTOM_LDFLAGS="-fopenmp" # much faster. Important to pass openMP during both compilation and linking!

# without debug info
hcustom -e -i ./dso SOP_principalcurvature.C -I eigen -I libigl/include

# with debug info
# hcustom -e -i ./dso -g SOP_principalcurvature.C -I eigen -I libigl/include
