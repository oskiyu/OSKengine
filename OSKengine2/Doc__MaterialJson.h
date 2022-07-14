/*!
@page jsonfiles Archivos JSON
@brief Documentaci�n de los varios archivos JSON que usa OSKengine.

@section Material
@subsection specv0 Spec V0

@brief Un archivo que desribe un material, configurando el proceso de renderizado.
En esta versi�n de la especificaci�n, la informaci�n del material se divide entre
el archivo de material y el archivo de shader.

@li "file_type" = "MATERIAL"
@li "spec_ver" = 0
@li "name".														@brief Nombre del material.
@li "shader_file".												@note El shader indicado debe ser compatible con el layout definido en este archivo.
																@brief Direcci�n del archivo de shader enlazado al material.
@li "layout"													@brief Define el layout del material, indicando sus bindings (buffers, texturas, etc...)
																	y los push contsants.

@li PushConstant:

*/
