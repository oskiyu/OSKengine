# OSKengine Version History.

## Alpha 0 (2019.12.3a)

###### WindowAPI:
- WindowAPI.
    - Interfaz con GLFW.
    - Carga de OpenGL.
    - Actualiza los valores de un KeyboardState.
- Keyboard Input.
    - KeyboardState.

###### OpenGL RenderAPI:
- RenderAPI.
    - Inicializa OpenGL.
    - Opciones disponibles: `BLEND`, `CULL_FACE`, `DEPTH_TEST`, `STENCIL_TEST`, `MULTISAMPLE`.
    - Aún no es funcional.
- Shader.
    - Interfaz con un shader de `GLSL`.
    - Carga a partir de un archivo.
    - Soporta los tipos: `bool`, `int`, `float`, `mat4`, `vec2`, `vec3`, `vec4`.
    - Funciones especiales para las matrices `View`, `Projection` y `Model`.

###### Types:
- `result_t` (`bool`).
- `keyState_t` (`bool`).
- `keyCode_t` (`int`).

###### Util:
- DebugOutput.
    - Muestra un mensaje por la consola.
- Exit. 
    - Termina la ejecución mostrando un mensaje.


## Alpha 1 (2019.12.4a)

###### WindowAPI:
- Mouse Input.
    - MouseState:
        - Mantiene información sobre la posición y los botones pulsados en el ratón.
        - También matiene información del scroll.
- WindowAPI:
    - Se puede establecer la versión de OpenGL.
    - Tiene su propio MouseState actualizado.
        - Puede actualizar otros MouseState.
        
###### OpenGL RenderAPI:
- Vertex: posición, vector normal y TexCoords.
- Mesh.
    - Se crea a partir de una lista de vértices.
    - Se dibuja a si mismo.
- Model.
    - Se carga a través de un archivo (**aún no implementado**):
    - Tiene su propia matriz modelo, con su propia posición, rotación (**aún no implementado**) y escala.
- Camera3D.
    - Tiene la capacidad de girar y moverse por si misma.
    - Se maneja mediante el `enum CameraDirection`.
    
###### ContentAPI:
- ContentAPI (**aún no implementado**).

###### Types:
Añadidos `typedef`s para algunas variables de OpenGL y para las variables del MouseState.
- `vertexIndex_t` (`uint32_t `).
- `vertexArrayObject_t` (`uint32_t `).
- `bufferObject_t` (`uint32_t `).
- `cameraVar_t` (`oskFloat_t `).
- `deltaTime_t` (`oskFloat_t `).
- `mouseVar_t` (`oskFloat_t `).
- `buttonState_t` (`bool`).
- `buttonCode_t` (`int32_t `).

###### Util:
- **ELIMINADO:** `DebugOutput` (sustituido por *Log*);
- Log: 
    - Muestra el mensaje por la consola (DEBUG) o guardándolo en un archivo .txt (RELEASE).
    - Se guarda con el comando *"CMD: SAVE"*.

    
## Alpha 2 (2019.12.6a)

Añadidos comentarios. Renderizado 3D.

###### OpenGL RenderAPI:

- Añadida la clase `Textura`.
- Añadidos `RenderMode`:
  - `Straight` (forward rendering).

- RenderAPI:
  - Añadido método para limpiar la ventana.
  - Añadidos métodos para establecer shaders yy la cámara 3D.
  - Añadido método para renderizar un modelo 3D.
- Model:
  - Contiente dos texturas (diffuse y specular).
  
- DirectionalLight: añadido.

###### ContentAPI:

- ContentAPI:
  - Clase para la carga de recursos.
  - Puede cargar:
    - Modelos 3D.
    - Texturas.
  - NO es owner de los recursos.

###### WindowAPI:

- Añadidos `MouseInputMode` y `MouseVisibility`.

- WindowAPI:
  - Almacena el ratio de aspecto.
  - Puede modificar el comportamiento del ratón.

###### Bugfixes

- **Bugfix**: eliminar la ventana correctamente libera memoria de la ventana nativa.

    
## Alpha 3 (2019.12.8a)

###### AudioAPI:

- AudioAPI:
  - Sistema que permite la reproducción de sonidos.
  - Permite ajustar la posición y la velocidad de la cámara de sonidos.
  
- SoundEntity:
  - Representa un sonido.
  - Se reproduce mediante `AudioAPI`.
  - Se pueden ajustar su psoición, dirección y otros ajustes de audio.

###### ContentAPI:

- ContentAPI:
  - Permite cargar nuevos tipos:
    - `Shader`.
    - `SoundEntity`.
     
###### WindowAPI:

- Añadido soporte para el input del ratón sin aceleración.

## 2020.10.13a

###### CollisionSystem:
- **ELIMINADO:**: OBB.
- SAT_Collider:
    - Número indefinido de caras, con un número indefinido de puntos cada una.
    - Deduplicación de puntos.
    - Optimización de caras (para los cálculos de ejes).
    - Optimización para elementos de posición/rotación/escala estáticos.
- SAT_Projection.
- SAT_CollisionInfo: cálculo de *MTV*.

###### ContentManager:
- `std::vector` -> `std::list`.
    - Para evitar punteros desreferenciados.

###### RenderAPI:
- Terreno:
   - Cálculo más fiable de normales.
- PHONG SHADER SET:
    - Normales calculadas correctamente.
    - La parte specular refleja menos luz.
    - ATTENUATION más fiel a la luz real.
        - Fórmula: `<1.0 / (1.0 + 0.1 * distance + (1.0 / (light.infos.y * light.infos.y * 0.001)) * distance * distance)>`.
    - **OPTIMIZACIÓN**: Los frags fuera del radio de luces puntuales ya no se calculan.
    - La información de las luces puntuales se transmite correctamente a los shaders.

###### Transform:
- Cálculo de matriz más correcto.
- Posición global calculada correctamente.
 
###### Types:
- Vector2, Vector3 & Vector4:
    - Operator `+=`, `-=`, `*=`, `/=`.
    - Operator `==`, `!=`.
- Nuevo `typedef`: VectorElementPtr_t.
    - Guarda el index de un elemento concreto de un `std::vector`.
- **EXPERIMENTAL**: DynamicArray<>.

###### Bugfixes:
- **Bugfix:** cargar nuevos elementos en un ContentManager no hará que se pierdan los punteros a los elementos antes cargados.
- **Bugfix:** la posición global se calcula correctamente para Transforms atados a otros.
- **Bugfix:** las normales del terreno se calculan mejor.
- **Bugfix:** las normales se calculan correctamente en los shader para cualquier objeto con `<model != mat4(1.0)>`.


## 2020.10.15a

###### PhysicsEngine:
- PhysicsEntity (**REWORK**):
    - Eliminados *fuerza* y *aceleración angular*.
    - Ahora mantiene información sobre si puede o no moverse en los ejes X, Y y Z.
    - **WIP**: `<ApplyForce>`: aplica una fuerza sobre un punto de la entidad.
- PhysicsScene (**REWORK**):
    - Tiene información sobre el terreno (que ahora tiene un coeficiente de rozamiento).
    - Primero simula todas las entidades, después resuelve colisiones.
    - Las entitdades ya no pueden tener velocidad hacia el suelo si ya están en la superficie del terreno.
    - La rotación de las entidades disminuye con el tiempo.
    - El *MTV* de una colisión se aplica a ambas entidades, siempre que ninguna de ellas tenga movimiento limitado.
        - Si una de ellas tenga movimiento limitado, todo el MTV se aplica a la otra entidad.
    - Se aplica movimiento a las dos entidades de una colisión, dependiendo de su velocidad y masa.
    - Se aplica rotación a las dos entidades de una colisión (rotación esférica).

###### CollisionSystem:
- ColliderCollisionInfo.
    - Contiene información sobre una colisión *Collider*-*Collider*:
        - IsBroadColliding.
        - SAT_1 & SAT_2: SAT_Colliders que están colisionando.
- Collider:
    - Ahora calcula correctamente las colisiones *Collider*-*Collider*.
    - Añadido `<SetPosition>`, para poder establecer la posición del *BroadCollider* independientemente del tipo que sea.
- CollisionBox:
    - Añadido `<GetMin>` para obtener la esquina contraria a `GetTop`.
- RayCast.
    - Efectua un *raycast* contra un `SAT_Collider`.
    - A partir del origen del rayo y su dirección.
    - Sin límite de longitud.
    - Devuelve la información: 
        - Si hay intersección (`<true>` o `<false>`).
        - Distancia desde el origen del rayo (si ha habido intersección).
        
###### Util:
- Math:
    - Añadido `<CompareFloats>`, para comparar la igualdad de dos `float`s.

###### Bugfixes:
- **Bugfix**: *OSKengine* ya no crashea al cerrarse habiendo sido iniciado en modo **DEBUG** sin tener acceso a las capas de validación.
- **Bugfix**: `Collider.IsColliding` y `Collider.GetCollisionInfo` ya calculan correctamente las colisiones.
- **Bugfix**: Las entitdades ya no pueden tener velocidad hacia el suelo si ya están en la superficie del terreno.


## Alpha 16 (2020.10.20a)

- Añadido soporte para DLL (`<OSKAPI_CALL>`) a todos los elementos que no lo tenían.
- Primeros pasos para dejar el soporte OpenGL oficialmente.
- Añadida documentación a clases que aún no tenían.
- Normalizados los `#include` en varios archivos.

###### Types:
- Transform:
    - Mejora global.
    - Ya no es un `template`, siemrpe usa `<float>`.
    - Ahora usa `Quaternion` en vez de ángulos euclídeos.
    - Permite la rotación tanto en ejes locales como en ejes del mundo.
    - Ya no almacena independientemente:
        - Offsets de rotación y posición.
        - Rotación global.
    - Ahora usa la matriz padre para hacer los cálculos.
    - Renombrados *Position* y *Scale* a *LocalPosition* y *LocalScale*.
    - Ya no tiene u constructor a partir de una matriz, ni tiene comportamiento distinto para esqueletos.

- Quaternion:
    - Representa la orientación de u objeto.
    - Puede rotar, tanto en ejes locales como del mundo.

###### PhysicsEngine:
- PhysicsEntity:
    - Puede calcular la velocidad lineal de un punto.
    - Se puede configurar su comportamiento en el motor de físicas (`<PhysicalEntityResponseBitFlags>`):
        - Respuesta ante colisiones: moverse (*MTV*), aceleración y rotación.
        - Respuesta que produce a otras entidades en colisiones: aceleración y rotación.
- PhysicsEngine:
    - Mejorada la respuesta de respuesta de rotación:
        - Ya no es rotación esférica.
        - Toma en cuenta el punto de colisión.
        - Velocidad de rotación independiente de la masa de la otra entidad.
    - Se puede su comportamiento:
        - Simulación: `<PhysicalSceneSimulateBitFlags>`:
            - Movimiento, rotación y aceleración (global).
        - Respuesta de colisiones (`<PhysicalEntityInteractionBitFlags>`):
            - Aceleración y rotación.
###### CollisionSystem:
- SAT Collider:
    - `<GetCollisionInfo>` ahora calcula el punto de colisión.
    -  Ya no se puede especificar si la rotación/posición/escala son estáticos, solo hay un único `<IsStatic>` que incluye las tres anteriores.
- SAT Collision info:
    - Ahora incluye cual es el punto de colisión, tanto en A como en B.
    - Ahora incluye cual es el eje del MTV.
    
###### RenderAPI:
- Camera3D:
    - Ahora usa `float` en vez de `double`.
    - Ahora almacena la rotación en X e Y de manera local.

###### AudioAPI:
- Ahora usa la *Camera3D*, en vez de *OldCamera3D*.

###### UserInterface:
- Ya no utiliza el renderizador de legado.


## Alpha 16.1 (2020.10.21a)

###### PhysicsEngine:
- Ahora calcula colisiones con el terreno. Hay tres modalidades:
    - `<PhysicalSceneTerrainResolveType::DO_NOTHING>`: ignora cualquier colisión.
    - `<PhysicalSceneTerrainResolveType::CHANGE_HEIGHT_ONLY>`: simplemente cambia la posición de la entidad para que no termine por debajo del terreno.
        - Igual que funcionaba antes, pero ahora se puede indicar la distancia desde el centro de la entidad y el punto que se compara con el terreno.
    - `<PhysicalSceneTerrainResolveType::RESOLVE_DETAILED>`.
        - Detección y resolución de colisiones avanzada.
        - Computacionalmente caro.
        - Aplica cambio de posición y rotación.
        - Funciona bien juando el tamaño de las entidades y los *quads* del terreno son comparables.
        - Dispone de optimizaciones:
            - `<TerrainCollisionDetectionPrecision>`: para valores mayores que 1, disminuye el número de triángulos que se comparan (4 veces menos por cada unidad).
            - `<TerrainCollisionDetectionSingleTimePerFrame>`: una vez que se produzca una colisión, ignora el resto hasta el siguiente frame.
        - Puede configurarse un margen de error, para evitar comportamientos erráticos.

###### RenderAPI:
- SpriteBatch:
    - Ahora usa `std::deque` en vez de `std::vector`.
        - Evita excesivos movimientos de memoria.
        - Aumento de rendimiento: +10% para 176 elementos.


## 2020.10.25a

- Comienzo de la marca **OSK** y **OSKengine**: añadidos iconos de ventana y de inicio.
- Optimizaciones para el renderizado de texto.

###### ContentManager:
- Ahora las fuentes se cargan en una sola textura, en vez de usar una textura por cada caracter.
- Ahora puede inicializar los buffers de un sprite vacío.
    - Serán destruidos por el ContentManager.

###### RenderAPI:
- ReservedText.
    - Permite reservar memoria para almacenar los sprites de un texto.
    - Puede asignarse memoria mediante `<Reserve()>` o `<SetText()>`.
- SpriteBatch:
    - Renderizado de texto:
        - Permite renderizar *ReservedText*s.
        - Puede precalcular *ReservedText*s, para precalcularlo una sola vez, en vez de cada frame.
        - Optimización: no se renderizarán caracteres que estén fuera de la pantalla.
        - Optimización: se reserva memoria para los sprites de todo un texto de una vez (*SpriteContainer*) en vez de uno a uno.
    - Optimización: tanto para texto como para sprite, se usa *SpritreContainer* en vez de *Sprite*.
- SpriteContainer:
    - Permite almacenar juntos varios sprites que se van a renderizar.
    - Sólo se modifica la memoria una vez por container, en vez de una vez por sprite.
- Sprite: los indices y el index buffer son ahora estáticos.

###### Bugfixes:
- **Bugfix**: las *texcoords* de los sprites se establecen correctamente (X, Y, WIDTH, HEIGHT en vez de X1, Y1, X2, Y2).


## 2020.10.26a
- Eliminado oficialmente el soporte de OpenGL.

###### OpenGL RenderAPI (**ELIMINADO**)
- RenderAPI: **eliminado**.
- Material: **eliminado**.
- Shader: **eliminado**.
- Mesh: **eliminado**.
- OldModel: **eliminado**.
- OldCamera: **eliminado**.
- OldFont: **eliminado**.
- OldVertex: **eliminado**.
- RenderAPI: **eliminado**.
- Animación esqueletal:
    - Bone: **eliminado**.
    - Skeleton: **eliminado**.
    - Animation: **eliminado**.

###### ContentAPI
- ContentAPI: **eliminado** (reempazado por *ContentManager*).

###### UserInterface
- Ya no usa funciones del renderizador OpenGL.

###### RenderAPI:
- *VulkanRenderer* renombrado a *RenderAPI*.


## 2020.11.13a

- *Validation layers* reactivados.
- Renderizado a texturas.

###### RenderAPI
- RenderTarget (**WIP!**):
    - Sirve para renderizar una imagen a una textura en vez de a la pantalla.
    - Contiene:
        - Sprite donde se renderiza.
        - Framebuffers.
        - Referencias a pipelines.

- Framebuffer:
    - Encapsula un framebuffer de Vulkan.

- RenderAPI:
    - Ahora renderiza por defecto a un render target.
        - Puede renderizar a una resolución distinta a la final.
    - Ya no tiene LightUBOs.
    - Se pueden crear `<DescriptorSets>` para *n* elementos, en vez de para `<Settings.MaxTextures>` elementos.

- VulkanImageGen:
    - `<TransitionImageLayout`> ahora puede utilizar un Command Buffer que le pases, en vez del SingleTimeCommandBuffer por defecto.

- RenderpassSubpass:
    - Ahora puedes añadir varias dependencias.

- 2D rendering:
    - Ahora la cámara está embebida en los push constants.
    - Los shaders 2D no usan la cámara del UBO.

###### Bugfixes:
- **Bugfix**: `<SkyboxDescriptorLayout.PoolSize = 1>`.
- **Bugfix**: `<LoadModelTexture()>` ahora usa correctaments `<VK_IMAGE_USAGE_TRANSFER_SRC_BIT>` para generar los mipmaps.


## 2020.12.6a

###### RenderAPI

Aztualizado a **Vulkan 1.2.154**.

- ShadowMap:
    - Implementa un sombreado de luz direccional.
    - Contiene un `GraphicsPipeline`, `descriptor layout` y el `'RenderTarget'`.
    - Pueden configurarse su resolución, densidad y distancia de renderizado.

- RenderizableScene:
    - Incluye un `ShadowMap`: renderiza sobras.

- PHONG SHADER SET:
    - Añadida niebla distante.
    - Ahora calcula sobreado de luz direccional.
        - Antiailasing 3x3.

- RenderStage:
    - Almacena una escena y spritebatches.
    - Puede usarse para juntar llamadas de renderizado de distintas fuentes.

- RenderAPI:
    - Se pueden añadir y eliminar spritebatches al stage principal.
    - `<CreateNewRenderpass()`>.
    - `<CreateNewRenderTarget()`>.
    - Almacena los stages principales y de un único uso.

- RenderTarget:
    - Formalizado:
        - Funciones para crear sus elementos.
        - Puede transicionar sus imágenes para ser usadas como target o como textura.

- SpriteBatch:
    - Puede tener una cámara específica, independiente al resto de spritebatches.

- VulkanImageGen:
    - `<TransitionImageLayout`> ahora soporta layouts de *depth_stencil*.

- Misc:
    - **EXPERIMENTAL**: `OSK::SafeDelete<>()`.

###### Bugfixes
- **Bugfix**: ahora las texturas eliminan sus descriptor sets al eliminarse.
- **Bugfix**: ahora los graphics pipelines no intentan eliminar sus componentes que ya habían sido eliminados.
- **Bugfix**: `'PhongPipeline` renderiza correctamente en la resolución objetivo.


## 2021.2.22a

Primera implementación de un sistema ECS.

###### ECS

- Component:
    - OnCreate().
    - OnTick().
    - OnRemove().

- ComponentArray:
    - Almacena los componentes de un mismo tipo en un packed array.
    - Todos los componentes estan enlazados al ID de un GameObject.

- ComponentManager:
    - Registra un componente.
    - Obtiene el componente de un tipo de un GameObject.
    - Añade / elimina un componente de un GameObject.

- EntityComponentSystem: almacena referencias a:
    - ComponentManager.
    - SystemManager.
    - GameObjectManager.

- GameObject:
    - Representa la entidad.
    - OnCreate().
    - OnTick().
    - OnRemove().
    - Puede añadirse / eliminarse componentes.

- GameObjectManager:
    - Puede crear y destruir objetos.

- System:
    - OnCreate().
    - OnTick().
    - OnRemove().

- SystemManager:
    - Registra sistemas y sus signatures (componentes necesarios).

Sistemas creados:

- RenderSystem3D

###### WindowAPI
 
- Ya no puede cargar OpenGL.

###### AudioAPI

Transformado en sistema.

###### Physics

Transformado en sistema.


## 2021.2.22b

###### ECS

- InputSystem:
    - Hace reaccionar a los componentes de input.
    
    - InputComponent:
        - Reacciona a eventos de imput.
        - Permite registrar funciones que se ejecutarán al producitse un evento con el mismo nombre.

    - InputEvent: 
        - Nombre.
        - Teclas y botones enlazados.
        

## 2021.2.24a

###### ECS
   
- GameObject:
    - Ahora tiene Transform.

- RenderSystem3D:
    - Añadido método `OnDraw()`.
    - Añadido `ModelComponent`.

- InputSystem:
    - Ahora diferencia entre eventos prolongados y eventos de una sola vez.

- OnTickSystem: añadido.

###### Game

- Añadida clase base Game:
    - Contiene el `ECS`.
    - Métodos: `OnCreate()`, `LoadContent()`, `OnExit()`, `OnTick()`, `OnDraw()`. `` ``

###### IMGI
   
- Añadido soporte (no completado).


## 2021.2.24b

###### RenderAPI

- Eliminado soporte para interfaz de usuario.

###### IMGI
   
- Eliminado.
   

## 2021.2.24c

###### RenderAPI

- Model:
    - Ahora almacena su propio UBO para animaciones.


## 2021.2.25a

Ahora las animaciones usan un formato interno.

###### RenderAPI

- Animaciones:
    - SAnimation: representa una animación.
        - Tiene los SNodeAnims.
    - SNodeAnim: representa un nodo animado.
        - Tiene los valores de posición, escala y rotación para los keyframes de la animación.
    - SNode: representa un nodo.
    - SVectorKey: representa un vector y su keyframe.
    - SQuaternionKey: representa un vector y su keyframe.
    - Ahora usan `glm::mat4`.
    
###### ContentAPI

- ContentManager:
    - LoadAnimatedModel():
        - Carga la información de Assimp al formato interno.


## 2021.2.27a

Ahora las animaciones usan un buffer dinámico.

- AnimatedModel:
    - Se puede configurar la velocidad de la animación.
    - Tiene el offset para acceder a su animación en el buffer.

- DescriptorSet:
    - Soporte para buffers dinámicos.

- RenderAPI:
    - Puede crear buffers dinámicos.

###### ContentAPI

- ContentManager:
    - LoadAnimatedModel():
        - Se le asigna a cada modelo un ID único que representa su animaciónn en el buffer dinámico.


## 2021.2.27b

###### Bugfixes
- **Bugfix**: las estructuras de animación ya no tienen memory leaks.


## 2021.2.27c

###### RenderAPI

- AnimatedModel:
    - **Optimización:** ahora los nodos contienen un puntero hacia los nodos animados que le coresponden.

###### Game

- Game:
    - Ahora tiene su propio `SpriteBatch` por defecto.

###### Bugfixes
- **Bugfix**: ahora los framebuffers de efectos de post-procesamiento usan la imagen de profundidad.
- **Bugfix**: ahora la escena principal no se renderiza dos veces.


## 2021.2.28a

Eliminada la libreria KTX.

###### ContentAPI

- ContentManager:
    - LoadSkybox():
        - Ya no usa el formato `.ktx`.
        - Toma el path a las 6 imágenes del skybox.


## 2021.3.2a

###### RenderAPI

- RenderAPI:
    - Añadido soporte para postprocesamiento.
    - Añadido FXAA.

- RenderSystem3D: ahora renderiza el spritebatch.

- GraphicsPipeline:
    - Ahora puede aceptar más de un `PushConstant`.

- AnimatedModel:
    - SetAnimation(): ahora puede aceptar el nombre de la animación. 

###### ECS

- RenderSystem3D: ahora renderiza el spritebatch.

###### Types

- Quaternion:
    - Interpolate()


## 2021.3.3a

###### Bugfixes
- **Bugfix**: ahora los framebuffers de efectos de post-procesamiento usan la imagen de profundidad.


## 2021.3.3b

###### RenderAPI

Ahora usa los nuevos VulkanBuffer.

- VulkanBuffer:
    - Ahora manejan ellos mismos sus operaciones, en vez de hacerse a través del RenderAPI.
    - Write() / WriteMapped().
    - Map() / Unmap().
    - Allocate() / Destroy().
    
###### Colections

- DynamicArray: añadido.


## 2021.3.4a

###### RenderAPI

Ahora usa viewports dinámicos, que se especifican una vez por renderpass.
Primeras clases del sistema de materiales.

- RenderAPI:
    - `<SetViewport()>`: establece el viewport (por defecto, el tamaño de la ventana).
    
- Material System:
    - Enum `<MaterialBindingType>`: `DATA_BUFFER`, `DYNAMIC_DATA_BUFFER`, `TEXTURE`.
    - Enum `<MaterialBindingShaderStage >`: `VERTEX`, `FRAGMENT`.

    - MaterialBinding: un binding del material (tipo + shader stage).

    - MaterialInstance: almacena el descriptor set.

    - MaterialPool: 
        - Almacena los descriptor pools. 
        - Se usa para crear instancias del material.
        - Almacena una lista de MaterialPoolData.

    - MaterialPoolData:
        - Almacena un descriptor pool con 32 espacios.

###### Collections

- DynamicArray:
    - RemoveAndMoveLast: elimina un elemento y lo intercambia con el último.
    - GetPosition: devuelve la posición en el array del elemento.
    - HasElement: true si el elemento existe en el array.


## Alpha 17 (2021.4.1a)

###### RenderAPI

Introducido el sistema de materiales.

- Material System:
    - Material:
        - Representa un tipo de material.
        - Contiene:
            - MaterialPool.
            - DescriptorLayout.
            - GraphicsPipelines (una por renderpass).
            - Bindings.

    - MaterialInstance:
        - Ahora tiene funciones para establecer texturas o buffers.

    - MaterialPoolData:
        - Contiene un descriptor pool para 32 descriptor sets.
        - Los descriptor sets se 'liberan' al eliminarse un MaterialInstance y se pueden reutilizar.

    - MaterialPool:
        - Contiene una lista de MaterialPoolData.

    - MaterialSystem:
        - Controla los materiales.
        - Puede registrar materiales y renderpasses.

    - MaterialBinding:
        - Ahora almacena su nombre.
        
    - MaterialPipelineInfo:
        - Información de un pipeline, para crearse en Material cada vez que se añada un renderpass.

- DescriptorLayout:
    - Ahora solo almacena el descriptor layout de vulkan, y los bindings.

- DescriptorPool:
    - Almacena el descriptor pool de vulkan.

- DescriptorSet:
    - Ahora puede actualizar sus valores.

- RenderizableScene:
    - Ya no almacena graphics pipelines ni descriptor layouts.
    - Ya no puede crear descriptor sets.

- RenderSystem3D:
    - Ya no comprueba los descriptors sets de los modelos.

- ShadowMap: 
    - Ya no puede crear descriptor sets.

- Sprite:
    - `UpdateSpriteMaterial()`.

- ReservedText:
    - **ELIMINADO**.

- SpriteBatch:
    - Ahora usa `ArrayQueue`.
    - Ya no renderiza `ReservedText`.

- SpriteContainer:
    - Ya no contiene uno o varios Sprites.
    - Ahora contiene una copia de algunos aspectos de un Sprite en concreto.

- Terrain:
    - Ahora tiene materiales normal y de sombras.

- Texture:
    - Sólo contiene VulkanImage y tamaño.
    
- ModelTexture:
    - **ELIMINADO**.

- SkyboxTexture:
    - **ELIMINADO**.

- VulkanBuffer:
    - Contiene el tamaño de cada estructura individual en un dynamic UBO.

- RenderAPI:
    - Ya no puede crear graphics pipelines de PHONG y Skybox.
    - Ya no puede crear descriptor sets/layout de PHONG y Skybox.
    - No almacena graphics pipelines.
    - No almacena descriptor layouts.
    - No crea graphics pipelines por defecto.
    - Eliminado `GetCommandBuffers()`.
    - Añadido MaterialSystem.

- Model:
    - Tiene dos materiales: normal y shadows.

- Font:
    - Ahora tiene su propio material.

###### ContentAPI

- ContentManager:
    - `DefaultTexture`: ahora es de tipo `Texture`.
    - `LoadModelTexture()`: **ELIMINADO**.
    - `ModelTextures`, `SkyboxTextures`: **ELIMINADO**.
        
###### Colections

- ArrayQueue: **añadido**.
- ArrayStack: **añadido**.
- LinkedList: **añadido**.
  
###### Memory

- SharedPtr: **añadido**.
- UniquePtr: **añadido**.

###### Bugfixes

- **Bugfix**: ahora eliminar el RenderAPI elimina correctamente sus contenidos.


## 2021.4.23a

Gran actualización de la documentación.
Las dependencias están ahora localizadas dentro del proyecto.
Los archivos .h con varias clases se han dividido en varios archivos .h.
La gran mayoría de funciones en .h han sido movidas a .cpp.

###### RenderAPI

- RenderTarget:
    - Ya no almacena pipelines.
    
- GPUDataBuffer:
    - Renombrado `VulkanBuffer` a `GPUDataBuffer`.

- GPUImage:
    - Renombrado `VulkanImage` a `GPUImage`.
    
- Vertex:
    - Ahora solo tiene un único `attributes description`.

- DirectionsEnum: **ELIMINADO**.
- ModelFormat: **ELIMINADO**
- RenderMode **ELIMINADO**
- RenderOutput **ELIMINADO**
- Rendersettings **ELIMINADO**

###### WindowAPI

- WindowAPI:
    - Ya no contiene información de una versión de OpenGL.
    - **ELIMINADO** `SwapBuffers()` (OpenGL).

- Renombrado `MouseMovementMode` a `MouseAccelerationMode`.

###### ECS

- GameObject:
    - Renombrado `Create` a `Spawn`.
    
- Scene: **ELIMINADO**.
    
###### Memory

- SafeDelete & SafeDeleteArray: ahora son parte de Memory.
- AddPtrOffset: ahora es parte de Memory.

###### OSK

- OSK_SHOW_ERROR -> **ELIMINADO** (Usar Log).
- OSKresult: **ELIMINADO**.
- OSK_NOT_IMPLEMENTED: **ELIMINADO**.
- OSK_OBSOLETE: **ELIMINADO**.

- Ahora almacena la versión en el .dll (.cpp).

###### Bugfixes

- **Bugfix**: FileIO::ReadFromFile hace newline correctamente, en vez de añadir (int)'\n'.


## 2021.5.2a

Mayor versatilidad en los GameObjects.

###### ECS
   
- GameObject:
    - Ahora son creadas y son propiedad de `EntityComponentSystem`.
        - Para eliminarlo, se tiene que llamar a `GameObject::Remove()`.
    - Ahora las clases derivadas tienen que ser registradas con `OSK_GAME_OBJECT()`.
    - Ahora tiene métodos para añadir un modelo 3D (que automáticamente añande el componente de modelos).
    - `Create()` renombrado a `Spawn()`.

###### Bugfixes

- **Bugfix**: spawnear un GameObject ahora ocupa una ID, y no dos.


## 2021.5.14a

Reformateado el código para que cumpla el nuevo estándar de C++ de OSK.

Añadido el sistema de escenas y soporte para cargarlas.

###### ECS
   
- Scene:
  - Representa una escena que combina entidades con una escena renderizable 3D.
  - Pueden cargarse escenas desde un archivo `.sc` (véase la especificación en `Documentation/ScenerySystemSpec.md`).
  - Las entidades spawneadas desde la escena serán eliminadas al descargarse (o cargarse otra escena).
    - Se pueden spawnear objetos persisitentes a través del `EntityComponentSystem`.
  - Se puede obtener un GameObject dado su nombre de instancia.

- SceneSystem.Loader.Scanner:
  - Implementa un tokenizador capaz de leer scripts de escena.
  - Puede cargar los tokens del script desde un archivo.
  - Los tokens son procesados por la propia escena, que los ejecuta.
  - Tokens incluidos:
    - `version`.
    - `skybox`.
    - `terrain`.
    - `place`.

- GameObject:
  - Ahora almacena un nombre que lo identifica (`instanceName`).

- Entity Component System:
  - Se puede obtener un GameObject dado su nombre de instancia.

###### RenderAPI

- Point light:
  - Ahora tiene funciones para establecer sus parámetros de una manera más sencilla.
   
###### WindowAPI

- MouseState:
  - Ahora contiene la diferencia de scroll desde el estado anterior.

###### Colections

- DynamicArray:
  - Ahora tiene constructor y operador de movimiento.
  
###### Bugfixes

- **Bugfix**: eliminar un GameObject (con `GameObject::Remove()`) ya no crashea OSKengine.
- **Bugfix**: eliminar un modelo animado ya no crashea OSKengine.


## 2021.5.27a

Añadido soporte para hasta 4 mandos.

###### WindowAPI

- GamepadState:
  - Almacena el estado de los botones y los ejes de un mando en concreto.
  - Actualiza internamente los estados de los 4 primeros mandos.
  - Los gatillos y los joysticks funcionan con un valor 0.0 - 1.0.

###### ECS

- InputSystem:
  - AxisInputEvent:
    - Evento de input que se ejecuta a traves de un eje.
    - Puede funcionar con valores 0.0 - 1.0 ó -1.0 - 1.0.   
    - Funciona con ejes, o con botones.
  - Todos los eventos pueden responder a los botones del mando.
  - Cada evento tiene asignado un mando.

###### OSK

- OSK_ASSERT: 
  - Comprueba un valor booleano.
  - Si el resultado es falso, se imprime un mensaje de error.
- OSK_CHECK: 
  - Comprueba un valor booleano.
  - Si el resultado es falso, se imprime un mensaje de aviso.


## 2021.5.29a

Nuevo sistema de interfaz de usuario. 
Primeros pasos para un editor.

###### RenderAPI

- Sprite:
  - Ahora soportan transparencias.

###### ContentAPI

- ContentManager:
  - Ahora puede cargar texturas con un filtro en concreto:
    - NEAREST: pixelado.
    - LINEAR: no pixelado (por defecto).

###### UI

- UiElement:
  - Representa un elemento de interfaz de usuario.
  - Única clase del sistema de UI.
  - Puede personalizarse su comportamiento mediante `UiFuncionality`.

- UiFuncionality:
  - Sistema que permite añadir funcionalidades extra a un `UiElement`:
    - Draggable
    - Checkbox
    - Slider
    - Dropdown
  
###### Bugfixes

- **Bugfix**: la cámara actualiza correctamente su `targetSize` para que sea igual que el tamaño de la ventana.
- **Bugfix**: los sprites se renderizan correctamente en su lugar cuando cambia la resolución de la pantalla.
- **Bugfix**: los caracteres de una fuente ya no tienen los bordes oscuros.


## 2021.6.10a

Asignador de memoria de Vulkan.


Actualización de los estándares de C++ de OSK:
- Los atributos pasados a funciones que son modificados por la función se pasan por puntero y no por referencia.

###### RenderAPI

- VulkanMemoryAllocator:
  - Clase que maneja la memoria de la GPU.
  - Agrupa asignaciones de memoria pequeña en asignaciones grandes de bloques.
  - De momento solo funciona con buffers de GPU.
    - VulkanMemoryBlock:
      - Bloque de memoria de la GPU.
      - Los subbloques representan una región de memoria de un bloque.
    - VulkanMemorySubblock:
      - Región de memoria que pertenece a un buffer.
  - Los bloques tienen un tamaño mínimo de 32 megabytes.
  - Los subbloques se asignan linealmente.
  - Al liberarse un subbloque, su región de memoria queda marcada para ser reutilizada.

###### Assert

- OSK_ASSERT_FALSE: 
  - Comprueba un valor booleano.
  - Si el resultado es verdadero, se imprime un mensaje de error.
- OSK_CHECK_FALSE: 
  - Comprueba un valor booleano.
  - Si el resultado es verdadero, se imprime un mensaje de aviso.

###### Bugfixes

- **Bugfix**: ahora no se muestran mensajes innecesarios de Vulkan (DEBUG).
- **Bugfix**: cambiar el tamaño de la ventana ya no genera memory leaks (por la recreación del swapchain).
- **Bugfix**: los GPU buffers ya no generan memory leaks en la GPU.


## WIP

TODO
