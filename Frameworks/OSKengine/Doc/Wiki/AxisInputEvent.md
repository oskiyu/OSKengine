## _struct_ AxisInputEvent

Estructura que representa un evento de input, cuyo valor puede variar dentro de un rango.

Principalmente para usarse con joystics y triggers de mandos, pero también puede funcionar enlazando teclas del teclado, usando la estructura [OSK::KeyToAxis](KeyToAxis.md).

- Namespace: `OSK`.
- Include: `<AxisInputEvent.h>`.

### Atributos y funciones:


##### De AxisInputEvent:

- `std::string` eventName:
    - Nombre del evento. Debe ser único entre los eventos de eje.

- `std::vector<`_`<KeyToAxis>`_`>`:
    - Teclas del teclado que ejecutan el evento.

- `std::vector<`_`<GamepadAxis>`_`>`:
    - Inputs de eje (de mando) que ejecutan el evento.

- `int` gamepadCode:
  - Gamepad que controla el input.
  - **NO IMPLEMENTADO**.
     
### Véase también:

- [OSK::KeyToAxis](KeyToAxis.md).
- [OSK::GamepadAxis](GamepadAxis.md).
- [OSK::GamepadState](GamepadState.md).
- [OSK::InputSystem](InputSystem.md).