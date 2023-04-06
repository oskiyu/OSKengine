# OSKengine Version History.

## 2023.03.16a

###### STD

- HashMap:
  - Ahora es un wrapper sobre `std::unordered_map`.

###### Rendering

- V�rtices:
  - Los v�rtices `Vertex3D` y `VertexAnim3D` ahora tienen un campo `tangent`, en preparaci�n para normal mapping.

- GpuImage:
  - Ahora permite obtener resoluciones directamente en 1D, 2D y 3D (antes s�lo ofrec�a tama�o 3D).
  - Ya no permite escribir datos directamente sobre el buffer (`SetData()` eliminado).

- ***NEW***: `GpuImageViewConfig`:
  - Estructura que almacena las caracter�sticas de un image view.
  - Contiene constructores de ayuda.

- IGpuImageView:
  - Ahora se crea a partir de `GpuImageViewConfig`.
 
- IMaterialSlot:
  - Ahora aceptan `IGpuImageView` en vez de `GpuImage` a la hora de establecer im�genes sampled y de storage. Esto permite un mayor control sobre el acceso a la imagen.
  - Interfaz para establecer im�genes samplde y de storage simplificada.

- IPostProcessPass:
  - Ahora permite indicar el `GpuImageViewConfig` que se usar� para establecer el input a partir de un rendertarget de cualquier tipo.
  - Ahora acepta `IGpuImageView` en vez de `GpuImage` a la hora de establecer im�genes de entrada.

- Bloom Post-Processing:
  - Ahora usa la cadena de mip-levels de una sola imagen, en vez de dos im�genes en ping-pong.
  - Aumentado el n�mero m�ximo de pases de 4 a 8 pases.
  - Ya no realiza una copia final.
  - Corregidos varios bugs.

###### Bugfixes

- **Bugfix**: *Bloom* ya no tiene leakeos de luminosidad en los bordes.
- **Bugfix**: *Bloom* ahora adiciona correctamente todos los niveles de downscale/upscale.
- **Bugfix**: `HashMap` ya no tiene UB.

###### Otros

- A�adido el macro `OSK_ASSUME(X)`
- Corregidos algunos errores menores (class/struct).
- Renombrados algunos archivos que faltaban (...Vulkan -> ...Vk).


## 2023.03.18a

###### Types

- Quaternion:
  - Ahora permite rotar un vector de acuerdo a la orientaci�n del cuaterni�n.

###### Rendering

Ahora se puede obtener estad�sticas sobre la cantidad de memoria usada y disponible.

Ahora se puede elegir distintos formatos de profundidad.

- ***NEW:*** GpuHeapMemoryUsageInfo:
  - Muestra la capacidad m�xima y el espacio usado de un memory heap.
  
- ***NEW:*** GpuMemoryUsageInfo:
  - Muestra la capacidad m�xima y el espacio usado de toda la memoria de la GPU, dividida por tipo de memoria.
  - Contiene una lista con un `GpuHeapMemoryUsageInfo` por cada memory heap disponible.

- IGpu:
  - Ahora permite obtener estad�sticas sobre la cantidad de memoria usada y disponible.

- IGpuMemoryAllocator:
  - Ahora permite obtener estad�sticas sobre la cantidad de memoria usada y disponible.

- Material:
  - Ya no presupone un formato de profundidad espec�fico a la hora de generar pipelines, permitiendo elegir distintos formatos de profundidad.

- RenderTarget:
  - Ahora puede usar una imagen de profundidad con formato �nicamente de profundidad, sin la parte stencil.

- ViewUsage:
  - ***NUEVO:*** `ViewUsage::DEPTH_ONLY_TARGET`.

- ICommandList:
  - Ahora permite usar im�genes exclusivas de profundidad para renderizado 3D.
  - `BeginGraphicsRenderpass()` y `EndGraphicsRenderpass()` ahora permiten desactivar la sincronizaci�n autom�tica de im�genes (por defecto, est� activada).

- GpuImageUsage:
  - ***ELIMINADO:*** `GpuImageUsage::DEPTH_STENCIL`.
  - ***NUEVO:*** 
    - `GpuImageUsage::DEPTH`
    - `GpuImageUsage::STENCIL`.

- GBuffer:
  - Ahora usa una imagen de profundidad con formato �nicamente de profundidad, sin la parte stencil.

- ShadowMap:
  - Ligeramente optimizado (~17% aprox.).
  - Reducido considerablemente su tama�o en memoria de la GPU.
  - Ahora usa un formato de half-precission.

- Collider Render System:
  - Ahora usa un formato de profundidad exclusiva de half-precission.

- Skybox Render System:
  - Ahora usa un formato de profundidad exclusiva de half-precission.

- Bloom Post-Processing:
  - Ligera optimizaci�n.
  - Ahora desescala y escala de una manera ligeramente distinta.

###### Bugfixes

- **Bugfix**: `GpuImage` ahora funciona correctamente al usar formatos exclusivos de profundidad: ya no intentar� establecer flags de *stencil*.
- **Bugfix**: `IBottomLevelAccelerationStructure` y `BottomLevelAccelerationStructureVk` ya no tienen memory leaks al eliminarse.
- **Bugfix**: `ITopLevelAccelerationStructure` y `TopLevelAccelerationStructureVk` ya no tienen memory leaks al eliminarse.
- **Bugfix**: La c�mara 3D ahora aplica correctamente rotaci�n cuando cambia la orientaci�n de su transform padre.

## 2023.04.06a

###### Rendering

Normal mapping y TAA.

- Model y mesh loaders:
  - Ahora cargan los vectores tangenciales de los v�rtices.
  - Ahora cargan las texturas de vectores normales.
    - En caso de no tener, usar�n una textura azul por defecto.

- ***NUEVO:*** Temporal Anti-Aliasing
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

###### Bugfixes

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