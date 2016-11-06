# 2D-water-gpu
Interactive 2D water simulation running on GPU, using C++, SFML, OpenGL shaders.


# Usage
You can interact with the water with left mouse button.
Press R to reset the simulation.

As the simulation runs at 60 fps, sometimes the time frame between two simulation steps isn't small enough, resulting in instabilities.

![alt text](screenshots/screen0.png "Screenshot of a simulation")
![alt text](screenshots/screen1.png "Screenshot of a simulation")
![alt text](screenshots/screen2.png "Screenshot of a simulation")


# Algorithm
The water surface is stored as a heightmap in a sf::Texture (FBO).

blah blah

