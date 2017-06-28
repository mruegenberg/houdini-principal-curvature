# houdini-principal-curvature
A Houdini DSO that extracts the minimal and maximal principal curvature directions from geometry.

## Installation
0. Clone the repository, including dependencies, e.g `git clone --recursive https://github.com/mruegenberg/houdini-principal-curvature.git`
1. Modify `compile.sh` or `compile.bat` (on Windows) to use your Houdini installation
2. Compile with the script
3. Move the resulting `dll`/`dylib`/`so` to your `HOUDINI_DSO_PATH` or add the path where it is located to it, 
   e.g export HOUDINI_DSO_PATH="`pwd`/dso:&"

## Usage
The node takes just one input. It writes `min_principalcurvature` and `max_principalcurvature` point attributes.

The input mesh must be triangular (e.g using the *Divide* or *Remesh* SOPs).

## Dependencies

- [libigl](http://libigl.github.io/libigl/) performs the actual principal curvature direction determination

- [Eigen](http://eigen.tuxfamily.org) is used heavily by libigl

## License
The code itself is under the MIT and MPL2 licenses.
Eigen and libigl (the dependencies) are under the MPL2 license.

In short, this means that commercial use within your company is no problem at all.

If you want to redistribute this, you have to provide some notice on where to find the original source code.


