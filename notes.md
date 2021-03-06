Project
=======

**Implmentation of Marching Cubes Algorithm**

References:

1. https://dgr582.com/articles/2020/marching-cubes
2. http://paulbourke.net/geometry/polygonise/
3. http://www.it.hiof.no/~borres/j3d/explain/marching/p-march.html
4. Dual Contouring: https://www.boristhebrave.com/2018/04/15/dual-contouring-tutorial/
5. Even more advances: https://swiftcoder.wordpress.com/planets/isosurface-extraction/


TODO
----

- Smooth vs Flat Shading -- similar to the task from Homework 02! But need to store 
  vertex normals interpolated from the faces it lies on.
- Update GUI display -- only essential parts -- DONE.
- Use other volume data models -- take from internet -- DONE
- Make a GIF file? -- varying view depth and number of slices etc
- Why is RAM usage so high? -- memory leak? RAM requirements -- how many points are 
  generated and what is going on? how does the memory requirements scale drastically!
- Effecient Techniques as a follow up?

- Better normal computation -- take average from all faces! Doing this efficiently?
- Dual Contouring implementation
- Read the other advances papers and try to write down your own summary. There is so much
  to learn in this topic and its seems approachable and exciting at the same time! Something
  to ponder about when trying to relax from work/research!
- Add even more references from the last two lectures of the course! Make detailed notes
  about it!


Parameters
----------

- Bucky: 0.6, 30
- Teapot: 0.1, 120
- Tooth: 0.4, 90
- Engine: 0.5, 125
- Lobster: 0.2, 35
- Bonsai: 0.11, 150
- Head: 0.5, 150


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

Image based demo and later explanation of marching squares.







