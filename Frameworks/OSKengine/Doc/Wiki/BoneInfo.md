## _struct_ BoneInfo

Estructura que contiene información sobre el transform de un hueso en concreto.

- Namespace: `OSK`.
- Include: `<BoneInfo.h>`.

### Atributos y funciones:

##### De BoneInfo:

- `glm::mat4` offset:
    - Offset de transform del hueso respecto al centro del modelo.
      - Offset de:
        - Posición.
        - Escala.
        - Rotación.

- `glm::mat4` finalTransform:
    - Transform actual del hueso.
    - Cambia dependiendo del estado de la animación.

### Véase también:

- [OSK::AnimatedModel](AnimatedModel.md).