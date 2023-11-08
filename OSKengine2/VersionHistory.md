# OSKengine Version History.

## 2023.02.18a

### Rendering

- `GBuffer`
    - Ahora usa imágenes de 16 bits por canal, en vez de 32 bits por canal.

- `Material`
    - Ahora permite recargar los shaders.

- `MaterialSystem`
    - Ahora permite obtener un material previamente cargado a partir de su nombre.
    - Ahora permite recargar los materiales.

#### Bugfixes

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
    - Número de vértices.
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
  - Permite crear un `IBottomLevelAccelerationStructure` a partir de subrangos de buffers de vértices e índices.

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

##### Bugfixes

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

##### Bugfixes

- **Bugfix**: `OSK_DEFINE_AS` ahora no genera *undefined behaviour*.
- **Bugfix**: `OwnedPtr` ahora se comporta correctamente al moverse.
- **Bugfix**: `Color::Blue` ahora devuelve un color azul en vez de un color turquesa.
- **Bugfix**: `Color::*` ahora multiplica correctamente el valor alpha.
- **Bugfix**: `IMaterialSlot::SetTexture` ahora usa correctamente el canal indicado.

## 2023.07.15a

### Physics

###### Rework de los sistemas de respuesta ante colisiones y de físicas.

- `PhysicsComponent`
    - Aumentada la encapsulación para evitar la corrupción de los valores por modificaciones externas que se podrían saltar las leyes de la física.
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

##### Bugfixes

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

##### Bugfixes

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
    - `AddFace` ya no necesita que los vértices estén en sentido horario.
    - `GetLocalSpaceAxis` ahora devuelve el vector correcto independientemente del orden de los vértices.
    - `GetWorldSpaceAxis` ahora devuelve el vector correcto independientemente del orden de los vértices.

##### Bugfixes

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

##### Bugfixes

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
        - Evita que al hacer click se ejecuten funciones de elementos que están por debajo de otros.

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
    
##### Bugfixes

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
    - Pase de renderizado para la  resolución de **g-buffer*.

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
    - Ahora usa momentos estadísticos (media y desviación típica) para establecer la tolerancia de color.
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

##### Bugfixes

- **Bugfix**: `DynamicArray` ahora ejecuta correctamente los destructores de los elementos al destruirse la lista.
- **Bugfix**: `BloomPass::ExecuteSinglePass` ahora lanza el número correcto de hilos.
- **Bugfix**: el shader *TAA* ahora interpola correctamente los colores histórico y nuevo (hasta ahora siempre descartaba el histórico).
- **Bugfix**: cargar los materiales de animación del renderizador en diferido ya no genera errores en las capas de validación.
- **Bugfix**: cargar los materiales de animación del renderizador directo ya no genera errores en las capas de validación.
- **Bugfix**: `CollisionSysyem` ahora genera correctamente un evento por cada colisión, en vez de un evento por colisión por cada pareja de objetos.
