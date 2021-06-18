## _enum_ BroadColliderType

Enumeración que contiene los tipos de colisionadores básicos que puede implementar un broad collider.

Un broad collider es un colisionador básico, que se usa para descartar colisiones. Si un segundo colisionador colisiona con el broad collider, se comprobará la colisión de una manera más precisa.

- Namespace: `OSK`.
- Include: `<BroadColliderType.h>`.

### Valores:

- `BOX_AABB`.
    - Véase [OSK::CollisionBox](CollisionBox.md).

- `SPHERE`.
    - Véase [OSK::CollisionSphere](CollisionSphere.md).

### Véase también:

- [OSK::Collider](Collider.md).