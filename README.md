# houdini-transform-matcher
A Houdini DSO that extracts the transform between two sets of points using the Procrustes transform.

## Usage
Put the source object into the first input, the transformed object into the second input of a `Match Transform` node.

The node will write detail attributes `scale`, `transform`, `rotation`. To get the transformed object, do `P = (s*P)*R + t`.

## License
The code itself is under the MIT license; Eigen and libigl (the dependencies) are under the MPL2 license.


