# OSKengine 
![alt text](./Marca/card.png?raw=true)

**NOTE: this project is still in early development, and may be unstable.**

**OSKengine** is a general purpose 3D game engine.

# Main features & design philosophy

The main features can be checked in [my website](https://oskiyu.github.io/). Some key features include:
- ECS architecture.
- Low-level, API agnostic rendering access.
- *Forward* & *deferred* PBR render systems, with:
    - Cascaded shadow mapping.
    - Compute based post-processing.
    - HDR bloom.
    - 3D animation support.
    - FXAA.
    - Tesselation & ray-tracing support.
- Extensive and detailed API documentation (in Spanish).

**OSKengine** has been designed following a modular design philosophy that allows for great customisation from the developer's side. You can read more about the design decissions taken and implementation details [here](./philosophy.md).

# Build Instructions

This project is being developed in *Visual Studio 2022* and all dependencies are included in the repository, so it can be built and run without any additional installations:

1. Clone this repository.
2. Build using the `Release x64` build configuration.
3. Run.

The only prerequesite is having a GPU with `Vulkan 1.3` support, as well as support for the `VK_KHR_dynamic_rendering` extension.

# Project Structure

 Folders:
 - *Dependencies*: contains all dependencies of this project for an easy build process.
 - *Frameworks*: includes:
    - All `*.h` and `*.hpp` files, intender for the use of OSKengine as a DLL.
    - Documentation, including:
        - Detailed API documentation, generated via Doxygen.
        - Diagrams.
- *OSKengine*: old source code.
- *OSKengine2*: full source code of the engine.
- *UnitTests*: project with unit tests.

