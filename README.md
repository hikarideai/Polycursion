[poly]: https://image.ibb.co/gLKDgS/23_39_46_10_2_119.png "Polygon preview"

![alt text][poly]

# Polycursion
### General
Polycursion can produce really beautiful patterns!

They are generated iteratively : 
 1. An initial n-gon, whose vertices are located randomly, is generated.
 2. On each edge of this n-gon, a point is selected (specifically, a lerp of the two end points, t = 0.2)
 3. The new points are connected together and the step 2 is repeated for this new polygon.

(All in all, please, have fun hacking it).

### Controls
* To zoom in/out use Z and X
* To move around use arrow keys
* To roll a new pattern use C
* To center the image and reset zoom use V
* To change number of sides of the base polygon use N
* To change the amount of iterations use M
* To save a beautiful masterpiece to a file use S

### Dependencies
This project uses GLAD loader and GLFW to make OpenGL work and LodePNG to make screenshots. 

### How to build
The project has submodules, so in order to clone the repository correctly, one may clone with `--recursive` flag, like so
```
$ git clone --recursive <url-of-this-repo>
```

If you've already cloned the repository, you may download the submodules as follows
```
$ cd <path-to-repo>
$ git submodule update --init
```

To build the project run
```
$ cd <path-to-repo>
$ mkdir build
$ cd build
$ cmake ..
$ make
```

The build directory will contain the executable and the required shaders. To run the application simply type
```
$ ./polycur
```
