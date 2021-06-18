## _struct_ KeyToAxis

Estructura que representa un mapeo entre dos teclas del teclado y sus valores para poder ser indentificadas como un evento de eje.

- Namespace: `OSK`.
- Include: `<AxisInputEvent.h>`.

### Atributos y funciones:

##### De KeyToAxis:

- `<Key>` keyA:
  - Primera tecla del teclado.
   
- `<Key>` keyB:
  - Segunda tecla del teclado.
  
- `float` valueA:
    - Peso que tiene la primera tecla.
  
- `float` valueB:
    - Peso que tiene la segunda tecla.


### Véase también:

- [OSK::Key](Key.md).
- [OSK::AxisInputEvent](AxisInputEvent.md).
- [OSK::InputSystem](InputSystem.md).