Project
=======

- Use other volume data models
- Effecient Techniques
- Make a GIF file? -- varying depth etc
- Why is RAM usage so high? -- memory leak?
- RAM requirements -- how many points are generated and what is going on? how does the
  memory requirements scale drastically!

Marching Cubes
==============

Marching cubes is an algorithm/technique to generate a *polygonal surface representation*
for an *isosurface* of 3D scalar field i.e extract isosurface from the scalar field and
render them using polygons, i.e it is a surface reconstruction algorithm. The surface can
be defined implicitly in a mathematical expression $f(x,y,z) = 0$ for which we may not 
have an explicit surface representation but we can still know whether a point lies
inside (`f < 0`), on (`f = 0`) or outside (`f > 0`), eg: a sphere centered at origin 
with radius `r` can be represented as $f(x,y,z): x^2 + y^2 + z^2 - r^2 = 0$.
Alternatively, the surface
can come from a volumetric datasets (MRI scans) -- they contain information at regularly sampled
locations in 3D space. The sampling is doing on voxels which are the 3D equivalent of pixels
and hence each dataset also has a sampling *resolution* attached to it. If the 3D region
is divided into a large number of voxels (higher sampling rate) then the resolution is also
higher.

A 3D scalar field is a just a function which assigns a real number (i.e scalar, and NOT vector)
to every point in 3D space and that number usually has a *physical* meaning attached to it
like density, charge etc (hence its called a *field*). So we can think about it like
$f: R^3 \to R$. Now that we have noted down what a scalar field is, we move onto the
definition of an isosurface: for a given (constant) scalar value (i.e real number) `c`, the isosurface
associated with the function `f` and constant `c` is simply the set of points in 3D whose
function value is equal to `c`. So the isosurface can be represented as a set: 
$S_c = \{ (x,y,z) \in R^3 : f(x,y,z) = c \}$ 

Due to computational constraints we need to sample the function at discrete points in
space in a grid like fashion. Let's see how the 2D counterpart for surface extraction aptly
named *marching squares*.















