# OSKengine Material Shader File Specification

- Tipo: JSON.

## Version 0

### Estructura general:

- `"file_type"`: `"SHADER"`
- `"spec_ver"`: `"0"`
- `"glsl"`:
  - `"vertex_file"`: "ruta al shader de vértices"
  - `"fragment_file"`: "ruta al shader de vértices"
- `"hlsl"`
  - `"shader_file"`: "ruta al shader".
- `"sets"`
  - "Nombre del set":
    - `"bindings"`:
      - "Nombre del binding individual":
        - `"glsl"`:
          - `"set"`: set dentro del shader.
          - `"index"`: índice dentro del set.
        - `"hlsl"`:
          - `"index"`: índice dentro del shader.
