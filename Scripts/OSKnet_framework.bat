@ECHO OFF

ECHO Preparando librerías de OSKnet para su uso en otros proyectos...

ECHO Copiando archivos .h ...

for /R C:\Users\Oscar\source\repos\OSKengine\enet %%f in (*.h) do copy %%f C:\Users\Oscar\source\repos\OSKengine\Frameworks\OSKnet\Includes\OSKnet

ECHO Copiando archivos .hpp ...

for /R C:\Users\Oscar\source\repos\OSKengine\enet %%f in (*.hpp) do copy %%f C:\Users\Oscar\source\repos\OSKengine\Frameworks\OSKnet\Includes\OSKnet

ECHO Copiando librerías (DEBUG) ...

xcopy C:\Users\Oscar\source\repos\OSKengine\x64\Debug\OSKnet\OSKnet.dll C:\Users\Oscar\source\repos\OSKengine\Frameworks\OSKnet\LibBinDebug\OSKnet.dll
xcopy C:\Users\Oscar\source\repos\OSKengine\x64\Debug\OSKnet\OSKnet.lib C:\Users\Oscar\source\repos\OSKengine\Frameworks\OSKnet\LibBinDebug\OSKnet.lib

ECHO Copiando librerías (RELEASE) ...

xcopy C:\Users\Oscar\source\repos\OSKengine\x64\Release\OSKnet\OSKnet.dll C:\Users\Oscar\source\repos\OSKengine\Frameworks\OSKnet\LibBin\OSKnet.dll
xcopy C:\Users\Oscar\source\repos\OSKengine\x64\Release\OSKnet\OSKnet.lib C:\Users\Oscar\source\repos\OSKengine\Frameworks\OSKnet\LibBin\OSKnet.lib

ECHO Listo.

PAUSE