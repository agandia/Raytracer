# Raytracer
![Complex scene rendered with this code](https://github.com/agandia/Raytracer/blob/master/full_5000spp_50d.jpg)
![BSSRDF using random walk](https://github.com/agandia/Raytracer/blob/master/SSS.jpg)

Just a tag-along of this resource https://raytracing.github.io/
It did end up spawning a side microproject to revive (more accurately rework) my old BSSRDF implementation. 
This time I found a simple enough to follow Random-Walk algorithm instead of going with Dipole Diffusion.
Future updates(?): integrate the Random-Walk material with all other geometries of the main pathtracer project.

##Build and Run this program

If you add any cpp file to the solution, is recommended to do
```cmake -B Build```

otherwise when changing code
```cmake --build Build```

To generate an image, depending on the config [Debug/Release], the resulting image should appear at the root with the name ```image.ppm```
```build\Raytracer\[Config]\Raytracer.exe > image.ppm```

# Proper acknowledgment
## Based on
Title (series): “Ray Tracing in One Weekend Series”
Title (book): “Ray Tracing in One Weekend”
Author: Peter Shirley, Trevor David Black, Steve Hollasch
Version/Edition: v4.0.2
Date: 2025-04-25
URL (series): https://raytracing.github.io
URL (book): https://raytracing.github.io/books/raytracinginoneweekend.html
