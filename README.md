[poly]: https://image.ibb.co/gLKDgS/23_39_46_10_2_119.png "Polygon preview"

![alt text][poly]

# Polycursion
### General
Polycursion can produce really beautiful patterns!

They are generated iteratively : first a base n-gon is generated, second algo picks
a point on each side of a polygon and connects them together to create a new one,
and then repeats the same process for a new polygon.

It is important to remember that this procedure is not set in stone, e.g. I encourage you to modify and play with the code, hf.

### Controls
* To zoom in/out use Z and X
* To move around use arrow keys
* To roll a new pattern use C
* To center the image and reset zoom use V
* To change number of sides of the base polygon use N
* To change the amount of iterations use M
* To save a beautiful masterpiece to a file use S

### Dependencies
This project uses GLAD loader and GLFW 3.3 to make OpenGL work and LodePNG to make screenshots. 
