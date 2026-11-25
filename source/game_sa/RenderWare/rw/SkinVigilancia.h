#pragma once
// Vigilancia de matrices de hueso y de frame (fallo "textura de CJ se expande en una pared"). Solo DIAGNOSTICO: no altera el juego.
// Activacion: por defecto APAGADA en el codigo normal. Se enciende con la variable de entorno GTASA_SKIN_VIGILANCIA=1 o compilando este archivo con
// /DGTASA_SKIN_VIGILANCIA_POR_DEFECTO (el exe de vigilancia, gta_sa_standalone_vigila.exe); GTASA_SKIN_VIGILANCIA=0 la apaga siempre.
// Que hace cuando esta activa:
//  - justo antes de subir las matrices de hueso al vertex shader (rpskin) comprueba que sean finitas, acotadas y de escala razonable (CJ, peatones y cualquier atomo con skin);
//  - en RpHAnimHierarchyUpdateMatrices comprueba la matriz local que sale del fotograma interpolado (aqui nace el valor si el dato de animacion es basura);
//  - en CVehicle::PreRender comprueba la matriz de la entidad y la de modelado de cada frame del clump del vehiculo (por si se estira la bici/coche y no CJ);
//  - escribe logs/skin.log (reloj de pared y del juego, fotograma, objeto/modelo, hueso, valores, contexto: a pie/bici/coche, animaciones activas, velocidad y giro; con tope de tamano);
//  - antes de cada Present guarda una copia reducida del backbuffer en un anillo de 3 fotogramas y, si hubo anomalia en ese fotograma, escribe los PNG del fotograma y de los 2 anteriores
//    en logs/skin_cap (con tope de eventos).
// GTASA_SKIN_VIGILANCIA_PRUEBA=1 inyecta a proposito un NaN en una matriz de hueso de CJ cada ~600 fotogramas (para probar la propia vigilancia; ese NaN SI llega al shader).
#include "rwcore.h"

class CVehicle;

struct SkinVigilanciaEstado {
    uint32 magic;          // 'SKVG' = 0x47564B53 cuando la vigilancia esta activa
    uint32 anomalias;      // numero de subidas con algun valor no finito o enorme
    uint32 ultimoFotograma;// CTimer::m_FrameCounter de la ultima anomalia
    uint32 comprobaciones; // subidas comprobadas
    float  maxAbs;         // mayor |valor| visto en una subida correcta
    float  maxEscala;      // mayor longitud de columna 3x3 vista
};
extern SkinVigilanciaEstado g_SkinVigilanciaEstado;

bool SkinVigilanciaActiva();
// out: numNodes matrices de 12 floats (stride 12) que se van a subir al shader. skinToBone/hierarchyMatrices/inverseWorld: origen (pueden ser null). clump: del atomo (para identificar al dueno).
void SkinVigilanciaComprobar(const char* origen, float* out, int numNodes, const RwMatrix* skinToBone, const RwMatrix* hierarchyMatrices, const RwMatrix* inverseWorld, const void* atomic, const void* hierarchy, const void* clump = nullptr);
// Diagnostico en RpHAnimHierarchyUpdateMatrices: matriz local del fotograma interpolado (keyFrame: q xyzw, t xyz, tiempo...) y producto con el padre.
void SkinVigilanciaNodo(const char* etapa, int nodo, int numNodes, const RwMatrix& local, const RwMatrix* producto, const float* keyFrame, const void* hierarchy);
// Desde CVehicle::PreRender: matrices de la entidad y de los frames del vehiculo.
void SkinVigilanciaVehiculo(const CVehicle* vehicle);
// Antes de Present (fin del fotograma): anillo de capturas y volcado de los PNG si hubo anomalia. device = IDirect3DDevice9*.
void SkinVigilanciaFinFotograma(void* device);
// Libera los recursos de pila por defecto de la vigilancia: se llama al principio de _rwD3D9DeviceReleaseVideoMemory (antes de todo Reset del dispositivo).
void SkinVigilanciaLiberarRecursos();
