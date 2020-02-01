# OpenGL Labs

This is yet another pet project of mine to play around and do something cool with OpenGL and C.  
Check out the Up and Running section to get it started

## Tech Stack
- C
- Glew, Glfw, Glm

## Up and Running 

### MacOS
```
$ brew install glfw glew glm 
```

Also you will have to change the `working directory` in `Xcode`, like this:

- First go to `Product` > `Scheme` > `Edit Scheme...`

<img width="940" alt="how-to-find-product-edit-scheme" src="https://user-images.githubusercontent.com/15306309/72669950-9e6f3080-3a16-11ea-8dfd-22f98b6a8722.png">

- Then go to `Options` and change the `Working Directory` to a folder like this:

```
/where/youve/cloned/opengl-labs-cpp/opengl-labs-cpp
```

Which is where the project code is (`.h`, `.hpp` and `cpp` files).

<img width="897" alt="how-to-edit-working-directory" src="https://user-images.githubusercontent.com/15306309/72669951-9e6f3080-3a16-11ea-83dd-da116b5cdb06.png">

### Linux

#### Installing Dependencies
```
$ # Install OpenGL Development Tools
$ sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev libglew-dev

$ # Install GLFW Development Tools
$ sudo apt-get install libglfw3-dev

$ # Install GLM Development Tools
$ sudo apt-get install libglm-dev
```

#### Running

You will need `g++` installed in order to compile this project. You can install it just like the other dependencies, but you probably already have it on your computer.
```
$ # Compile the project
$ make build

$ # Run it!
$ make run
```

### Windows
```
T.B.D.
```
