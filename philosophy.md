# Design philosophy & implementation

**OSKengine** is being developed following the core ideas of **customization** and **extensibility**. 

Its design allows the game developers to easily modify the engine by adding new gameplay systems, render systems, asset types and loaders whithout actually changing the engine's code.

In fact, its modular design allows the developers to disable whole systems they may not use, and they may even bypass core elements of the engine such as the ECS functionality.

## ECS architecture

The ECS system is the core of the engine. With this architecture, game functionality is divided into:
- **Entities** (known as *game objects* in **OSKengine**) represent gameplay actors, such as players, NPCs, enemies or innanimated objects. Entities themselves have no data or functionality and consist of an unique identifier.
Each entity can have components attached to it.
- **Components** represent a structure with data. The data contained within a component contains information that is used toghether. For example, the `Transform3D` component contains an object's position, rotation and scale; while the component `Model3D` contains relevant data used to render a 3D object.
Components themselves have no functionality and act as mere data holders.
- **Systems** implement functionality. There are different systems, each one implementing a certain functionality. For example, a *render system* will hanlde 3D rendering, while a *collision system* will handle collisions between entities.
In order to execute the funtionality, each system will iterate through all relevant entities (which contain all components needed to implement the functionality), grab all the necessary components components and execute the functionality.

The ECS system provides a great amount of customization and extensibility, as developers can develop their own compoonents and systems and add them to the engine without actually chainging the engine source code, and the engine will correctly manage them. 

**OSKengine** has a series of built-in components and systems, including:
- Rendering components and systems.
- 3D animation system.
- Hierarchical transform components (representing location, rotation and scale).
- Input system.
- Collision & physics systems(*work in progress*).

Developers can disable any of these systems if they want.

## Rendering

The renderer is designed to allow a lower-level access to the GPU while still offering an abstraction layer over graphics APIs like *DirectX* or *Vulkan*.
To accomplish this, a fine abstraction layer is developed over native API elements, such as:
- Images.
- Buffers (including vertex buffers, index buffers, uniform buffers, etc...).
- Command lists.

In the case of pipelines, a higher level abstraction layer is offered: the **material system**.

### Material System

The material system encapsules both the pipeline objects used for rendering and compute, as well as all the data necesary to perform those actions.
The following classes were developed:

##### Material
Represents a *material schema*, which defines:
- The specific shaders used, which can be:
    - *Vertex*, *fragment* and/or *tesselation shaders*, in the case of a **graphics material**.
    - *Compute shaders*, in the case of a **compute material**.
    - *Ray generation*, *closest-hit* and *miss shaders* in case of a **ray-tracing material**.
- A schema for input/output resources (such as buffers or images) in the form of a *material slot layout*.

##### Material Slot
A material slot (similar to a *descriptor set* in *Vulkan*) is used to group input resources instances that are used toghether.
It works as a dictionary, mapping the name of a binding to its resource.

##### Material Binding
A specific input resource, which can be:
- Uniform (read-only) buffers.
- Storage (read-write) buffers.
- Samplers: read-only images.
- Storage images: read-write images.
Its name will be the one this specific binding is given within the shader.

##### Material Instance
Created from a specific *material*. Contains:
- The native pipeline that groups shaders being used.
- A map of initialized material slots.


A material is defined in a `JSON` file. This file includes:
- Path(s) to precompiled shaders used by this material.
- Names set to specific slots for easier use (otherwise their names will be their descriptor set index).

When loading a material, it will automatically create its layout according to the shaders used, so there is no need to manually declare all slots and bindings in C++ code.

### High-level abstraction

**OSKengine**'s render systems are implemented within the ECS system, using the previously metioned abstraction layer.
