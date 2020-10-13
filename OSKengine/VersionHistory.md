# OSKengine Version History.

## Alpha 0 (2019.12.3a)

###### WindowAPI:
- WindowAPI.
    - Interfaz con GLFW.
    - Carga de OpenGL.
    - Actualiza los valores de un KeyboardState.
- Keyboard Input: KeyboardState.

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
- `keyCode_t` (int).

###### Util:
- DebugOutput. muestra un mensaje por la consola.
- Exit. termina la ejecución mostrando un mensaje.


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
- Añadidos `typedef`s para algunas variables de OpenGL y para las variables del MouseState.
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

## 2020.10.13a

###### CollisionSystem:
- Eliminado *OBB*.
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