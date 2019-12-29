# Introduction

Smile Engine is a game engine created for the the subject Video Game Engines made by Marc Doctor and Eric Navarro. In our engine, we have implemented a Particle System that we will explain later.

# Team

### Marc Doctor: [Github Account](https://github.com/thedoctormarc)

![Marc](https://github.com/thedoctormarc/SMILEENGINE/blob/master/WebDocs/marc.png?raw=true)

## Individual work:

- Main Core of Particle System.

- Load and Save for the emitter and particles.


### Eric Navarro: [Github Account](https://github.com/lakaens)

![Eric](https://github.com/thedoctormarc/SMILEENGINE/blob/master/WebDocs/Eric.PNG?raw=true)

## Individual work:

- Inspector for the Emitter of particles.

- Load and Save for the emitter and particles.

# Short explanation of the main core sub-systems

In the last two deliveries we have performed the following tasks:
serialization of scenes, for saving / loading them, import for meshes and materials, implementation for game objects, components(camera, material, mesh, transform), resource manager of the previous components, ui of inspectors and configurations and quadtree.

# Detailed explanation of last assignment sub-system

For our Particle System, first of all, we created an emitter, that contains the principal characteritics for the particles distinguished in two categories, Initial State and Emission Data.

For The Initial State we have initial life and decreasing life, speed, initial and final size, transparency, color and texture.
For The Emission Data we have gravity, number of maximum particles, random speed, random color, time of spawn, burst time, current time, current burst time, expire time, total time, shape (CONE, CIRCLE, SPHERE) and spawn radius for those shapes.
We have implemented blend mode that can be additive or alpha blend.

You can create a new particle in the menu creation, emitter. It will appear a emitter in the scene and the inspector will be active for the emitter. In the inspector you can personalize the emitter however you want. 
There are several sections.

General data Section: the maximum number of particles, the size of the bounding and the blend mode can be changed.

Particle Speed Section: you can decide if the speed is random and if being random you want a range or two to do the random, if it is not random, the user decides it and you can decide if you want gravity or not.

Particle Life Section: you can change the initial life and the decrease of this.

Particle Color Section: you can decide if you want a random color or not, if not you have a color palette to choose two colors, the initial and the final

Particle Size Section: you can decide which initial and final size you want for the particles.

Particle Spawn Section: you can change the shape for the emission of the particles, the spawn time, if you want burst and the spawn radius for each shape.

Particle Texture Section: you can decide if the particles have texture, if they have, you can decide the transparency of them and you can change the texture if you want.

Expiration Section: you can decide the expiration time for the emitter.
# Video

One minute video speed-up that shows the process of authoring scenes in your editor, especially
with your core technology from the last assignment (captured from the computer, not mobile).




* [Link to our repository](https://github.com/thedoctormarc/SMILEENGINE)
* [Link to our last release]()
