[poly]: https://ibb.co/cUqKMS "Polygon preview"

![alt text][poly]

# Polycursion
### General
Polycursion is a neat thingy, which can produce really beautiful patterns.

They are generated iteratively : first a base n-gon is generated, second algo picks
a point on each side of a polygon and connects them together to create a new one,
and then repeats the same process for a new polygon.

### Controls
* To zoom in/out use Z and X
* To move around use arrow keys
* To roll a new pattern use C
* To center the image and reset zoom use V
* To change number of sides of the base polygon use N
* To change the amount of iterations use M
* To save a beautiful masterpiece to a file use S

### Dependencies
This project uses GLEW and GLFW to make OpenGl work and tgaimage (scraped from /ssloy/tinyrenderer) to make screenshots. 
