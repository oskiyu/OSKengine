# OSKengine Version History.

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