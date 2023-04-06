# OSKengine Version History.

## 2023.03.16a

###### STD

- HashMap:
  - Ahora es un wrapper sobre `std::unordered_map`.

###### Rendering

- Vértices:
  - Los vértices `Vertex3D` y `VertexAnim3D` ahora tienen un campo `tangent`, en preparación para normal mapping.

- GpuImage:
  - Ahora permite obtener resoluciones directamente en 1D, 2D y 3D (antes sólo ofrecía tamaño 3D).
  - Ya no permite escribir datos directamente sobre el buffer (`SetData()` eliminado).

- ***NEW***: `GpuImageViewConfig`:
  - Estructura que almacena las características de un image view.
  - Contiene constructores de ayuda.

- IGpuImageView:
  - Ahora se crea a partir de `GpuImageViewConfig`.
 
- IMaterialSlot:
  - Ahora aceptan `IGpuImageView` en vez de `GpuImage` a la hora de establecer imágenes sampled y de storage. Esto permite un mayor control sobre el acceso a la imagen.
  - Interfaz para establecer imágenes samplde y de storage simplificada.

- IPostProcessPass:
  - Ahora permite indicar el `GpuImageViewConfig` que se usará para establecer el input a partir de un rendertarget de cualquier tipo.
  - Ahora acepta `IGpuImageView` en vez de `GpuImage` a la hora de establecer imágenes de entrada.

- Bloom Post-Processing:
  - Ahora usa la cadena de mip-levels de una sola imagen, en vez de dos imágenes en ping-pong.
  - Aumentado el número máximo de pases de 4 a 8 pases.
  - Ya no realiza una copia final.
  - Corregidos varios bugs.

###### Bugfixes

- **Bugfix**: *Bloom* ya no tiene leakeos de luminosidad en los bordes.
- **Bugfix**: *Bloom* ahora adiciona correctamente todos los niveles de downscale/upscale.
- **Bugfix**: `HashMap` ya no tiene UB.

###### Otros

- Añadido el macro `OSK_ASSUME(X)`
- Corregidos algunos errores menores (class/struct).
- Renombrados algunos archivos que faltaban (...Vulkan -> ...Vk).


## 2023.03.18a

###### Types

- Quaternion:
  - Ahora permite rotar un vector de acuerdo a la orientación del cuaternión.

###### Rendering

Ahora se puede obtener estadísticas sobre la cantidad de memoria usada y disponible.

Ahora se puede elegir distintos formatos de profundidad.

- ***NEW:*** GpuHeapMemoryUsageInfo:
  - Muestra la capacidad máxima y el espacio usado de un memory heap.
  
- ***NEW:*** GpuMemoryUsageInfo:
  - Muestra la capacidad máxima y el espacio usado de toda la memoria de la GPU, dividida por tipo de memoria.
  - Contiene una lista con un `GpuHeapMemoryUsageInfo` por cada memory heap disponible.

- IGpu:
  - Ahora permite obtener estadísticas sobre la cantidad de memoria usada y disponible.

- IGpuMemoryAllocator:
  - Ahora permite obtener estadísticas sobre la cantidad de memoria usada y disponible.

- Material:
  - Ya no presupone un formato de profundidad específico a la hora de generar pipelines, permitiendo elegir distintos formatos de profundidad.

- RenderTarget:
  - Ahora puede usar una imagen de profundidad con formato únicamente de profundidad, sin la parte stencil.

- ViewUsage:
  - ***NUEVO:*** `ViewUsage::DEPTH_ONLY_TARGET`.

- ICommandList:
  - Ahora permite usar imágenes exclusivas de profundidad para renderizado 3D.
  - `BeginGraphicsRenderpass()` y `EndGraphicsRenderpass()` ahora permiten desactivar la sincronización automática de imágenes (por defecto, está activada).

- GpuImageUsage:
  - ***ELIMINADO:*** `GpuImageUsage::DEPTH_STENCIL`.
  - ***NUEVO:*** 
    - `GpuImageUsage::DEPTH`
    - `GpuImageUsage::STENCIL`.

- GBuffer:
  - Ahora usa una imagen de profundidad con formato únicamente de profundidad, sin la parte stencil.

- ShadowMap:
  - Ligeramente optimizado (~17% aprox.).
  - Reducido considerablemente su tamaño en memoria de la GPU.
  - Ahora usa un formato de half-precission.

- Collider Render System:
  - Ahora usa un formato de profundidad exclusiva de half-precission.

- Skybox Render System:
  - Ahora usa un formato de profundidad exclusiva de half-precission.

- Bloom Post-Processing:
  - Ligera optimización.
  - Ahora desescala y escala de una manera ligeramente distinta.

###### Bugfixes

- **Bugfix**: `GpuImage` ahora funciona correctamente al usar formatos exclusivos de profundidad: ya no intentará establecer flags de *stencil*.
- **Bugfix**: `IBottomLevelAccelerationStructure` y `BottomLevelAccelerationStructureVk` ya no tienen memory leaks al eliminarse.
- **Bugfix**: `ITopLevelAccelerationStructure` y `TopLevelAccelerationStructureVk` ya no tienen memory leaks al eliminarse.
- **Bugfix**: La cámara 3D ahora aplica correctamente rotación cuando cambia la orientación de su transform padre.

## 2023.04.06a

###### Rendering

Normal mapping y TAA.

- Model y mesh loaders:
  - Ahora cargan los vectores tangenciales de los vértices.
  - Ahora cargan las texturas de vectores normales.
    - En caso de no tener, usarán una textura azul por defecto.

- ***NUEVO:*** Temporal Anti-Aliasing
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

###### Bugfixes

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