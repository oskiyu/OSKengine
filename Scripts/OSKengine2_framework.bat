@ECHO OFF

ECHO Preparando librerías de OSKengine para su uso en otros proyectos...

ECHO Copiando archivos .h ...

for /R C:\Users\Oscar\source\repos\OSKengine\OSKengine2 %%f in (*.h) do copy %%f C:\Users\Oscar\source\repos\OSKengine\Frameworks\OSKengine\Includes\OSKengine

ECHO Copiando archivos .hpp ...

for /R C:\Users\Oscar\source\repos\OSKengine\OSKengine2 %%f in (*.hpp) do copy %%f C:\Users\Oscar\source\repos\OSKengine\Frameworks\OSKengine\Includes\OSKengine

ECHO Listo.

PAUSE