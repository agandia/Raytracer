# Raytracer
![Complex scene rendered with this code](https://github.com/agandia/Raytracer/blob/master/full_5000spp_50d.jpg)

![BSSRDF using random walk](https://github.com/agandia/Raytracer/blob/master/SSS.jpg)

The foundation for the pathtracer is just a tag-along of this resource https://raytracing.github.io/

It did end up spawning a side microproject to revive (more accurately rework) my old BSSRDF implementation. 
Previously I had implemented Subsurface Scattering with the Dipole Diffusion Method;
This work implements subsurface scattering based on the random-walk approach described in “Separable Subsurface Scattering” by Kulla, Conty Estevez,
and Drobot (Sony Pictures Imageworks, SIGGRAPH 2017), combining Fresnel transmission, volumetric scattering via the Radiative Transfer Equation,
Henyey–Greenstein sampling, and exponential free path sampling.



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
