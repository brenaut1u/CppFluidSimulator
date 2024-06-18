# CppFluidSimulator
Katia Brenaut

This fluid simulation project was made for the C++ course in the Department of Computer Science at École des Mines de Nancy.
It is based on several tutorials, but mostly Sebastian Lague's "Coding Adventure: Simulating Fluids" (https://www.youtube.com/watch?v=rSKMYc1CQHE) and Jean Tampon's "Writing a Physics Engine from scratch - collision detection optimization" (https://www.youtube.com/watch?v=9IULfQH7E90).

The physical approach is entirely based on the first tutorial. Therefore, many physical-related functions, such as the pressure force calculation, are quite similar. However, since the tutorial was made on Unity, the data structures, optimization and user interface are very different here.

The project actually contains two sub-projects : an interactive fluid simulator, where the user can set the physical parameters and interact with the particles in real-time using the mouse by adding attractive and repulsive forces; and a "fluid painter", wich can create an animation using particles to make them create an image (the idea comes from Jean Tampon's tutorial).

## Building
The project uses C++ 17 with Qt 5.14.2. It uses qmake for building, and MinGW.

The build was tested from Qt Creator 4.11.1, on Windows 11.

## Physical approach
As said previously, the physical approach is based on Sebastian Lague's tutorial.

The fluid is represented by a set of particles that bounce on the screen's edges and interact with each other.

Unlike the particles simulation project we did last year in C, the particles don't collide with each other. They can actually overlap. Instead, they interact with each others by attractive or repulsive forces. For each particle, by itterating over its neighbors, we find the particles density at its position. Then, we move the particle with a pressure force (calculated using the density) following the direction that will bring it to the zone where the density is the closest to the target fluid density. Therefore, the particles that go too close to each other will have a repulsive force, and the particles that go too far will have an attractive force. Unfortunately, this is not enough to make the fluid uncompressible, but the overall density gets stabilized quite fast, especially when the pressure force is high (by multiplying it by a pressure coefficient).

To allow the fluid surface (especially for small balls of liquid, or droplets) to remain consistent, we added a second pressure force (called "near pressure"). The particles need to be closer for this force to appear, and this force is exclusively repulsive. Therefore, when the particles are not too close, the "normal" pressure can be attractive (and set with a very high coefficient), which allows the surface to remain consistent, but when the particles get too close from each other, the near pressure pushes them appart.

The fluid also has a viscosity parameter. Viscosity works by bringing speeds to an equilibrum between neighbor particles: when a particle has a high speed but ends up among low-speed particles, it will slow down quickly.

Each particle has a radius (used only for bouncing on the screen edges, and for display) and an interaction radius. The interaction radius is the perimeter within which a particle can be influenced by another.

Finally, the collision damping parameter is used to slow down the particles when they bounce on the screen edges. When its value is 0, the particle's speed is completely dissipated, and when its value is 1, the bounce is completely elastic.

## Data structures
The project contains four main classes:
- MainWindow: this class shows the window and the widgets. It deals with the direct user inputs and transmits them to ParticleSystem.
- ParticleSystem: the UI component that shows the simulation. It owns a Grid and a list of Particle. It deals directly the mouse events, and displays the simulation on screen. It also owns the physical parameters.
- Grid: in order to optimize the collision detections, the particles are set in a grid that divides the world into cells. Each particle only checks collision (or, rather, proximity forces) with the particles in the neighboring cells. Grid manages the physical forces by calculating them when iterating over the particles. The grid's cells are set to have the same size as the particles' influence radius. Grid uses multithread to calculate forces, in order to improve the simulation's performances.
- Particle: a tiny "piece" of liquid. It has a position, a speed, and other individual properties such as an id and a color. Particles are responsible for calculating their own position after their forces have been calculated by Grid.

The two sub-projects could have shared the same files for these classes. However, since they have a few differences (for example, the Interactive simulator sub-project needs an Interaction class, and the Fluid painter's particles colors are managed differently), the files were kept duplicated.

## Known issues
When the physical parameter are very high (such as the influence radius), the simulation gets quickly chaotic, and the program can crash. This is probably because of integer (and float) overflow: the values (speed, forces...) simply get too high. But this can be avoided by using "reasonable" parameters and adjusting them slowly.

Moreover, implementing Jean Tampon's trick in order to avoid multithreading making the simulation non-deterministic did not work. The several threads split the grid in vertical regions. Tampon's solution was to first run threads on regions 0, 2, 4... and then on regions 1, 3... in order to avoid having several threads working on the same cells at the same time. As this did not work, multithread was disabled for the Fluid Painter.

## Interactive simulator
The user can set the different physical parameters using the sliders at the top of the screen. For some parameters, the scale is logarithmic in order to allow both precision with small numbers, and very high values. By clicking, the user can create forces to interact with the particles: a left click will create a repulsive force, and a right click will create an attractive force. The particles color represent their speed.

## Fluid painter
This program's purpose is to create animations using fluids. The user can draw an image thanks to the fluid simulation, using its deterministic aspect.

How to use it:
- First, set up the physical parameters using the sliders.
- Then, by clicking on the "Preview" button, launch the simulation for the first time.
- When the particles reach a satisfying state, click on the "Stop" button. This will set up the end frame for the animation.
- Next, click the "Set image..." button to choose a file.
- The image rectangle will appear behind the particles. You can move the rectangle by clicking and draging (click on the corners and edges to resize it). The particles' color in front of the rectangle will change according to the image's pixels.
- If, after all, the particles did not end up in a satisfying position, or you want to change the physical parameters, you can click again on the "Preview" button to recalculate the simulation. You will have to choose the image file again.
- Once the image is correctly positioned, click the "Play" button to launch the animation. This will also generate a video file which will be located in the same directory as the executable.

There is a demo video in the Fluid painter directory.

The video generator uses Ion Vasilief's libqtavi library, realeased under the GNU GPL v. 3.0 licence. It is available here: https://www.iondev.ro/qtavi/

This library uses another C library called libgwavi. Since this library did not compile on my computer, I had to make minor changes to make it C++ compatible. These libraries are included in this git repository, so you (normally) do not need to download anything.
