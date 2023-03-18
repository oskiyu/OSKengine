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
