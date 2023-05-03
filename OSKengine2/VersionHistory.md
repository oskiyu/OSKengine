# OSKengine Version History.

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