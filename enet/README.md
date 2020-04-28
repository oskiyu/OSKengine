#OSK::NET

##Documentación de enet.


```cpp

ENetAddress address;
ENetHost* server;

address.host = ENET_HOST_ANY;
address.port = 1234; //Puerto.

//Crea el servidor.
//	-Address.
//	-Número máximo de conexiones.
//  -Número de canales.
//	-Límite de ancho de banda de entrada (0 = cualquiera).
//	-Límite de ancho de banda de salida (0 = cualquiera).

server = enet_host_create(&address, 32, 2, 0, 0);
```

