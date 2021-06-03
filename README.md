# vulkan

Physically-based Rendering with Image-based Lighting in Vulkan.

<img width="100%" alt="PBR with IBL" src="https://i.imgur.com/QFfwLmt.png">

## Physically-based Rendering
<img width="100%" alt="PBR" src="https://i.imgur.com/KyMc9Mk.png">

The direct lighting term is calculated with Cook-Torrance's Bidirectional Reflective Distribution Function (BRDF). This implementation uses Trowbridge-Reitz GGX Normal Distribution Function, Schlick-Smith GGX Geometry Function, and Shlick Fresnel Function.

<p align="center"><img src="https://i.imgur.com/QAkWHmI.png"></p>
<p align="center"><img src="https://i.imgur.com/3n3MgXl.png"></p>
<p align="center"><img src="https://i.imgur.com/fQhvIhW.png"></p>
<p align="center"><img src="https://i.imgur.com/Qr9MKjd.png"></p>
<p align="center"><img src="https://i.imgur.com/2pZu8q2.png"></p>
<p align="center"><img src="https://i.imgur.com/65DeCzp.png"></p>
<p align="center"><img src="https://i.imgur.com/kCkuKW6.png"></p>

## Image-based Lighting
<img width="100%" alt="IBL" src="https://i.imgur.com/HjsWypX.png">

The indirect lighting term is approximated with Image-based Lighting.  
The diffuse term is pre-computed by calculating an irradiance cube map which stores the light radiated from the surrounding environment.

<p><img width="25%" alt="skybox" src="https://i.imgur.com/BadkwR1.png"><img width="25%" alt="irradiance" src="https://i.imgur.com/X8luQu8.png"></p>

The specular term also requires the pre-computation of two maps: a BRDF Lookup table and a prefiltered cubemap. The BRDFLUT contains environment BRDF values for different roughness and view angle inputs. The prefiltered cubemap stores the specular contribution from the surrounding environment, with the multiple mipmaps storing this value for increasing values of roughness. 

<img width="25%" alt="brdflut" src="https://i.imgur.com/3uJmg3P.png">

<div>
  <img width="75px" alt="prefilter" src="https://i.imgur.com/dFeLGUm.png">
  <img width="75px" alt="prefilter" src="https://i.imgur.com/MZhtpnX.png">
  <img width="75px" alt="prefilter" src="https://i.imgur.com/PN4adzr.png">
  <img width="75px" alt="prefilter" src="https://i.imgur.com/L70WwZJ.png">
  <img width="75px" alt="prefilter" src="https://i.imgur.com/AakFCkG.png">
  <img width="75px" alt="prefilter" src="https://i.imgur.com/d7Pj4r1.png">
  <img width="75px" alt="prefilter" src="https://i.imgur.com/hThPQb5.png">
  <img width="75px" alt="prefilter" src="https://i.imgur.com/l2DcVqF.png">
  <img width="75px" alt="prefilter" src="https://i.imgur.com/ynb1R7l.png">
  <img width="75px" alt="prefilter" src="https://i.imgur.com/Je091x8.png">
<div/>
  
### Getting started
Clone the repository
```sh
git clone --recursive https://github.com/rendoir/vulkan.git
```
Configure the project
```sh
cmake -S . -B _build_
```
Build the project
```sh
cmake --build _build_
```

### Acknowledgements
[Sascha Willems' Vulkan glTF PBR IBL](https://github.com/SaschaWillems/Vulkan-glTF-PBR): For the overall vulkan codebase, model loading and PBR with IBL pipelines.  
[Joey de Vries' OpenGL Tutorial](https://learnopengl.com/): For the computer graphics theory and shaders.  
[Alexander Overvoorde's Vulkan Tutorial](https://vulkan-tutorial.com/): For the vulkan API tutorial.  
