
# Historial de versiones.


## 2022.12.21a

### ***Nuevo***: Collision

- ***Nuevo***: `Collider`
    - Representa un volumen que puede interseccionar con otros.
	- Compuesto en dos capas:
        - Capa de alto nivel representada por un volumen que cubre toda la geometría.
        - Capa de bajo nivel representada por uno o varios volúmenes que cubren partes de la geomtería.
        
- ***Nuevo***: `ITopLevelCollider`
    - Representa un volumen de colisión de alto nivel.
    - Permite saber si está colisionando con otro elemento de alto nivel.
    - Permite saber si hay un punto dentro del colisionador.
    - Permite lanzar un rayo y comprobar su intersección.
    
- ***Nuevo***: `AxisAlignedBoundingBox`
    - Volumen de colisión de nivel alto.
    - Representa una caja alineada con los ejes (no se puede rotar).

- ***Nuevo***: `SphereCollider`
    - Volumen de colisión de nivel alto.
    - Representa una esfera.

- ***Nuevo***: `IBottomLevelCollider`
    - Representa un volumen de colisión de bajo nivel.
    - Permite establecer el transform del colisionador.
    - Permite saber si está colisionando con otro elemento de bajo nivel.
    - Permite saber si hay un punto dentro del colisionador.

- ***Nuevo***: `CollisionInfo`
    - Incluye la información de una colisión potencial entre dos objetos:
        - Si hay colisión o no.
        - Si los niveles de alto nivel están colisionando.

- ***Nuevo***: `ConvexVolume`
    - Elemento de colisión de bajo nivel que representa un volumen convexo arbitrario.
    - Almacena una serie de vértices agrupados en caras.
    - Comprueba la colisión usando un algoritmo SAT.

- ***Nuevo***: `FaceProjection`
    - Proyección de una cara de un volumen convexo sobre una línea.
    - Para detección mediante SAT.

- ***Nuevo***: `IRayCollider` (*WIP*)
    - Representa un volumen que puede ser interseccionado por un rayo.

- ***Nuevo***: `Ray`
    - Representa un rayo lanzado en el mundo que puede colisionar.
    - Contiene:
        - Origen.
        - Dirección.
        - Longitud máxima.

- ***Nuevo***: `RayCastResult`
    - Incluye la información de una intersección potencial entre un rayo y un objeto:
        - Si ha interseccionado.
        - Punto de intersección.


### Graphics

- `MaterialLayout`
    - Ahora permite obtener todos los slots.
    - Ahora permite obtener todos los push constants.

- ***Nuevo***: `CopyImageInfo`
    - Incluye todos los parámetros para la copia de imágenes en la GPU:
        - Tamaño de copia.
        - Offset de la imagen fuente.
        - Offset de la imagen de destino.
        - Nivel mip de la fuente.
        - Nivel mip del destino.
        - Capa de la imagen fuente.
        - Capa de la imagen de destino.
        - Número de capas a copiar.

- `ICommandList`
    - Ahora usa `CopyImageInfo` para las operaciones de copia.

- ***Nuevo***: `GpuImageCreateInfo`
    - Incluye todos los parámetros para la creación de imágenes en la GPU:
        - Resolución.
        - Formato.
        - Uso.
        - Dimensionalidad.
        - Número de capas.
        - Número de muestras MSAA.
        - Descriptor de sampler.
        - Tipo de memoria GPU en la que se alojará.

- `IGpuMemoryAllocator`
    - Ahora usa `GpuImageCreateInfo` para la creación de imágenes.

### ECS

- `RenderSystem3D`
    - Ahora usa un skybox.

- ***Nuevo***: ``IsEcsComponent``
    - Permite saber si una clase cumple los requisitos para ser un componente ECS.

- ``Transform3D``
    - Ahora permite transformar un punto.

###### Implementado completamente sistema de sombras suaves.

- `HybridRenderSystem`
    - Mejorado proceso de eliminación de ruido.
    - Añadido proceso de reproyección temporal.

### Assets

- `StaticMeshLoader`, `AnimMeshLoader`
    - Ahora carga los colores de los vértices.
    
- ***Nuevo***: `SpecularMapLoader`

### STD

- `HashMap`
    - Ahora permite saber si contiene una clave en concreto.
    
- ***Nuevo***: `Span`
    - Representa una región lineal de memoria manejada por otro contenedor.

### Types

- `Color`
    - Ahora se pueden sumar (se suman todos los componentes).

### Bugfix
- Ahora `IGpuImageView::operator==(...)` es const.


## 2022.12.31a

### Assets

- ***Nuevo***: `SpecularMap`
    - Representa un mapa IBL especular.
    - También incluyse su LUT.
    
- `SpecularMapLoader`
    - Implementado completamente.

### Graphics

- `Format`
    - *Renombrado*: `RG16` -> `RG16_SFLOAT`
    - *Renombrado*: `RG32` -> `RG32_SFLOAT`

- `RendererVulkan`
    - Añadido soporte para el comando `vkCmdBeginDebugUtilsLabelEXT`.

### ECS

- `RenderSystem3D`
    - Ya no usa un skybox.
    - Ahora usa un mapa IBL especular.


## 2023.01.09a

### ECS

- ***Nuevo***: `ColliderRenderSystem`
    - Renderiza, en wireframe, los colisionadores de alto nivel.

- `RenderSystem3D`
    - Ahora renderiza sobre un render target con formato `Format::RGBA16_SFLOAT`.

### Graphics

- `ICommandList`
    - Ahora permite añadir marcas de depuración en la lista de comandos:
        - ***Nuevo***: `AddDebugMarker(...)`
        - ***Nuevo***: `StartDebugSection(...)`
        - ***Nuevo***: `EndDebugSection()`

- ***Nuevo***: `VertexCollisionDebug3D`
    - Contiene únicamente una posición 3D.
    
- `IPostProcessPass`, `BloomPass`
    - Ahora usa imágenes con formato `Format::RGBA16_SFLOAT`.

### Collision

- `Collider`
    - Ahora es un componente ECS.

### Bugfixes

- **Bugfix**: mejorado const-correctness de `AxisAlignedBoundingBox`.


## 2023.01.15a

### Collision

- `AxisAlignedBoundingBox`, `SphereCollider`
    - Ahora permiten saber si un punto se encuentra dentro del collider.
    
### Persistence

- ***IFieldWrapper***
    - Interfaz para un wrapper de un elemento.
    - Permite obtener el tipo de elemento (`FieldType`) y el tipo de contenedor (`ContainerType`).
    
- ***Nuevo***: `SingleFieldWrapper<>`
    - Wrapper para un único elemento (`ContainerType::SINGLE`).
- ***Nuevo***: `ListFieldWrapper<>`
    - Wrapper para una lista de elementos (`ContainerType::LIST`).

- ***Nuevo***: `Data`
    - Contiene `IFieldWrapper`

- ***Nuevo***: `FieldType`
    - Indica el tipo de dato almacenado en un nodo.
    - Incluye:
        - `STRING`
        - `INT`
        - `FLOAT`
        - `ECS_ID`

- ***Nuevo***: `ContainerType`
    - Indica el tipo de contenedor almacenado en un nodo.
    - Incluye:
        - `SINGLE` (un único elemento).
        - `LIST` (cero, uno o varios elementos).
        
- ***Nuevo***: `ISerializable`
    - Interfaz para elementos que pueden serializarse a si mismos.
    - Permite generar un nodo `Data`.
    
- ***Nuevo***: `ISerializer`
    - Guarda un árbol de datos (almacenado en un nodo `Data`) en un archivo.

- ***Nuevo***: `IDeserializer`
    - Recrea un árbol de datos almacenado en un archivo.

### Bugfixes

- **Bugfix**: `SphereCollider::SetRadius(...)` ahora asigna correctamente el radio (en vez de establecerlo en 1.0).
- **Bugfix**: `SphereCollider::IsColliding(...)` ahora funciona correctamente cuando el otro colisionador es una caja (`AxisAlignedBoundingBox`).


## 2023.01.25a

### Graphics

- ***Nuevo***: `CommandQueueSupport`
    - Indica los tipos de comandos que puede soportar una cola de comandos.

- ***Eliminado***: `QueueFamilyIndices` (dividido en `QueueFamily` y `QueueFamiles`)
- ***Nuevo***: `QueueFamily`
    - Representa una familia de colas.
    - Incluye:
        - Comandos soportados (`CommandQueueSupport`).
        - Índice de la familia.
        - Número de colas.
- ***Nuevo***: `QueueFamiles`
    - Almacena todas las familias de una GPU.
    - Permite obtener las familias que soportan un conjunto de tipos de comandos en concreto.

- `CommandQueueVulkan`
    - Ahora permite obtener el índice de la familia a la que pertenece.

- `GpuVk`
    - Ahora da prioridad a colas con soporte completo, pero permite tener varias colas distintas para las tareas distintas.

### Persistence

- ***Eliminado***: `Data`
- ***Nuevo***: `DataNode`
    - Hereda de `IDataElements`.
    - Nodo de un árbol de persistencia.
    - Permite añadir y eliminar `IDataElements`s.

- ***Nuevo***: `IDataElement`
    - Almacena un dato de cualquier tipo.

- ***Eliminado***: `FieldType`
- ***Eliminado***: `ContainerType`
- ***Nuevo***: `DataType`
    - Incluye:
        - `UNKNOWN`
        - `DATA_NODE`
        - `STRING`
        - `INT`
        - `FLOAT`

- `IFieldWrapper`
    - Ahora usa `DataType` en vez de `FieldType`.
    
- ***Eliminado***: `SingleFieldWrapper`
- ***Eliminado***: `ListFieldWrapper`
- ***Nuevo***: `FieldWrapper`
    - Usado para almacenar una lista de atributos de un mismo tipo.

- ***Eliminado***: `IDeserializer`
- ***Nuevo***: `ISerializable`
    - Interfaz que permite crear y procesar los datos de un árbol de persistencia.
    - Se (de)serializa a así misma.
- ***Nuevo***: `ISerializer`
    - Interfaz que permite crear y procesar los datos de un árbol de persistencia.
    - (De)serializa otras clases.

### Bugfixes

- **Bugfix**: `Assert` ya no tiene undefined behaviour al mostrar un diálogog de error.
- **Bugfix**: `BloomPass` ahora crea las imágenes con los usos correctos.
- **Bugfix**: `BloomPass` ahora establece correctamente los GPU barriers.


## 2023.01.27a

### Bugfixes

- **Bugfix**: ahora `DynamicArray` mueve correctamente todos los datos al eliminar un elemento que no está al final.


## 2023.01.28a

### Graphics

- `MaterialSlotVulkan`
    - Ahora puede establecerse un nombre de debug.
    
Añadidos nombres de debug a las imágenes GPU de:
- `FontInstance`
- `Model3D`
- `IrradianceMap`
- Renderpass principal.
- `ShadowMap`
- `Texture`

### Bugfixes

- **Bugfix**: `AssetManager` ahora elimina correctamente los loaders.
- **Bugfix**: `HashMap` ahora elimina correctamente los elementos almacenados.
- **Bugfix**: `IRenderer`, `RendererVulkan` ahora esperan correctamente a que se completen los trabajos de la GPU antes de destruirse.
- **Bugfix**: `Material::GetGraphicsPipeline()` ahora es const-correct.
- **Bugfix**: `MaterialSystem::LoadMaterialV1()` ahora no tiene memory leaks.


## 2023.02.02a

### ECS

###### Añadido soporte para eventos.

- ***Nuevo***: `EventContainer<>`
    - Contiene una lista de eventos particular.
    - Permite:
        - Registrar eventos.
        - Publicar eventos.
        - Obtener listas de eventos.

- ***Nuevo***: `IEventContainer`
    - Interfaz para `EventContainer<>`

- ***Nuevo***: `EventManager`
    - Contiene un `EventContainer` por cada tipo de evento registrado.
    - Permite:
        - Registrar eventos.
        - Publicar eventos.
        - Obtener listas de eventos.

- `EntityComponentSystem`
    - Ahora limpia las listas de eventos al final de cada frame.
    - Ahora incluye un `EventManager`.
    - Ahora permite:
        - Registrar eventos.
        - Publicar eventos.
        - Obtener listas de eventos.

### Graphics

- `GpuBufferUsage`
    - ***Nuevo***: `UPLOAD_ONLY`
        - Representa un staging buffer.

- `IGpuMemoryAllocator`
    - Ahora permite liberar toda la memoria usada para staging buffers (y que ahor está en desuso).

- `IGpuMemoryBlock`
    - Ahora permite saber si hay algún subbloque activo que referencie a su memoria.

- `RendererVulkan`
    - Ahora libera la memoria staging en desuso al finalizar cada frame.

- `GpuMemoryAllocatorVulkan`
    - `CreateStagingBuffer()` ahora establece el uso `GpuBufferUsage::UPLOAD_ONLY`.

### Bugfixes

- **Bugfix**: corregido un `GpuImageUsage` mal configurado en el pase `BloomPass`.
- **Bugfix**: las clases hijas de `IGpuStorageBuffer` ahora llaman correctamente al destructor.
- **Bugfix**: las clases hijas de `IGpuUniformBuffer` ahora llaman correctamente al destructor.


## 2023.02.02b

### ECS

- ***Nuevo***: `IConsumerSystem`
    - Representa un sistema cuya funcionalidad consume eventos.
    
- ***Nuevo***: `IIteratorSystem`
    - Representa un sistema cuya funcionalidad itera sobre entidades (comportamiento previamente por defecto).
    
- ***Nuevo***: `IProducerSystem`
    - Representa un sistema iterador que genera eventos.

- ***Nuevo***: `IPureSystem`
    - Representa un sistema iterador que ni genera ni consume eventos.

- `SystemManager`
    - Almacena los sistemas en tres grupos:
        - Sistemas productores.
        - Sistemas consumidores.
        - Sistemas puros.
    - Ejecuta los sistemas en el siguiente orden:
        - Sistemas productores.
        - Sistemas consumidores.
        - Sistemas puros.
        
- `IRenderSystem` ahora hereda de `IPureSystem`.
- `TerrainRenderSystem` ahora hereda de `IPureSystem`.


## 2023.02.03a

### Collision

- ***Nuevo***: `CollisionEvent`
    - Representa una colisión entre dos entidades.

- ***Nuevo***: `CollisionSystem`
    - Detecta colisiones y genera eventos.

- ***Nuevo***: `PhysicsResolver` (*WIP*)

- ***Nuevo***: `RayCastResult`
    - Almacena un resultado de un ray-cast.
    - Incluye el punto de contacto.

### ECS

- `EntityComponentSystem`
    - Permite vaciar las listas de eventos.
    - Permite saber si una entidad está viva (existe).

- `GameObjectManager`
    - Ahora permite saber si una entidad está viva (existe).

- `EventContainer`
    - Ahora hereda de `IEventContainer`.

- `ColliderRenderSystem`
    - Implementada funcionalidad inicial (para renderizado de volúmenes de nivel alto).
    
### Types

- `Color`
    - ***Nuevo***: `GREEN()`.

### Bugfixes

- **Bugfix**: `AxisAlignedBoundingBox::GetMin()` y `AxisAlignedBoundingBox::GetMax()` ahora devuelven el valor correcto.
- **Bugfix**: `ITopLevelCollider::AabbSphereCollision` ahora funciona correctamente.


## 2023.02.04a

### Graphics

###### Añadido soporte para wideframe.

- `PolygonMode`
    - *Renombrado*: `FILL` -> `TRIANGLE_FILL`.
    - ***Nuevo***: `TRIANGLE_WIDEFRAME`.

- `GraphicsPipelineVulkan`, `GraphicsPipelineDx12`
    - Ahora soporta el modo de triángulo `TRIANGLE_WIDEFRAME`.

- `MaterialSystem`
    - Ahora soporta el modo de triángulo `TRIANGLE_WIDEFRAME`.

### ECS

- `ColliderRenderSystem`
    - Ahora renderiza los colliders de bajo nivel (siempre que se hayan registrado previamente).
    - Ahora usa colores ligeramente más suaves.

- `IBottomLevelCollider`
    - Ya no permite establecer un transform de offset.

- `ConvexVolume`
    - Ahora permite saber si contiene un punto en concreto.
    - Ahora permite obtener los vértices en espacio local.
    - Ahora permite obtener los índices de las caras.

### Bugfixes

- **Bugfix**: `CollisionSystem` ahora genera eventos sólamente si colisionan los volúmenes de nivel bajo.


## 2023.02.05a

### Collision

- ***Nuevo***: `DetailedCollisionInfo`
    - Incluye la información detallada de una colisión.
    - Incluye el MTV.

- `IBottomLevelCollider`, `ConvexVolume`
    - Ahora permiten obtener información detallada de la colisión (`DetailedCollisionInfo`).
    - Ahora calculan el MTV.
    
### Physics

- ***Nuevo***: `PhysicsComponent`
    - Permite simular las físicas de un objeto (cuerpo rígido).
    - Incluye velocidad, aceleración y peso.

- `PhysicsResolver`
    - Ahora se incluye por defecto en el motor.
    - Ahora resuelve las colisiones separando las entidades (aplicando el MTV).

- ***Nuevo***: `PhysicsSystem`
    - Simula el movimiento de un objeto de acuerdo a su velocidad y aceleración.
    

## 2023.02.05b

### Assets

- `AnimMeshLoader`
    - Eliminados logs de debug.

### ECS

- `ColliderRenderSystem`
    - Ahora usa imágenes de formato `RGBA8_SRGB` en vez de `RGBA32_SFLOAT`.
    - Ahora registra automáticamente todos los objetos compatibles.

### Bugfixes

- **Bugfix**: `BloomPass` ahora no genera errores de validación.


## 2023.02.06a

### ECS

- `ISystem`
    - Ahora se puede activar y/o desactivar su ejecución.

### Bugfixes

- **Bugfix**: `ModelLoader3D` ahora respeta const-correctness del modelo cargado.


## 2023.02.07a

### Rendering

- `GBuffer`
    - Ahora usa imágenes de 16 bits por canal, en vez de 32 bits por canal.

- `Material`
    - Ahora permite recargar los shaders.

- `MaterialSystem`
    - Ahora permite obtener un material previamente cargado a partir de su nombre.
    - Ahora permite recargar los materiales.

### Bugfixes

- **Bugfix**: `StaticMeshLoader` ahora carga correctamente los vectores normales.
- **Bugfix**: `AnimMeshLoader` ahora carga correctamente los vectores normales.
- **Bugfix**: `IMeshLoader` ahora no suaviza incorrectamente los vectores normales.
- **Bugfix**: `PbrDeferredSystem` ahora acepta mapas de IBL especular.
- **Bugfix**: `PbrDeferredSystem` ahora funciona correctamente.
- **Bugfix**: `HybridRenderSystem` ahora funciona correctamente.


## 2023.02.18a

### Assets

- `IMeshLoader`
    - Ahora define la lógica para cargar todos los vértices y sus atributos.

### Collision

###### Detección de puntos de contacto

- `ConvexVolume`
    - Ahora detecta los puntos de contacto de una colisión.
    - Ahora permite generar un OBB.

- `DetailedCollisionInfo`
    - Ahor permite obtener los puntos de contacto.
    - Ahor permite obtener un punto de contacto medio.

- `ColliderRenderer`
    - Ahora renderiza los puntos de contacto.
    - Ahora renderiza el punto de contacto medio.

### Rendering

###### Soporte para el renderizado de puntos

- `PolygonMode`
    - ***Nuevo***: `POINT`.

- `IGraphisPipeline...`, `MaterialSystem`
    - Añadido soporte para `PolygonMode::POINT`.

### Types

- `Vector3`
    - Permite compararse con otro `Vector3`, usando un valor *epsilon*.


## 2023.02.24a

### Physics

- `PhysicsComponent`
    - ***Renombrado***: *weight* -> *mass*.
    - Ahora permite aplicar fuerzas.
    - Ahora permite aplicar impulsos.
    - Ahora permite obtener el momento lineal.

- `PhysicsResolver`
    - Ahora aplica impulsos a los objetos de una colisión.
 
- `PhysicsSystem`
    - Ahora simula la resistencia del aire.

### Bugfixes

- `PhysicsResolver` ya no afecta a objetos que no contienen un `PhysicsComponent`.


## 2023.02.24b

### Physics

- `PhysicsComponent`
    - Ahora almacena la velocidad angular.
    - Ahora almacena el centro de gravedad.
    - Separada la lógica del impulso en dos:
        - `ApplyLinealImpulse()`
        - `ApplyAngularImpulse()`
    - Permite obtener el torque de un punto del objeto.

- `PhysicsResolver`
    - Ahora aplica impulsos angulares a los objetos de una colisión.
 
- `PhysicsSystem`
    - Ahora tiene en cuenta la velocidad angular de los objetos.
    - Ahora simula la resistencia del aire en la rotación.

### Types

- `Quaternion`
    - Ya no se normaliza después de aplicarse una rotación.


## 2023.03.14a

### Rendering

###### Refactor de las clases de Vulkan

Se han renombrado las clases, enumeraciones y demás de renderizado del backend Vulkan: *[...]Vulkan* -> *[...]Vk*.

###### Reorganización del asignador de memoria

Ahora la lógica de asignación de bloques de memoria está implementada en `IGpuMemoryAllocator`, en vez de en las clases hijas.

## 2023.03.16a

### STD

- HashMap:
  - Ahora es un wrapper sobre `std::unordered_map`.

### Rendering

- Vértices:
  - Los vértices `Vertex3D` y `VertexAnim3D` ahora tienen un campo `tangent`, en preparación para normal mapping.
  
- `GpuImage`:
  - Ahora permite obtener resoluciones directamente en 1D, 2D y 3D (antes sólo ofrecía tamaño 3D).
  - Ya no permite escribir datos directamente sobre el buffer (`SetData()` eliminado).
  
- Bloom Post-Processing:
  - Ahora usa la cadena de mip-levels de una sola imagen, en vez de dos imágenes en ping-pong.
  - Aumentado el número máximo de pases de 4 a 8 pases.
  - Ya no realiza una copia final.
  - Corregidos varios bugs.

###### Rework del sistema de vistas de imágenes

Ahora los procesos que cosumen una imagen lo hacen a través de un `IGpuImageView`, en vez de a partir de una imagen.
Esto ofrece un mayor control sobre el acceso a la imagen, ya que antes se elegía siempre un mismo view por defecto.

- ***Nuevo***: `GpuImageViewConfig`:
  - Estructura que almacena las características de un image view.
  - Contiene constructores de ayuda.

- `IGpuImageView`:
  - Ahora se crea a partir de `GpuImageViewConfig`.
 
- `IMaterialSlot`:
  - Ahora aceptan `IGpuImageView` en vez de `GpuImage` a la hora de establecer imágenes sampled y de storage. Esto permite un mayor control sobre el acceso a la imagen.
  - Interfaz para establecer imágenes samplde y de storage simplificada.

- `IPostProcessPass`:
  - Ahora permite indicar el `GpuImageViewConfig` que se usará para establecer el input a partir de un rendertarget de cualquier tipo.
  - Ahora acepta `IGpuImageView` en vez de `GpuImage` a la hora de establecer imágenes de entrada.

### Bugfixes

- **Bugfix**: *Bloom* ya no tiene leakeos de luminosidad en los bordes.
- **Bugfix**: *Bloom* ahora adiciona correctamente todos los niveles de downscale/upscale.
- **Bugfix**: `HashMap` ya no tiene UB.

### Otros

- Añadido el macro `OSK_ASSUME(X)`
- Corregidos algunos errores menores (class/struct).
- Renombrados algunos archivos que faltaban (...Vulkan -> ...Vk).


## 2023.03.18a

### Types

- Quaternion:
  - Ahora permite rotar un vector de acuerdo a la orientación del cuaternión.

### Rendering

- Bloom Post-Processing:
  - Ligera optimización.
  - Ahora desescala y escala de una manera ligeramente distinta.

###### Estadísticas de memoria

Ahora se puede obtener estadísticas sobre la cantidad de memoria usada y disponible en la GPU.

- ***Nuevo:*** `GpuHeapMemoryUsageInfo`:
  - Muestra la capacidad máxima y el espacio usado de un memory heap.
  
- ***Nuevo:*** `GpuMemoryUsageInfo`:
  - Muestra la capacidad máxima y el espacio usado de toda la memoria de la GPU, dividida por tipo de memoria.
  - Contiene una lista con un `GpuHeapMemoryUsageInfo` por cada memory heap disponible.

- `IGpu`, `IGpuMemoryAllocator`:
  - Ahora permite obtener estadísticas sobre la cantidad de memoria usada y disponible.

  ###### Formatos exclusivos de precisión

Ahora se puede elegir distintos formatos de profundidad.
Además, se pueden usar formatos únicamente de profundidad, sin la parte stencil.

- `GpuImageUsage`:
  - ***Eliminado:*** `GpuImageUsage::DEPTH_STENCIL`.
  - ***Nuevo:*** 
    - `GpuImageUsage::DEPTH`
    - `GpuImageUsage::STENCIL`.

- `Material`:
  - Ya no presupone un formato de profundidad específico a la hora de generar pipelines, permitiendo elegir distintos formatos de profundidad.

- `RenderTarget`:
  - Ahora puede usar una imagen de profundidad con formato únicamente de profundidad, sin la parte stencil.

- `ViewUsage`:
  - ***Nuevo:*** `ViewUsage::DEPTH_ONLY_TARGET`.

- `ICommandList`:
  - Ahora permite usar imágenes exclusivas de profundidad para renderizado 3D.
  - `BeginGraphicsRenderpass()` y `EndGraphicsRenderpass()` ahora permiten desactivar la sincronización automática de imágenes (por defecto, está activada).

- `GBuffer`:
  - Ahora usa una imagen de profundidad con formato únicamente de profundidad, sin la parte stencil.

- Collider Render System:
  - Ahora usa un formato de profundidad exclusiva de half-precission.

- Skybox Render System:
  - Ahora usa un formato de profundidad exclusiva de half-precission.
  
- `ShadowMap`:
  - Ligeramente optimizado (~17% aprox.).
  - Reducido considerablemente su tamaño en memoria de la GPU.
  - Ahora usa un formato de half-precission.

### Bugfixes

- **Bugfix**: `GpuImage` ahora funciona correctamente al usar formatos exclusivos de profundidad: ya no intentará establecer flags de *stencil*.
- **Bugfix**: `IBottomLevelAccelerationStructure` y `BottomLevelAccelerationStructureVk` ya no tienen memory leaks al eliminarse.
- **Bugfix**: `ITopLevelAccelerationStructure` y `TopLevelAccelerationStructureVk` ya no tienen memory leaks al eliminarse.
- **Bugfix**: La cámara 3D ahora aplica correctamente rotación cuando cambia la orientación de su transform padre.

## 2023.04.06a

### Rendering

Normal mapping y TAA.

- Model y mesh loaders:
  - Ahora cargan los vectores tangenciales de los vértices.
  - Ahora cargan las texturas de vectores normales.
    - En caso de no tener, usarán una textura azul por defecto.

- ***Nuevo:*** Temporal Anti-Aliasing
  - Implementado en `TaaProvider`.
  - Incluye dos pases:
    - Pase de acumulación temporal.
    - Pase de afilado de imagen.
  - Implementa una estrategia de descartado agresiva junto a un patrón de *jitter* de convergencia rápida para evitar *motion blur*.
  - Permite obtener un índice para modificar el *jitter* para el renderizado de la escena original.
    - Por defecto devuelve un índice entre 1 y 4 (ambos incluidos), ya que la implementación en `RenderSystem3D` y `PbrDeferredRenderSystem` usa un patrón de hélice para el *jitter*.
    - Devuelve 0 si está desactivado.
  - No realiza sincornización de las imágenes de entrada y salida.

- `RenderSystem3D`:
  - Ahora genera una imagen con los vectores de movimiento.
  - Ahora usa normal mapping.
  - Ahora usa TAA.
  - Reorganización general.

- `PbrDeferredRenderSystem`:
  - Ahora usa normal mapping.
  - Ahora usa TAA.
  - Ahora usa shaders de computación para el proceso de resolve.

- `Bloom`:
  - Ajustado su intensidad para obtener una imagen con colores más nítidos preservando una intensidad de bloom adecuada en las partes más brillantes:
    - En el úlitmo paso aumenta la intensidad de la imagen borrosa de manrea exponencial, incrementando así más intensamente las partes más brillantes.
    - Al realizar la adición final, se usa un factor más pequeño para que las partes menos brillantes no queden con un color menos intenso.

### Bugfixes

- **Bugfix**: Los modelos 3D ya no se cargan con el doble de índices (la mitad de los cuales son 0).
- **Bugfix**: Ahora se puede usar la imagen de profundidad de `GBuffer` como sampled.
- **Bugfix**: Copiar de un buffer a una imagen ya no necesita que la imagen haya sido creada con `GpuImageUsage::TRANSFER_SOURCE` si no usa mip-maps.
- **Bugfix**: `PbrDeferredRenderSystem` ahora genera correctamente las imágenes de movimiento.
- **Bugfix**: `ShadowMap` ahora genera correctamente las sombras.
- **Bugfix**: `ShadowMap` ahora crea los búfers de matrices con el tamaño correcto.
- **Bugfix**: Renderizar una escena con `ShadowMap` ya no disminuye la cantidad de luz de la escena.
- **Bugfix**: Ahora no se generan puntos excesivamente brillantes en el renderizado PBR.

###### Bugs conocidos:

- **Bug**: `ShadowMap` genera sombras incoherentes en las vallas.
- **Bug**: `ShadowMap` genera sombras incoherentes en el renderizador deferred.


## 2023.05.03a

### UI

Recomenzado el desarrollo del sistema de interfaz de usuario.

- ***Nuevo*** `UI::IElement`
  - Clase abstracta, base de la jerarquía de elementos UI.
  - Declara funcionalidad de renderizado y de actualización a la entrada del ratón.
  - Pueden establecerse las siguientes características de cada elemento:
    - Posición (tanto relativa al padre como global).
    - Tamaño.
    - Márgenes (no entran dentro del cómputo del tamaño).
    - Padding (márgenes internos).
    - Tipo de anclaje (`UI::Anchor`).

###### Contenedores

- ***Nuevo*** `UI::IContainer`
  - Clase abstracta, base de cualquier contenedor de elementos UI.
  - Cada clase derivada define un layout.
  - Renderiza y actualiza todos los hijos.
    - Si un hijo no debe renderizarse o actualizarse, esa comprobación se realiza en el propio hijo.
  - Puede tener una imagen de fondo.
  - Puede asignarse una clave a cada hijo añadido, para despues obtenerlo.
  - Puede ajustar su tamaño al tamaño de sus hijos mediante una función.
  
- ***Nuevo*** `UI::HorizontalContainer`
  - Contenedor que coloca los elementos de manera horizontal, uno después del otro.
  - De izquierda a derecha.
  - Ignora cualquier valor del Anchor del eje horizontal.
  
- ***Nuevo*** `UI::VerticalContainer`
  - Contenedor que coloca los elementos de manera vertical, uno después del otro.
  - De arriba a abajo.
  - Ignora cualquier valor del Anchor del eje vertical.
  
- ***Nuevo*** `UI::FreeContainer`
  - Contenedor que coloca los hijos en un lado, una esquina o centro, dependiendo de su Anchor.
  - No puede haber más de 1 elemento en cada posición, ya que se solaparán.
  
###### Elementos

- ***Nuevo*** `UI::ImageView`
  - Elemento que muestra una imagen.
  - Contiene un `Sprite`.
  
- ***Nuevo*** `UI::TextView`
  - Elemento que muestra un texto.
  - Puede establecerse su fuente y su tamaño de fuente.
  
- ***Nuevo*** `UI::Button`
  - Implementa un botón con el que el usuario puede interaccionar.
  - Tiene tres posibles estados, dados por `UI::Button::ButtonState`.
  - Puede mostrar una imagen dependiendo de su estado.
  - Puede contener un texto.
    - El texto estará siempre centrado.
  - Disponible en dos modos (dados por `UI::Button::Type`):
    - Botón normal.
    - Botón toggle, a modo de checkbox. En este caso guarda su estado `UI::Button::ButtonState::PRESSED` después de pulsarlo, y recupera el estado `UI::Button::ButtonState::DEFAULT` al volver a pulsarlo.
  - En cualquier modo, ejecuta el callback.
    - En modo normal, el argumento de estado será siempre `true`.
    - En modo toggle, el argumento de estado dependerá de si el botón pasa a estar pulsado o no.
    
- ***Nuevo***: `UI::Button::ButtonState`
    Representa el estado del botón (en el siguiente orden de prioridad):
    - `SELECTED`: el ratón está encima del botón, o el botón está siendo seleccionado por un mando o teclado.
    - `PRESSED`: el botón está siendo pulsado.
    - `DEFAULT`: el botón no está siendo pulsado.

- ***Nuevo***: `UI::Button::Type`
    Representa el tipo de comportamiento del botón.
    - `NORMAL`: funcionamiento estándar de un botón. Si se pulsa, se ejecuta el callback con argumento `true`.
    - `TOGGLE`: funcionamiento estilo checkbox. El botón recuerda su estado, que va alternando entre `PRESSED` y `DEFAULT` cada vez que se pulsa.
        - Cada vez que se pulsa, se ejecuta el callback con el argumento:
            - `true` si el estado pasa a ser `PRESSED`.
            - `false` si el estado pasa a ser `DEFAULT`.

### Rendering

- ***Nuevo***: `FrameCombiner`
  - Permite combinar dos imágenes del mismo tamaño superponiendo una ante la otra, como si se renderizara una sobre la otra.
  - Considerablemente más eficiente que rasterizado equivalente.
  - Usable con imágenes de los siguientes formatos:
    - `RGBA8_UNORM`
    - `RGBA16_SFLOAT`

- Formats:
  - ***Nuevo***: `RGBA8_UINT`
  - ***Nuevo***: `RGB10A2_UNORM`

- `IGpuImageView`:
  - Ahora almacena el tamaño de la imagen original.

###### Reorganización del GBuffer

Reorganización general de los render targets.
Ahora la posición se reconstruye a partir de la imagen de profundidad.

- `PbrDeferredRenderSystem3D`:
  - Ahora reconstruye la posición a partir de la imagen de profundidad.
  - Corregidos errores varios.

- `GBuffer`

  - ***Eliminado***: render target de posición.
  - Render target de normales;
    - Ahora usa un formato `RGB10A2_UNORM` de mayor precisión.
    - Ya no contiene información del material (metallic-roughness).
  - Render target de motion vectors ahora usa un formato `RG16_SFLOAT` más pequeño.
  - ***Nuevo***: render target de metallic-roughness.
    - Usa un formato `RG16_SFLOAT`.

###### Unificación de buffers

Ahora existe un único tipo de GPU buffer. Un GPU buffer se puede usar de cualquiera de las maneras anteriores (vertex, index, storage, uniform).

Para los buffers usados como vertex buffer / index buffer, ahora existen nuevas estructuras llamadas *views*, que contienen información sobre cómo procesarlos.

- ***Eliminados:*** `IGpuVertexBuffer`, `IGpuIndexBuffer`, `IGpuSotrageBuffer`, `IGpuUniformBuffer` y derivados.

- `GpuBuffer` (antes `IGpuBuffer`)
  - Renombrado.

- ***Nuevo***: `VertexBufferView`
  - Estructura que permite procesar los datos de un buffer de vértices.
  - Puede usarse para procesar un subrango de un buffer.
  - Cada buffer contiene uno, facilitando el uso de un buffer entero como vertex buffer.
  - Contiene:
    - Información del vértice (`VertexInfo`).
    - Nmero de vértices.
    - Offset del primer vértice respecto al inicio del buffer.

- ***Nuevo***: `IndexBufferView`
  - Estructura que permite procesar los datos de un buffer de índices.
  - Puede usarse para procesar un subrango de un buffer.
  - Cada buffer contiene uno, facilitando el uso de un buffer entero como index buffer.
  - Contiene:
    - Tipo de índice (`OSK::GRAPHICS::IndexType`). Puede ser:
      - `U8` (byte).
      - `U16` (byte).
      - `U32` (byte).
    - Número de índices.
    - Offset del primer índice respecto al inicio del buffer.

- `ICommandList`
  - Ahora permite establecer un subrango de un buffer como buffer de vértices (mediante un `VertexBufferView`).
  - Ahora permite establecer un subrango de un buffer como buffer de índices (mediante un `IndexBufferView`).
  
- `IBottomLevelAccelerationStructure`
  - Pueden crearse a partir de un subrango de buffers mediante `VertexBufferView` e `IndexBufferView`.

- `IGpuMemoryAllocator`:
  - Todas las funciones de creación de buffers de un tipo específico (vertex, index, storage, uniform) siguen existiendo para simplificar su creación, pero ahora devuelven buffers genéricos que pueden usarse para otros motivos.
    - `CreateVertexBuffer()` ahora establece su `VertexBufferView` por defecto para que apunte a todo el buffer, de acuerdo al tipo de vértice indicado.
    - `CreateIndexBuffer()` ahora establece su `IndexBufferView` por defecto para que apunte a todo el buffer, de acuerdo al tipo de índice indicado.
  - Permite crear un `IBottomLevelAccelerationStructure` a partir de subrangos de buffers de vértices e ndices.

###### Sincronización  

  Se ha revisado el diseño de la sincronización de comandos e imágenes, renombrando las estructuras y los enums para que representen con mayor fidelidad su funcionamiento real.
  También se ha mejorado su documentación.

  También se ha mejorado la sincornización del proyecto entero, moviendo las acciones de sincronización al momento en el que son realmente necesarias.

  - `GpuCommandStage` (antes `GpuBarrierStage`)
    - ***Eliminado***: `DEFAULT`.
    - ***Nuevo***: `NONE`.
      - Indica que ningún comando será afectado.
    - ***Nuevo***: `ALL`.
      - Indica que todos los comandos serán afectados.

  - `GpuAccessStage` (antes `GpuBarrierAccessStage`)
    - ***Nuevo***: `SAMPLED_READ`.
    - ***Nuevo***: `UNIFORM_BUFFER_READ`.
    - ***Nuevo***: `STORAGE_BUFFER_READ`.
    - ***Nuevo***: `STORAGE_IMAGE_READ`.
    - ***Nuevo***: `NONE`.
      - No indica ningún acceso desde / hacia memoria.

  - `GpuImageRange` (antes `GpuImageBarrierInfo`).
    - Renombrado.
    - Puede usarse en tareas no relacionadas con la sincronización.

  - `GpuImage`:
    - Ahora mantiene un seguimiento de cual ha sido el úlitmo image barrier aplicado, simplificando las tareas posteriores de sincronización.
      - Ya no es necesario conocer de manera explícita el image barrier anterior al introducir un nuevo barrier.
    - Ahora mantiene correctamente un seguimiento del layout de cada una de las capas y mip-levels.

###### Renderizado 2D

Se ha modernizado el renderizado de sprites, simplificando el material del sprite y la configuración que necesita para facilitar su uso.

- `Sprite`
  - Su material ya no tiene una referencia al buffer de la cámara 2D, por lo que se podrá renderizar usando cualquier cámara 2D.
  - Se puede establecer específicamente el `IGpuImageView` que se renderizará.
    - Antes creaba el view por defecto a partir de una imagen de GPU.

- `SpriteRenderer`
  - Ahora permite establecer la cámara 2D que se usará en los siguientes sprites renderizados.
  - Si se cambia el material de renderizado 2D o se enlaza un nuevo material slot, se debe hacer a través de la clase `SpriteRenderer`.
    - Esto permite usar estrategias de renderizado distintas en el futuro, como un renderizado con sprite bacthes.
  - La matriz de la cámara 2D ahora se aplica a la matriz modelo en la CPU, enviando a la GPU la matriz final.
  - Ahora usa string views para el renderizado de texto.

- `Texture`
  - Ahora permite obtener fácilmente los siguientes `IGpuImageView`:
    - View con un único nivel de mip-map.
    - View con un único todos los niveles de mip-map.
    
### Otros

- El efecto de *tone-mapping* ahora genera una imagen ligeramente menos azul.
- Ajustado ligeramente el efecto de *bloom*, revirtiendo parcialmente los cambios realizados en **2023.04.06a**.

### Bugfixes

- **Bugfix**: `PbrDeferredRenderSystem3D` ya no genera sombras incoherentes.
- **Bugfix**: `PbrDeferredRenderSystem3D` ya no genera normales de menos precisión cuando un objeto se aleja del origen de coordenadas.
- **Bugfix**: BeginRenderpass ahora por defecto sincroniza correctamente las imágenes de profundidad.


## 2023.06.17a

### ***Nuevo***: Audio

###### Implementación inicial

Implementación inicial de un sistema de carga y reproducción de audio usando *OpenAL Soft*.
Carga de archivos *wav*.

- ***Nuevo***: `AudioApi`
    - Clase principal del sistema.
    - Contiene las instancias de *AUDIO::Device* disponibles.
    - Permite establecer un *AUDIO::Device* como dispositivo de salida activo.
    - Permite establecer la posición, orientación y velocidad del escuchador.

- ***Nuevo***: `AUDIO::Device`
    - Representa un dispositivo de salida de audio.
    - Permite establecerse como dispositivo de salida activo.

- ***Nuevo***: `AUDIO::Buffer`
    - Representa un buffer alojado en un dispositivo de salida.
    - Contiene la información de un sonido / música.

- ***Nuevo***: `AUDIO::Source`
    - Representa un "reproductor" de audio.
    - Reproduce un sonido / música almacenado en un *AUDIO::Buffer*.
    - Permite establecer la posición, velocidad, tono y volumen.
    - Permite reproducción en bucle.

- ***Nuevo***: `AUDIO::Format`:
    - Indica un formato de audio.
    - Disponibles.
        - `MONO8`.
        - `MONO16`.
        - `STEREO8`.
        - `STEREO16`.

### Assets

###### Assets de audio

- ***Nuevo***: `AudioAsset`
    - Asset que representa un sonido o música.
    - Contiene un buffer con los datos del sonido.

- ***Nuevo***: `AudioAssetLoader`
    - Permite cargar archivos de audio en formato *wav*.


### Error Handling

Reformado el sistema de manejo de errores, usando excepciones específicas para cada tipo de error.

- `EngineException`:
    - Clase base para todas las excepciones del motor y del juego.
    - Contiene una descripción de la excepción y la localización en la que se lanzó.

- Excepciones añadidas:
    - `FontLibraryInitializationException`
    - `FontLodaingException`
    - `NoVertexPositionsFoundException`
    - `NoVertexNormalsFoundException`
    - `NoVertexTangentsFoundException`
    - `NoVertexTexCoordsFoundException`
    - `UnsupportedIndexTypeException`
    - `UnsupportedJointTypeException`
    - `UnsupportedModelImageFormatException`
    - `UnsupportedPolygonModeException`
    - `AssetDescriptionFileNotFoundException`
    - `AssetLoaderNotFoundException`
    - `InvalidDescriptionFileException`
    - `RawAssetFileNotFoundException`
    - `AudioDeviceCreationException`
    - `EventNotRegisteredException`
    - `SystemAlreadyRegisteredException`
    - `InvalidObjectException`
    - `ComponentNotFoundException`
    - `ComponentNotRegisteredException`
    - `ObjectAlreadyHasComponentException`
    - `ComponentAlreadyRegisteredException`
    - `SystemNotFoundException`
    - `WindowNotCreatedException`
    - `RenderedNotCreatedException`
    - `InvalidArgumentException`
    - `InvalidObjectStateException`
    - `NotImplementedException`
    - `FileNotFoundException`
    - `InitializeWindowException`
    - `LoggerNotInitializedException`
    - `BadAllocException`
    - `MatrixBufferNotCreatedException`
    - `AccelerationStructureCreationException`
    - `RtShaderBindingTableCreationException`
    - `CommandListResetException`
    - `CommandListStartException`
    - `CommandListEndException`
    - `CommandListCreationException`
    - `InvalidVertexBufferException`
    - `InvalidIndexBufferException`
    - `GpuBufferCreationException`
    - `GpuMemoryAllocException`
    - `NoCompatibleGpuMemoryException`
    - `GpuMemoryBlockNotEnoughSpaceException`
    - `GpuMemoryNotMappedException`
    - `MaterialNotFoundException`
    - `DescriptorPoolCreationException`
    - `DescriptorLayoutCreationException`
    - `MaterialSlotCreationException`
    - `InvalidMaterialException`
    - `PipelineCreationException`
    - `ShaderLoadingException`
    - `ShaderCompilingException`
    - `PipelineLayoutCreationException`
    - `RayTracingNotSupportedException`
    - `RendererCreationException`
    - `GpuNotCompatibleException`
    - `GpuNotFoundException`
    - `CommandListSubmitException`
    - `CommandQueueSubmitException`
    - `CommandPoolCreationException`
    - `LogicalDeviceCreationException`
    - `ImageCreationException`
    - `ImageViewCreationException`
    - `SwapchainCreationException`

### Rendering

###### Ecuación de renderizado mini rework

- PBR Rendering
    - Se ha normalizado la ecuación de renderizado para que los pesos de las distintas fuentes de color (ambiente y cubemaps) sean iguales.
    - Se ha modificado la ecuación de renderizado para ofrecer colores más intensos.

###### Cleanup

Eliminadas las clases SyncDevice, ya que la sincornización se realiza en las clases principales `IRenderer`.

- ***Eliminado***: `ISyncDevice`, `SyncDeviceDx12`, `SyncDeviceVk`.

### Types

Se han introducido nuevos tipos de datos básicos y renombrado otros para que queden más clara su intención y su tamaño.

- `USize32`: renombrado (antes `TSize`).
- `UIndex32`: renombrado (antes `TIndex`).
- ***Nuevo***: `USize64`.
- ***Nuevo***: `UIndex64`.

###### Memoria de 64 bits

Debido a los cambios en los tipos de datos básicos, numerosas clases y funciones (sobre todo aquellas que tienen que ver con memoria, ahora soportan tamaños y direcciones de 64 bits. Incluyendo:

- `DynamicArray`.
- `MemorySwap()`.
- `GpuMemoryBlock` y todos los recursos derivados, como buffers o texturas.
- Otras.

### Otros

- `OSK_DEFINE_AS(x)`: ahora genera dos funciones en vez de una:
    - ***Eliminado***: *`constexpr T* As() const`*.
    - ***Nuevo***: *`constexpr const T* As() const`*.
    - ***Nuevo***: *`constexpr T* As()`*.

- ***Nuevo***: `OSK_DEFAULT_MOVE_OPERATOR(x)`.
   - Genera el constructor y el operador de movimiento por defecto.

### Bugfixes

- **Bugfix**: `OSK_DEFINE_AS` ahora no genera *undefined behaviour*.
- **Bugfix**: `OwnedPtr` ahora se comporta correctamente al moverse.
- **Bugfix**: `Color::Blue` ahora devuelve un color azul en vez de un color turquesa.
- **Bugfix**: `Color::*` ahora multiplica correctamente el valor alpha.
- **Bugfix**: `IMaterialSlot::SetTexture` ahora usa correctamente el canal indicado.

## 2023.07.15a

### Physics

###### Rework de los sistemas de respuesta ante colisiones y de físicas.

- `PhysicsComponent`
    - Aumentada la encapsulación para evitar la corrupción de los valores por modificaciones externas que se podrán saltar las leyes de la física.
    - Almacena el inverso de la masa.
        - Se puede usar para representar masas infinitas, estableciendo el inverso de la masa a 0.
    - Añadido soporte para el *momento de inercia angular*, que regula la facilidad de un objeto para rotar.
        - Alacenados tanto el tensor de inercia como su inverso para una mayor eficiencia.
    - Almacena el cambio de velocidad en cada frame, para una mayor estabilidad.
    - Añadido el coeficiente de restitución, que indica cuánto rebota el objeto tras una colisión.
    - Permite resetear las fuerzas aplicadas en cada frame, para poder recalcular su superposición.
    - A la hora de aplicar un impulso, no se hace distinción entre impulso lineal y angular, efectuando ambos al mismo tiempo.
    
- `PhysicsResolver`
    - Ahora la aplicación del MTV se reparte entre los dos objetos dependiendo de sus masas.
        - Los objetos con menor masa que el otro objeto se mueven más.
        - Los objetos con masa infinita no se mueven.
    - Ahora la aplicación de los impulsos tras una colisión sigue las leyes de la física:
        - Obtiene la velocidad de separación final y aplica los impulsos para conseguirla.
        - Ambos impulsos aplicados son iguales en intensidad, pero en sentidos contrarios.
    - Ahora aplica fricción a los objetos.

- `PhysicsSystem`
    - Ahora simula la resistencia del aire, ralentizando la velocidad angular de los objetos en el tiempo.

- `ConvexVolume`
    - Proceso de detección de puntos de contacto optimizado.
    - Ahora es más permisivo al incluir nuevos puntos de contacto.

- `DetailedCollisionInfo`
    - Permite diferenciar el orden de los objetos.
    
### IO

- ***Nuevo***: `Console`
    - Representa una consola de output dentro del juego.
    - Permite añadir nuevos mensajes de texto.
    - Almacena información sobre el momento en el que se añadió el mensaje.
    - _Prototipo_.

### Engine

- ***Nuevo*** `gameFrameIndex`.
    - Indica el índice del frame actual desde el inicio de la ejecución del juego.
- ***Nuevo*** `Update()`
    - Actualiza el `gameFrameIndex`.
    
### Types

- `Vector3`
    - Añadido valor estático `Zero`, que representa un vector con todos los campos a 0.
    
### Error Handling

- ***Nuevo***: `DivideByZeroException`.

### Bugfixes

- **Bugfix**: `PhysicsSystem` ya no mueve y/o rota objetos inmovibles.
- **Bugfix**: `COLLISION::ConvexVolume` ahora calcula correctamente los puntos de contacto en todos los casos.
- **Bugfix**: Añadir un componente a un objeto no hace que se vuelva a registrar en los sistemas en los que ya estaba presente.
- **Bugfix**: `ECS::AddComponent(GameObjectIndex, const TComponent&)` ahora se puede usar.
- **Bugfix**: `FaceProjection` ahora calcula correctamente las proyecciones de los puntos.


## 2023.08.13a

### ECS

###### Orden de ejecución de los sistemas

- `EntityComponentSystem`, `SystemManager`
    - Ahora se le puede asignar un índice de ejecución a cada sistema (por defecto: 0). Todos los sistemas con el mismo índice se agrupan en un conjunto.
    - La ejecución de los sistemas se realiza de acuerdo a estos índices, ejecutándose primero los sistemas con un índice más bajo.
    - Dentro de cada conjunto de sistemas, se ejecutan en el siguiente orden:
        - Sistemas *productores*. 
        - Sistemas *consumidores*. 
        - Sistemas *puros*. 

- `ISystem`
    - Ahora almacena su índice de ejecución.

- Sistemas con un índice de ejecución distinto a 0:
    - `PhysicsSystem`: -2.
    - `CollisionSystem`: -1.
    - `PhysicsResolver`: -1.
    
### Rendering

###### Rework de los shaders PBR para obtener una imagen de mayor calidad.

### Memory

- `Buffer`
    - Representa un buffer con información arbitraria en memoria RAM.
    - Puede ser copiado.
    - No tiene un tamaño fijo: funciona como un *DynamicArray*.
    
### Persistencia

###### Inicio del rework para mejorar el rendimiento potencial y adaptarlo mejor aun futuro sistema de networking.

- ***Eliminado***: `Field`
- ***Eliminado***: `IDataElement`
- ***Eliminado***: `ISerializer`
- ***Eliminado***: `ISerializable`

- `DataNode`
    - Ahora funciona escribiendo la información sobre un buffer de memoria.
    - Permite leer datos del buffer de memoria.
    - Contiene un schema que indica cómo interpretar los datos.
    - Tiene mapas para facilitar la interpretación de los datos.

- `DataType`
    - Ahora se representa por un número.
    - Los primeros 100 números están reservados para el motor.
    
- ***Nuevo***: `Serialize<>()`
    - Define cómo serializar un elemento a un `DataNode`.
    - Debe definirse para todos los tipos que se quieran usar.

- ***Nuevo***: `Deserialize<>()`
    - Define cómo deserializar un `DataNode` a un elemento.
    - Debe definirse para todos los tipos que se quieran usar.

- ***Nuevo***: `SchemaId`
    - Define un código único para cada esquema de `DataNode`.

### STD

###### Corregidas incongruencias en las funciones de las clases, con lo que ahora siempre respetan la corrección de *const*.

- `UniquePtr`
    - Ahora todas sus operaciones respetan la corrección de *const*.
    - Ahora tiene una comprobación en tiempo de compilación que se asegura de que el objeto está definido.
    - ***Eliminado***: `UniquePtr<T[]>`.
    - ***Eliminado***: `SetValue()`.
    - Añadidas todas las posibles combinaciones de *const* / *not const* para:
        - `GetPointer()`.
        - Operador `->`.

- `DynamicArray`
    - `DynamicArray::Iterator` 
        - Simplificado (ahora solo almacena un puntero).
        - Ya no necesita que la colección que lo creó tenga estabilidad de puntero.
            - Siguen necesitando que no se añadan y/o eliminen elementos.
    - ***Nuevo***: `DynamicArray::ConstIterator`
        - Iterador para acceso a un elemento de una colección inmutable.
    - Ahora soporta tipos que tengan sobreescrito el operador `&`.
    - Ahora genera código distino dependiendo de si el tipo puede ser copiado y/o movido.
    - Añadidas todas las posibles combinaciones de *const* / *not const* para:
        - Operador `[]`.
        - `At()`.
        - `Find()`.
        - `GetData()`.
        - `begin()`.
        - `end()`.
        - `GetIterator()` / `GetConstIterator()`.

- `SharedPtr`
    - Añadidas todas las posibles combinaciones de *const* / *not const* para:
        - Operador `->`.
        - Operador `*`.
        - `GetPointer()`.
        - `Get()`.

- ***Eliminado***: `HashMap`
    - Reemplazado por `std::unordered_map`.
    - Ahora se puede usar `std::string_view` para acceder a elementos de un mapa cuya clave es del tipo `std::string`.
    
### Otros

- ***Nuevo***: `OSK_DISABLE_MOVE(x)`.
- ***Nuevo***: `OSK_DEFAULT_COPY_OPERATOR(x)`.

### Bugfixes

- **Bugfix**: ahora se exportan correctamente el proyecto para poder usarse como DLL / LIB.
- **Bugfix**: `UniquePtr` ya no genera *undefined behaviour* en los *getters*.
- **Bugfix**: eliminar un elemento de un `DynamicArray` ahora ejecuta el operador de copia / movimiento para los elementos posteriores que se mueven a la izquierda.
- **Bugfix**: `ConvexVolume::GetWorldSpaceAxis` ahora devuelve correctamente el eje en espacio del mundo.
    - **Bugfix**: ya no se generan respuestas exageradas a colisiones.
    - **Bugfix**: ahora se detectan correctamente las colisiones en las coordenadas (0, 0 ,0).
- **Bugfix**: `StaticMeshLoader`, `AminMeshLoader` ahora cargan correctamente los vectores normales de los modelos 3D en todos los casos.
- **Bugfix**: corregidos numerosos bugs en los shaders de renderizado PBR.
- **Bugfix**: `TextureLoader` ahora carga las imágenes en el espacio de color adecuado.
- **Bugfix**: `Transform3D::TransformPoint()` ahora devuelve el resultado correcto.

TODO:

fix ComponentManager::InsertCopy


## 2023.09.08a

### UI

- `UI::IElement`:
    - Ahora puede configurarse su visibilidad.
        - Los contenedores invisibles no renderizarán a sus hijos.
    - Ahora puede configurarse su capacidad para actualizar su estado.
        - Los contenedores desactivados no actualizarán a sus hijos.
    - Ahora puede configurarse para mantener un tamaño relativo respecto al padre, cuando el padre cambia de tamaño.
    
### Collisions

- `CollisionSystem`
    - *Optimización*: Ahora sólamente transforma los vértices de los colliders de nivel bajo una sola vez por fotograma, en vez de una vez por posible colisión.

- `ConvexVolume`
    - Ahora también almacena los vértices transformados (se actualiza cada fotograma).
    - `AddFace` ya no necesita que los vértices están en sentido horario.
    - `GetLocalSpaceAxis` ahora devuelve el vector correcto independientemente del orden de los vértices.
    - `GetWorldSpaceAxis` ahora devuelve el vector correcto independientemente del orden de los vértices.

### Bugfixes

- **Bugfix**: `VerticalContainer::EmplaceChild()` ahora coloca correctamente los elementos en el eje horizontal.
- **Bugfix**: `FreeContainer::EmplaceChild()` ahora tiene en cuenta correctamente los márgenes del elemento añadido.
- **Bugfix**: `ConvexVolume::GetCollisionInfo()` ahora detectamente correctamente la información de las colisiones en todas las direcciones (test cases *T-COL-09,11,13,15*)
- **Bugfix**: `ConvexVolume::GetWorldSpaceAxis()` ahora devuelve un vector correctamente normalizado en todos los casos.
- **Bugfix**: `PhysicsResolver` ahora aplica correctamente impulsos en todos los casos.



## 2023.09.11a

### Graphics

###### Introducido HBAO como técnica de oclusión ambiental.

- ***Nuevo***: `HbaoPass`:
    - Implementa oclusión ambiental como efetdo de post-procesado de computación.
    - Incluye 4 pases:
        - Pase inicial que genera una imagen de oclusión ambiental con ruido.
        - Pase de difuminado gaussiano vertical.
        - Pase de difuminado gaussiano horizontal..
        - Pase de resolución.
    - Usa como input las imágenes de profundidad y de normales.
   
- `RenderSystem3D`:
    - Ahora genera imágenes de normales.

###### Invertida la coordenada Z de profundidad y uso de far-plane infinito.

Esto aumenta enormemente la precisión de objetos alejados, y permite renderizar toda la escena gracias al far-plane infinito.

- `CameraComponent3D`
    - Ahora `GetProjectionMatrix()` genera una proyección con:
        - Z invertida.
        - Far-plane infinito.
    - ***Nuevo***: `GetProjectionMatrix_UnreversedZ()`, que implementa la funcionalidad que previamente tenía `GetProjectionMatrix()`.

###### Misc.

- `Material`, `MaterialSystem`:
    - Los materiales gráficos ya no están obligados a tener shaders de fragmentos.

- `IGraphicsPipeline`, `GraphicsPipelineVk`:
    - Ahora usa un formato de profundidad con z invertida.
    - Los fragment shaders ya no son estrictamente necesarios.

- `Format`
    - ***Nuevo***: `R16_SFLOAT`

- `CopyImageInfo`
    - Ahora permite indicar los canales de origen y destino.

- `ICommandList`
    - ***Nuevo*** `CopyImageToImage()`.
        - Permite copiar una imagen a otra cuando ambas imágenes tienen distinto tamaño y/o formato.
    - `ICommandList::RawCopyImageToImage()` ahora puede efectuar copias en canales distintos a *COLOR*.

- `GpuImageVk`
    - ***Nuevo***: `GetAspectFlags()`.
    - `GetFilterTypeVulkan()` ahora es public.
    - `GetAddressModeVulkan()` ahora es public.

### Bugfixes

- **Bugfix**: ahora las sombras no tienen errores de z-fighting en el renderizado diferido.
- **Bugfix**: ahora ningún renderizado 3D tienen z-fighting en zonas alejadas de la cámara.
- **Bugfix**: `ICommandList::RawCopyImageToImage()` ahora tiene en cuenta los offsets indicados para las imágenes de origen y destino.
- **Bugfix**: ahora los mapas de sombras no usan una proyección con un rango de profundidad incorrecto (-1, 1).
- **Bugfix**: ahora la cámara 3D no usa una proyección con un rango de profundidad incorrecto (-1, 1).
- **Bugfix**: ahora `GBuffer` tiene un *getter* de imágenes no-const.
- **Bugfix**: `PbrDeferredRenderSystem` ya no reenlaza el material de GBuffer una vez por modelo 3D.
 

## 2023.09.15a

### UI

- ***Nuevo***: `Dropdown`
    - Elemento de UI que permite al usuario seleccionar una opción entre varias mediante un menú desplegable.

- `IElement`
    - `UpdateByCursor()`:
        - Ahora devuelve un bool indicando si el input ha sido procesado dentro del elemento.
        - Si devuelve true, el input se ha procesado y no hay que comprobar el resto de elementos de la interfaz.
        - Evita que al hacer click se ejecuten funciones de elementos que estén por debajo de otros.

- `IContainer`
    - Si `UpdateByCursor()` devuelve true para alguno de sus hijos, detiene la ejecución para el resto de hijos.

- `TextView`
    - Ahora permite obtener la fuente usada y su tamaño.

### Assets

- `Font`
    - ***Nuevo***: `GetExistingInstance()` permite obtener una instancia previamente cargada.
    - ***Nuevo***: `ContainsInstance()` permite comprobar si una instancia ha sido previamente cargada.
    - Llamar a `LoadSizedFont()` con un tamaño que ya ha sido cargado no hará nada, en vez de volver a cargar la fuente.

### Graphics

- `IRenderer`
    - ***Nuevo***: `SetPresentMode()` permite cambiar el modo de presentación en tiempo de ejecución.

- `ISwapchain`
    - ***Nuevo***: `SetPresentMode()` permite cambiar el modo de presentación en tiempo de ejecución.
    
### Bugfixes

- **Bugfix**: `UI::ImageView` ya no genera una excepción cuando se intenta renderizar sin haber establecido antes la iamgen (ahora no hace nada).
- **Bugfix**: `Sprite` ya no genera una excepción cuando se intenta establecer un image view nulo.
 

## 2023.11.08a

### Collisions

###### Rework de la detección de colisiones (GJK-Culling).

- `Collider`
    - Ahora permite construirse copiando otro collider.
    
- `ITopLevelCollider` ahora permite obtener una copia.
- `IBottomLevelCollider`:
    - Ahora permite obtener una copia.
    - Hereda `IGjkCollider`.
    - Hereda `ISatCollider`.

- `ConvexVolume`
    - Ahora implementa detección de colisiones mediante *GJK-Clipping*.
    - Ahora permite mergear los vértices y las caras.
    - Ahora permite obtener la cara que contiene una lista de vértices.

- `CollisionInfo`
    - Ahora contiene varios `DetailedCollisionInfo` cuando dos entidades colisionan 
con varios colliders.

- `DetailedCollisionInfo` ahora permite saber la dirección del MTV (respecto a los objetos).

- ***Nuevo***: `Simplex`
    - Grupo de vértices que trata de encerrar el origen de coordenadas.
    - Se genera usando la diferencia de Minkowski de dos volúmenes.
    - Permite saber si dos volúmenes colisionan o no.

- ***Nuevo***: `MinkowskiHull`
    - Volumen 3D que trata de expandirse para ocupar toda la diferencia de Minkowski de dos colliders.
    - Permite conocer el MTV y los vértices que generaron el MTV.

- ***Nuevo***: `IGjkCollider`
    - Interfaz para colliders que soportan detección de colisiones mediante GJK-EPA.

- ***Nuevo***: `GjkSupport`
    - Indica el soporte de un volumen y el vértice que lo produjo.

- ***Nuevo***: `MinkowskiSupport`
    - Indica el soporte de la diferencia de Minkowski de dos volúmenes y los vértices que lo produjeron.

- ***Nuevo***: `ClipFaces()` permite realizar un clipping de dos caras, obteniendo el parche de contacto.

###### Añadido frustum culling.

- `ITopLevelCollider`, `SphereCollider`, `AxisAlignedBoundingBox`
    - Ahora permiten comprobar si están delante o en un plano.
    - Ahora permiten comprobar si están dentro de un frustum.

### Graphics

- `SkyboxRenderSystem`
    - Normalizado el brillo del cubemap a 1.

- `IGpuMemoryAllocator`
    - Ahora genera y es dueño de la textura normal por defecto.
   
- ***Renombrado***: *`PbrDeferredRenderSystem`* -> `DeferredRenderSystem`.

###### Pases de renderizado.

- ***Nuevo***: `IRenderPass`
    - Representa un pase de renderizado de un material en una escena.
    - Almacena las instancias de materiales de los objetos.

- ***Nuevo***: `StaticGBufferPass`
- ***Nuevo***: `AnimatedGBufferPass`
- ***Nuevo***: `BillboardGBufferPass`
- ***Nuevo***: `TreeNormalsRenderPass`
- ***Nuevo***: `TreeGBufferRenderPass`

- ***Nuevo***: `IDeferredResolver`
    - Pase de renderizado para la resolución de **g-buffer*.

- ***Nuevo***: `PbrResolvePass`

- `IRenderSystem`
    - Ahora almacena, maneja y ejecuta una lista de `IRenderPass`.
    
- ***Nuevo***: `MeshMapping`
    - Contiene un registro de los datos de renderizado de los modelos de un pase de renderizado.
    - Contiene una lista de `PerModelData`.
- ***Nuevo***: `PerModelData`
    - Contiene un registro de los datos de renderizado de un modelo en concreto.
    - Contiene una lista de `PerMeshData`.
- ***Nuevo***: `PerMeshData`
    - Contiene un registro de los datos de renderizado de un mesh en concreto.
    - Contiene las instancias de materiales.

###### Mejoras y optimizaciones de efectos de post-procesado.

- `HbaoPass`
    - Ahora permite ejecutar el pase de generación de HBAO en resoluciones distintas a la nativa.
        - Por defecto, se ejecuta a una resolución del 50%.
    - Reajustados parámetros de generación para un mejor resultado.
    - Los pases de desenfoque ahora:
        - Son independientes de la resolución de entrada.
        - Usan un radio del filtro gaussiano de 3.25.
        - Usan interpolación bilineal al procesar la imagen anterior.
        - Usan comprobaciones de resultado para intentar preservar los bordes nítidos.

- *TAA*
    - Ahora usa un filtro gaussiano al obtener el color de la imagen histórica, para un mejor funcionamiento en movimiento.
    - Ahora usa momentos estadásticos (media y desviación típica) para establecer la tolerancia de color.
    - Reducida la cantidad de ghosting en elementos que se mueven rápidamente, disminuyendo la tolerancia de color de elementos con un movimiento muy grande.

###### Añadido frustum culling.

- `PbrDeferredRenderSystem`
    - Ahora realiza frustum culling en los meshes.

- ***Nuevo***: `RenderBoundsRenderer`
    - Renderiza los volúmenes usados para realizar el frustum culling.


### Assets

- `ModelLoader3D`
    - ***Eliminado***: `SetupPbrModel()`.
    - ***Eliminado***: `SetupDefaultNormalTexture()`.
    - ***Eliminado***: `defaultNormalTexture`.

- ***Nuevo***: `UnsupportedMeshProperties`

- `Model3D`
    - Ahora almacena un ID.
    - Ahora permite saber cual es el pase de renderizado que debe renderizarlo.
    - *WIP* Introducidos primeros cambios para un sistema de *LOD*.

- ***Nuevo***: `PreBuiltCollider`.
- ***Nuevo***: `PreBuiltColliderLoader`.

###### Añadido frustum culling.

- Model loading:
    - Ahora genera colliders esféricos por cada mesh.
 
- `Mesh3D`
    - Ahora tiene un collider esférico.
   
### ECS

- `ModelComponent3D`
    - Ya no almacena instancias de material.

###### Añadido frustum culling.

- `CameraComponent3D`
    - Ahora permite obtener los planos de un frustum.
      
### Types

- `Vector3`
    - ***Renombrado***: *Z* -> z.
    - Ahora no puede construirse de manera implícita.
    - Ahora pude comprobarse si es *NaN*.

###### Añadido frustum culling.

- ***Nuevo***: `Plane`
    - Representa un plano en el mundo 3D.
    - Vector normal + punto del plano.

- ***Nuevo***: `AnyFrustum`
    - Representa un frustum con un número indeterminado de planos.

### Bugfixes

- **Bugfix**: `DynamicArray` ahora ejecuta correctamente los destructores de los elementos al destruirse la lista.
- **Bugfix**: `BloomPass::ExecuteSinglePass` ahora lanza el número correcto de hilos.
- **Bugfix**: el shader *TAA* ahora interpola correctamente los colores histórico y nuevo (hasta ahora siempre descartaba el histórico).
- **Bugfix**: cargar los materiales de animación del renderizador en diferido ya no genera errores en las capas de validación.
- **Bugfix**: cargar los materiales de animación del renderizador directo ya no genera errores en las capas de validación.
- **Bugfix**: `CollisionSysyem` ahora genera correctamente un evento por cada colisión, en vez de un evento por colisión por cada pareja de objetos.


## 2023.11.12a

### Assets

- Ahora el caché de *assets* se almacena en cada uno de los *loaders*.

- ***Nuevo***: `AssetOwningRef`
    - Dueño de una instancia de un *asset*.
    - Permite crear `AssetRef`s.
    - Tiene un sistema que cuenta el número de referencias creadas hacia el *asset*.
    - Permite comprobar si está en uso, y eliminar el *asset* si no se está usando.
    
- ***Nuevo***: `AssetRef`
    - Referencia hacia un *asset*
    - Todos los *assets* deben ser referenciados a partir de un `AssetRef`.

### ECS

- ***Nuevo***: `CollisionComponent`
    - Separa la funcionalidad de componente de la clase `Collider`.

 
## 2023.11.15a

### Graphics

- `DeferredRenderSystem`
    - Ahora permite acceder a y actualizar la luz direccional.

###### Añadidos materiales emisores. Añadidos buffers para las características de los materiales.

- `GBuffer`
    - ***Nuevo***: render target `EMISSIVE`.
        - Para los materiales emisivos.
        - Formato: `RGBA8_UNORM`.

- `DeferredPushConstants`
    - Ya no contiene:
        - Índice de rugosidad.
        - Índice de metalicidad.

- ***Nuevo***: `PbrMaterialInfo`
    - Contiene:
        - Índice de rugosidad.
        - Índice de metalicidad.
        - Color emisivo.
        
- `MeshMetadata`
    - Ya no contiene:
        - Índice de rugosidad.
        - Índice de metalicidad.

- `ModelMetadata`
    - Ahora contiene buffers con la información de `PbrMaterialInfo`.

- `PerMeshData`
    - Su material instance ahora contiene los buffers del material.
 
### Assets

###### Añadidos materiales emisores.

- `IGltfLoader`, `StaticMeshLoader`, `AnimMeshLoader`
    - Ahora cargan el color emisor de los materiales.
    
### Bugfixes

- **Bugfix**: `UiContainer` ahora tiene getters de hijos que respetan const-correctnes (eliminando UB).
- **Bugfix**: `UiVerticalContainer` ahora no desplaza los elementos hijos de manera horizontal.

 
## 2023.11.18a

### Graphics

- ***Nuevo***: `PbrIblConfig`
    - Configuración de las intensidades de los mapas IBL en el renderizado.

- `DeferredRenderSystem`
    - Ahora permite actualizar los mapas especular y de irradiancia.
    - Ahora almacena `PbrIblConfig`.
   
- `ShadowMap`
    - Vuelve a usar 4 niveles.

### ECS
 
- `Transform3D`
    - Permite configurar si un transform hereda la posición de su padre.
    - Permite configurar si un transform hereda la rotación de su padre.
    - Permite configurar si un transform hereda la escala de su padre.

### Types

- ***Nuevo***: `Vector3f::One`

### Bugfixes

- **Bugfix**: `GameObjectManager` ahora detecta correctamente si un objeto ha sido registrado.

 
## 2023.11.27a

### Graphics

- `ShadowMap`
    - Vuelve a tener 4 niveles.
    - Ahora permite establecer los splits.

### Bugfixes

- **Bugfix**: ahora `GpuImageAddressMode::BACKGROUND_WHITE` funciona correctamente en la implementación de Vulkan.
- **Bugfix**: ahora `GpuImageAddressMode::BACKGROUND_BLACK` funciona correctamente en la implementación de Vulkan.
- **Bugfix**: los shaders ahora calculan correctamente las coordenadas del mapa de sombras.
- **Bugfix**: los shaders ahora no muestran sombars fuera de los límites del `ShadowMap`.
- **Bugfix**: `PbrIblConfig` ahora tiene el alineamiento correcto.

 
## 2023.12.07a

### Types

- ***Nuevo***: `Spline3D`
    - Representa un segmento o conjunto de segmentos.
    - Definido por puntos 3D.

### Assets

- ***Nuevo***: `PreBuiltSpline3D`
    - Representa un `Spline3D` definido en un archivo externo.

- ***Nuevo***: `PreBuiltSplineLoader3D`
    - Carga un `PreBuiltSpline3D` a partir de un archivo `.gltf`.

### ECS

- `PhysicsComponent`, `PhysicsResolver`
   - Ahora permite establecer la cantidad de fricción.

### Bugfixes

- **Compile time**: ahora `IGltfLoader` no incluye archivos de Windows que incluyen definiciones.

 
## 2023.12.11a

### ECS

- `CollisionSystem`
    - Permite lanzar un rayo a la escena, pudiendo establecer el objeto lanzador para ignorarlo.
 
- `AxisAlignedBoundingBox`
    - Implementada intersección con rayos.
    
- `ConvexVolume`
    - Implementada intersección con rayos.

- `PhysicsComponent`
    - Permite establecer el tensor de inercia.
    
TODO:

fix culling static meshes


## 2023.12.12a

### Graphics

- `RenderPassImageInfo`, `ICommandList`
    - Permite comenzar un pase de renderizado sin limpiar las imágenes de color o profundidad.
    
### Bugfixes

- **Bugfix**: `PreBuiltSplineLoader3D` carga correctamente los vértices del spline en el orden correcto.


## 2023.12.15a

### Graphics

- `RenderTargetAttachment`, `RenderTarget`, `RtRenderTarget`
    - Ahora no tiene una imagen por cada fotograma en vuelo.

### Collision

- `ConvexVolume`
    - Simplificado el proceso de creación de `OOB`.

### Bugfixes

- **Bugfix**: añadir caras a un `ConvexVolume` se refleja instantaneamente al llamar a `GetVertices()` y `GetAxes()`.
 

## 2024.01.24a

### Graphics

###### (WIP) Renderizado bind-less.

- ***Nuevo***: (*WIP*) `GdrDeferredRenderSystem`
    - Implementará un sistema de renderizado bind-less.

###### Atributos de vértices para renderizado bind-less.

- ***Nuevo***: `OSK_VERTEX_ATTRIB(x)`
    - Debe usarse en todos los atributos.

- ***Nuevo***: `VertexPositionAttribute3D`
    - Contiene la posición del vértice.

- ***Nuevo***: `VertexAttributes3D`
    - Contiene los siguientes atributos:
        - Vector normal.
        - Color.
        - Coordenadas de texturas (UV).
        - Vector tangente.

- ***Nuevo***: `VertexAnimationAttributes3D`
    - Contiene los siguientes atributos:
        - Índices de los huesos.
        - Pesos de los huesos.

- ***Nuevo***: `VerticesAttributesMaps`
    - Contiene los atributos de todos los vértices de un modelo 3D.
    - Soporta atributos customizados.

- ***Nuevo***: `GdrVertex3D`
    - Para renderizado bind-less.
    - Únicamente representa el índice en el que se encuentran los atributos.

- `VertexInfo::Entry::Type`
    - ***Nuevo***: `UNSIGNED_INT`.

- `IMaterialSlot`
    - Ahora soporta arrays de recursos.

### Assets

###### Atributos de vértices para renderizado bind-less.

- `Model3D`
    - Ahora contiene los atributos de los vértices en memoria RAM.

- ***Nuevo***: `MaterialMetadata`
    - Contiene:
        - Factor metálico.
        - Factor de rugosidad.
        - Color.

- `IGltfMeshLoader`, `StaticMeshLoader`, `AnimMeshLoader`
    - Ahora cargan los atributos de los modelos 3D en memoria RAM.

### Persistence

- ***Nuevo***: `SerializeJson<>`
     - Función sobreescribible para la serialización de datos en un archivo JSON.

- ***Nuevo***: `DeserializeJson<>`
    - Función sobreescribible para deserializar datos almacenados en un archivo JSON.

- ***Nuevo***: definidas funciones `SerializeJson<>` y `DeserializeJson<>` para:
    - `AxisAlignedBoundingBox`
    - `SphereCollider`
    - `CameraComponent2D`
    - `CameraComponent3D`
    - `Collider`
    - `CollisionComponent`
    - `Transform3D`
    - `ConvexVolume`
    - `ModelComponent3D`
    - `PhysicsComponent`
    - `glm::mat3`
    - `glm::mat4`

    
## 2024.01.26a

### Graphics

- `Format`
    - ***Nuevo***: `UNKNOWN`

- `GpuImageUsage`
    - ***Nuevo***: `UNKNOWN`

### Collision

- `ConvexVolume`
    - ***Nuevo***: `AddOffset`
        - Permite añadir un offset a todos los vértices del volumen en la fase de construcción.

### Bugfixes

- **Bugfix**: `IAudioSource::SetPitch` ahora funciona correctamente para el backend de `OpenAL`.
- **Bugfix**: `IAudioSource::SetGain` ahora funciona correctamente para el backend de `OpenAL`.
- **Bugfix**: `IAudioSource::SetLooping` ahora funciona correctamente para el backend de `OpenAL`.


## 2024.01.29a

### Graphics

- `PipelineCreateInfo`, `MaterialSystem`
    - Ahora permite definir el número máximo de elementos de un array de descriptores.

- `ICommandList`
    - ***Nuevo***: `DrawInstances()`
        - Permite renderizar varias instancias de un mismo modelo.

### Bugfixes

- **Bugfix**: `MaterialSlotVk` ya no crea un descriptor pool 3 veces más grande de lo necesario.
- **Bugfix**: `MaterialSlotVk` ahora es compatible con arrays de recuros.
- **Bugfix**: `MaterialSlotVk` ahora es compatible con arrays de recursos.
- **Bugfix**: `VertexInfo::Entry::Type::UNSIGNED_INT` ahora se puede usar para un único unsigned int.


## 2024.02.11a

### General

- Añadida nueva dependencia: `stb_image_write`.

### Graphics

###### Soporte para capturas de pantalla.

- `ISwapchain`
    - Ahora permite obtener capturas de pantalla y guardarlas en disco.

- ***Nuevo***: `GpuImageTiling`
    - Indica la manera en la que se almacenan los píxeles en memoria.
    - Puede ser:
        - `GpuImageTiling::LINEAL`: se almacenan de manera lineal, por filas.
        - `GpuImageTiling::OPTIMAL`: se almacenan de manera óptima para renderizado.

- `IGpuMemorySubblock`
    - Ahora permite obtener directamente la región de memoria mapeada.
    

## 2024.03.28a

Mejoras de arquitectura:
- Modernización de las arquitecturas relacionadas con modelos 3D.
- Preparación para la implementación de un modelo de ejecución multihilo.

### Graphics

- `Animation`
    - Mejorado encapsulamiento.

- `Animator`
    - Ahora se almacena en el componente ECS `ModelComponent3D`, permitiendo que cada entidad pueda animarse por separado.
    - Ya no contiene las instancias de los materiales de animación.
    - Ya no contiene los buffers con los transforms de los huesos.
    - Ahora permite comprobar si contiene alguna animación.

- `AnimationBone` (antes `MeshNode`)
    - Renombrado.

- `PbrMaterialInfo`
    - Ahora indica si el material contiene textura normal.

- `ShadowMap`
    - Ya no contiene los materiales de generación de sombras y sus instancias.

- ***Nuevo***: `PositionOnlyVertex3D`

###### Rework de modelos 3D.

- ***Nuevo***: `GpuModelUuid`
- ***Nuevo***: `GpuMeshUuid`

- `GpuMesh3D` (antes `Mesh3D`)
    - Renombrado.
    - Ahora usa `GpuMeshUuid`.

- ***Nuevo***: `GpuModel3D`
    - Representa un modelo 3D almacenado en GPU.
    - Almacena los buffers de vértices e índices.
    - Almacena un `VerticesAttributesMaps`.
    - Almacena una lista de `GpuMesh3D`.
    - Puede almacenar, de manera opcional, el `CpuModel3D` a partir del que se ha creado.
    - Almacena una lista de `GpuModel3D::Lod`.
    - Almacena una lista de `GpuModel3D::Material`.
    - Almacena una `GpuModel3D::TextureTable`.

- ***Nuevo***: `GpuModel3D::Lod`
    - Indica los meshes de un modelo que conforman un Lod.

- ***Nuevo***: `GpuModel3D::Material`
    - Indica las propiedades de un material PBR.
    - Incluye:
        - Color base.
        - Color de emisión.
        - Rugosidad.
        - Metalicidad.
        - Índice de textura de color (opcional).
        - Índice de textura normal (opcional).
        - Índice de textura de rugosidad (opcional).
        - Índice de textura de metalicidad (opcional).

- ***Nuevo***: `GpuModel3D::TextureTable`
    - Almacena las texturas usadas por un modelo.

###### Renombre de pases de renderizado, y ampliación a shadow maps.

- ***Eliminado***: `MeshMapping`
- ***Eliminado***: `PerModelData`
- ***Eliminado***: `PerMeshData`

- ***Nuevo***: `GlobalMeshMapping`
    - Contiene la información de un modelo 3D que se comparte con todos los pases de renderizado.
    - Incluye:
        - Matrices de los objetos en el frame anterior.
        - Mapa de `GlobalPerModelData`.

- ***Nuevo***: `GlobalPerModelData`
    - Almacena la información global de un modelo en concreto.
    - Incluye:
        - Instancia de material de animación.
        - Buffer de GPU con los huesos de la animación.

- ***Nuevo***: `GlobalMeshMapping`
    - Contiene la información de un modelo 3D que se comparte con todos los pases de renderizado.
    - Incluye:
        - Matrices de los objetos en el frame anterior.
        - Mapa de `GlobalPerModelData`.

- ***Nuevo***: `LocalMeshMapping`
    - Contiene la información de los modelos 3D usada por un pase de renderizado.
    - Contiene un mapa de `LocalPerModelData`.
    
- ***Nuevo***: `LocalPerModelData`
    - Contiene la información de un modelo 3D en concreto usada por un pase de renderizado.
    - Contiene un mapa de `LocalPerMeshData`.

- ***Nuevo***: `LocalPerMeshData`
    - Contiene la información de una malla 3D en concreto usada por un pase de renderizado.
    - Contiene:
        - Instancia de material de renderizado PBR.
        - Información del material PB en un GPU buffer.

- `IShaderPass` (antes `IRenderPass`)
    - Renombrado.
    - Ahora contiene un `LocalMeshMapping`.
    - También usa un `GlobalMeshMapping`, dado por el sistema de renderizado.

- ***Nuevo***: `IShadowsPass`
    - Pase de renderizado de sombras.

- ***Nuevo***: `ShadowsStaticPass`
    - Implementa `IShadowsPass` para mallas estáticas.
    
- ***Nuevo***: `ShaderPassTable`
    - Almacena los pases de renderizado de un sistema de renderizado.
    - Alamcena los IDs de los objetos compatibles con cada pase de renderizado.

### Assets

- `AssetManager`
    - Ahora permite obtener directamente los loaders.

- ***Nuevo***: `AssetLoaderAlreadyRegisteredException`

- ***Eliminado***: `ModelMobilityType`
- ***Eliminado***: `ModelType`
- ***Eliminado***: `MeshMetadata`
- ***Eliminado***: `MaterialMetadata`
- ***Eliminado***: `ModelMetadata`
- ***Eliminado***: `Model3D::Lod`

- `Model3D`
    - Ya no contiene el mapa de atributos de vértices de un modelo.
    - Ya no contiene el identificador único de un modelo.
    - Ya no contiene los niveles Lod del modelo.
    - Ya no contiene el nombre del pase de renderizado aplicable al modelo.
    - Ya no contiene el animador de un modelo 3D animado.
    - Ahora contiene una instancia de la clase `GpuModel3D`.
    
- `TextureLoader`
    - Ahora permite almacenar referencias `AssetOwningRef<Texture>` cargadas fuera del loader.
    
###### Rework de la carga de modelos.

- ***Eliminado***: `GltfMaterialInfo`
- ***Eliminado***: `GltfModelInfo`

- `GltfLoader` (antes `IGltfLoader`)
    - Ahora carga un modelo almacenado en la CPU (`CpuModel3D`).
    - Actualizado para seguir un modelo de programación funcional.
    - Ya no almacena información intermedia en atributos de la clase.
    
- `StaticMeshLoader`
    - Ahora configura un `GpuModel3D` a partir de un `CpuModel3D`.
- `AnimMeshLoader`
    - Ahora configura un `GpuModel3D` a partir de un `CpuModel3D`.
    
- `PreBuiltSplineLoader3D`
    - Ahora carga el spline a partir de un `CpuModel3D`, cargándolo mediante `GltfLoader`.
- `PreBuiltColliderLoader`
    - Ahora carga el collider a partir de un `CpuModel3D`, cargándolo mediante `GltfLoader`.

###### Nuevas excepciones para la carga de modelos 3D.

- ***Nuevo***: `NoVertexColorFoundException`
- ***Nuevo***: `NoVertexBoneWeightsFoundException`
- ***Nuevo***: `NoVertexBoneIndicesFoundException`

### ECS

- `ModelComponent3D`
    - Ahora contiene el animador del modelo, permitiendo que cada entidad se anime por separado.
    - Ahora contiene los nombres de los pases de renderizado aplicables al modelo.

- ***Nuevo***: `EventAlreadyRegisteredException`

###### Grafos de ejecución y dependencias explícitas entre sistemas.

- ***Eliminado***: `DEFAULT_EXECUTION_ORDER`

- ***Nuevo***: `SystemDependencies`
    - Permite establecer dependencias de ejecución entre sistemas.
    - `executeAfterThese`: sistemas que deben ejecutarse antes.
    - `executeBeforeThese`: sistemas que deben ejecutarse después.

- ***Nuevo***: `SystemExecutionGraph`
    - Contiene un grafo indicando el orden de ejecución de los sistemas, respetando sus dependencias de ejecución.
    - Los sistemas se organizan en sets, de tal manera que se respeten las dependencias.
    - Detecta dependencias cíclicas.
    
- ***Nuevo***: `SystemCyclicDependencyException`
    
- ***Eliminado***: `IPureSystem`
- ***Eliminado***: `IProducerSystem`

- ***Nuevo***: `EventQueueSpan`
    - Representa un rango de eventos.
    - Debe ser casteado al tipo de evento.
 
- `ISystem`
    - ***Eliminado***: `OnTick()`.

- `IConsumerSystem`
    - Ahora debe declara explócitamente el tipo de evento al que está enlazado mediante `GetEventName()`.
    - ***Nuevo***: `OnExecutionStart()`: función que se ejecuta antes de comenzar el procesamiento de eventos.
    - ***Nuevo***: `OnExecutionEnd()`: función que se ejecuta después de finalizar el procesamiento de eventos.
    - El procesamiento ahora se realiza en la función `Execute`.
    - Obtiene los eventos a través de un `EventQueueSpan`.
        - Puede castearse a un span del tipo de evento que sea con la función `BuildSpan()`.
 
- ***Nuevo***: `ITypedConsumerSystem<>`
    - Opción más sencilla para implementar un sistema consumidor.
    - Template class que se construye con el tipo de evento en concreto.
    - Incluye una función `Execute()` con un span ya casteado al tipo de evento.

- `IIteratorSystem`
    - ***Nuevo***: `OnExecutionStart()`: función que se ejecuta antes de comenzar el procesamiento de objetos.
    - ***Nuevo***: `OnExecutionEnd()`: función que se ejecuta después de finalizar el procesamiento de objetos.
    - La ejecución de la lógica ahora ocurre en `Execute()`.
        - Ahora obtiene directamente la lsita de objetos a procesar.

- `IRenderSystem`
    - ***Nuevo***: `OnRenderStart()`: función que se ejecuta antes de comenzar el proceso de renderizado.
    - ***Nuevo***: `OnRenderEnd()`: función que se ejecuta después de finalizar el proceso de renderizado.
    - Ahora permite añadir pases de renderizado de sombras.

- `SystemManager`
    - Ahora usa un `SystemExecutionGraph` para conocer el orden de ejecución de los sistemas.
    - Ahora manda explícitamente un rango de objetos o eventos a los sistemas que lo procesan.
    - Ahora ejecuta los renderizados de los sistemas ECS.

- `EntityComponentSystem`
    - Ahora ejecuta los renderizados de los sistemas ECS.

- `IGame`
    - Ahora ya no ejecuta los renderizados de los sistemas ECS.
    - ***Eliminado***: `OnTick()`.
    - ***Nuevo***: `OnTick_BeforeEcs()`.
    - ***Nuevo***: `OnTick_AfterEcs()`.
    
###### Mini-rework de Transform3D para adaptarlo al futuro sistema de trabajos.

- `Transform3D`
    - Cambiar sus valores (posición, rotación o escala) a través de la interfaz no aplica directamente los cambios, sino que almacena el delta internamente.
    - Actualizar la matriz modelo (`UpdateModel()`) ya no llama a la función `UpdateModel()` de los elementos hijos.
    - Ahora la jerarquía se almacena únicamente hacia abajo:
        - ***Eliminado***: ID del padre.
        - ***Eliminado***: `AttachToObject()`.
        - ***Eliminado***: `Unattach()`.
        - ***Nuevo***: `AddChild()`.
        - ***Nuevo***: `RemoveChild()`.
        - ***Nuevo***: `GetChildren()`.
    - ***Nuevo***: `_ApplyChanges()`: aplica los cambios almacenados durante el frame.

- ***Nuevo***: `TransformApplierSystem`
    - Se encarga de aplicar todos los cambios de los `Transform3D`.
    - Respeta las jerarquías.

### Types

- `DynamicArray`
    - Ahora puede construir un span con todos los elementos del array.
    - Ahora puede construir un span con un subrango de elementos del array.

- `Quaternion`
    - ***Nuevo***: `Empty()`: crea un cuaternión vacío.
    - ***Nuevo***: operadores `+` y `+=`: aplican una rotación sobre el cuaternión.
    - ***Nuevo***: operadores `-` y `-=`: obtienen la diferencia entre dos cuaterniones.

###### Modelos 3D almacenados en memoria RAM.

- ***Nuevo***: `CpuVertex3D`
    - Vértice con los atributos que pueden ser cargados de un archivo.
    - Contiene (todos son opcionales):
        - Posición.
        - Coordenadas de textura.
        - Vector normal.
        - Vector tangente.
        - Color.
        - IDs de huesos.
        - Pesos de huesos.

- ***Nuevo***: `CpuMesh3D`
    - Malla de vértices.
    - Contiene los vértices, así como los índices.
    - Puede diferenciar entre:
        - Índices de triángulos (`TriangleIndices`).
        - Índices de líneas (`LineIndices`).
        - Índices de puntos aislados (`PointIndex`).
    - Puede contener, de manera opcional, un índice de material.

- ***Nuevo***: `CpuModel3D`
    - Modelo almacenado en memoria RAM.
    - Contiene una lista de mallas.
    - Puede contener animaciones, y sus skins.

###### Añadidos UUIDs formales.

- ***Nuevo***: `BaseUuid`
    - Clase template que representa un identificador numérico único.
    - Pueden crearse distintos tipos.
    - Cada tipo es incompatibe con otros tipos.

- ***Nuevo***: `UuidProvider`
    - Genera nuevos UUIDs.
    
### Macros

- `OSK_DEFINE_AS(x)`
    - Ahora define una función `Is<>` que permite saber en tiempo de ejecución si un objeto puede convertirse en un tipo en concreto.

### Bugfixes

- **Bugfix**: `GameObjectManager::IsGameObjectAlive`, `EntityComponentSystem::IsGameObjectAlive` ahora devuelven el valor correcto.
- **Bugfix**: `AabbCollider::GetMin`, `AabbCollider::GetMax` ahora devuelven el valor correcto.
- **Bugfix**: `SystemManager::GetSystem`, `EntityComponentSystem::GetSystem` ahora devuelven *nullptr* si el sistema no está registrado.
- **Bugfix**: las funciones *const* de `Skeleton` que permiten obtener huesos ya no generan bucles infinitos.
- **Bugfix**: `PreBuiltSplineLoader3D` ahora carga correctamente los puntos si previamente se ha cargado un spline con un transform específico.
- **Bugfix**: `VerticesAttributesMap::GetVerticesAttributes` ahora devuelve correctamente una referencia a la lista de atributos.


## 2024.04.16a

Añadido soporte para ejecución multihilo.

### Threading

- ***Nuevo***: `IJob`
    - Interfaz para las operaciones ejecutables de manera paralela.
    - Puede ser lanzado mediante `JobSystem`.
    - Puede contener *tags* (`std::string`).

- ***Nuevo***: `JobSystem`
    - Sistema que es capaz de ejecutar trabajos (`IJob`) de manera paralela.
    - Tiene un *thread-pool*, con un número de hilos que depende del número de núcleos de la CPU.
    - Distribuye los trabajos asignados equitativamente entre los hilos disponibles.
    - Permite esperar a que se completen los trabajos de un tipo dado.
    - Permite esperar a que se completen los trabajos con un *tag* dado.
    - Permite esperar a que se completen todos los trabajos.

- ***Nuevo***: `JobQueue`
    - Representa una cola *thread-safe* que contiene trabajos (`IJob`).
    - Implementada mediante un *ring-buffer* ampliable.

- ***Nuevo***: `JobCountMap`
    - Mantiene el conteo del número de trabajos activos con un *tag* en concreto.
    - Se usa también para los trabajos de un tipo dado, considerando el tipo como un *tag*.

###### Trabajos añadidos:

- ***Nuevo***: `IteratorSystemExecutionJob`
    - Ejecuta la funcionalidad de un sistema iterador para un rango de objetos dado.

- ***Nuevo***: `ConsumerSystemExecutionJob`
    - Ejecuta la funcionalidad de un sistema consumidor para un rango de eventos dado.

- ***Nuevo***: `AssetLoaderJob`
    - Ejecuta el proceso de carga de un asset.

###### Tipos añadidos:

- ***Nuevo***: `AtomicHolder`
    - Permite usar una variable atómica como miemro de una clase sin invaldar sus operaciones de copia y movimiento.
    - La copia tendrá el mismo valor.

- ***Nuevo***: `MutexHolder`
    - Permite usar un mutex como miemro de una clase sin invaldar sus operaciones de copia y movimiento.
    - La copia tendrá un mutex *unlocked*.

###### Clases adaptadas:

Las siguientes clases han sido modificadas para ofrecer su funcionalidad principal de manera parcial o totalmente *thread-safe* sin cambiar su interfaz:
- `IGpuMemoryAllocator`: para todas las asignaciones de memoria y creación de recursos.
- `IGpuMemoryBlock`: toda la clase.
- `ILogger`: para los métodos de escritura.
- `PhysicsComponent`: toda la estructura.
- `UuidProvider`: toda la clase.

### ECS

- `SystemManager`
    - Ahora permite la ejecución de sistemas en paralelo.
    - Los ejecuta en grupos de 5 entidades o eventos.
    - Puede ejecutar varios sistemas a la vez, si están en el mismo grupo de ejecución.
    - Las funciones `OnExecutionStart()` y `OnExecutionEnd()` de los sistemas se ejecutan en un modelo *single-thread*.

- `EntityComponentSystem`
    - `PublishEvent()` ahora es *thread-safe*.
 
- `EventManager`
    - `PublishEvent()` ahora es *thread-safe*.

- `SystemDependencies`
    - Ahora permite indicar si un sistema debe ejecutarse en entorno de un único hilo (por defecto lo hace en modo multi-hilo).
    - Ahora permite indicar si un sistema debe estar en un set de ejecución exclusivo (sin que haya otros sistemas en el set).

- `GameObjectIndex`
    - Ahora es un UUID formal (especialización de `BaseUuid`).

- `EventQueueSpan`
    - Ahora permite obtener un subspan.

- `Transform3D`
    - Añadidas funciones *thread-safe* para añadir posición, rotación o escala.

### Assets

- `AssetManager`
    - Ahora permite cargar assets de manera asíncrona.

- `AssetRef`, `AssetOwningRef`
    - Ahora usan un contador atómico.
    - Ahora permiten saber si el asset ha sido cargado o no (para carga asíncrona).

- ***Nuevo***: `AssetsMap`
    - Encapsula el caché de un loader.
    - Almacena los assets cargados, y los enlaza con la ruta.
    - Creado para ser *thread-safe*.
    
- `IAssetLoader`
    - La función `Load(const std::string&, void*)` ha sido dividida en varias, para permitir registrar un asset sin cargarlo directamente (para carga asíncrona):
        - ***Nuevo***: `void FullyLoad(const std::string&, void*)`
            - Realiza todo el proceso de carga.
            - Equivalente a la función antigua `Load(const std::string&, void*)`.
        - ***Nuevo***: `void RegisterWithoutLoading(...)`
            - Registra un asset (introduciendo el `AssetOwningRef` al mapa) sin cargarlo.
        - ***Nuevo***: `void Load(...)`
            - Carga un archivo previamente registrado.
            - Puede llamarse en un hilo distinto al que lo ha registrado.
        - ***Nuevo***: `void SetAsLoaded(...)`
            - Marca el asset como completamente cargado.
        - Se ha actualizado el macro `OSK_DEFAULT_LOADER_IMPL(x)` para implementar todas estas funciones.

### Graphics

- `ISwapchain`
    - Mejorado encapsulamiento.
    - Incrementado *dependency injection*.

###### Adaptado para ejecución multihilo.

- ***Nuevo***: `ThreadedCommandPoolMap`
    - Mapa que asigna un `ICommandPool` a cada hilo.
    - Necesario para poder grabar comandos en hilos distintos.

###### Colas GPU.

- ***Nuevo***: `GpuQueueType`
    - Indica los tipos de colas GPU usados por el motor:
        - `MAIN`
            - Usada para renderizado general.
        - `PRESENTATION`
            - Usada para presentación de imágenes por pantalla.
        - `ASYNC_TRANSFER`
            - Cola exclusiva de transferencia.

- ***Nuevo***: `CommandsSupport`
    - Indica los tipos de comandos soportados por una lista de comandos, un pool de comandos o una cola de comandos.
    - Incluye (se pueden mezclar):
        - `GRAPHICS`
        - `COMPUTE`
        - `TRANSFER`
        - `PRESENTATION`

- `ICommandPool`
    - Ahora permite conocer los comandos que soporta (`CommandsSupport`).
    - Ahora permite cononcer la cola sobre la que se deben insertar las listas creadas por el pool (`GpuQueueType`).

- `ICommandQueue`
    - Ahora almacena información sobre la familia a la que pertenece.
    - Ahora almacena el índice de la cola dentro de la familia.
    - Ahora permite conocer los comandos que soporta (`CommandsSupport`).

- `IGpu`
    - Ahora permite crear un command pool de transferencia.

- `IGpuImage`
    - Ahora almacena una referencia a la cola GPU que la posee.

- `GpuBufferRange`
    - Representa un rango de información de un buffer.
 
- `GpuBuffer`
    - Ahora almacena una referencia a la cola GPU que lo posee.
    - Ahora almacena información sobre la última barrera GPU que se le ha aplicado.
    - Ahora permite obtener un rango del buffer.

- ***Nuevo***: `ResourceQueueTransferInfo`
    - Permite indicar una transferencia de la propiedad de un recurso de una cola a otra.

- `ICommandList`
    - `SetGpuImageBarrier(...)` ahora permite definir una transferencia del recurso a otra cola.
    - ***Nuevo***: `SetGpuBufferBarrier(...)`
        - Permite añadir una barrera de ejecución que afecta a un buffer.
        - Permite definir una transferencia del recurso a otra cola.
    -  ***Nuevo***: `TransferToQueue(...)`
        - Permite definir una transferencia de un buffer o imagen a otra cola.
    - Ahora almacena una referencia a la cola sobre la que se tiene que enviar.

- `GpuImageCreateInfo`
    - Ahora permite establecer el tipo de cola que poseerá el recurso.

- `IGpuMemoryAllocator`
    - `CreateCubemapImage(...)`, `CreateVertexBuffer(...)`, `CreateIndexBuffer(...)`, `CreateUniformBuffer(...)`, `CreateStorageBuffer(...)`, `CreateStagingBuffer(...)`, `CreateBuffer(...)` ahora permiten establecer la cola que posee el recurso inicialmente.
    - `CreateVertexBuffer(...)` ahora tiene una versión que permite indicar una cola de transferencia y una de propiedad final.
    - `CreateIndexBuffer(...)` ahora tiene una versión que permite indicar una cola de transferencia y una de propiedad final.

- `IRenderer`
    - Mejorado encapsulamiento.
    - `CreateSingleUseCommandList(...)` ahora permite indicar la cola sobre la que se crea.
    - ***Nuevo***: `CreateCommandPool(...)`
    - Ahora usa varias colas:
        - Cola unificada: cola que incluye renderizado principal y presentación.
        - Cola principal de renderizado (para cuando no existe cola unificada).
        - Cola de presentación (para cuando no existe cola unificada).
        - ***Nueva***: cola de transferencia exclusiva.
    - Permite obtener la cola óptima para una operación en concreto.
    - Almacena varios mapas de command pools (`ThreadedCommandPoolMap`), uno por cada tipo de cola soportado.

- `RendererVk`
    - Eliminados semáforos innecesarios.

### Types

- ***Nuevo***: `StopWatch`
    - Permite medir un intervalo de tiempo.
   
- `Uuid`
    - ***Nuevo***: `OSK_DEFINE_UUID_FORMATTER(x)`
        - Permite usar un `Uuid` de un tipo específico dentro de `std::format(x)`.

### Bugfixes

- **Bugfix**: `OwnedPtr<GpuBuffer> CreateVertexBuffer()` (sobrecarga con lista de vértices) ahora usa correctamente el valor del parámetro `usage`.


## 2024.04.29a

### Persistence

- ***Eliminado***: `DataNode`
- ***Eliminado***: `TDataType`
    - ***Eliminado***: `DATA_TYPE_UNKNOWN`
    - ***Eliminado***: `DATA_TYPE_STRING`
    - ***Eliminado***: `DATA_TYPE_INT`
    - ***Eliminado***: `DATA_TYPE_FLOAT`
    - ***Eliminado***: `DATA_TYPE_RESERVED_MAX`

- Implementada serialización y deserialización para:
    - `Color`
    - `PbrIblConfig`
    - `DirectionalLight`
    - `ShadowMap`

### Bugfixes

- **Bugfix**: `SerializeJson<ModelComponent3D>(...)` y `DeserializeJson<ModelComponent3D>(...)` ahora funcionan correctamente.
- **Bugfix**: `SerializeJson<Transform3D>(...)` y `DeserializeJson<Transform3D>(...)` ahora procesan correctamente la jerarquía de objetos.
- **Bugfix**: `GameObjectManager::CreateGameObject()` ahora no asigna de manera incorrecta identificadores inválidos.
- **Bugfix**: `Transform3D` ahora aplica correctamente las rotaciones.
- **Bugfix**: `EntityComponentSystem::DestroyObject(...)` ahora llama correctamente a `SystemManager::GameObjectDestroyed(...)`.
- **Bugfix**: `RendererVk` ahora da prioridad a las GPUs dedicadas.
- **Bugfix**: `SystemManager::OnTick(...)` ahora ignora correctamente los sistemas desactivados.
- **Bugfix**: `SystemManager::OnRenderer(...)` ahora ignora correctamente los sistemas desactivados.


## 2024.05.18a

### Persistence

- ***Eliminado***: `SerializeJson`, `DeserializeJson`

- ***Nuevo***: `SerializeComponent`, `DeserializeComponent`
    - Operaciones de serialización y deserialización en formato JSON.
- ***Nuevo***: `SerializeData`, `DeserializeData`
    - Operaciones de serialización y deserialización en formato JSON.

- ***Nuevo***: `BinarySerializeComponent`, `BinaryDeserializeComponent`
    - Operaciones de serialización y deserialización en formato binario.
- ***Nuevo***: `BinarySerializeData`, `BinaryDeserializeData`
    - Operaciones de serialización y deserialización en formato binario.

- Implementadas operaciones de serialización y deserialización (formatos JSON y binario) para:
    - `AxisAlignedBoundingBox`
    - `SphereCollider`
    - `Collider`
    - `Color`
    - `ConvexVolume`
    - `IblConfig`
    - `DirectionalLight`
    - ``ShadowMap``
    - `Vector2` (todos los tipos)
    - `Vector3` (todos los tipos)
    - `Vector4` (todos los tipos)
    - `Quaternion`
    - `glm::mat3`, `glm::mat4`
    - Todos los componentes ECS.

- ***Nuevo***: `BinaryBlock`
    - Permite escribir datos, que se almacenan en un formato binario.

- ***Nuevo***: `BinaryBlockReader`
    - Permite leer datos de un bloque binario.
    - Contiene un cursor interno.
   
### ECS

- `SavedGameObjectTranslator`
    - Permite obtener el nuevo índice de una entidad que ha sido guardada en disco.

- `EntityComponentSystem`
    - Ahora permite guardar el estado del juego (formatos JSON y binario).
    - Ahora permite cargar el estado del juego (formatos JSON y binario).

- `SystemManager`
    - Ahora permite desactivar todos los sistemas.
    - Ahora permite obtener un sistema por su nombre.
    - Ahora permite obtener el grafo de ejecución.

- `ComponentManager`
    - Ahora permite obtener todos los contenedores.
    - Ahora permite obtener un contenedor por el nombre del tipo de componente.

- `IComponentContainer`, `ComponentContainer<T>`
    - Ahora permite obtener el nombre del tipo de componente que almacena.
    - Ahora permite serializar todos los componentes (formatos JSON y binario).
    - Ahora permite deserializar y añadir componentes individuales (formatos JSON y binario).
    
- ***Eliminado***: `Scene` (no se usaba)
- ***Eliminado***: `SystemPriorityQueue` (no se usaba)

### Graphics

- ***Nuevo***: `ShaderPassFactory`
    - Permite crear un pase de shader a partir de su nombre
    - El tipo de shader debe ser registrado previamente.

- `IRenderer`
    - Ahora contiene un `ShaderPassFactory`.

- `ShaderPassTable`
    - ***Nuevo***: `RemoveAllPasses()`

### IO

- `FileIO`
    - ***Nuevo***: `WriteBinaryFile(...)`
        - Permite escribir archivos binarios.

### Error Handling

- ***Nuevo***: `FinishedBlockReaderException`
- ***Nuevo***: `InvalidBinaryDeserializationException`

### Bugfixes

- **Bugfix**: `EntityComponentSystem::DestroyObject(...)` ahora elimina correctamente los componentes asociados al objeto eliminado.


## 2024.05.29a

###### Añadido editor 3D.

### General

- ***Nuevo***: `IDebugGame`
    - Hereda de `IGame`.
    - Para ayudar con tareas de debug/desarrollo.
    - Incluye la funcionalidad de:
        - Editor 3D.
        - Consola de comandos.

- Dividido el archivo `OSKmacros.h` en:
    - `ApiCall.h`: para `OSKAPI_CALL`.:
    - `NumericTypes.h`: para tipos numéricos.
    - `DefineAs.h`: para `OSK_DEFINE_AS(...)`.
    - `DefineConstructors.h`: para `OSK_DISABLE_COPY(...)`, `OSK_DISABLE_MOVE(...)`, `OSK_DEFAULT_COPY_OPERATOR(...)`, `OSK_DEFAULT_MOVE_OPERATOR(...)`.
    - `DefineIuuid.h`: para `OSK_DEFINE_IUUID(...)`, `OSK_IUUID(...)`.
    - `VulkanTypedefs.h`: para `OSK_VULKAN_TYPEDEF(...)`, `OSK_VULKAN_FLAGS_TYPEDEF(...)`.

### UI

- `UI::IElement`
    - Ahora permite actualizarse por teclado.

- `UI::TextView`
    - Ahora hereda de `UI::ImageView`, por lo que se puede configurar una imagen de fondo.

- ***Nuevo***: `UI::TextInput`
    - Permite introducir texto mediante teclado.
    - Permite establecer un texto por defecto (placeholder).

- ***Nuevo***: `UI::BorderLayout`
    - Divide el espacio en 5 áreas (norte, sur, este, oeste, centro).

- ***Nuevo***: `UI::Console`
    - Consola debug del motor.
    - Permite añadir mensajes.
    - Permite ejecutar comandos.
    - Permite ver los últimos mensajes añadidos.
    - Disponible por defecto si se hereda la clase `IDebugGame`.

### ***Nuevo***: Console commands

- ***Nuevo***: `ConsoleCommand`
    - Representa un comando ejecutable por consola.
    - Permite definir el texto del comando.
    - Permite definir un callback que se ejecutará, con unos parámetros (pasados por consola).

- ***Nuevo***: `ConsoleCommandExecutor`
    - Permite ejecutar comandos a partir de un mensaje de consola.
    - Se deben registrar los tipos de comandos.
    
- ***Nuevo***:
    - `GpuStatsCommand`
        - Muestra por consola la información del consumo de memoria VRAM.

- ***Nuevo***:
    - `EditorShowCommand`, `EditorHideCommand`
        - Permite mostrar o quitar el editor.

- ***Nuevo***:
    - `ExitCommand`
        - Sale del juego.

### ***Nuevo***: Editor

###### Editor 3D que muestra información de la escena.

- ***Nuevo***: `EditorUi`
    - Clase principal de la interfaz del editor.
    - Contiene dentro los elementos de interfaz.

- ***Nuevo***: `EditorHeader`
    - Barra superior de la interfaz del editor.
    - Contiene un logo.

- ***Nuevo***: `EditorBottomBar`
    - Barra inferior de la interfaz del editor.
    - Contiene:
        - Icono.
        - Versión del motor.
        - Número de hilos.
        - Nombre de la GPU.
        - FPS.
        
- ***Nuevo***: `Editor::ObjectList`
    - Barra derecha del editor.
    - Contiene una lista con los objetos de la escena.
    - Contiene un panel de propiedades (`Editor::PropertiesPanel`) que se actualiza para mostrar los componentes del objeto seleccionado.
    
- ***Nuevo***: `Editor::SystemList`
    - Barra izquierda del editor.
    - Contiene una lista con los sistemas de la escena.
    - Contiene un panel de propiedades (`Editor::PropertiesPanel`) que se actualiza para mostrar las propiedades del sistema seleccionado.
        - Los datos son los obtenidos al serializar el sistema en formato JSON.

- ***Nuevo***: `Editor::PropertiesPanel`
    - Muestra las propiedades de un sistema o los componentes de un objeto.

- ***Nuevo***: `EditorPanelTitle`
    - Título de un panel del editor.
    - Tiene un subrayado azul.

### ECS

- ``ComponentContainer``
    - Ahora almacena el nombre de cada tipo de componente registrado.

- `EntityComponentSystem`
    - Ahora permite obtener todos los objetos vivos.
    - Ahora permite obtener todos los tipos de componentes que posee un objeto.
    - Ahora permite obtener el nombre de cada tipo de componente registrado.
    
### IO

- ***Eliminado***: `Console`

- ***Nuevo***: `PrintableKeys`
    - Array que contiene todas las teclas (`IO::Key`) que se corresponden con caracteres que se pueden imprimir.

- ***Nuevo***: `GetPrintableKeyText`
    - Devuelve el carácter que se corresponde con una tecla (`IO::Key`).
    - Puede ser mayúscula.

### Graphics

- `IGpu`
    - Ahora almacena su nombre.

### Threading

- `JobSystem`
    - Ahora permite obtener el número de hilos usados.

### Bugfixes

- **Bugfix**: `Font::LoadSizedFont(...)` ahora carga de manera correcta todos los caracteres.
- **Bugfix**: `ImageView::Render(...)` ahora renderiza correctamente usando su tamaño (ignorando padding).
- **Bugfix**: mejorado *const-correctnes* de: 
    - `UniquePtr`
    - `ShaderPtr`
    - `CameraComponent2D`
    - `CubemapTexture`
    - `EntityComponentSystem`
    - `IGpuMemoryAllocator`
    - `TerrainComponent`
    - `Texture`
    - `RtRenderTarget`
    - `SystemManager`


## 2024.06.18a

### Graphics

- `IMaterialSlot`
    - Ahora almacena los descriptores de un único recurso (en vez de los tres recursos en vuelo).
        - De esta manera se normaliza con los demás elementos de renderizado, que se deben desarrollar especificando explícitamente los recursos en vuelo a usar.

- `GpuBuffer`, `IGpuMemorySubblock`
    - Ahora permite obtener el cursor usado al escribir.

- ``GpuBufferRange``
    - Ahora permite indicar si representa todo el buffer.

- ***Eliminado***: `TreeGBufferPass`
- ***Eliminado***: `TreeNormalsPass`

- ``IPostProcessPass``
    - Ya no almacena el material principal del pase.
    - Ya no almacena la instancia de material principal del pase.
    - Ya no almacena la imagen de entrada, ni su view.
    - ***Eliminado***: `SetInput(...)`, `SetInputTarget(...)` (se deberán crear las funciones para establecer las imágenes de entrada en las clases implementadoras).

- ***Nuevo***: `PostProcessInputNotSetException`

- `ICommandList`
    - Ahora permite obtener el `Viewport` establecido en un momento dado.

- `RendererVk`
    - Ya no ignora ciertos mensajes de validation layers en modo debug.
    
- `MaterialSlotVk`
    - Optimizado: ahora `FlushUpdate()` sólamente actualiza los recursos que hayan cambiado.

- `IPipelineVk`
    - ***Nuevo***: `CreateShaderModule(...)`
        - Centraliza la creación de un shader module en una función, para ser usada por todos los tipos de pipeline de Vulkan.

###### Mejoras en el manejo de alineamientos de memoria VRAM.

- ***Nuevo***: `GpuMemoryAlignments`
    - Indica las necesidades de alineamiento de memoria VRAM de una GPU para los siguientes recursos:
        - Buffer de vértices.
        - Buffer de índices.
        - Uniform buffer.
        - Storage buffer.

- `IGpu`
    - Ahora permite obtener las necesidades de alineamiento de memoria VRAM (`GpuMemoryAlignments`).
    - Ahora permite comprobar si es compatible con ray-tracing.
    - Ahora permite comprobar si es compatible con renderizado bind-less.

- `IGpuMemoryAllocator`
    - Ahora permite obtener (públicamente) las necesidades de alineamiento de un buffer GPU.
    - Ya no almacena localmente las necesidades de alineamiento (ahora las obtiene a partir de la GPU).
    - ***Nuevo***: `GPU_MEMORY_NO_ALIGNMENT`
        - Indica que un recurso no tiene ninguna necesidad especial de alineamiento (a parte de las propias del tipo de recurso).

###### Añadido soporte para renderizado bind-less.

- ***Nuevo***: `MAX_BINDLESS_RESOURCES = 4096`
    - Indica el tamaño máximo de un array de recursos en modo bind-less.

- `DescriptorPoolVk`
    - Ahora soporta recursos bind-less.
    - Ahora permite a un `MaterialSlotVk` actualizar recursos (`FlushUpdate()`) tras haberse enlazado.

###### Desarrollo de un sistema de renderizado 2D que combine renderizado de imágenes con Signed-Distance Fields (SDF).

- ***Nuevo***: `SdfDrawCallContentType2D`
    - Indica el tipo de contenido de una llamada de renderizado 2D:
        - Textura.
        - Color plano.
        - Degradado de dos colores.

- ***Nuevo***: `SdfShape2D`
    - Indica la fomra de una llamada de renderizado 2D:
        - Rectángulo.
        - Círculo.

- ***Nuevo***: `SdfDrawCall2D`
    - Contiene la información de una llamada de renderizado 2D:
        - Transform.
        - Forma (`SdfShape2D`).
        - Tipo de contenido (`SdfDrawCallContentType2D`).
        - Si está relleno o hueco por dentro.
        - Tamaño del borde (si está hueco).
        - Textura y sus coordenadas (si el tipo de contenido es `SdfDrawCallContentType2D::TEXTURE`).
        - Color principal.
        - Color secundario (para gradientes de color).

- ***Nuevo***: `SdfStringInfo`
    - Contiene la información de una llamada de renderizado de texto:
        - Transform.
        - Texto.
        - Fuente.
        - Color.

- ***Nuevo***: `SdfBindlessBatch2D`
    - Agrupa varias llamadas de renderizado 2D (`SdfDrawCall2D`).
    - También indica el material a usar para su renderizado.

- ***Nuevo***: `SdfGlobalInformationBufferContent2D`
    - Contiene la información global compartida por todas las llamadas de renderizado 2D:
        - Matriz modelo de la cámara.
        - Resoulción del target de renderizado.

- ***Nuevo***: `SdfBindlessBufferContent2D`
    - Contiene la información de una llamada de renderizado 2D (`SdfDrawCall2D`) formateada para su uso desde la GPU.

- ***Nuevo***: `SdfBindlessRenderer2D`
    - Implementa un renderizador de llamas SDF que usa un diseño bind-less junto a renderizado instanciado.
    - Agrupa el renderizado en batches de 512 llamadas que se condensan en una.

### UI

- `UI::IElement`
    - Ahora contiene una lista de llamdas de renderizado SDF 2D (`SdfDrawCall2D`).
    - Ahora almacena la posición global, en vez de la posición relativa.
    - Permite recolocar y reescalar todas las llamadas `SdfDrawCall2D`, para que se alineen con el tamaño y posición del elemento.
    - ***Nuevo***: `OnPositionChanged(...)`.
        - Se ejecuta cada vez que se cambia la posición del elemento.
        - Por defecto, recoloca las llamadas de renderizado `SdfDrawCall2D` para que sigan teniendo la misma posición con respecto al elemento.
    - ***Nuevo***: `OnSizeChanged(...)`.
        - Se ejecuta cada vez que se cambia la posición del elemento.
        - Por defecto, y si se debe mantener la escala relativa respecto al padre, reescala las llamadas de renderizado `SdfDrawCall2D` para que sigan teniendo el mismo tamaño con respecto al elemento.
    - ***Nuevo***: `RepositionDrawCalls(...)`
        - Añade un offset a la posición de todas las llamadas `SdfDrawCall2D` del elemento.
    - ***Nuevo***: `ResizeDrawCalls(...)`
        - Multiplica el tamaño de todas las llamadas `SdfDrawCall2D` del elemento.


- `UI::IContainer`
    - Ahora usa `UniquePtr<>` para almacenar los elementos hijos (en vez de `SharedPtr<>`).
    - Ahora usa correctamente `std::string_view` en los getters de los elementos.

- `UI::TextView`
    - Ahora hereda directamente de `UI::IElement`.

- `UI::ImageView`
    - Ya no contiene un `Sprite`, la imagen se puede establecer mediante una llamada de renderizado SDF 2D.

- `UI::Button`
    - Ya no permite obtener sprites de cada estado del botón.
    - Ahora permite añadir llamadas de renderizado SDF 2D para cada estado del botón.

### ECS

- ***Eliminado***: `TreeNormalsRenderSystem`

### Bugfixes

- **Bugfix**: `IGpuMemoryAllocator::GetAlignment(...)` ahora devuelve el valor correcto.
- **Bugfix**: `IGpuMemoryBlock::GetNextMemorySubblock(...)` ahora respeta las alineaciones de memoria correspondientes.
- **Bugfix**: `IGpuMemoryAllocator::CreateBuffer(...)`, `IGpuMemoryAllocator::CreateVertexBuffer(...)`, `IGpuMemoryAllocator::CreateIndexBuffer(...)`, `IGpuMemoryAllocator::CreateUniformBuffer(...)`, `IGpuMemoryAllocator::CreateStorageBuffer(...)`, `IGpuMemoryAllocator::CreateStagingBuffer(...)` ahora respetan las alineaciones de memoria correspondientes.


## 2024.08.15a

### Editor

- ***Nuevo***: `Editor`
    - Clase principal del editor.
	- Maneja la interfaz de usuario y su conexión con el juego.
    - Contiene la interfaz de usuario del editor (`EditorUi`).
    - Contiene los controladores de componentes ECS del editor (`IComponentController`) y sus métodos de factoría.
    - Contiene los métodos de factoría de las vistas de componentes ECS.

- `ObjectList`
    - Ahora contiene un `ObjectPropertiesPanel` en vez de un `PropertiesPanel` para mostrar los componentes de los objetos.

- `SystemList`
    - Ahora contiene un `SystemPropertiesPanel` en vez de un `PropertiesPanel` para mostrar las propiedades de un sistema.
   
- `PropertiesPanel`
    - Ya no contiene la funcionalidad para mostrar los componentes de los objetos.
    - Ya no contiene la funcionalidad para mostrar las propiedades de un sistema.
    
- ***Nuevo***: `ObjectPropertiesPanel`
    - Implementa la funcionalidad para mostrar los componentes de los objetos.

- ***Nuevo***: `SystemPropertiesPanel`
    - Implementa la funcionalidad para mostrar las propiedades de un sistema.

- ***Nuevo***: `Constants::EditorFontPath`
- ***Nuevo***: `Constants::MainFontSize`
- ***Nuevo***: `Constants::SecondaryFontSize`

###### Implementada una arquitectura component-controller-view para poder interactuar con los componentes de un objeto mediante el editor.

- ***Nuevo***: `IComponentController`
    - Elemento que conecta un componente ECS con una vista que muestra su estado por pantalla.
- ***Nuevo***: `TComponentController`
    - Clase template que contiene métodos auxiliares para obtener el componente en cuestión.
- ***Nuevo***: `IComponentView`
    - Elemento de interfaz de usuario del editor que muestra las propiedades de un componente.
    
- ***Nuevo***: `CameraComponentController3D`
- ***Nuevo***: `PhysicsComponentController3D`
- ***Nuevo***: `TransformComponentController3D`
- ***Nuevo***: `ModelComponentController3D`
    
- ***Nuevo***: `CameraComponentView3D`
- ***Nuevo***: `PhysicsComponentView`
- ***Nuevo***: `TransformComponentView3D`
- ***Nuevo***: `ModelComponentView3D`

### UI

- `UI::IContainer`
    - ***Nuevo***: `DeleteChild(...)` ahora permite eliminar un hijo en concreto.

### Types

- `Vector2`
    - ***Nuevo***: `TNumericType`
        - Representa el tipo numérico usado en el vector.
- `Vector3`
    - ***Nuevo***: `TNumericType`
        - Representa el tipo numérico usado en el vector.
- `Vector4`
    - ***Nuevo***: `TNumericType`
        - Representa el tipo numérico usado en el vector.
    - *Renombrado*: `Z` -> `z`.
    - *Renombrado*: `W` -> `w`.

### STD

- `DynamicArray`
    - *Renombrado*: `CreateReservedArray(...)` -> `CreateReserved(...)`.
    - *Renombrado*: `CreateResizedArray(...)` -> `CreateResized(...)`.
    - *Renombrado*: `CreateResizedArrayMove(...)` -> `CreateResizedMove(...)`.
    - ***Nuevo***: `CreateEmpty()`.


## 2024.08.17a

### UI

- `IElement`
    - Ahora contiene un puntero (opcional) al elemento padre.

- ***Nuevo***: `CollapsibleWrapper`
    - Contiene un único elemento que puede contraerse y expandirse pulsando un botón superior.

### Editor

###### Views de componentes contraíbles y botones para la creación y eliminación de componentes.

- ``IComponentView``
    - Ahora hereda de `CollapsibleWrapper`, permitiendo contraer la información mostrada de un componente.
    
- `ObjectList`
    - Ahora permite crear nuevos objetos.

- `ObjectPropertiesPanel`
    - Ahora permite eliminar el objeto seleccionado.

- ***Nuevo***: `Constants::DefaultGreenColor`
- ***Nuevo***: `Constants::HoveredGreenColor`
- ***Nuevo***: `Constants::SelectedGreenColor`
- ***Nuevo***: `Constants::DefaultRedColor`
- ***Nuevo***: `Constants::HoveredRedColor`
- ***Nuevo***: `Constants::SelectedRedColor`

### Types

- `Color`
    - ***Nuevo***: `FromBytes(...)`
        - Permite crear una instancia proporcionando los valores en rango 0-255, en vez de 0-1.

### Bugfixes

- **Bugfix:** `IContainer::AdjustSizeToChildren()` ya no tiene en cuenta los elementos invisibles.
- **Bugfix:** `IContainer::Rebuild()` ahora también reconstruye sus hijos (si también son contenedores).
- **Bugfix:** `Editor::ObjectList` ahora coloca correctamente sus elementos.
- **Bugfix:** `Editor::SystemList` ahora coloca correctamente sus elementos.


## 2024.08.24a

### ECS

###### Adaptados sistemas para su uso en el editor, permitiendo obtener información adicional.

- `IRenderSystem`
    - Ahora permite obtener todos los pases de renderizado.
    - Ahora permite obtener todos los pases de renderizado de sombras.
    
- `DeferredRenderSystem`
    - Ahora permite obtener el objeto de la cámara.
    - Ahora permite obtener el mapa de irradiancia.
    - Ahora permite obtener el mapa especular.
    - Ahora permite obtener el nombre del pase de resolución.
    
- `ColliderRenderer`
    - Ahora permite obtener el objeto de la cámara.
    - Ahora permite obtener los materiales de renderizado (de alto nivel, de bajo nivel y de puntos).

- `RenderBoundsRenderer`
    - Ahora permite obtener el objeto de la cámara.
    - Ahora permite obtener el materiale de renderizado.

- `PhysicsSystem`
    - Ahora permite obtener la gravedad.

### UI

- `IContainer`
    - ***Nuevo***: `HasChild(...)`

- `Button`
    - Ahora permite establecer el anclaje del texto.

### Editor

- Mejorada encapsulación de algunos elementos:
    - `PropertiesPanel`
    - `PanelTitle`
    - `ObjectList`
    - `SystemList`
    
- ***Nuevo***: `Constants::TitleFontSize`
- ***Nuevo***: `Constants::SubtitleFontSize`

- `TransformComponentView3D`, `PhysicsComponentView`:
    - Ahora las coordenadas muestran 'X', 'Y' y 'Z'.

###### Añadidos vistas y controladores para sistemas.

- *Renombrado*: `ControllerFactoryMethod` -> `ComponentControllerFactoryMethod`
- *Renombrado*: `ViewFactoryMethod` -> `ComponentViewFactoryMethod`

- ***Nuevo***: `SystemControllerFactoryMethod`;
- ***Nuevo***: `SystemViewFactoryMethod`;

- ``Editor``
    - Ahora contiene los métodos de factoría (y permite registrarlos) de:
        - Controladores de sistemas (`SystemControllerFactoryMethod`).
        - Vistas de sistemas (`SystemViewFactoryMethod`).
    - Ahora permite deseleccionar el objeto seleccionado.
    - Ahora permite deseleccionar el sistema seleccionado.

- `EditorUi`
    - Ahora permite establecer el sistema seleccionado.

- `SystemList`
    - Ahora permite establecer el nombre del sistema seleccionado.
 
- `SystemPropertiesPanel`
    - Ahora no muestra automáticamente la información del sistema a partir de su método de serialización JSON.
    - Ahora permite establecer la vista del sistema seleccionado.
    - Ahora permite establecer el sistema seleccionado, incluso si no tiene asociado una vista.
    - Ahora contiene un botón que permite cambiar el estado del sistema seleccionado (activo o desactivado).

- ***Nuevo***: `ISystemController`
    - Elemento que conecta un sistema ECS con una vista que muestra su estado por pantalla.
- ***Nuevo***: `TSystemController`
    - Clase template que contiene métodos auxiliares para obtener el sistema en cuestión.
- ***Nuevo***: `ISystemView`
    - Elemento de interfaz de usuario del editor que muestra las propiedades de un sistema ECS.
    
- ***Nuevo***: `ColliderRenderSystemController`
- ***Nuevo***: `RenderBoundsRenderSystemController`
- ***Nuevo***: `DeferredRenderSystemController`
- ***Nuevo***: `SkyboxRenderSystemController`
- ***Nuevo***: `PhysicsSystemController`
    
- ***Nuevo***: `ColliderRenderSystemView`
- ***Nuevo***: `RenderBoundsRenderSystemView`
- ***Nuevo***: `DeferredRenderSystemView`
- ***Nuevo***: `SkyboxRenderSystemView`
- ***Nuevo***: `PhysicsSystemView`

### Otros

- Ahora por defecto los archivos de recursos y los archivos de descripción de recursos se almacenan en el mismo directorio.

### Bugfixes

- **Bugfix**: ahora el título de los paneles de propiedades, lista de objetos y lista de sistemas está correctamente alineado.
- **Bugfix**: `ModelComponentController3D` ya no actualiza los valores de la vista si no han cambiado.


## 2024.09.12a

- Introducidos pequeños cambios para facilitar el desarrollo multiplataforma.
- Añadidos errores en caso de llegar a código teóricamente inalcanzable.


## 2024.12.05a

### ***Nuevo*** Plataformas

- Ahora se establece la plataforma de destino en tiempo de compilación. Opciones disponibles:
    - `OSK_WINDOWS`
    - `OSK_ANDROID` (no implementado).
    - `OSK_LINUX` (no implementado).

### ***Nuevo*** Perfiles de contenido

- ***Nuevo***: `DefaultContentProfile`  
    - Permiten indicar qué elementos por defecto (componentes ECS, sistemas ECS y loaders) se deben cargar al iniciar el juego.
    - Disponibles:
        - `DefaultContentProfile::ALL`.
        - `DefaultContentProfile::_3D_ONLY`.
        - `DefaultContentProfile::_2D_ONLY`.
        - `DefaultContentProfile::MINIMAL`.

- `Game`
    - Ahora se debe indicar el perfil de contenido a usar.

- `Engine`
    - Ahora se debe indicar el perfil de contenido a usar.
    - Ahora carga los componentes ECS, sistemas ECS y loaders indicados por el perfil de contenido.

### Graphics

- `IGpuImageView`
    - ***Eliminado***: `GetChannel()` (se obtiene a través de `GetConfig()`).
    - ***Eliminado***: `GetArrayType()` (se obtiene a través de `GetConfig()`).
    - ***Eliminado***: `GetBaseArrayLevel()` (se obtiene a través de `GetConfig()`).
    - ***Eliminado***: `GetLayerCount()` (se obtiene a través de `GetConfig()`).
    - ***Eliminado***: `GetViewUsage()` (se obtiene a través de `GetConfig()`).
    - ***Eliminado***: `GetBaseMipLevel()` (se obtiene a través de `GetConfig()`).
    - ***Eliminado***: `GetTopMipLevel()` (se obtiene a través de `GetConfig()`).

- *Renombrado*: `GpuImageFilteringType::LIENAR` -> `GpuImageFilteringType::LINEAR`.

- `IShaderPass`
    - ***Nuevo***: `Update(delta time)`.

- ***Nuevo***: `MaterialLayoutBuilder`
    - Permite construir layouts de materiales aplicando los datos de los shaders de un material.

###### Añadido soporte para mesh shaders.

- `ShaderStage`
    - ***Nuevo***: `ShaderStage::MESH`.
    - ***Nuevo***: `ShaderStage::MESH_AMPLIFICATION`.

- `PipelineCreateInfo`
    - ***Nuevo***: `meshAmplificationShaderPath`.
    - ***Nuevo***: `meshShaderPath`.

- ``Material``
    - Ahora mantiene un caché con los pipelines de meshes.
    - Permite obtener pipelines de meshes.

- `MaterialType`
    - ***Nuevo***: `MaterialType::MESH`.

- ***Nuevo***: `IMeshPipeline`, `MeshPipelineVk`
    - Pipeline para materiales de meshes.

- `ICommandList`
    - ***Nuevo***: `DrawMeshShader(...)`.
        - Ejecuta shaders de meshes (o de amplificación, si el material enlazado los tiene).

- `GraphicsPipelineVk`
    - ***Eliminado***: `GetResterizerInfo(...)`.
    - ***Eliminado***: `GetDepthInfo(...)`.
    - ***Eliminado***: `GetMsaaInfo(...)`.
    - ***Eliminado***: `GetTesselationInfo(...)`.

- `IGraphicsPipeline`
    - ***Nuevo***: `GetResterizerInfo(...)`.
    - ***Nuevo***: `GetDepthInfo(...)`.
    - ***Nuevo***: `GetMsaaInfo(...)`.
    - ***Nuevo***: `GetTesselationInfo(...)`.

- `IGpu`
    - Permite saber si soporta shaders de meshes.

- ``MaterialSystem``
    - Añadido soporte para materiales de meshes.
    - ***Eliminado***: `LoadMaterialV1(...)`.
        - Ahora es una función libre.

- ***Nuevo***: `OSK::GRAPHICS::LoadMaterialV1(...)`.
    - Ahora soporta la carga de materiales de meshes.

###### Renderizado de hierba mediante mesh shaders (experimental).

- ***Nuevo***: `GrassRenderPass` (*WIP*)
    - Efectúa renderizado de hierba procedural mediante mesh shaders.
    - Renderiza pequeños grupos de hojas de 64 triángulos.
    - Tiene tres LODs.
    - Simulación de viento.
    - Coloca los grupos de hojas dentro del área visible usando coordenadas polares.

- ``DeferredRenderSystem``
    - Ahora incluye por defecto el pase `GrassRenderPass`.

###### Añadidos samplers como objetos, de manera similar a las vistas de imagen. Ofrece un mayor control sobre el sampler usado.

- `IMaterialSlot`
    - `SetGpuImage(...)` ahora permite establecer el sampler de la imagen.

- ***Nuevo***: `IGpuImageSampler`
    - Representa un sampler para una imagen de GPU.
    - Puede ser compartido por varias imágenes.

- `GpuImageSamplerDesc`
    - Ahora tiene un hash.
    - *Reemplazado*: `CreateDefault()` -> `CreateDefault_NoMipMap()`.
    - *Reemplazado*: `CreateTextureDefault()` -> `CreateDefault_WithMipMap(...)`.
        - Ahora también establece el número máximo de mips dependiendo del tamaño de la imagen.

- `IRenderer`
    - Contiene una caché con las instancias de `IGpuImageSampler`.

- `IGpu`
    - Permite crear instancias de `IGpuImageSampler`.

###### Generalización del renderizado signed distance field 2D.

- ***Nuevo***: `ISdfRenderer2D`
- `SdfBindlessRenderer2D`
    - Ahora hereda de `ISdfRenderer2D`.

- `SdfDrawCall2D`
    - Ahora contiene el sampler a usar (o su descripción).

###### Utilidades para depuración.

- ***Nuevo***: `SpirvReflectionData`
    - Contiene información sobre un shader SPIR-V.

- `MaterialLayout`
    - Ahora permite obtener el nombre del material al que pertenece.
    
- ***Nuevo***: `OSK::ToString<OSK::GRAPHICS::MaterialLayout>(...)`.
- ***Nuevo***: `OSK::ToString<OSK::GRAPHICS::MaterialLayoutBinding>(...)`.
- ***Nuevo***: `OSK::ToString<OSK::GRAPHICS::MaterialLayoutSlot>(...)`.
- ***Nuevo***: `OSK::ToString<OSK::GRAPHICS::MaterialLayoutPushConstant>(...)`.

###### Cambios para permitir la compilación multiplataforma.

- Ahora el backend de Vulkan sólamente se compilará en las plataformas `OSK_WINDOWS`, `OSK_LINUX` y ``OSK_ANDROID``.
- Ahora el backend de DirectX 12 sólamente se compilará en la plataforma `OSK_WINDOWS`.

### ECS

- `IRenderSystem`
    - Ahora actualiza los pases de shaders al ejecutarse.

- `SystemManager`
    - Número máximo de entidades ejecutadas por cada hilo:  4294967295 -> 18446744073709551615.

###### Ahora la serialización de sistemas y componentes es opcional.

- ***Nuevo***: ``IsSerializableComponent<>``
    - Permite saber si un componente tiene los métodos necesarios para serialización y deserialización.

- `EntityComponentSystem`
    - `Save()`, `SaveBinary()` ahora solo guardan componentes y sistemas serializables.
    
- `ComponentContainer`
    - ``SerializeAll()``, `BinarySerializeAll()`, `DeserializeComponent()`, `BinaryDeserializeComponent()` ahora solo se aplican para componentes serializables.

- ***Nuevo***: `ISerializableSystem`
    - Interfaz para sistemas serializables.
    - Incluye:
        - `SaveConfiguration()`
        - `SaveBinaryConfiguration()`
        - `ApplyConfiguration(...)`
        
- ***Nuevo***: `IsSerializableSystem<>`
    - Concepto para saber si un sistema es serializable.

- `ISystem`
    - ***Eliminado***: `SaveConfiguration()` (transpasado a `ISerializableSystem`).
    - ***Eliminado***: `SaveBinaryConfiguration()` (transpasado a `ISerializableSystem`).
    - ***Eliminado***: `ApplyConfiguration(...)` (transpasado a `ISerializableSystem`).
    
- `IsEcsEvent<>`
    - Ahora está en el namespace `OSK::ECS`.

###### Eliminados sistemas obsoletos.

- ***Eliminado***: `RenderSystem3D`
- ***Eliminado***: `TerrainRenderSystem`
- ***Eliminado***: `TerrainComponent`

### UI

- `IElement`
    - Ahora usa `ISdfRenderer2D` para el renderizado.

### STD

- ***Eliminado***: `LinkedList`

### Otros

- Añadidos *includes* que faltaban en `Event.h` y ``AssetRef.hpp``.

### Error handling

- ***Nuevo***: `UnreachableException`
    - Excepción que se lanza al alcanzar código teóricamente inaccesible.

- ***Nuevo***: `ShaderReflectionException`
    - Excepción que se lanza al ocurrir un error al intentar obtener información sobre un shader.

### Bugfixes

- **Bugfix**: `ComponentContainer` ahora usa correctamente los operadores de copia y/o movimiento al mover de posición componentes.
- **Bugfix**: `OSK::ToString<OSK::GRAPHICS::ShaderStage>(...)` ahora funciona correctamente.
- **Bugfix**: `SystemExecutionGraph::Validate()` ahora funciona correctamente.
- **Bugfix**: los macros `OSK_SERIALIZATION(...)` y `OSK_COMPONENT_SERIALIZATION(...)` ahora exportan correctamente el código para uso en dll.
