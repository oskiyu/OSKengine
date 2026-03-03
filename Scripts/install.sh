version="2025_01_11_a"

if [ -z "${OSK_ENGINE_SDK_PATH}" ]; then
    read -p "No se ha establecido la variable de entorno OSK_ENGINE_SDK_PATH, abortando..."
    exit
fi

if [ -d "$OSK_ENGINE_SDK_PATH/$version" ]; then
  rm -rf  $OSK_ENGINE_SDK_PATH/$version
fi

echo Ruta de destino: $OSK_ENGINE_SDK_PATH
echo Versión de destino: $version

if [ ! -d "$OSK_ENGINE_SDK_PATH/$version" ]; then
  mkdir $OSK_ENGINE_SDK_PATH/$version
fi
if [ ! -d "$OSK_ENGINE_SDK_PATH/$version/Bin" ]; then
  mkdir $OSK_ENGINE_SDK_PATH/$version/Bin
fi
if [ ! -d "$OSK_ENGINE_SDK_PATH/$version/Include" ]; then
  mkdir $OSK_ENGINE_SDK_PATH/$version/Include
fi
if [ ! -d "$OSK_ENGINE_SDK_PATH/$version/Include/OSKengine" ]; then
  mkdir $OSK_ENGINE_SDK_PATH/$version/Include/OSKengine
fi
if [ ! -d "$OSK_ENGINE_SDK_PATH/$version/Lib" ]; then
  mkdir $OSK_ENGINE_SDK_PATH/$version/Lib
fi
if [ ! -d "$OSK_ENGINE_SDK_PATH/$version/ThirdPartyBin" ]; then
  mkdir $OSK_ENGINE_SDK_PATH/$version/ThirdPartyBin
fi

cp -r ../Includes/*                    $OSK_ENGINE_SDK_PATH/$version/Include/
cp -r ../Bin/Release/OSKengine.dll     $OSK_ENGINE_SDK_PATH/$version/Bin/OSKengine.dll
cp -r ../Bin/Release/OSKengine.lib     $OSK_ENGINE_SDK_PATH/$version/Lib/OSKengine.lib
cp -r ../Bin/Release/*.dll             $OSK_ENGINE_SDK_PATH/$version/ThirdPartyBin/
rm $OSK_ENGINE_SDK_PATH/$version/ThirdPartyBin/OSKengine.dll

# Copiar estructura de carpetas, sin contenido inicial.
find ../Source/Resources/ -type d -exec mkdir -p $OSK_ENGINE_SDK_PATH/$version/Resources/{} \;
mv $OSK_ENGINE_SDK_PATH/$version/Source/Resources/ $OSK_ENGINE_SDK_PATH/$version/
rmdir $OSK_ENGINE_SDK_PATH/$version/Source/

# Copiar únicamente recursos realmente usados:
#   Todos los materiales:
cp -r ../Source/Resources/Materials/    $OSK_ENGINE_SDK_PATH/$version/Resources/
#   Todos los shaders:
cp -r ../Source/Resources/Shaders/      $OSK_ENGINE_SDK_PATH/$version/Resources/
#   Todos los iconos:
cp -r ../Source/Resources/Icons/        $OSK_ENGINE_SDK_PATH/$version/Resources/
#   El gitignore:
cp ../Source/Resources/.gitignore       $OSK_ENGINE_SDK_PATH/$version/Resources/.gitignore
#   Los assets estrictamente necesarios (ver documentación):
cp ../Source/Resources/Assets/Models/cube.json                  $OSK_ENGINE_SDK_PATH/$version/Resources/Assets/Models/cube.json
cp ../Source/Resources/Assets/Models/cube.glb                   $OSK_ENGINE_SDK_PATH/$version/Resources/Assets/Models/cube.glb

cp -r ../Source/Resources/Assets/Models/Colliders/              $OSK_ENGINE_SDK_PATH/$version/Resources/Assets/Models/

cp -r ../Source/Resources/Assets/BlueNoise/                     $OSK_ENGINE_SDK_PATH/$version/Resources/Assets/
cp -r ../Source/Resources/Assets/Textures/BlueNoise/            $OSK_ENGINE_SDK_PATH/$version/Resources/Assets/Textures/

cp -r ../Source/Resources/Assets/Fonts/                         $OSK_ENGINE_SDK_PATH/$version/Resources/Assets/

cp -r ../Source/Resources/Assets/IBL/                           $OSK_ENGINE_SDK_PATH/$version/Resources/Assets/
cp -r ../Source/Resources/Assets/Skyboxes/                      $OSK_ENGINE_SDK_PATH/$version/Resources/Assets/

cp ../Source/Resources/Assets/Textures/editor_icon.json         $OSK_ENGINE_SDK_PATH/$version/Resources/Assets/Textures/editor_icon.json
cp ../Source/Resources/Assets/Textures/editorIcon.png           $OSK_ENGINE_SDK_PATH/$version/Resources/Assets/Textures/editorIcon.png
cp ../Source/Resources/Assets/Textures/engine_icon.json         $OSK_ENGINE_SDK_PATH/$version/Resources/Assets/Textures/engine_icon.json
cp ../Source/Resources/Assets/Textures/engineIcon.png           $OSK_ENGINE_SDK_PATH/$version/Resources/Assets/Textures/engineIcon.png

cp ../Source/Resources/Assets/Textures/texture0.json            $OSK_ENGINE_SDK_PATH/$version/Resources/Assets/Textures/texture0.json
cp ../Source/Resources/Assets/Textures/texture0.png             $OSK_ENGINE_SDK_PATH/$version/Resources/Assets/Textures/texture0.png



read -p "Finalizado"