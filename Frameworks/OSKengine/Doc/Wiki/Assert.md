## Assert

Serie de funciones y macros que permiten comprobar el funcionamiento de partes de la aplicación en tiempo real.

- Namespace: `OSK`, `<no namespace>`.
- Include: `<Assert.h>`.

### Atributos y funciones:

- RuntimeAssertIsTrueFunction(`bool`_`condition`_, `std::string`_`message`_, `std::string`_`functionName`_, `uint32_t`_`line`_, `std::string`_`file`_):
    - Comprueba el valor dado como condición.
    - Si el valor es **FALSO**, se invoca un error.
    - El mensaje de error incluye:
      - Mensaje dado.
      - Nombre de la función en la que ocurre el error.
      - Línea de código en la que está el error.
      - El archivo en el que está el error.

- RuntimeCheckIsTrueFunction(`bool`_`condition`_, `std::string`_`message`_, `std::string`_`functionName`_, `uint32_t`_`line`_, `std::string`_`file`_):
    - Comprueba el valor dado como condición.
    - Si el valor es **FALSO**, se muestra un mensaje de advertencia.
    - El mensaje de advertencia incluye:
      - Mensaje dado.
      - Nombre de la función en la que ocurre la advertencia.
      - Línea de código en la que está la advertencia.
      - El archivo en el que está la advertencia.

- RuntimeAssertIsFalseFunction(`bool`_`condition`_, `std::string`_`message`_, `std::string`_`functionName`_, `uint32_t`_`line`_, `std::string`_`file`_):
    - Comprueba el valor dado como condición.
    - Si el valor es **TRUE**, se invoca un error.
    - El mensaje de error incluye:
      - Mensaje dado.
      - Nombre de la función en la que ocurre el error.
      - Línea de código en la que está el error.
      - El archivo en el que está el error.

- RuntimeCheckIsFalseFunction(`bool`_`condition`_, `std::string`_`message`_, `std::string`_`functionName`_, `uint32_t`_`line`_, `std::string`_`file`_):
    - Comprueba el valor dado como condición.
    - Si el valor es **TRUE**, se muestra un mensaje de advertencia.
    - El mensaje de advertencia incluye:
      - Mensaje dado.
      - Nombre de la función en la que ocurre la advertencia.
      - Línea de código en la que está la advertencia.
      - El archivo en el que está la advertencia.

### Macros:

- **OSK_ASSERT**(_`condition`_, _`msg`_):
    - Llama a `<RuntimeAssertIsTrueFunction>`, rellenando automáticamente los parámetros necesarios.

- **OSK_CHECK**(_`condition`_, _`msg`_):
    - Llama a `<RuntimeCheckIsTrueFunction>`, rellenando automáticamente los parámetros necesarios.

- **OSK_ASSERT_FALSE**(_`condition`_, _`msg`_):
    - Llama a `<RuntimeAssertIsFalseFunction>`, rellenando automáticamente los parámetros necesarios.
  
- **OSK_CHECK_FALSE**(_`condition`_, _`msg`_):
    - Llama a `<RuntimeCheckIsFalseFunction>`, rellenando automáticamente los parámetros necesarios.