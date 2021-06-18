## _struct_ AnimUBO

Estructura que contiene información de huesos de un modelo animado.

Se envía esta información a la GPU a través de un uniform buffer.

- Namespace: `OSK`.
- Include: `<AnimUBO.h>`.

### Atributos y funciones:

##### De AnimUBO:

- AnimUBO():
    - Establece que todos los huesos tengan una matriz unitaria.
  
- `glm::mat4` bones:
    - Matrices que representan los huesos.
    - Número de matrices: **OSK_ANIM_MAX_BONES** (declarado en [OSK::Vertex](Vertex.md)).

### Véase también:

- [OSK::AnimatedModel](AnimatedModel.md).