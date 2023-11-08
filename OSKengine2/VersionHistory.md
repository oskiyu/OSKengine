# OSKengine Version History.

## 2023.02.18a

### Rendering

- `GBuffer`
    - Ahora usa im�genes de 16 bits por canal, en vez de 32 bits por canal.

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
    - Ahora define la l�gica para cargar todos los v�rtices y sus atributos.

### Collision

###### Detecci�n de puntos de contacto

- `ConvexVolume`
    - Ahora detecta los puntos de contacto de una colisi�n.
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
    - A�adido soporte para `PolygonMode::POINT`.

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
    - Ahora aplica impulsos a los objetos de una colisi�n.
 
- `PhysicsSystem`
    - Ahora simula la resistencia del aire.

### Bugfixes

- `PhysicsResolver` ya no afecta a objetos que no contienen un `PhysicsComponent`.


## 2023.02.24b

### Physics

- `PhysicsComponent`
    - Ahora almacena la velocidad angular.
    - Ahora almacena el centro de gravedad.
    - Separada la l�gica del impulso en dos:
        - `ApplyLinealImpulse()`
        - `ApplyAngularImpulse()`
    - Permite obtener el torque de un punto del objeto.

- `PhysicsResolver`
    - Ahora aplica impulsos angulares a los objetos de una colisi�n.
 
- `PhysicsSystem`
    - Ahora tiene en cuenta la velocidad angular de los objetos.
    - Ahora simula la resistencia del aire en la rotaci�n.

### Types

- `Quaternion`
    - Ya no se normaliza despu�s de aplicarse una rotaci�n.


## 2023.03.14a

### Rendering

###### Refactor de las clases de Vulkan

Se han renombrado las clases, enumeraciones y dem�s de renderizado del backend Vulkan: *[...]Vulkan* -> *[...]Vk*.

###### Reorganizaci�n del asignador de memoria

Ahora la l�gica de asignaci�n de bloques de memoria est� implementada en `IGpuMemoryAllocator`, en vez de en las clases hijas.

## 2023.03.16a

### STD

- HashMap:
  - Ahora es un wrapper sobre `std::unordered_map`.

### Rendering

- V�rtices:
  - Los v�rtices `Vertex3D` y `VertexAnim3D` ahora tienen un campo `tangent`, en preparaci�n para normal mapping.
  
- `GpuImage`:
  - Ahora permite obtener resoluciones directamente en 1D, 2D y 3D (antes s�lo ofrec�a tama�o 3D).
  - Ya no permite escribir datos directamente sobre el buffer (`SetData()` eliminado).
  
- Bloom Post-Processing:
  - Ahora usa la cadena de mip-levels de una sola imagen, en vez de dos im�genes en ping-pong.
  - Aumentado el n�mero m�ximo de pases de 4 a 8 pases.
  - Ya no realiza una copia final.
  - Corregidos varios bugs.

###### Rework del sistema de vistas de im�genes

Ahora los procesos que cosumen una imagen lo hacen a trav�s de un `IGpuImageView`, en vez de a partir de una imagen.
Esto ofrece un mayor control sobre el acceso a la imagen, ya que antes se eleg�a siempre un mismo view por defecto.

- ***Nuevo***: `GpuImageViewConfig`:
  - Estructura que almacena las caracter�sticas de un image view.
  - Contiene constructores de ayuda.

- `IGpuImageView`:
  - Ahora se crea a partir de `GpuImageViewConfig`.
 
- `IMaterialSlot`:
  - Ahora aceptan `IGpuImageView` en vez de `GpuImage` a la hora de establecer im�genes sampled y de storage. Esto permite un mayor control sobre el acceso a la imagen.
  - Interfaz para establecer im�genes samplde y de storage simplificada.

- `IPostProcessPass`:
  - Ahora permite indicar el `GpuImageViewConfig` que se usar� para establecer el input a partir de un rendertarget de cualquier tipo.
  - Ahora acepta `IGpuImageView` en vez de `GpuImage` a la hora de establecer im�genes de entrada.

### Bugfixes

- **Bugfix**: *Bloom* ya no tiene leakeos de luminosidad en los bordes.
- **Bugfix**: *Bloom* ahora adiciona correctamente todos los niveles de downscale/upscale.
- **Bugfix**: `HashMap` ya no tiene UB.

### Otros

- A�adido el macro `OSK_ASSUME(X)`
- Corregidos algunos errores menores (class/struct).
- Renombrados algunos archivos que faltaban (...Vulkan -> ...Vk).


## 2023.03.18a

### Types

- Quaternion:
  - Ahora permite rotar un vector de acuerdo a la orientaci�n del cuaterni�n.

### Rendering

- Bloom Post-Processing:
  - Ligera optimizaci�n.
  - Ahora desescala y escala de una manera ligeramente distinta.

###### Estad�sticas de memoria

Ahora se puede obtener estad�sticas sobre la cantidad de memoria usada y disponible en la GPU.

- ***Nuevo:*** `GpuHeapMemoryUsageInfo`:
  - Muestra la capacidad m�xima y el espacio usado de un memory heap.
  
- ***Nuevo:*** `GpuMemoryUsageInfo`:
  - Muestra la capacidad m�xima y el espacio usado de toda la memoria de la GPU, dividida por tipo de memoria.
  - Contiene una lista con un `GpuHeapMemoryUsageInfo` por cada memory heap disponible.

- `IGpu`, `IGpuMemoryAllocator`:
  - Ahora permite obtener estad�sticas sobre la cantidad de memoria usada y disponible.

  ###### Formatos exclusivos de precisi�n

Ahora se puede elegir distintos formatos de profundidad.
Adem�s, se pueden usar formatos �nicamente de profundidad, sin la parte stencil.

- `GpuImageUsage`:
  - ***Eliminado:*** `GpuImageUsage::DEPTH_STENCIL`.
  - ***Nuevo:*** 
    - `GpuImageUsage::DEPTH`
    - `GpuImageUsage::STENCIL`.

- `Material`:
  - Ya no presupone un formato de profundidad espec�fico a la hora de generar pipelines, permitiendo elegir distintos formatos de profundidad.

- `RenderTarget`:
  - Ahora puede usar una imagen de profundidad con formato �nicamente de profundidad, sin la parte stencil.

- `ViewUsage`:
  - ***Nuevo:*** `ViewUsage::DEPTH_ONLY_TARGET`.

- `ICommandList`:
  - Ahora permite usar im�genes exclusivas de profundidad para renderizado 3D.
  - `BeginGraphicsRenderpass()` y `EndGraphicsRenderpass()` ahora permiten desactivar la sincronizaci�n autom�tica de im�genes (por defecto, est� activada).

- `GBuffer`:
  - Ahora usa una imagen de profundidad con formato �nicamente de profundidad, sin la parte stencil.

- Collider Render System:
  - Ahora usa un formato de profundidad exclusiva de half-precission.

- Skybox Render System:
  - Ahora usa un formato de profundidad exclusiva de half-precission.
  
- `ShadowMap`:
  - Ligeramente optimizado (~17% aprox.).
  - Reducido considerablemente su tama�o en memoria de la GPU.
  - Ahora usa un formato de half-precission.

### Bugfixes

- **Bugfix**: `GpuImage` ahora funciona correctamente al usar formatos exclusivos de profundidad: ya no intentar� establecer flags de *stencil*.
- **Bugfix**: `IBottomLevelAccelerationStructure` y `BottomLevelAccelerationStructureVk` ya no tienen memory leaks al eliminarse.
- **Bugfix**: `ITopLevelAccelerationStructure` y `TopLevelAccelerationStructureVk` ya no tienen memory leaks al eliminarse.
- **Bugfix**: La c�mara 3D ahora aplica correctamente rotaci�n cuando cambia la orientaci�n de su transform padre.

## 2023.04.06a

### Rendering

Normal mapping y TAA.

- Model y mesh loaders:
  - Ahora cargan los vectores tangenciales de los v�rtices.
  - Ahora cargan las texturas de vectores normales.
    - En caso de no tener, usar�n una textura azul por defecto.

- ***Nuevo:*** Temporal Anti-Aliasing
  - Implementado en `TaaProvider`.
  - Incluye dos pases:
    - Pase de acumulaci�n temporal.
    - Pase de afilado de imagen.
  - Implementa una estrategia de descartado agresiva junto a un patr�n de *jitter* de convergencia r�pida para evitar *motion blur*.
  - Permite obtener un �ndice para modificar el *jitter* para el renderizado de la escena original.
    - Por defecto devuelve un �ndice entre 1 y 4 (ambos incluidos), ya que la implementaci�n en `RenderSystem3D` y `PbrDeferredRenderSystem` usa un patr�n de h�lice para el *jitter*.
    - Devuelve 0 si est� desactivado.
  - No realiza sincornizaci�n de las im�genes de entrada y salida.

- `RenderSystem3D`:
  - Ahora genera una imagen con los vectores de movimiento.
  - Ahora usa normal mapping.
  - Ahora usa TAA.
  - Reorganizaci�n general.

- `PbrDeferredRenderSystem`:
  - Ahora usa normal mapping.
  - Ahora usa TAA.
  - Ahora usa shaders de computaci�n para el proceso de resolve.

- `Bloom`:
  - Ajustado su intensidad para obtener una imagen con colores m�s n�tidos preservando una intensidad de bloom adecuada en las partes m�s brillantes:
    - En el �litmo paso aumenta la intensidad de la imagen borrosa de manrea exponencial, incrementando as� m�s intensamente las partes m�s brillantes.
    - Al realizar la adici�n final, se usa un factor m�s peque�o para que las partes menos brillantes no queden con un color menos intenso.

### Bugfixes

- **Bugfix**: Los modelos 3D ya no se cargan con el doble de �ndices (la mitad de los cuales son 0).
- **Bugfix**: Ahora se puede usar la imagen de profundidad de `GBuffer` como sampled.
- **Bugfix**: Copiar de un buffer a una imagen ya no necesita que la imagen haya sido creada con `GpuImageUsage::TRANSFER_SOURCE` si no usa mip-maps.
- **Bugfix**: `PbrDeferredRenderSystem` ahora genera correctamente las im�genes de movimiento.
- **Bugfix**: `ShadowMap` ahora genera correctamente las sombras.
- **Bugfix**: `ShadowMap` ahora crea los b�fers de matrices con el tama�o correcto.
- **Bugfix**: Renderizar una escena con `ShadowMap` ya no disminuye la cantidad de luz de la escena.
- **Bugfix**: Ahora no se generan puntos excesivamente brillantes en el renderizado PBR.

###### Bugs conocidos:

- **Bug**: `ShadowMap` genera sombras incoherentes en las vallas.
- **Bug**: `ShadowMap` genera sombras incoherentes en el renderizador deferred.


## 2023.05.03a

### UI

Recomenzado el desarrollo del sistema de interfaz de usuario.

- ***Nuevo*** `UI::IElement`
  - Clase abstracta, base de la jerarqu�a de elementos UI.
  - Declara funcionalidad de renderizado y de actualizaci�n a la entrada del rat�n.
  - Pueden establecerse las siguientes caracter�sticas de cada elemento:
    - Posici�n (tanto relativa al padre como global).
    - Tama�o.
    - M�rgenes (no entran dentro del c�mputo del tama�o).
    - Padding (m�rgenes internos).
    - Tipo de anclaje (`UI::Anchor`).

###### Contenedores

- ***Nuevo*** `UI::IContainer`
  - Clase abstracta, base de cualquier contenedor de elementos UI.
  - Cada clase derivada define un layout.
  - Renderiza y actualiza todos los hijos.
    - Si un hijo no debe renderizarse o actualizarse, esa comprobaci�n se realiza en el propio hijo.
  - Puede tener una imagen de fondo.
  - Puede asignarse una clave a cada hijo a�adido, para despues obtenerlo.
  - Puede ajustar su tama�o al tama�o de sus hijos mediante una funci�n.
  
- ***Nuevo*** `UI::HorizontalContainer`
  - Contenedor que coloca los elementos de manera horizontal, uno despu�s del otro.
  - De izquierda a derecha.
  - Ignora cualquier valor del Anchor del eje horizontal.
  
- ***Nuevo*** `UI::VerticalContainer`
  - Contenedor que coloca los elementos de manera vertical, uno despu�s del otro.
  - De arriba a abajo.
  - Ignora cualquier valor del Anchor del eje vertical.
  
- ***Nuevo*** `UI::FreeContainer`
  - Contenedor que coloca los hijos en un lado, una esquina o centro, dependiendo de su Anchor.
  - No puede haber m�s de 1 elemento en cada posici�n, ya que se solapar�n.
  
###### Elementos

- ***Nuevo*** `UI::ImageView`
  - Elemento que muestra una imagen.
  - Contiene un `Sprite`.
  
- ***Nuevo*** `UI::TextView`
  - Elemento que muestra un texto.
  - Puede establecerse su fuente y su tama�o de fuente.
  
- ***Nuevo*** `UI::Button`
  - Implementa un bot�n con el que el usuario puede interaccionar.
  - Tiene tres posibles estados, dados por `UI::Button::ButtonState`.
  - Puede mostrar una imagen dependiendo de su estado.
  - Puede contener un texto.
    - El texto estar� siempre centrado.
  - Disponible en dos modos (dados por `UI::Button::Type`):
    - Bot�n normal.
    - Bot�n toggle, a modo de checkbox. En este caso guarda su estado `UI::Button::ButtonState::PRESSED` despu�s de pulsarlo, y recupera el estado `UI::Button::ButtonState::DEFAULT` al volver a pulsarlo.
  - En cualquier modo, ejecuta el callback.
    - En modo normal, el argumento de estado ser� siempre `true`.
    - En modo toggle, el argumento de estado depender� de si el bot�n pasa a estar pulsado o no.
    
- ***Nuevo***: `UI::Button::ButtonState`
    Representa el estado del bot�n (en el siguiente orden de prioridad):
    - `SELECTED`: el rat�n est� encima del bot�n, o el bot�n est� siendo seleccionado por un mando o teclado.
    - `PRESSED`: el bot�n est� siendo pulsado.
    - `DEFAULT`: el bot�n no est� siendo pulsado.

- ***Nuevo***: `UI::Button::Type`
    Representa el tipo de comportamiento del bot�n.
    - `NORMAL`: funcionamiento est�ndar de un bot�n. Si se pulsa, se ejecuta el callback con argumento `true`.
    - `TOGGLE`: funcionamiento estilo checkbox. El bot�n recuerda su estado, que va alternando entre `PRESSED` y `DEFAULT` cada vez que se pulsa.
        - Cada vez que se pulsa, se ejecuta el callback con el argumento:
            - `true` si el estado pasa a ser `PRESSED`.
            - `false` si el estado pasa a ser `DEFAULT`.

### Rendering

- ***Nuevo***: `FrameCombiner`
  - Permite combinar dos im�genes del mismo tama�o superponiendo una ante la otra, como si se renderizara una sobre la otra.
  - Considerablemente m�s eficiente que rasterizado equivalente.
  - Usable con im�genes de los siguientes formatos:
    - `RGBA8_UNORM`
    - `RGBA16_SFLOAT`

- Formats:
  - ***Nuevo***: `RGBA8_UINT`
  - ***Nuevo***: `RGB10A2_UNORM`

- `IGpuImageView`:
  - Ahora almacena el tama�o de la imagen original.

###### Reorganizaci�n del GBuffer

Reorganizaci�n general de los render targets.
Ahora la posici�n se reconstruye a partir de la imagen de profundidad.

- `PbrDeferredRenderSystem3D`:
  - Ahora reconstruye la posici�n a partir de la imagen de profundidad.
  - Corregidos errores varios.

- `GBuffer`

  - ***Eliminado***: render target de posici�n.
  - Render target de normales;
    - Ahora usa un formato `RGB10A2_UNORM` de mayor precisi�n.
    - Ya no contiene informaci�n del material (metallic-roughness).
  - Render target de motion vectors ahora usa un formato `RG16_SFLOAT` m�s peque�o.
  - ***Nuevo***: render target de metallic-roughness.
    - Usa un formato `RG16_SFLOAT`.

###### Unificaci�n de buffers

Ahora existe un �nico tipo de GPU buffer. Un GPU buffer se puede usar de cualquiera de las maneras anteriores (vertex, index, storage, uniform).

Para los buffers usados como vertex buffer / index buffer, ahora existen nuevas estructuras llamadas *views*, que contienen informaci�n sobre c�mo procesarlos.

- ***Eliminados:*** `IGpuVertexBuffer`, `IGpuIndexBuffer`, `IGpuSotrageBuffer`, `IGpuUniformBuffer` y derivados.

- `GpuBuffer` (antes `IGpuBuffer`)
  - Renombrado.

- ***Nuevo***: `VertexBufferView`
  - Estructura que permite procesar los datos de un buffer de v�rtices.
  - Puede usarse para procesar un subrango de un buffer.
  - Cada buffer contiene uno, facilitando el uso de un buffer entero como vertex buffer.
  - Contiene:
    - Informaci�n del v�rtice (`VertexInfo`).
    - N�mero de v�rtices.
    - Offset del primer v�rtice respecto al inicio del buffer.

- ***Nuevo***: `IndexBufferView`
  - Estructura que permite procesar los datos de un buffer de �ndices.
  - Puede usarse para procesar un subrango de un buffer.
  - Cada buffer contiene uno, facilitando el uso de un buffer entero como index buffer.
  - Contiene:
    - Tipo de �ndice (`OSK::GRAPHICS::IndexType`). Puede ser:
      - `U8` (byte).
      - `U16` (byte).
      - `U32` (byte).
    - N�mero de �ndices.
    - Offset del primer �ndice respecto al inicio del buffer.

- `ICommandList`
  - Ahora permite establecer un subrango de un buffer como buffer de v�rtices (mediante un `VertexBufferView`).
  - Ahora permite establecer un subrango de un buffer como buffer de �ndices (mediante un `IndexBufferView`).
  
- `IBottomLevelAccelerationStructure`
  - Pueden crearse a partir de un subrango de buffers mediante `VertexBufferView` e `IndexBufferView`.

- `IGpuMemoryAllocator`:
  - Todas las funciones de creaci�n de buffers de un tipo espec�fico (vertex, index, storage, uniform) siguen existiendo para simplificar su creaci�n, pero ahora devuelven buffers gen�ricos que pueden usarse para otros motivos.
    - `CreateVertexBuffer()` ahora establece su `VertexBufferView` por defecto para que apunte a todo el buffer, de acuerdo al tipo de v�rtice indicado.
    - `CreateIndexBuffer()` ahora establece su `IndexBufferView` por defecto para que apunte a todo el buffer, de acuerdo al tipo de �ndice indicado.
  - Permite crear un `IBottomLevelAccelerationStructure` a partir de subrangos de buffers de v�rtices e �ndices.

###### Sincronizaci�n  

  Se ha revisado el dise�o de la sincronizaci�n de comandos e im�genes, renombrando las estructuras y los enums para que representen con mayor fidelidad su funcionamiento real.
  Tambi�n se ha mejorado su documentaci�n.

  Tambi�n se ha mejorado la sincornizaci�n del proyecto entero, moviendo las acciones de sincronizaci�n al momento en el que son realmente necesarias.

  - `GpuCommandStage` (antes `GpuBarrierStage`)
    - ***Eliminado***: `DEFAULT`.
    - ***Nuevo***: `NONE`.
      - Indica que ning�n comando ser� afectado.
    - ***Nuevo***: `ALL`.
      - Indica que todos los comandos ser�n afectados.

  - `GpuAccessStage` (antes `GpuBarrierAccessStage`)
    - ***Nuevo***: `SAMPLED_READ`.
    - ***Nuevo***: `UNIFORM_BUFFER_READ`.
    - ***Nuevo***: `STORAGE_BUFFER_READ`.
    - ***Nuevo***: `STORAGE_IMAGE_READ`.
    - ***Nuevo***: `NONE`.
      - No indica ning�n acceso desde / hacia memoria.

  - `GpuImageRange` (antes `GpuImageBarrierInfo`).
    - Renombrado.
    - Puede usarse en tareas no relacionadas con la sincronizaci�n.

  - `GpuImage`:
    - Ahora mantiene un seguimiento de cual ha sido el �litmo image barrier aplicado, simplificando las tareas posteriores de sincronizaci�n.
      - Ya no es necesario conocer de manera expl�cita el image barrier anterior al introducir un nuevo barrier.
    - Ahora mantiene correctamente un seguimiento del layout de cada una de las capas y mip-levels.

###### Renderizado 2D

Se ha modernizado el renderizado de sprites, simplificando el material del sprite y la configuraci�n que necesita para facilitar su uso.

- `Sprite`
  - Su material ya no tiene una referencia al buffer de la c�mara 2D, por lo que se podr� renderizar usando cualquier c�mara 2D.
  - Se puede establecer espec�ficamente el `IGpuImageView` que se renderizar�.
    - Antes creaba el view por defecto a partir de una imagen de GPU.

- `SpriteRenderer`
  - Ahora permite establecer la c�mara 2D que se usar� en los siguientes sprites renderizados.
  - Si se cambia el material de renderizado 2D o se enlaza un nuevo material slot, se debe hacer a trav�s de la clase `SpriteRenderer`.
    - Esto permite usar estrategias de renderizado distintas en el futuro, como un renderizado con sprite bacthes.
  - La matriz de la c�mara 2D ahora se aplica a la matriz modelo en la CPU, enviando a la GPU la matriz final.
  - Ahora usa string views para el renderizado de texto.

- `Texture`
  - Ahora permite obtener f�cilmente los siguientes `IGpuImageView`:
    - View con un �nico nivel de mip-map.
    - View con un �nico todos los niveles de mip-map.
    
### Otros

- El efecto de *tone-mapping* ahora genera una imagen ligeramente menos azul.
- Ajustado ligeramente el efecto de *bloom*, revirtiendo parcialmente los cambios realizados en **2023.04.06a**.

##### Bugfixes

- **Bugfix**: `PbrDeferredRenderSystem3D` ya no genera sombras incoherentes.
- **Bugfix**: `PbrDeferredRenderSystem3D` ya no genera normales de menos precisi�n cuando un objeto se aleja del origen de coordenadas.
- **Bugfix**: BeginRenderpass ahora por defecto sincroniza correctamente las im�genes de profundidad.


## 2023.06.17a

### ***Nuevo***: Audio

###### Implementaci�n inicial

Implementaci�n inicial de un sistema de carga y reproducci�n de audio usando *OpenAL Soft*.
Carga de archivos *wav*.

- ***Nuevo***: `AudioApi`
    - Clase principal del sistema.
    - Contiene las instancias de *AUDIO::Device* disponibles.
    - Permite establecer un *AUDIO::Device* como dispositivo de salida activo.
    - Permite establecer la posici�n, orientaci�n y velocidad del escuchador.

- ***Nuevo***: `AUDIO::Device`
    - Representa un dispositivo de salida de audio.
    - Permite establecerse como dispositivo de salida activo.

- ***Nuevo***: `AUDIO::Buffer`
    - Representa un buffer alojado en un dispositivo de salida.
    - Contiene la informaci�n de un sonido / m�sica.

- ***Nuevo***: `AUDIO::Source`
    - Representa un "reproductor" de audio.
    - Reproduce un sonido / m�sica almacenado en un *AUDIO::Buffer*.
    - Permite establecer la posici�n, velocidad, tono y volumen.
    - Permite reproducci�n en bucle.

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
    - Asset que representa un sonido o m�sica.
    - Contiene un buffer con los datos del sonido.

- ***Nuevo***: `AudioAssetLoader`
    - Permite cargar archivos de audio en formato *wav*.


### Error Handling

Reformado el sistema de manejo de errores, usando excepciones espec�ficas para cada tipo de error.

- `EngineException`:
    - Clase base para todas las excepciones del motor y del juego.
    - Contiene una descripci�n de la excepci�n y la localizaci�n en la que se lanz�.

- Excepciones a�adidas:
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

###### Ecuaci�n de renderizado mini rework

- PBR Rendering
    - Se ha normalizado la ecuaci�n de renderizado para que los pesos de las distintas fuentes de color (ambiente y cubemaps) sean iguales.
    - Se ha modificado la ecuaci�n de renderizado para ofrecer colores m�s intensos.

###### Cleanup

Eliminadas las clases SyncDevice, ya que la sincornizaci�n se realiza en las clases principales `IRenderer`.

- ***Eliminado***: `ISyncDevice`, `SyncDeviceDx12`, `SyncDeviceVk`.

### Types

Se han introducido nuevos tipos de datos b�sicos y renombrado otros para que queden m�s clara su intenci�n y su tama�o.

- `USize32`: renombrado (antes `TSize`).
- `UIndex32`: renombrado (antes `TIndex`).
- ***Nuevo***: `USize64`.
- ***Nuevo***: `UIndex64`.

###### Memoria de 64 bits

Debido a los cambios en los tipos de datos b�sicos, numerosas clases y funciones (sobre todo aquellas que tienen que ver con memoria, ahora soportan tama�os y direcciones de 64 bits. Incluyendo:

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

###### Rework de los sistemas de respuesta ante colisiones y de f�sicas.

- `PhysicsComponent`
    - Aumentada la encapsulaci�n para evitar la corrupci�n de los valores por modificaciones externas que se podr�an saltar las leyes de la f�sica.
    - Almacena el inverso de la masa.
        - Se puede usar para representar masas infinitas, estableciendo el inverso de la masa a 0.
    - A�adido soporte para el *momento de inercia angular*, que regula la facilidad de un objeto para rotar.
        - Alacenados tanto el tensor de inercia como su inverso para una mayor eficiencia.
    - Almacena el cambio de velocidad en cada frame, para una mayor estabilidad.
    - A�adido el coeficiente de restituci�n, que indica cu�nto rebota el objeto tras una colisi�n.
    - Permite resetear las fuerzas aplicadas en cada frame, para poder recalcular su superposici�n.
    - A la hora de aplicar un impulso, no se hace distinci�n entre impulso lineal y angular, efectuando ambos al mismo tiempo.
    
- `PhysicsResolver`
    - Ahora la aplicaci�n del MTV se reparte entre los dos objetos dependiendo de sus masas.
        - Los objetos con menor masa que el otro objeto se mueven m�s.
        - Los objetos con masa infinita no se mueven.
    - Ahora la aplicaci�n de los impulsos tras una colisi�n sigue las leyes de la f�sica:
        - Obtiene la velocidad de separaci�n final y aplica los impulsos para conseguirla.
        - Ambos impulsos aplicados son iguales en intensidad, pero en sentidos contrarios.
    - Ahora aplica fricci�n a los objetos.

- `PhysicsSystem`
    - Ahora simula la resistencia del aire, ralentizando la velocidad angular de los objetos en el tiempo.

- `ConvexVolume`
    - Proceso de detecci�n de puntos de contacto optimizado.
    - Ahora es m�s permisivo al incluir nuevos puntos de contacto.

- `DetailedCollisionInfo`
    - Permite diferenciar el orden de los objetos.
    
### IO

- ***Nuevo***: `Console`
    - Representa una consola de output dentro del juego.
    - Permite a�adir nuevos mensajes de texto.
    - Almacena informaci�n sobre el momento en el que se a�adi� el mensaje.
    - _Prototipo_.

### Engine

- ***Nuevo*** `gameFrameIndex`.
    - Indica el �ndice del frame actual desde el inicio de la ejecuci�n del juego.
- ***Nuevo*** `Update()`
    - Actualiza el `gameFrameIndex`.
    
### Types

- `Vector3`
    - A�adido valor est�tico `Zero`, que representa un vector con todos los campos a 0.
    
### Error Handling

- ***Nuevo***: `DivideByZeroException`.

##### Bugfixes

- **Bugfix**: `PhysicsSystem` ya no mueve y/o rota objetos inmovibles.
- **Bugfix**: `COLLISION::ConvexVolume` ahora calcula correctamente los puntos de contacto en todos los casos.
- **Bugfix**: A�adir un componente a un objeto no hace que se vuelva a registrar en los sistemas en los que ya estaba presente.
- **Bugfix**: `ECS::AddComponent(GameObjectIndex, const TComponent&)` ahora se puede usar.
- **Bugfix**: `FaceProjection` ahora calcula correctamente las proyecciones de los puntos.


## 2023.08.13a

### ECS

###### Orden de ejecuci�n de los sistemas

- `EntityComponentSystem`, `SystemManager`
    - Ahora se le puede asignar un �ndice de ejecuci�n a cada sistema (por defecto: 0). Todos los sistemas con el mismo �ndice se agrupan en un conjunto.
    - La ejecuci�n de los sistemas se realiza de acuerdo a estos �ndices, ejecut�ndose primero los sistemas con un �ndice m�s bajo.
    - Dentro de cada conjunto de sistemas, se ejecutan en el siguiente orden:
        - Sistemas *productores*. 
        - Sistemas *consumidores*. 
        - Sistemas *puros*. 

- `ISystem`
    - Ahora almacena su �ndice de ejecuci�n.

- Sistemas con un �ndice de ejecuci�n distinto a 0:
    - `PhysicsSystem`: -2.
    - `CollisionSystem`: -1.
    - `PhysicsResolver`: -1.
    
### Rendering

###### Rework de los shaders PBR para obtener una imagen de mayor calidad.

### Memory

- `Buffer`
    - Representa un buffer con informaci�n arbitraria en memoria RAM.
    - Puede ser copiado.
    - No tiene un tama�o fijo: funciona como un *DynamicArray*.
    
### Persistencia

###### Inicio del rework para mejorar el rendimiento potencial y adaptarlo mejor aun futuro sistema de networking.

- ***Eliminado***: `Field`
- ***Eliminado***: `IDataElement`
- ***Eliminado***: `ISerializer`
- ***Eliminado***: `ISerializable`

- `DataNode`
    - Ahora funciona escribiendo la informaci�n sobre un buffer de memoria.
    - Permite leer datos del buffer de memoria.
    - Contiene un schema que indica c�mo interpretar los datos.
    - Tiene mapas para facilitar la interpretaci�n de los datos.

- `DataType`
    - Ahora se representa por un n�mero.
    - Los primeros 100 n�meros est�n reservados para el motor.
    
- ***Nuevo***: `Serialize<>()`
    - Define c�mo serializar un elemento a un `DataNode`.
    - Debe definirse para todos los tipos que se quieran usar.

- ***Nuevo***: `Deserialize<>()`
    - Define c�mo deserializar un `DataNode` a un elemento.
    - Debe definirse para todos los tipos que se quieran usar.

- ***Nuevo***: `SchemaId`
    - Define un c�digo �nico para cada esquema de `DataNode`.

### STD

###### Corregidas incongruencias en las funciones de las clases, con lo que ahora siempre respetan la correcci�n de *const*.

- `UniquePtr`
    - Ahora todas sus operaciones respetan la correcci�n de *const*.
    - Ahora tiene una comprobaci�n en tiempo de compilaci�n que se asegura de que el objeto est� definido.
    - ***Eliminado***: `UniquePtr<T[]>`.
    - ***Eliminado***: `SetValue()`.
    - A�adidas todas las posibles combinaciones de *const* / *not const* para:
        - `GetPointer()`.
        - Operador `->`.

- `DynamicArray`
    - `DynamicArray::Iterator` 
        - Simplificado (ahora solo almacena un puntero).
        - Ya no necesita que la colecci�n que lo cre� tenga estabilidad de puntero.
            - Siguen necesitando que no se a�adan y/o eliminen elementos.
    - ***Nuevo***: `DynamicArray::ConstIterator`
        - Iterador para acceso a un elemento de una colecci�n inmutable.
    - Ahora soporta tipos que tengan sobreescrito el operador `&`.
    - Ahora genera c�digo distino dependiendo de si el tipo puede ser copiado y/o movido.
    - A�adidas todas las posibles combinaciones de *const* / *not const* para:
        - Operador `[]`.
        - `At()`.
        - `Find()`.
        - `GetData()`.
        - `begin()`.
        - `end()`.
        - `GetIterator()` / `GetConstIterator()`.

- `SharedPtr`
    - A�adidas todas las posibles combinaciones de *const* / *not const* para:
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
- **Bugfix**: `TextureLoader` ahora carga las im�genes en el espacio de color adecuado.
- **Bugfix**: `Transform3D::TransformPoint()` ahora devuelve el resultado correcto.

TODO:

fix ComponentManager::InsertCopy


## 2023.09.08a

### UI

- `UI::IElement`:
    - Ahora puede configurarse su visibilidad.
        - Los contenedores invisibles no renderizar�n a sus hijos.
    - Ahora puede configurarse su capacidad para actualizar su estado.
        - Los contenedores desactivados no actualizar�n a sus hijos.
    - Ahora puede configurarse para mantener un tama�o relativo respecto al padre, cuando el padre cambia de tama�o.
    
### Collisions

- `CollisionSystem`
    - *Optimizaci�n*: Ahora s�lamente transforma los v�rtices de los colliders de nivel bajo una sola vez por fotograma, en vez de una vez por posible colisi�n.

- `ConvexVolume`
    - Ahora tambi�n almacena los v�rtices transformados (se actualiza cada fotograma).
    - `AddFace` ya no necesita que los v�rtices est�n en sentido horario.
    - `GetLocalSpaceAxis` ahora devuelve el vector correcto independientemente del orden de los v�rtices.
    - `GetWorldSpaceAxis` ahora devuelve el vector correcto independientemente del orden de los v�rtices.

##### Bugfixes

- **Bugfix**: `VerticalContainer::EmplaceChild()` ahora coloca correctamente los elementos en el eje horizontal.
- **Bugfix**: `FreeContainer::EmplaceChild()` ahora tiene en cuenta correctamente los m�rgenes del elemento a�adido.
- **Bugfix**: `ConvexVolume::GetCollisionInfo()` ahora detectamente correctamente la informaci�n de las colisiones en todas las direcciones (test cases *T-COL-09,11,13,15*)
- **Bugfix**: `ConvexVolume::GetWorldSpaceAxis()` ahora devuelve un vector correctamente normalizado en todos los casos.
- **Bugfix**: `PhysicsResolver` ahora aplica correctamente impulsos en todos los casos.



## 2023.09.11a

### Graphics

###### Introducido HBAO como t�cnica de oclusi�n ambiental.

- ***Nuevo***: `HbaoPass`:
    - Implementa oclusi�n ambiental como efetdo de post-procesado de computaci�n.
    - Incluye 4 pases:
        - Pase inicial que genera una imagen de oclusi�n ambiental con ruido.
        - Pase de difuminado gaussiano vertical.
        - Pase de difuminado gaussiano horizontal..
        - Pase de resoluci�n.
    - Usa como input las im�genes de profundidad y de normales.
   
- `RenderSystem3D`:
    - Ahora genera im�genes de normales.

###### Invertida la coordenada Z de profundidad y uso de far-plane infinito.

Esto aumenta enormemente la precisi�n de objetos alejados, y permite renderizar toda la escena gracias al far-plane infinito.

- `CameraComponent3D`
    - Ahora `GetProjectionMatrix()` genera una proyecci�n con:
        - Z invertida.
        - Far-plane infinito.
    - ***Nuevo***: `GetProjectionMatrix_UnreversedZ()`, que implementa la funcionalidad que previamente ten�a `GetProjectionMatrix()`.

###### Misc.

- `Material`, `MaterialSystem`:
    - Los materiales gr�ficos ya no est�n obligados a tener shaders de fragmentos.

- `IGraphicsPipeline`, `GraphicsPipelineVk`:
    - Ahora usa un formato de profundidad con z invertida.
    - Los fragment shaders ya no son estrictamente necesarios.

- `Format`
    - ***Nuevo***: `R16_SFLOAT`

- `CopyImageInfo`
    - Ahora permite indicar los canales de origen y destino.

- `ICommandList`
    - ***Nuevo*** `CopyImageToImage()`.
        - Permite copiar una imagen a otra cuando ambas im�genes tienen distinto tama�o y/o formato.
    - `ICommandList::RawCopyImageToImage()` ahora puede efectuar copias en canales distintos a *COLOR*.

- `GpuImageVk`
    - ***Nuevo***: `GetAspectFlags()`.
    - `GetFilterTypeVulkan()` ahora es public.
    - `GetAddressModeVulkan()` ahora es public.

##### Bugfixes

- **Bugfix**: ahora las sombras no tienen errores de z-fighting en el renderizado diferido.
- **Bugfix**: ahora ning�n renderizado 3D tienen z-fighting en zonas alejadas de la c�mara.
- **Bugfix**: `ICommandList::RawCopyImageToImage()` ahora tiene en cuenta los offsets indicados para las im�genes de origen y destino.
- **Bugfix**: ahora los mapas de sombras no usan una proyecci�n con un rango de profundidad incorrecto (-1, 1).
- **Bugfix**: ahora la c�mara 3D no usa una proyecci�n con un rango de profundidad incorrecto (-1, 1).
- **Bugfix**: ahora `GBuffer` tiene un *getter* de im�genes no-const.
- **Bugfix**: `PbrDeferredRenderSystem` ya no reenlaza el material de GBuffer una vez por modelo 3D.
 

## 2023.09.15a

### UI

- ***Nuevo***: `Dropdown`
    - Elemento de UI que permite al usuario seleccionar una opci�n entre varias mediante un men� desplegable.

- `IElement`
    - `UpdateByCursor()`:
        - Ahora devuelve un bool indicando si el input ha sido procesado dentro del elemento.
        - Si devuelve true, el input se ha procesado y no hay que comprobar el resto de elementos de la interfaz.
        - Evita que al hacer click se ejecuten funciones de elementos que est�n por debajo de otros.

- `IContainer`
    - Si `UpdateByCursor()` devuelve true para alguno de sus hijos, detiene la ejecuci�n para el resto de hijos.

- `TextView`
    - Ahora permite obtener la fuente usada y su tama�o.

### Assets

- `Font`
    - ***Nuevo***: `GetExistingInstance()` permite obtener una instancia previamente cargada.
    - ***Nuevo***: `ContainsInstance()` permite comprobar si una instancia ha sido previamente cargada.
    - Llamar a `LoadSizedFont()` con un tama�o que ya ha sido cargado no har� nada, en vez de volver a cargar la fuente.

### Graphics

- `IRenderer`
    - ***Nuevo***: `SetPresentMode()` permite cambiar el modo de presentaci�n en tiempo de ejecuci�n.

- `ISwapchain`
    - ***Nuevo***: `SetPresentMode()` permite cambiar el modo de presentaci�n en tiempo de ejecuci�n.
    
##### Bugfixes

- **Bugfix**: `UI::ImageView` ya no genera una excepci�n cuando se intenta renderizar sin haber establecido antes la iamgen (ahora no hace nada).
- **Bugfix**: `Sprite` ya no genera una excepci�n cuando se intenta establecer un image view nulo.
 

## 2023.11.08a

### Collisions

###### Rework de la detecci�n de colisiones (GJK-Culling).

- `Collider`
    - Ahora permite construirse copiando otro collider.
    
- `ITopLevelCollider` ahora permite obtener una copia.
- `IBottomLevelCollider`:
    - Ahora permite obtener una copia.
    - Hereda `IGjkCollider`.
    - Hereda `ISatCollider`.

- `ConvexVolume`
    - Ahora implementa detecci�n de colisiones mediante *GJK-Clipping*.
    - Ahora permite mergear los v�rtices y las caras.
    - Ahora permite obtener la cara que contiene una lista de v�rtices.

- `CollisionInfo`
    - Ahora contiene varios `DetailedCollisionInfo` cuando dos entidades colisionan 
con varios colliders.

- `DetailedCollisionInfo` ahora permite saber la direcci�n del MTV (respecto a los objetos).

- ***Nuevo***: `Simplex`
    - Grupo de v�rtices que trata de encerrar el origen de coordenadas.
    - Se genera usando la diferencia de Minkowski de dos vol�menes.
    - Permite saber si dos vol�menes colisionan o no.

- ***Nuevo***: `MinkowskiHull`
    - Volumen 3D que trata de expandirse para ocupar toda la diferencia de Minkowski de dos colliders.
    - Permite conocer el MTV y los v�rtices que generaron el MTV.

- ***Nuevo***: `IGjkCollider`
    - Interfaz para colliders que soportan detecci�n de colisiones mediante GJK-EPA.

- ***Nuevo***: `GjkSupport`
    - Indica el soporte de un volumen y el v�rtice que lo produjo.

- ***Nuevo***: `MinkowskiSupport`
    - Indica el soporte de la diferencia de Minkowski de dos vol�menes y los v�rtices que lo produjeron.

- ***Nuevo***: `ClipFaces()` permite realizar un clipping de dos caras, obteniendo el parche de contacto.

###### A�adido frustum culling.

- `ITopLevelCollider`, `SphereCollider`, `AxisAlignedBoundingBox`
    - Ahora permiten comprobar si est�n delante o en un plano.
    - Ahora permiten comprobar si est�n dentro de un frustum.

### Graphics

- `SkyboxRenderSystem`
    - Normalizado el brillo del cubemap a 1.

- `IGpuMemoryAllocator`
    - Ahora genera y es due�o de la textura normal por defecto.
   
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
    - Pase de renderizado para la  resoluci�n de **g-buffer*.

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
    - Ahora permite ejecutar el pase de generaci�n de HBAO en resoluciones distintas a la nativa.
        - Por defecto, se ejecuta a una resoluci�n del 50%.
    - Reajustados par�metros de generaci�n para un mejor resultado.
    - Los pases de desenfoque ahora:
        - Son independientes de la resoluci�n de entrada.
        - Usan un radio del filtro gaussiano de 3.25.
        - Usan interpolaci�n bilineal al procesar la imagen anterior.
        - Usan comprobaciones de resultado para intentar preservar los bordes n�tidos.

- *TAA*
    - Ahora usa un filtro gaussiano al obtener el color de la imagen hist�rica, para un mejor funcionamiento en movimiento.
    - Ahora usa momentos estad�sticos (media y desviaci�n t�pica) para establecer la tolerancia de color.
    - Reducida la cantidad de ghosting en elementos que se mueven r�pidamente, disminuyendo la tolerancia de color de elementos con un movimiento muy grande.

###### A�adido frustum culling.

- `PbrDeferredRenderSystem`
    - Ahora realiza frustum culling en los meshes.

- ***Nuevo***: `RenderBoundsRenderer`
    - Renderiza los vol�menes usados para realizar el frustum culling.


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

###### A�adido frustum culling.

- Model loading:
    - Ahora genera colliders esf�ricos por cada mesh.
 
- `Mesh3D`
    - Ahora tiene un collider esf�rico.
   
### ECS

- `ModelComponent3D`
    - Ya no almacena instancias de material.

###### A�adido frustum culling.

- `CameraComponent3D`
    - Ahora permite obtener los planos de un frustum.
      
### Types

- `Vector3`
    - ***Renombrado***: *Z* -> z.
    - Ahora no puede construirse de manera impl�cita.
    - Ahora pude comprobarse si es *NaN*.

###### A�adido frustum culling.

- ***Nuevo***: `Plane`
    - Representa un plano en el mundo 3D.
    - Vector normal + punto del plano.

- ***Nuevo***: `AnyFrustum`
    - Representa un frustum con un n�mero indeterminado de planos.

##### Bugfixes

- **Bugfix**: `DynamicArray` ahora ejecuta correctamente los destructores de los elementos al destruirse la lista.
- **Bugfix**: `BloomPass::ExecuteSinglePass` ahora lanza el n�mero correcto de hilos.
- **Bugfix**: el shader *TAA* ahora interpola correctamente los colores hist�rico y nuevo (hasta ahora siempre descartaba el hist�rico).
- **Bugfix**: cargar los materiales de animaci�n del renderizador en diferido ya no genera errores en las capas de validaci�n.
- **Bugfix**: cargar los materiales de animaci�n del renderizador directo ya no genera errores en las capas de validaci�n.
- **Bugfix**: `CollisionSysyem` ahora genera correctamente un evento por cada colisi�n, en vez de un evento por colisi�n por cada pareja de objetos.
