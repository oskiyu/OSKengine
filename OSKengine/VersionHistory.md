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
    - A�n no es funcional.
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
    - Termina la ejecuci�n mostrando un mensaje.


## Alpha 1 (2019.12.4a)

###### WindowAPI:
- Mouse Input.
    - MouseState:
        - Mantiene informaci�n sobre la posici�n y los botones pulsados en el rat�n.
        - Tambi�n matiene informaci�n del scroll.
- WindowAPI:
    - Se puede establecer la versi�n de OpenGL.
    - Tiene su propio MouseState actualizado.
        - Puede actualizar otros MouseState.
        
###### OpenGL RenderAPI:
- Vertex: posici�n, vector normal y TexCoords.
- Mesh.
    - Se crea a partir de una lista de v�rtices.
    - Se dibuja a si mismo.
- Model.
    - Se carga a trav�s de un archivo (**a�n no implementado**):
    - Tiene su propia matriz modelo, con su propia posici�n, rotaci�n (**a�n no implementado**) y escala.
- Camera3D.
    - Tiene la capacidad de girar y moverse por si misma.
    - Se maneja mediante el `enum CameraDirection`.
    
###### ContentAPI:
- ContentAPI (**a�n no implementado**).

###### Types:
A�adidos `typedef`s para algunas variables de OpenGL y para las variables del MouseState.
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
    - Muestra el mensaje por la consola (DEBUG) o guard�ndolo en un archivo .txt (RELEASE).
    - Se guarda con el comando *"CMD: SAVE"*.

    
## Alpha 2 (2019.12.4a)


###### 

## 2020.10.13a

###### CollisionSystem:
- **ELIMINADO:**: OBB.
- SAT_Collider:
    - N�mero indefinido de caras, con un n�mero indefinido de puntos cada una.
    - Deduplicaci�n de puntos.
    - Optimizaci�n de caras (para los c�lculos de ejes).
    - Optimizaci�n para elementos de posici�n/rotaci�n/escala est�ticos.
- SAT_Projection.
- SAT_CollisionInfo: c�lculo de *MTV*.

###### ContentManager:
- `std::vector` -> `std::list`.
    - Para evitar punteros desreferenciados.

###### RenderAPI:
- Terreno:
   - C�lculo m�s fiable de normales.
- PHONG SHADER SET:
    - Normales calculadas correctamente.
    - La parte specular refleja menos luz.
    - ATTENUATION m�s fiel a la luz real.
        - F�rmula: `<1.0 / (1.0 + 0.1 * distance + (1.0 / (light.infos.y * light.infos.y * 0.001)) * distance * distance)>`.
    - **OPTIMIZACI�N**: Los frags fuera del radio de luces puntuales ya no se calculan.
    - La informaci�n de las luces puntuales se transmite correctamente a los shaders.

###### Transform:
- C�lculo de matriz m�s correcto.
- Posici�n global calculada correctamente.
 
###### Types:
- Vector2, Vector3 & Vector4:
    - Operator `+=`, `-=`, `*=`, `/=`.
    - Operator `==`, `!=`.
- Nuevo `typedef`: VectorElementPtr_t.
    - Guarda el index de un elemento concreto de un `std::vector`.
- **EXPERIMENTAL**: DynamicArray<>.

###### Bugfixes:
- **Bugfix:** cargar nuevos elementos en un ContentManager no har� que se pierdan los punteros a los elementos antes cargados.
- **Bugfix:** la posici�n global se calcula correctamente para Transforms atados a otros.
- **Bugfix:** las normales del terreno se calculan mejor.
- **Bugfix:** las normales se calculan correctamente en los shader para cualquier objeto con `<model != mat4(1.0)>`.


## 2020.10.15a

###### PhysicsEngine:
- PhysicsEntity (**REWORK**):
    - Eliminados *fuerza* y *aceleraci�n angular*.
    - Ahora mantiene informaci�n sobre si puede o no moverse en los ejes X, Y y Z.
    - **WIP**: `<ApplyForce>`: aplica una fuerza sobre un punto de la entidad.
- PhysicsScene (**REWORK**):
    - Tiene informaci�n sobre el terreno (que ahora tiene un coeficiente de rozamiento).
    - Primero simula todas las entidades, despu�s resuelve colisiones.
    - Las entitdades ya no pueden tener velocidad hacia el suelo si ya est�n en la superficie del terreno.
    - La rotaci�n de las entidades disminuye con el tiempo.
    - El *MTV* de una colisi�n se aplica a ambas entidades, siempre que ninguna de ellas tenga movimiento limitado.
        - Si una de ellas tenga movimiento limitado, todo el MTV se aplica a la otra entidad.
    - Se aplica movimiento a las dos entidades de una colisi�n, dependiendo de su velocidad y masa.
    - Se aplica rotaci�n a las dos entidades de una colisi�n (rotaci�n esf�rica).

###### CollisionSystem:
- ColliderCollisionInfo.
    - Contiene informaci�n sobre una colisi�n *Collider*-*Collider*:
        - IsBroadColliding.
        - SAT_1 & SAT_2: SAT_Colliders que est�n colisionando.
- Collider:
    - Ahora calcula correctamente las colisiones *Collider*-*Collider*.
    - A�adido `<SetPosition>`, para poder establecer la posici�n del *BroadCollider* independientemente del tipo que sea.
- CollisionBox:
    - A�adido `<GetMin>` para obtener la esquina contraria a `GetTop`.
- RayCast.
    - Efectua un *raycast* contra un `SAT_Collider`.
    - A partir del origen del rayo y su direcci�n.
    - Sin l�mite de longitud.
    - Devuelve la informaci�n: 
        - Si hay intersecci�n (`<true>` o `<false>`).
        - Distancia desde el origen del rayo (si ha habido intersecci�n).
        
###### Util:
- Math:
    - A�adido `<CompareFloats>`, para comparar la igualdad de dos `float`s.

###### Bugfixes:
- **Bugfix**: *OSKengine* ya no crashea al cerrarse habiendo sido iniciado en modo **DEBUG** sin tener acceso a las capas de validaci�n.
- **Bugfix**: `Collider.IsColliding` y `Collider.GetCollisionInfo` ya calculan correctamente las colisiones.
- **Bugfix**: Las entitdades ya no pueden tener velocidad hacia el suelo si ya est�n en la superficie del terreno.


## Alpha 16 (2020.10.20a)

- A�adido soporte para DLL (`<OSKAPI_CALL>`) a todos los elementos que no lo ten�an.
- Primeros pasos para dejar el soporte OpenGL oficialmente.
- A�adida documentaci�n a clases que a�n no ten�an.
- Normalizados los `#include` en varios archivos.

###### Types:
- Transform:
    - Mejora global.
    - Ya no es un `template`, siemrpe usa `<float>`.
    - Ahora usa `Quaternion` en vez de �ngulos eucl�deos.
    - Permite la rotaci�n tanto en ejes locales como en ejes del mundo.
    - Ya no almacena independientemente:
        - Offsets de rotaci�n y posici�n.
        - Rotaci�n global.
    - Ahora usa la matriz padre para hacer los c�lculos.
    - Renombrados *Position* y *Scale* a *LocalPosition* y *LocalScale*.
    - Ya no tiene u constructor a partir de una matriz, ni tiene comportamiento distinto para esqueletos.

- Quaternion:
    - Representa la orientaci�n de u objeto.
    - Puede rotar, tanto en ejes locales como del mundo.

###### PhysicsEngine:
- PhysicsEntity:
    - Puede calcular la velocidad lineal de un punto.
    - Se puede configurar su comportamiento en el motor de f�sicas (`<PhysicalEntityResponseBitFlags>`):
        - Respuesta ante colisiones: moverse (*MTV*), aceleraci�n y rotaci�n.
        - Respuesta que produce a otras entidades en colisiones: aceleraci�n y rotaci�n.
- PhysicsEngine:
    - Mejorada la respuesta de respuesta de rotaci�n:
        - Ya no es rotaci�n esf�rica.
        - Toma en cuenta el punto de colisi�n.
        - Velocidad de rotaci�n independiente de la masa de la otra entidad.
    - Se puede su comportamiento:
        - Simulaci�n: `<PhysicalSceneSimulateBitFlags>`:
            - Movimiento, rotaci�n y aceleraci�n (global).
        - Respuesta de colisiones (`<PhysicalEntityInteractionBitFlags>`):
            - Aceleraci�n y rotaci�n.
###### CollisionSystem:
- SAT Collider:
    - `<GetCollisionInfo>` ahora calcula el punto de colisi�n.
    -  Ya no se puede especificar si la rotaci�n/posici�n/escala son est�ticos, solo hay un �nico `<IsStatic>` que incluye las tres anteriores.
- SAT Collision info:
    - Ahora incluye cual es el punto de colisi�n, tanto en A como en B.
    - Ahora incluye cual es el eje del MTV.
    
###### RenderAPI:
- Camera3D:
    - Ahora usa `float` en vez de `double`.
    - Ahora almacena la rotaci�n en X e Y de manera local.

###### AudioAPI:
- Ahora usa la *Camera3D*, en vez de *OldCamera3D*.

###### UserInterface:
- Ya no utiliza el renderizador de legado.


## Alpha 16.1 (2020.10.21a)

###### PhysicsEngine:
- Ahora calcula colisiones con el terreno. Hay tres modalidades:
    - `<PhysicalSceneTerrainResolveType::DO_NOTHING>`: ignora cualquier colisi�n.
    - `<PhysicalSceneTerrainResolveType::CHANGE_HEIGHT_ONLY>`: simplemente cambia la posici�n de la entidad para que no termine por debajo del terreno.
        - Igual que funcionaba antes, pero ahora se puede indicar la distancia desde el centro de la entidad y el punto que se compara con el terreno.
    - `<PhysicalSceneTerrainResolveType::RESOLVE_DETAILED>`.
        - Detecci�n y resoluci�n de colisiones avanzada.
        - Computacionalmente caro.
        - Aplica cambio de posici�n y rotaci�n.
        - Funciona bien juando el tama�o de las entidades y los *quads* del terreno son comparables.
        - Dispone de optimizaciones:
            - `<TerrainCollisionDetectionPrecision>`: para valores mayores que 1, disminuye el n�mero de tri�ngulos que se comparan (4 veces menos por cada unidad).
            - `<TerrainCollisionDetectionSingleTimePerFrame>`: una vez que se produzca una colisi�n, ignora el resto hasta el siguiente frame.
        - Puede configurarse un margen de error, para evitar comportamientos err�ticos.

###### RenderAPI:
- SpriteBatch:
    - Ahora usa `std::deque` en vez de `std::vector`.
        - Evita excesivos movimientos de memoria.
        - Aumento de rendimiento: +10% para 176 elementos.