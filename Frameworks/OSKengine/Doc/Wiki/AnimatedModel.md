## _class_ AnimatedModel : Model

Clase que representa un modelo que puede ser animado mediante un sistema de huesos.

La animación debe ser actualizada cada tick.
Al cargarse el modelo, también se cargan las animaciones enlazadas a él. A cada animación se le asigna un identificador en forma de número. También se puede elegir una animación por su nombre.

- Namespace: `OSK`.
- Include: `<AnimatedModel.h>`.
- Clase base: [OSK::Model](Model.md).

### Atributos y funciones:

##### Heredadas de Model:

- GetTransform():
    - Devuelve el transform del modelo.

##### De AnimatedModel:

- SetAnimation(`uint32_t`_`animID`_):
    - Establece la animación que se reproduce.
  
- SetAnimation(`std::string`_`name`_):
    - Establece la animación que se reproduce.

- Update(`float`_`deltaTime`_):
    - Actualiza los huesos de la animación, de acuerdo a la animación actual.
    - Debe llamarse cada tick.
     
- `float` animationSpeed:
    - Establece la velocidad de la animación.
    - Puede ser negativa, en cuyo caso la animación se reproduce inversamente.
    - Por defecto = <0.75f>.

### Véase también:

- [OSK::AnimUBO](AnimUBO.md).
- [OSK::Vector3](Vector3.md).
- [OSK::ContentManager](ContentManager.md).
- [OSK::Transform](Transform.md).