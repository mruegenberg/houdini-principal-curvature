# houdini-principal-curvature
A Houdini DSO that extracts the minimal and maximal principal curvature directions from geometry.

## Usage
The node takes just one input. It writes `min_principalcurvature` and `max_principalcurvature` point attributes.

The input mesh must be triangular (e.g using the `Divide` SOP).

## License
The code itself is under the MIT license; Eigen and libigl (the dependencies) are under the MPL2 license.


