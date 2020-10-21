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

    
## Alpha 2 (2019.12.4a)


###### 

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