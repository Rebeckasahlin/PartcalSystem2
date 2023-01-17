## TNM094 Labs - Particle System

A minimal particle system acting as a foundation for the course labs.
A small graphical interface is provided for visually editing parameters.

#### Folder structure

- /include: Header files
- /src: Cpp files
- /unittest: Examples of tests
ChangeB

changeA

newcode here for task 5
#### Setup instructions
Dependencies:
 - [CMake](https://cmake.org/download/) For cross-platform compiler project generation.
 - [Vcpkg](https://github.com/microsoft/vcpkg) For dependency management
 - C++20 Required, e.g. [Visual Studio](https://visualstudio.microsoft.com/downlnow 
 New changes again, AGAIN Again

1)  Make a project folder for example 'tnm094'

2)  In the project folder:
    * git clone https://github.com/microsoft/vcpkg 
    * git clone https://gitlab.liu.se/tnm094-labs/particlesystem.git

2)  Open CMake (we recommend using the GUI here), enter the source path to the particlesystem git repo
    and select one of the "presets" ("MSVC 2022", "Xcode", "Ninja", "Unix Makefiles")
    and hit configure. This will build and the dependencies using vcpkg and make them available to 
    the project. Then the will be configured 

3)  Hit Generate and then Open Project to open the project in your IDE.

4)  Build and run the 'application' executable.
