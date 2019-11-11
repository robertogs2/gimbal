

# Requisitos
Descargar los binarios más recientes de:
https://gstreamer.freedesktop.org/data/pkg/android/

Extraer en cualquier lugar

En Android Studio abrir el proyecto "android" y modificar en el tutorial 3:

## gradle.properties
Revisar que esté la línea:
gstAndroidRoot=<Direccion de los bianrios extraidos>


## local.properties
* no se si es necesario
gstAndroidRoot=<Direccion de los bianrios extraidos>

Finalmente cargar el tutorial 3 en el teléfono.
_________________

TODO: 

## Android.mk 
* Verificar si son necesarios: ugly, good, bad, codecs

### Modificación de plugins
GSTREAMER_PLUGINS         := $(GSTREAMER_PLUGINS_CORE) $(GSTREAMER_PLUGINS_SYS) $(GSTREAMER_PLUGINS_EFFECTS) $(GSTREAMER_PLUGINS_BASE) $(GSTREAMER_PLUGINS_PLAYBACK) $(GSTREAMER_PLUGINS_CODECS) $(GSTREAMER_PLUGINS_NET) $(GSTREAMER_PLUGINS_UGLY) $(GSTREAMER_PLUGINS_GOOD) $(GSTREAMER_PLUGINS_BAD)

## Eliminar los otros tutoriales
