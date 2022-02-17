# OSKengine Material Description File Specification

- Tipo: JSON.

## Version 0

### Estructura general:

- `"file_type"`: `"MATERIAL"`
- `"spec_ver"`: `"0"`
- `"name"`: "Nombre del material"
- `"shader_file"`:"Ruta relativa al archivo de shader"
  
- `"layout"`
  - `"slots"`
    - "Nombre del slot":
      - `"bindings"`:
        - "Nombre del binding individual":
          - `"type"`: "tipo de binding"
