# Solar system simulation
Visual simulation of a solar system.

## Introduction
This project has been created as an assignment in graphics classes on university with aim to summarize the knowledge gained during the course.
The projects goal was to create visual simulation of a solar system, meaning there are no physics involved. To keep things simpler and easier 
to visualize the system is not to scale.

![Systemview test](./Screenshots/sunview3.png)

## Features
### Simulation
* The solar system consists of sun, which provides light, and planets. The sun is stationary while planets follow their orbits.
* Each orbit is an ellipse with arbitrary period, dimensions and rotation with one of its foci in the sun's position.
* Orbits are visualised.
* Objects are textured.
* Planets spin around their axes with given period.
* Background with stars.

### Interaction
* User can use mouse to zoom in/out and to move around the chosen planet(or sun).
* Camera movement is smoothed out to provide better user expirience.
* Using keys from '1' to '8' user changes currently observed planet and using key '0' allows to observe a sun. 
* 'Space' key stops/resumes simulation.
* 'm' key stops/resumes music.

## Technologies
* C++14
* OpenGL, GLUT 3.7
* GLM 0.9.5.3
* Libpng 1.6.37
* Zlib 1.2.11

## Launch
To compile the project you will need to download and link against libraries GLUT 3.7, Libpng 1.6.37 and Zlib 1.2.11

## Sources
* [Textures](https://www.solarsystemscope.com/textures/)
* [Music -- Christian Bodhi, Voyager Probe - Deep space](https://pixabay.com/music/ambient-voyager-probe-deep-space-559/)