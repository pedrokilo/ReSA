// Lotes S30A y S31A del plan de sustitucion (docs/ExeIndependiente/plan-sustitucion.md): raices de RpWorld que antes ejecutaba el EXE (envolturas por direccion y callbacks).
// Cada funcion esta enganchada con RH_ScopedGlobalInstall (sin validar, validated=0 en docs/hooks.csv) en RpWorldInjectHooksS30A31A, al final del archivo.
// Herramientas y desensamblados: reversing/FnS30A33A. Las funciones del EXE que aun no estan reconstruidas se llaman por direccion y se anotan en cada una.
#include "StdInc.h"

#include "rpworld.h"
#include "X87Math.h"
#include "game_sa/Crt/CrtPow.h"
#include "game_sa/Crt/CrtQsort.h"
#include <rpdbgerr.h>
#include <bit>
#include <cmath>
#include <cstring>

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

// Codigo vivo CAM: funciones de rpworld.cpp que este archivo usaba por direccion del EXE.
const RpLight* RpLightStreamWrite(const RpLight* light, RwStream* stream);                // 0x751E30
RwInt32 RpGeometryStreamStructSize(const RpGeometry* geometry);                           // 0x74CE60
RwUInt32 RpWorldSectorStreamDataSize(void* sector, RpWorld* world, RwUInt32 flags);       // 0x762440
RwUInt32 RpWorldPlaneStreamSize(void* sector, RpWorld* world, RwUInt32 flags);            // 0x762040

namespace {
// Valor float a partir de sus bits (las constantes del .rdata del EXE); memcpy para no pasar por la x87.
inline float F32(uint32 bits) {
    float value;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

inline uint32 Bits(float value) {
    uint32 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return bits;
}

inline RwGlobals* Globals() { return static_cast<RwGlobals*>(RwEngineInstance); }

// fistp con el redondeo por defecto (al mas cercano, pares en empate).
inline int32 RoundToInt(double value) { return static_cast<int32>(std::nearbyint(value)); }

// Estado del modulo de mundo en RwEngineInstance (+ [0xC9259C]): +8 contador de fotogramas de render.
inline RwUInt32& WorldRenderFrame() {
    return *reinterpret_cast<RwUInt32*>(reinterpret_cast<RwUInt8*>(RwEngineInstance) + StaticRef<RwInt32>(0xC9259C) + 8);
}

// Datos de plugin del mundo en la camara (+0 puntero a la lista de sectores en el frustum, +8 su numero), el clump (+4 marca de fotograma) y el atomo (+0x60 de la propia estructura, marca de 16 bits).
inline RwUInt8* CameraWorldData(const RwCamera* camera) {
    return const_cast<RwUInt8*>(reinterpret_cast<const RwUInt8*>(camera)) + StaticRef<RwInt32>(0xC925A4);
}
inline RwUInt32* ClumpWorldData(RpClump* clump) {
    return reinterpret_cast<RwUInt32*>(reinterpret_cast<RwUInt8*>(clump) + StaticRef<RwInt32>(0xC925AC));
}

// Los 'enganches' de lista que el EXE usa para recorrer una lista mientras la callback puede quitar el nodo actual: un RwLLLink local {next, prev} entre el nodo y su siguiente.
struct ListMarker {
    RwLLLink link;
};
inline void MarkerInsertAfter(ListMarker& marker, RwLLLink* node) {
    marker.link.next = node->next;
    marker.link.prev = node;
    node->next->prev = &marker.link;
    node->next = &marker.link;
}
inline void MarkerRemove(ListMarker& marker) {
    marker.link.prev->next = marker.link.next;
    marker.link.next->prev = marker.link.prev;
}

// RwSqrt del SDK: _rwSqrt (0x7EDB30, ya reconstruida); devuelve la raiz como float.
inline float SdkSqrt(float value) { return _rwSqrt(value); }
} // namespace

// ---------------------------------------------------------------------------------------------------------------------------------------------
// S30A: flujos, camara, luces, mallas y callbacks del pipeline de vertex shaders de D3D9
// ---------------------------------------------------------------------------------------------------------------------------------------------

// 0x74A5B0 (lote S30A, sin validar; 41 bytes; cdecl, (atomo)): tamano en flujo = RpGeometryStreamGetSize(geometria) + 0x28 (struct + cabeceras) + el tamano de los plugins del atomo (_rwPluginRegistryGetSize sobre el registro 0x8D624C) + 0xC.
RwUInt32 RpAtomicStreamGetSize(RpAtomic* atomic) {
    const RwUInt32 geometrySize = RpGeometryStreamGetSize(atomic->geometry) + 0x28;
    return _rwPluginRegistryGetSize(reinterpret_cast<const RwPluginRegistry*>(0x8D624C), atomic) + geometrySize + 0xC;
}

// Indice de la primera aparicion anterior (buscando desde i - 1 hacia 0) del material i en la lista, o -1 si es nuevo.
static RwInt32 MaterialListPreviousIndex(const RpMaterialList* matList, RwInt32 i) {
    for (RwInt32 j = i - 1; j >= 0; --j) {
        if (matList->materials[j] == matList->materials[i]) {
            return j;
        }
    }
    return -1;
}

// 0x74E4B0 (lote S30A, sin validar; 331 bytes; cdecl, (lista de materiales, flujo)): escribe la lista: tamano total = numMateriales * 4 + 0x10 + (para cada material que no se repite) RpMaterialStreamGetSize + 0xC; cabecera 8 (version
// 0x36003, build 0xFFFF), cabecera 1 con numMateriales * 4 + 4 bytes, numMateriales, un indice por material (el del material anterior igual o -1) y despues RpMaterialStreamWrite de cada material nuevo. Si algo falla devuelve NULL.
const RpMaterialList* _rpMaterialListStreamWrite(const RpMaterialList* matList, RwStream* stream) {
    const RwInt32 numMaterials = matList->numMaterials;
    RwUInt32 size = static_cast<RwUInt32>(numMaterials) * 4 + 0x10;
    for (RwInt32 i = 0; i < numMaterials; ++i) {
        if (MaterialListPreviousIndex(matList, i) < 0) {
            size += RpMaterialStreamGetSize(matList->materials[i]) + 0xC;
        }
    }
    if (!_rwStreamWriteVersionedChunkHeader(stream, 8, size, 0x36003, 0xFFFF)) {
        return nullptr;
    }
    if (!_rwStreamWriteVersionedChunkHeader(stream, 1, static_cast<RwUInt32>(numMaterials) * 4 + 4, 0x36003, 0xFFFF)) {
        return nullptr;
    }
    if (!RwStreamWriteInt32(stream, &matList->numMaterials, 4)) {
        return nullptr;
    }
    for (RwInt32 i = 0; i < numMaterials; ++i) {
        const RwInt32 index = MaterialListPreviousIndex(matList, i);
        if (!RwStreamWriteInt32(stream, &index, 4)) {
            return nullptr;
        }
    }
    for (RwInt32 i = 0; i < numMaterials; ++i) {
        if (MaterialListPreviousIndex(matList, i) < 0) {
            if (!RpMaterialStreamWrite(matList->materials[i], stream)) {
                return nullptr;
            }
        }
    }
    return matList;
}



// 0x74F590 (codigo vivo CAM, sin validar; 118 bytes; cdecl, (sector, dato sin uso)): callback de RpWorldRender. Renderiza el sector con RpWorldSectorRender (0x761C50; su resultado no cambia lo que sigue) y recorre los
// atomos de colision del sector (lista +0x38; el atomo esta en +8 del enlace; el siguiente enlace se lee tras el callback): a cada atomo con la bandera de render (flags & 4) que aun no se dibujo en este marco
// (renderFrame +0x60 distinto del del motor, +8) le pasa RwCameraFrustumTestSphere(camara actual, esfera de mundo) y, si es distinto de cero, llama a su renderCallBack (+0x48); despues le anota el marco actual.
// Devuelve el sector.
static RpWorldSector* WorldSectorRenderCallBack(RpWorldSector* sector, void* /*data*/) {
    if (RpWorldSectorRender(sector) == nullptr) {
        return sector;
    }
    RwLLLink* const end = &sector->collAtomicsInWorldSector.link;
    for (RwLLLink* link = end->next; link != end; link = link->next) {
        RpAtomic* const atomic = *reinterpret_cast<RpAtomic**>(reinterpret_cast<RwUInt8*>(link) + 8);
        if ((atomic->object.object.flags & 4) != 0 && atomic->renderFrame != Globals()->renderFrame) {
            if (RwCameraFrustumTestSphere(static_cast<const RwCamera*>(Globals()->curCamera), RpAtomicGetWorldBoundingSphere(atomic)) != 0) {
                atomic->renderCallBack(atomic);
            }
            atomic->renderFrame = Globals()->renderFrame;
        }
    }
    return sector;
}

// 0x74F570 (lote S30A, sin validar; 31 bytes; cdecl, (mundo)): recorre los sectores en el frustum de la camara actual (RwCameraForAllSectorsInFrustum, 0x751660; lote de otro agente, se llama por su funcion) con el callback
// WorldSectorRenderCallBack (0x74F590, renderiza el sector y sus atomos; codigo vivo CAM) y el mundo como datos. Devuelve el mundo.
RpWorld* RpWorldRender(RpWorld* world) {
    RwCameraForAllSectorsInFrustum(static_cast<RwCamera*>(Globals()->curCamera), WorldSectorRenderCallBack, world);
    return world;
}

// 0x7516C0 (lote S30A, sin validar; 299 bytes; cdecl, (camara, datos)): llama al callback del clump de cada atomo de los sectores en el frustum de la camara (lista de sectores y numero en los datos de plugin del mundo de la camara). Cada
// clump se visita una vez por pasada: el contador de fotogramas global (++) se anota en los datos del clump (+4) cuando el esfera del atomo esta (total o parcialmente) en el frustum. Recorre las ties de cada sector con un enganche
// local para que el callback pueda quitar el nodo. Si un callback devuelve NULL se detiene y devuelve NULL; si no devuelve la camara.
RwCamera* RwCameraForAllClumpsInFrustum(RwCamera* camera, void* data) {
    const RwUInt32 frame = ++WorldRenderFrame();
    RwUInt8* const cameraData = CameraWorldData(camera);
    RpWorldSector** sectors = *reinterpret_cast<RpWorldSector***>(cameraData);
    RwUInt32 remaining = *reinterpret_cast<RwUInt32*>(cameraData + 8);
    while (remaining != 0) {
        RwLinkList& ties = (*sectors)->collAtomicsInWorldSector;
        RwLLLink* const sentinel = &ties.link;
        RwLLLink* node = sentinel->next;
        while (node != sentinel) {
            RpTie* const tie = reinterpret_cast<RpTie*>(node);
            RpAtomic* const atomic = tie->apAtom;
            RpClump* const clump = atomic->clump;
            ListMarker marker;
            MarkerInsertAfter(marker, node);
            if (clump != nullptr) {
                RwUInt32* const clumpData = ClumpWorldData(clump);
                if (clumpData[1] != frame) {
                    if (RwCameraFrustumTestSphere(camera, RpAtomicGetWorldBoundingSphere(atomic)) != rwSPHEREOUTSIDE) {
                        clumpData[1] = frame;
                        if (clump->callback(clump, data) == nullptr) {
                            MarkerRemove(marker);
                            return nullptr;
                        }
                    }
                }
            }
            RwLLLink* const next = marker.link.next;
            MarkerRemove(marker);
            node = next;
        }
        --remaining;
        ++sectors;
    }
    return camera;
}

// 0x7517F0 (lote S30A, sin validar; 279 bytes; cdecl, (camara, callback, datos)): igual que RwCameraForAllClumpsInFrustum pero por atomo: la marca es la palabra de 16 bits +0x60 del atomo (se compara con el contador completo de
// 32 bits y se guarda su parte baja) y se llama a callback(atomo, datos). Si un callback devuelve NULL se detiene; en ese caso el EXE devuelve la camara (no NULL), igual que al terminar.
RwCamera* RwCameraForAllAtomicsInFrustum(RwCamera* camera, RpAtomicCallBack callback, void* data) {
    const RwUInt32 frame = ++WorldRenderFrame();
    RwUInt8* const cameraData = CameraWorldData(camera);
    RpWorldSector** sectors = *reinterpret_cast<RpWorldSector***>(cameraData);
    RwUInt32 remaining = *reinterpret_cast<RwUInt32*>(cameraData + 8);
    while (remaining != 0) {
        RwLinkList& ties = (*sectors)->collAtomicsInWorldSector;
        RwLLLink* const sentinel = &ties.link;
        RwLLLink* node = sentinel->next;
        while (node != sentinel) {
            RpTie* const tie = reinterpret_cast<RpTie*>(node);
            RpAtomic* const atomic = tie->apAtom;
            ListMarker marker;
            MarkerInsertAfter(marker, node);
            if (atomic != nullptr) {
                RwUInt16& stamp = *reinterpret_cast<RwUInt16*>(reinterpret_cast<RwUInt8*>(atomic) + 0x60);
                if (static_cast<RwUInt32>(stamp) != frame) {
                    if (RwCameraFrustumTestSphere(camera, RpAtomicGetWorldBoundingSphere(atomic)) != rwSPHEREOUTSIDE) {
                        stamp = static_cast<RwUInt16>(frame);
                        if (callback(atomic, data) == nullptr) {
                            MarkerRemove(marker);
                            return camera;
                        }
                    }
                }
            }
            RwLLLink* const next = marker.link.next;
            MarkerRemove(marker);
            node = next;
        }
        --remaining;
        ++sectors;
    }
    return camera;
}

// 0x751AE0 (lote S30A, sin validar; 566 bytes; cdecl, (luz)): angulo del cono = acos(-minusCosAngle) con la acosf de fdlibm del CRT del EXE inlinada (ramas |x| >= 1, |x| < 0.5 con tabla de 6+5 coeficientes, x < -0.5 y x > 0.5 con la raiz
// de RenderWare (_rwSqrt) y el truncado del cociente a 12 bits). Los intermedios son doble (la x87 los guarda en 80 bits); x, z (y la raiz truncada) se guardan como float como en el EXE. Constantes del .rdata del EXE.
RwReal RpLightGetConeAngle(const RpLight* light) {
    const float x = -light->minusCosAngle;
    const uint32 hx = Bits(x);
    const uint32 ix = hx & 0x7FFFFFFFu;
    if (ix >= 0x3F800000u) {
        return static_cast<int32>(hx) > 0 ? 0.0f : F32(0x40490FDB);   // 0 o pi
    }
    const double pS0 = F32(0x3811EF08), pS1 = F32(0x3A4F7F04), pS2 = F32(0x3D241146), pS3 = F32(0x3E4E0AA8), pS4 = F32(0x3EA6B090), pS5 = F32(0x3E2AAAAB);
    const double qS1 = F32(0x3D9DC62E), qS2 = F32(0x3F303361), qS3 = F32(0x4001572D), qS4 = F32(0x4019D139);
    const double pioHi = F32(0x3FC90FDA), pioLo = F32(0x33A22168), pi = F32(0x40490FDA);
    if (ix < 0x3F000000u) {
        if (ix <= 0x23000000u) {
            return F32(0x3FC90FDB);                                        // pi / 2
        }
        const double z = static_cast<double>(x) * static_cast<double>(x);
        double p = pS0 * z + pS1;
        p = p * z - pS2;
        p = p * z + pS3;
        p = p * z - pS4;
        p = p * z + pS5;
        p = p * z;
        double q = static_cast<double>(F32(0x3D9DC62E)) * z - qS2;
        q = q * z + qS3;
        q = q * z - qS4;
        q = q * z + 1.0;
        const double r = p / q;
        return static_cast<float>(pioHi - (static_cast<double>(x) - (pioLo - r * static_cast<double>(x))));
    }
    if (static_cast<int32>(hx) < 0) {
        const float z = static_cast<float>((static_cast<double>(x) + 1.0) * 0.5);
        const double s = SdkSqrt(z);
        double p = static_cast<double>(z) * pS0 + pS1;
        p = p * z - pS2;
        p = p * z + pS3;
        p = p * z - pS4;
        p = p * z + pS5;
        p = p * z;
        double q = static_cast<double>(z) * qS1 - qS2;
        q = q * z + qS3;
        q = q * z - qS4;
        q = q * z + 1.0;
        const double w = (p / q) * s - pioLo;
        return static_cast<float>(pi - 2.0 * (s + w));
    }
    const float z = static_cast<float>((1.0 - static_cast<double>(x)) * 0.5);
    const double s = SdkSqrt(z);
    const float sFloat = static_cast<float>(s);
    const double df = static_cast<double>(F32(Bits(sFloat) & 0xFFFFF000u));
    double p = static_cast<double>(z) * pS0 + pS1;
    p = p * z - pS2;
    p = p * z + pS3;
    p = p * z - pS4;
    p = p * z + pS5;
    p = p * z;
    double q = static_cast<double>(z) * qS1 - qS2;
    q = q * z + qS3;
    q = q * z - qS4;
    q = q * z + 1.0;
    const double w = (p / q) * s;
    const double c = (static_cast<double>(z) - df * df) / (df + s);
    return static_cast<float>(2.0 * ((w + c) + df));
}

// 0x751D20 (lote S30A, sin validar; 55 bytes; cdecl, (luz, angulo)): si el angulo no es negativo y no pasa de pi / 2 (con NaN acepta) guarda minusCosAngle = -cos(angulo) y devuelve la luz; si no devuelve NULL.
RpLight* RpLightSetConeAngle(RpLight* light, RwReal angle) {
    if (!(angle < 0.0f) && !(angle > F32(0x3FC90FDB))) {
        light->minusCosAngle = -TrigSimpleCosCpp(angle);
        return light;
    }
    return nullptr;
}

// 0x7531B0 (lote S30A, sin validar; 2410 bytes; cdecl, (tipo, destino, normales origen, numero, paso); `_rpD3D9VertexDeclarationInstV3dComp` del SDK): convierte `numero` normales de 3 bytes con signo (x, y, z, pad; /128)
// al formato de la declaracion de vertices `tipo` (D3DDECLTYPE 0..14) y los escribe cada `paso` bytes: 0 FLOAT1, 1 FLOAT2, 2 FLOAT3, 3 FLOAT4 (w = 1), 4 D3DCOLOR (bytes z, y, x del valor *255 y A = 255), 5 UBYTE4 (valores redondeados y 1),
// 6 SHORT2, 7 SHORT4 (w = 1), 8 UBYTE4N (*255, A = 255), 9 SHORT2N (*32767), 10 SHORT4N (w = 0x7FFF), 11 USHORT2N (*65535), 12 USHORT4N (w = 0xFFFF), 13 UDEC3 (10 bits cada uno) y 14 DEC3N (*511). Redondeo al mas cercano (fistp).
// Devuelve el tamano en bytes del tipo (tabla de 15 dwords del EXE en 0x874EF8); con tipo > 14 (el EXE leeria memoria fuera de la tabla) o numero <= 0 no escribe nada.
RwUInt32 _rpD3D9VertexDeclarationInstV3dComp(RwUInt32 type, RwUInt8* mem, const RpVertexNormal* src, RwInt32 numVerts, RwUInt32 stride) {
    static constexpr RwUInt32 kTypeSize[15] = { 4, 8, 12, 16, 4, 4, 4, 8, 4, 4, 8, 4, 8, 4, 4 };
    if (type > 14) {
        return 0;
    }
    constexpr double kScale = 0.0078125; // 1/128 (0x858B88)
    for (RwInt32 n = 0; n < numVerts; ++n, mem += stride, ++src) {
        const double x = static_cast<double>(src->x) * kScale;
        const double y = static_cast<double>(src->y) * kScale;
        const double z = static_cast<double>(src->z) * kScale;
        float* const f = reinterpret_cast<float*>(mem);
        int16* const s = reinterpret_cast<int16*>(mem);
        switch (type) {
        case 0: f[0] = static_cast<float>(x); break;
        case 1: f[0] = static_cast<float>(x); f[1] = static_cast<float>(y); break;
        case 2: f[0] = static_cast<float>(x); f[1] = static_cast<float>(y); f[2] = static_cast<float>(z); break;
        case 3: f[0] = static_cast<float>(x); f[1] = static_cast<float>(y); f[2] = static_cast<float>(z); f[3] = 1.0f; break;
        case 4:
            mem[2] = static_cast<RwUInt8>(RoundToInt(static_cast<float>(x * 255.0)));
            mem[1] = static_cast<RwUInt8>(RoundToInt(static_cast<float>(static_cast<float>(y) * 255.0)));
            mem[0] = static_cast<RwUInt8>(RoundToInt(static_cast<float>(static_cast<float>(z) * 255.0)));
            mem[3] = 0xFF;
            break;
        case 5:
            mem[0] = static_cast<RwUInt8>(RoundToInt(static_cast<float>(x)));
            mem[1] = static_cast<RwUInt8>(RoundToInt(static_cast<float>(y)));
            mem[2] = static_cast<RwUInt8>(RoundToInt(static_cast<float>(z)));
            mem[3] = 1;
            break;
        case 6:
            s[0] = static_cast<int16>(RoundToInt(static_cast<float>(x)));
            s[1] = static_cast<int16>(RoundToInt(static_cast<float>(y)));
            break;
        case 7:
            s[0] = static_cast<int16>(RoundToInt(static_cast<float>(x)));
            s[1] = static_cast<int16>(RoundToInt(static_cast<float>(y)));
            s[2] = static_cast<int16>(RoundToInt(static_cast<float>(z)));
            s[3] = 1;
            break;
        case 8:
            mem[0] = static_cast<RwUInt8>(RoundToInt(static_cast<float>(x * 255.0)));
            mem[1] = static_cast<RwUInt8>(RoundToInt(static_cast<float>(static_cast<float>(y) * 255.0)));
            mem[2] = static_cast<RwUInt8>(RoundToInt(static_cast<float>(static_cast<float>(z) * 255.0)));
            mem[3] = 0xFF;
            break;
        case 9:
            s[0] = static_cast<int16>(RoundToInt(static_cast<float>(x * 32767.0)));
            s[1] = static_cast<int16>(RoundToInt(static_cast<float>(static_cast<float>(y) * 32767.0)));
            break;
        case 10:
            s[0] = static_cast<int16>(RoundToInt(static_cast<float>(x * 32767.0)));
            s[1] = static_cast<int16>(RoundToInt(static_cast<float>(static_cast<float>(y) * 32767.0)));
            s[2] = static_cast<int16>(RoundToInt(static_cast<float>(static_cast<float>(z) * 32767.0)));
            s[3] = 0x7FFF;
            break;
        case 11:
            s[0] = static_cast<int16>(RoundToInt(static_cast<float>(x * 65535.0)));
            s[1] = static_cast<int16>(RoundToInt(static_cast<float>(static_cast<float>(y) * 65535.0)));
            break;
        case 12:
            s[0] = static_cast<int16>(RoundToInt(static_cast<float>(x * 65535.0)));
            s[1] = static_cast<int16>(RoundToInt(static_cast<float>(static_cast<float>(y) * 65535.0)));
            s[2] = static_cast<int16>(RoundToInt(static_cast<float>(static_cast<float>(z) * 65535.0)));
            s[3] = static_cast<int16>(0xFFFF);
            break;
        case 13: {
            const uint32 ix = static_cast<uint32>(RoundToInt(static_cast<float>(x)));
            const uint32 iy = static_cast<uint32>(RoundToInt(static_cast<float>(y)));
            const uint32 iz = static_cast<uint32>(RoundToInt(static_cast<float>(z)));
            *reinterpret_cast<uint32*>(mem) = (((iz & 0x3FFu) << 10 | (iy & 0x3FFu)) << 10) | (ix & 0x3FFu);
            break;
        }
        case 14: {
            const uint32 ix = static_cast<uint32>(RoundToInt(static_cast<float>(x * 511.0)));
            const uint32 iy = static_cast<uint32>(RoundToInt(static_cast<float>(static_cast<float>(y) * 511.0)));
            const uint32 iz = static_cast<uint32>(RoundToInt(static_cast<float>(static_cast<float>(z) * 511.0)));
            *reinterpret_cast<uint32*>(mem) = (((iz & 0x3FFu) << 10 | (iy & 0x3FFu)) << 10) | (ix & 0x3FFu);
            break;
        }
        default: break;
        }
    }
    return kTypeSize[type];
}

// 0x755830 (lote S30A, sin validar; 189 bytes; cdecl, (vertices, numero, minimo*, maximo*; `_rpD3D9GetMinMaxValuesV3d` del SDK)): minimo y maximo por componente. Ambos empiezan en el primer vertice y el bucle (numero - 1 vueltas) arranca
// EN el primer vertice y no llega al ultimo: el ultimo vertice no cuenta (igual que el EXE). Por componente: si min > v lo copia a min; si no, si max < v lo copia a max (NaN no cambia nada; copia por palabras).
void _rpD3D9GetMinMaxValuesV3d(const RwV3d* src, RwInt32 numElements, RwV3d* min, RwV3d* max) {
    std::memcpy(min, src, sizeof(RwV3d));
    std::memcpy(max, src, sizeof(RwV3d));
    if (numElements <= 1) {
        return;
    }
    const float* v = reinterpret_cast<const float*>(src);
    float* const lo = reinterpret_cast<float*>(min);
    float* const hi = reinterpret_cast<float*>(max);
    for (RwInt32 i = 0; i < numElements - 1; ++i, v += 3) {
        for (int c = 0; c < 3; ++c) {
            if (lo[c] > v[c]) {
                std::memcpy(&lo[c], &v[c], sizeof(float));
            } else if (hi[c] < v[c]) {
                std::memcpy(&hi[c], &v[c], sizeof(float));
            }
        }
    }
}

namespace {
// Estado del plugin de luces del controlador D3D9 (EXE): indice de luz D3D de cada RpLight (dword en luz + [0xC926B0]; -1 = sin asignar), lista libre de indices (cuenta 0xC926B8, capacidad 0xC926BC, siguiente 0xC926C0,
// vector 0xC926C4) y lista de luces activadas (vector 0xC926C8, cuenta 0xC926CC, capacidad 0xC926D0), mas el D3DLIGHT9 de trabajo (0xC92648) y los acumuladores de ambiente (0x8E2418..0x8E2424).
constexpr uintptr_t kLightIndexOffset      = 0xC926B0;
constexpr uintptr_t kFreeCount             = 0xC926B8;
constexpr uintptr_t kFreeCapacity          = 0xC926BC;
constexpr uintptr_t kNextIndex             = 0xC926C0;
constexpr uintptr_t kFreeArray             = 0xC926C4;
constexpr uintptr_t kEnabledArray          = 0xC926C8;
constexpr uintptr_t kEnabledCount          = 0xC926CC;
constexpr uintptr_t kEnabledCapacity       = 0xC926D0;
constexpr uintptr_t kWorkingLight          = 0xC92648;
constexpr RwUInt32  kLightArrayHint        = 0x103050D;

// Reserva el indice de luz D3D de la luz si no lo tiene (de la lista libre o el siguiente nuevo), carga en el D3DLIGHT9 de trabajo el color y la direccion (eje at del LTM del marco) y lo envia con RwD3D9SetLight; despues anota el
// indice en la lista de luces activadas (crece de 8 en 8 poniendo a 0 las entradas nuevas). Es el cuerpo comun de _rwD3D9LightDirectionalEnable y del caso direccional de _rwD3D9LightsGlobalEnable.
void D3D9EnableDirectionalLight(RpLight* light) {
    RwInt32* const slot = reinterpret_cast<RwInt32*>(reinterpret_cast<RwUInt8*>(light) + StaticRef<RwInt32>(kLightIndexOffset));
    if (*slot < 0) {
        RwInt32& freeCount = StaticRef<RwInt32>(kFreeCount);
        if (freeCount != 0) {
            --freeCount;
            *slot = StaticRef<RwInt32*>(kFreeArray)[freeCount];
        } else {
            *slot = StaticRef<RwInt32>(kNextIndex);
            ++StaticRef<RwInt32>(kNextIndex);
        }
    }
    RwUInt32* const d3dLight = reinterpret_cast<RwUInt32*>(kWorkingLight);
    d3dLight[1] = Bits(light->color.red);
    d3dLight[2] = Bits(light->color.green);
    d3dLight[3] = Bits(light->color.blue);
    const RwMatrix* const ltm = RwFrameGetLTM(static_cast<RwFrame*>(light->object.object.parent));
    d3dLight[0x10] = Bits(ltm->at.x);
    d3dLight[0x11] = Bits(ltm->at.y);
    d3dLight[0x12] = Bits(ltm->at.z);
    RwD3D9SetLight(*slot, d3dLight);
    const RwInt32 index = *slot;
    RwUInt32& capacity = StaticRef<RwUInt32>(kEnabledCapacity);
    if (StaticRef<RwUInt32>(kEnabledCount) >= capacity) {
        const RwUInt32 oldCapacity = capacity;
        capacity += 8;
        RwInt32*& array = StaticRef<RwInt32*>(kEnabledArray);
        if (array == nullptr) {
            array = static_cast<RwInt32*>(RwMalloc(capacity * 4, kLightArrayHint));
        } else {
            array = static_cast<RwInt32*>(RwRealloc(array, capacity * 4, kLightArrayHint));
        }
        for (RwUInt32 i = oldCapacity; i < StaticRef<RwUInt32>(kEnabledCapacity); ++i) {
            StaticRef<RwInt32*>(kEnabledArray)[i] = 0;
        }
    }
    StaticRef<RwInt32*>(kEnabledArray)[StaticRef<RwUInt32>(kEnabledCount)] = index;
    ++StaticRef<RwUInt32>(kEnabledCount);
}
} // namespace

// 0x755BF0 (lote S30A, sin validar; 230 bytes; cdecl, (luz, desplazamiento, tamano); destructor del plugin de luces del controlador D3D9; devuelve la luz): si la luz tiene indice D3D (dword >= 0) lo devuelve a la lista libre; si la
// lista libre ya contiene todos los indices usados (cuenta >= siguiente - 1) reinicia todo (cuenta, capacidad y siguiente a 0) y libera el vector; si no, lo anade al vector (que crece de 256 en 256 con RwMalloc/RwRealloc).
void* _rwD3D9LightPluginDestruct(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    if (object == nullptr) {
        return object;
    }
    const RwInt32 index = *reinterpret_cast<RwInt32*>(static_cast<RwUInt8*>(object) + StaticRef<RwInt32>(kLightIndexOffset));
    if (index < 0) {
        return object;
    }
    RwInt32& count = StaticRef<RwInt32>(kFreeCount);
    if (count >= StaticRef<RwInt32>(kNextIndex) - 1) {
        count = 0;
        StaticRef<RwInt32>(kFreeCapacity) = 0;
        StaticRef<RwInt32>(kNextIndex) = 0;
        if (StaticRef<void*>(kFreeArray) != nullptr) {
            RwFree(StaticRef<void*>(kFreeArray));
            StaticRef<void*>(kFreeArray) = nullptr;
        }
        return object;
    }
    if (count >= StaticRef<RwInt32>(kFreeCapacity)) {
        StaticRef<RwInt32>(kFreeCapacity) += 0x100;
        RwInt32*& array = StaticRef<RwInt32*>(kFreeArray);
        if (array == nullptr) {
            array = static_cast<RwInt32*>(RwMalloc(static_cast<RwUInt32>(StaticRef<RwInt32>(kFreeCapacity)) * 4, kLightArrayHint));
        } else {
            array = static_cast<RwInt32*>(RwRealloc(array, static_cast<RwUInt32>(StaticRef<RwInt32>(kFreeCapacity)) * 4, kLightArrayHint));
        }
    }
    StaticRef<RwInt32*>(kFreeArray)[StaticRef<RwInt32>(kFreeCount)] = index;
    ++StaticRef<RwInt32>(kFreeCount);
    return object;
}

// 0x756070 (lote S30A, sin validar; 489 bytes; cdecl, (banderas); `_rwD3D9LightsGlobalEnable`): pone a cero los acumuladores de ambiente (0x8E2418..0x8E2420) y a 1.0 el cuarto (0x8E2424) y recorre la lista de luces direccionales del mundo
// actual: las que comparten alguna bandera con `banderas` se tratan segun su subtipo: direccional (1) = D3D9EnableDirectionalLight; otro (ambiente) = suma su color a los acumuladores. Con alguna luz usada: RwD3D9SetRenderState(AMBIENT
// = 0x8B, 0xFFFFFFFF) y devuelve 1; sin ninguna: AMBIENT = 0 y devuelve 0.
RwBool _rwD3D9LightsGlobalEnable(RpLightFlag flags) {
    RwBool any = FALSE;
    StaticRef<RwUInt32>(0x8E2424) = 0x3F800000;
    StaticRef<RwUInt32>(0x8E2418) = 0;
    StaticRef<RwUInt32>(0x8E241C) = 0;
    StaticRef<RwUInt32>(0x8E2420) = 0;
    RpWorld* const world = static_cast<RpWorld*>(Globals()->curWorld);
    RwLLLink* const sentinel = &world->directionalLightList.link;
    for (RwLLLink* node = sentinel->next; node != sentinel; node = node->next) {
        RpLight* const light = reinterpret_cast<RpLight*>(reinterpret_cast<RwUInt8*>(node) - offsetof(RpLight, inWorld));
        if (light == nullptr || (static_cast<RwUInt8>(flags) & light->object.object.flags) == 0) {
            continue;
        }
        if (light->object.object.subType == 1) {
            D3D9EnableDirectionalLight(light);
        } else {
            float& r = StaticRef<float>(0x8E2418);
            float& g = StaticRef<float>(0x8E241C);
            float& b = StaticRef<float>(0x8E2420);
            r = static_cast<float>(static_cast<double>(r) + static_cast<double>(light->color.red));
            g = static_cast<float>(static_cast<double>(g) + static_cast<double>(light->color.green));
            b = static_cast<float>(static_cast<double>(b) + static_cast<double>(light->color.blue));
        }
        any = TRUE;
    }
    RwD3D9SetRenderState(0x8B, any ? 0xFFFFFFFFu : 0u);
    return any;
}

// 0x756260 (lote S30A, sin validar; 301 bytes; cdecl, (luz); `_rwD3D9LightDirectionalEnable`): activa la luz direccional en el dispositivo (D3D9EnableDirectionalLight). Devuelve TRUE.
RwBool _rwD3D9LightDirectionalEnable(RpLight* light) {
    D3D9EnableDirectionalLight(light);
    return TRUE;
}

// 0x758910 (lote S30A, sin validar; 12 bytes; cdecl, (cabecera de mallas)): libera la cabecera con la funcion de liberacion del motor (RwFree; en el EXE es un `jmp` a [motor + 0x138]).
void _rpMeshHeaderDestroy(RpMeshHeader* meshHeader) {
    RwFree(meshHeader);
}

// 0x75A940 (lote S30A, sin validar; 21 bytes; cdecl, (RpBuildMesh, datos)): genera la cabecera de mallas con el generador exhaustivo del EXE 0x75A960 con el tercer argumento a 1 (NO es raiz del plan: se conserva por direccion). 0x75B4E0 (lote S31A) es igual con otro valor.
RpMeshHeader* RpBuildMeshGenerateExhaustiveTriStrip(RpBuildMesh* buildmesh, void* data) {
    return reinterpret_cast<RpMeshHeader*(__cdecl*)(RpBuildMesh*, void*, RwInt32)>(0x75A960)(buildmesh, data, 1);
}

// 0x75D730 (lote S30A, sin validar; 567 bytes; cdecl, (malla, cabecera de mallas, datos); RpMeshCallBack que usa _rpTriListMeshGenerate; devuelve la malla): reordena los triangulos de la malla en pasadas: en cada pasada recorre los triangulos
// (en una copia) y deja, en orden en el vector original, los que no comparten ningun vertice con los ya elegidos en esa pasada (mapa de bits de vertices puesto a cero al empezar cada pasada); los elegidos se marcan en la copia con 0xFFFF.
// Repite hasta haber colocado todos los triangulos (numIndices / 3). Si no hay memoria no hace nada. (El mapa de bits se reserva y se pone a cero redondeado a multiplo de 4 bytes: el EXE reserva (maximo + 31) / 8 bytes, deja sin poner a cero hasta 3 y puede desbordar 1 a 3.)
RpMesh* _rpTriListMeshOptimize(RpMesh* mesh, RpMeshHeader* meshHeader, void* data) {
    UNUSED(meshHeader); UNUSED(data);
    RwUInt16* const indices = mesh->indices;
    const RwUInt32 numIndices = mesh->numIndices;
    const RwUInt32 numTriangles = numIndices / 3;
    RwUInt32 orAll = 0;
    for (RwUInt32 i = 0; i < numIndices; ++i) {
        orAll |= indices[i];
    }
    const RwUInt32 bitmapBytes = (orAll + 0x1F) >> 3;
    const RwUInt32 bitmapAlloc = (bitmapBytes + 3) & ~3u;
    RwUInt32* const bitmap = static_cast<RwUInt32*>(RwMalloc(bitmapAlloc, 0x10502));
    if (bitmap == nullptr) {
        return mesh;
    }
    RwUInt16* const copy = static_cast<RwUInt16*>(RwMalloc(numIndices * 2, 0x10502));
    if (copy == nullptr) {
        RwFree(bitmap);
        return mesh;
    }
    std::memcpy(copy, indices, numIndices * 2);
    RwUInt32 placed = 0;
    RwUInt16* out = indices;
    if (numTriangles != 0) {
        do {
            std::memset(bitmap, 0, bitmapAlloc);
            RwUInt16* tri = copy;
            for (RwUInt32 t = 0; t < numTriangles; ++t, tri += 3) {
                const RwUInt32 a = tri[0], b = tri[1], c = tri[2];
                if (static_cast<RwUInt16>(a & b & c) == 0xFFFF) {
                    continue;
                }
                const RwUInt32 bitA = 1u << (a & 0x1F), bitB = 1u << (b & 0x1F), bitC = 1u << (c & 0x1F);
                if ((bitmap[a >> 5] & bitA) != 0 || (bitmap[b >> 5] & bitB) != 0 || (bitmap[c >> 5] & bitC) != 0) {
                    continue;
                }
                bitmap[a >> 5] |= bitA;
                bitmap[b >> 5] |= bitB;
                bitmap[c >> 5] |= bitC;
                out[0] = static_cast<RwUInt16>(a);
                out[1] = static_cast<RwUInt16>(b);
                out[2] = static_cast<RwUInt16>(c);
                out += 3;
                tri[0] = tri[1] = tri[2] = 0xFFFF;
                ++placed;
            }
        } while (placed < numTriangles);
    }
    RwFree(copy);
    RwFree(bitmap);
    return mesh;
}

// 0x75D9D0 (lote S30A, sin validar; 261 bytes; cdecl, (geometria, resultado*); `RpGeometryIsCorrectlySorted`): sin la bandera nativa (0x1000000) calcula, por material, el rango [minimo, maximo] de indices de vertice de sus triangulos
// y suma (maximo - minimo + 1) de todos los materiales; *resultado = TRUE si esa suma <= numero de vertices (cada material usa un rango contiguo). Con geometria nativa la suma es 0 (TRUE). Sin memoria devuelve NULL. Devuelve la geometria.
const RpGeometry* RpGeometryIsCorrectlySorted(const RpGeometry* geometry, RwBool* result) {
    RwInt32 total = 0;
    if ((geometry->flags & 0x1000000) == 0) {
        struct Range { RwUInt16 minimum; RwUInt16 maximum; };
        const RwInt32 numMaterials = geometry->matList.numMaterials;
        Range* const ranges = static_cast<Range*>(RwMalloc(static_cast<RwUInt32>(numMaterials) * 4, 0x1050D));
        if (ranges == nullptr) {
            return nullptr;
        }
        for (RwInt32 i = 0; i < numMaterials; ++i) {
            ranges[i].minimum = 0xFFFF;
            ranges[i].maximum = 0;
        }
        for (RwInt32 t = 0; t < geometry->numTriangles; ++t) {
            const RpTriangle& triangle = geometry->triangles[t];
            Range& range = ranges[triangle.matIndex];
            for (int k = 0; k < 3; ++k) {
                const RwUInt16 vertex = triangle.vertIndex[k];
                if (vertex > range.maximum) {
                    range.maximum = vertex;
                }
                if (vertex < range.minimum) {
                    range.minimum = vertex;
                }
            }
        }
        for (RwInt32 i = 0; i < numMaterials; ++i) {
            total = total + static_cast<RwInt32>(static_cast<RwUInt32>(ranges[i].maximum) - ranges[i].minimum) + 1;
        }
        RwFree(ranges);
    }
    *result = total <= geometry->numVertices ? TRUE : FALSE;
    return geometry;
}

// 0x760DF0 (lote S30A, sin validar; 525 bytes; cdecl, (objeto, tipo, descriptor*); `_rxD3D9VertexShaderDefaultBeginCallBack`): rellena el descriptor de vertex shader (4 bytes) para el objeto. Tipo 1 (atomo): morphing si la geometria tiene
// mas de un morph target, el interpolador pasa de un objetivo a otro y posicion * 1/duracion != 0; numero de coordenadas de textura de la geometria; prelit (bandera 8), normales (0x10: normalizar si el uso D3D9 de la geometria tiene
// banderas o si el LTM no es identidad/ortonormal o la longitud al cuadrado de algun eje esta fuera de [0.9, 1.1]) y modulacion de material (0x40). Otro tipo (sector): lo mismo con las banderas del mundo actual (0x80 = 2 coordenadas,
// 4 = 1; 8 prelit; 0x10 normales; 0x40 modular). Al final lee la niebla (RwD3D9GetRenderState(0x1C)): con niebla activa y su tipo (1..3) fija fogMode.
void _rxD3D9VertexShaderDefaultBeginCallBack(void* object, RwUInt32 type, _rpD3D9VertexShaderDescriptor* desc) {
    RwUInt8* const bytes = reinterpret_cast<RwUInt8*>(desc);
    *reinterpret_cast<RwUInt32*>(desc) = 0;
    bool modulate;
    if (type == 1) {
        RpAtomic* const atomic = static_cast<RpAtomic*>(object);
        RpGeometry* const geometry = atomic->geometry;
        const RwUInt32 geometryFlags = geometry->flags;
        if (geometry->numMorphTargets != 1) {
            const RwUInt8* const raw = reinterpret_cast<const RwUInt8*>(atomic);
            if (*reinterpret_cast<const RwUInt16*>(raw + 0x50) != *reinterpret_cast<const RwUInt16*>(raw + 0x52)) {
                const double product = static_cast<double>(*reinterpret_cast<const float*>(raw + 0x5C)) * static_cast<double>(*reinterpret_cast<const float*>(raw + 0x58));
                if (!(product == 0.0)) {
                    bytes[2] |= 1;
                }
            }
        }
        const RwUInt8 texCoords = static_cast<RwUInt8>(static_cast<RwUInt8>(geometry->numTexCoordSets) << 4);
        bytes[1] = static_cast<RwUInt8>(((bytes[1] ^ texCoords) & 0xF) ^ texCoords);
        if (geometryFlags & 8) {
            bytes[2] |= 0x10;
        }
        if (geometryFlags & 0x10) {
            const RwUInt32 usage = static_cast<RwUInt32>(RpD3D9GeometryGetUsageFlags(geometry));
            bytes[2] |= 0x20;
            if ((usage & 0xFFF) != 0) {
                bytes[2] |= 0x40;
            } else {
                const RwMatrix* const ltm = RwFrameGetLTM(static_cast<RwFrame*>(atomic->object.object.parent));
                if ((ltm->flags & 0x20001) == 0) {
                    const uint32 maxBits = Bits(1.1f);   // 0x3F8CCCCD
                    const uint32 minBits = Bits(0.9f);   // 0x3F666666
                    const float* const m = reinterpret_cast<const float*>(ltm);
                    for (int row = 0; row < 3; ++row) {
                        const double x = m[row * 4], y = m[row * 4 + 1], z = m[row * 4 + 2];
                        const uint32 lengthBits = Bits(static_cast<float>((x * x + y * y) + z * z));
                        if (static_cast<int32>(lengthBits) > static_cast<int32>(maxBits) || static_cast<int32>(lengthBits) < static_cast<int32>(minBits)) {
                            bytes[2] |= 0x40;
                            break;
                        }
                    }
                }
            }
        }
        modulate = (geometryFlags & 0x40) != 0;
    } else {
        const RwUInt32 worldFlags = static_cast<RpWorld*>(Globals()->curWorld)->flags;
        if (static_cast<RwUInt8>(worldFlags) & 0x80) {
            bytes[1] = static_cast<RwUInt8>((bytes[1] & 0xF) | 0x20);
        } else if (worldFlags & 4) {
            bytes[1] = static_cast<RwUInt8>((bytes[1] & 0xF) | 0x10);
        }
        if (worldFlags & 8) {
            bytes[2] |= 0x10;
        }
        if (worldFlags & 0x10) {
            bytes[2] |= 0x20;
        }
        modulate = (worldFlags & 0x40) != 0;
    }
    if (modulate) {
        bytes[2] |= 0x80;
    }
    RwUInt32 fogEnabled = 0;
    RwD3D9GetRenderState(0x1C, &fogEnabled);
    if (fogEnabled != 0) {
        RwUInt32 fogType = 0;
        RwRenderStateGet(rwRENDERSTATEFOGTYPE, &fogType);
        if (fogType == 1) {
            bytes[3] = static_cast<RwUInt8>((bytes[3] & 0xFD) | 1);
        } else if (fogType == 2) {
            bytes[3] = static_cast<RwUInt8>((bytes[3] & 0xFE) | 2);
        } else if (fogType == 3) {
            bytes[3] |= 3;
        }
    }
}

// 0x761030 (lote S30A, sin validar; 311 bytes; cdecl, (cabecera de recurso, malla instanciada, descriptor, despacho); `_rxD3D9VertexShaderDefaultMeshRenderCallBack`): quita el pixel shader si hay uno puesto (cache 0x8E244C), configura la etapa 0
// (con textura: textura + modulacion color/alfa textura x difuso; sin textura: SELECTARG1 con el difuso), pone el vertex shader de la malla si cambio (cache 0x8E2448), vuelca los estados pendientes y dibuja (indexado con DrawIndexedPrimitive si hay buffer
// de indices en la cabecera; si no DrawPrimitive).
void _rxD3D9VertexShaderDefaultMeshRenderCallBack(RxD3D9ResEntryHeader* resEntryHeader, RxD3D9InstanceData* instancedMesh, const _rpD3D9VertexShaderDescriptor* desc, const _rpD3D9VertexShaderDispatchDescriptor* dispatch) {
    UNUSED(desc); UNUSED(dispatch);
    if (StaticRef<void*>(0x8E244C) != nullptr) {
        StaticRef<void*>(0x8E244C) = nullptr;
        _RwD3DDevice->SetPixelShader(nullptr);
    }
    RwTexture* const texture = instancedMesh->material->texture;
    if (texture != nullptr) {
        RwD3D9SetTexture(texture, 0);
        RwD3D9SetTextureStageState(0, 1, 4);
        RwD3D9SetTextureStageState(0, 2, 2);
        RwD3D9SetTextureStageState(0, 3, 0);
        RwD3D9SetTextureStageState(0, 4, 4);
        RwD3D9SetTextureStageState(0, 5, 2);
        RwD3D9SetTextureStageState(0, 6, 0);
    } else {
        RwD3D9SetTexture(nullptr, 0);
        RwD3D9SetTextureStageState(0, 1, 3);
        RwD3D9SetTextureStageState(0, 3, 0);
        RwD3D9SetTextureStageState(0, 4, 3);
        RwD3D9SetTextureStageState(0, 6, 0);
    }
    IDirect3DVertexShader9* const shader = instancedMesh->vertexShader;
    if (StaticRef<void*>(0x8E2448) != static_cast<void*>(shader)) {
        StaticRef<void*>(0x8E2448) = shader;
        _RwD3DDevice->SetVertexShader(instancedMesh->vertexShader);
    }
    if (resEntryHeader->indexBuffer != nullptr) {
        _rwD3D9RenderStateFlushCache();
        _RwD3DDevice->DrawIndexedPrimitive(resEntryHeader->primType, instancedMesh->baseIndex, 0, instancedMesh->numVertices, instancedMesh->startIndex, instancedMesh->numPrimitives);
    } else {
        _rwD3D9RenderStateFlushCache();
        _RwD3DDevice->DrawPrimitive(resEntryHeader->primType, instancedMesh->baseIndex, instancedMesh->numPrimitives);
    }
}

// 0x761720 (lote S30A, sin validar; 242 bytes; cdecl, (constantes*, descriptor*, coef. ambiente, coef. difuso); `_rpD3D9VertexShaderUpdateLightsColors`): multiplica las componentes x, y, z del color ambiente (constante 0) por el coeficiente ambiente
// y los colores de las luces por el difuso (direccionales: color en la constante 2 + 2k; puntuales: 2 constantes por luz y una mas cada 4; focos: 4 por luz y una mas cada 4), con el orden de constantes que dejan _rxD3D9VertexShaderDefaultLightingCallBack; despues
// envia al dispositivo desde el registro 4 todas las constantes tocadas (SetVertexShaderConstantF).
void _rpD3D9VertexShaderUpdateLightsColors(RwV4d* shaderConstantPtr, const _rpD3D9VertexShaderDescriptor* desc, RwReal ambientCoef, RwReal diffuseCoef) {
    const RwUInt8* const bytes = reinterpret_cast<const RwUInt8*>(desc);
    RwV4d* const base = shaderConstantPtr;
    base[0].x *= ambientCoef;
    base[0].y *= ambientCoef;
    base[0].z *= ambientCoef;
    RwV4d* e = base + 1;
    for (RwUInt32 n = bytes[0] & 0xF; n != 0; --n) {
        e[1].x *= diffuseCoef;
        e[1].y *= diffuseCoef;
        e[1].z *= diffuseCoef;
        e += 2;
    }
    for (RwUInt32 k = 0; k < static_cast<RwUInt32>(bytes[0] >> 4); ++k) {
        if ((k & 3) == 0) {
            ++e;
        }
        e[1].x *= diffuseCoef;
        e[1].y *= diffuseCoef;
        e[1].z *= diffuseCoef;
        e += 2;
    }
    for (RwUInt32 k = 0; k < static_cast<RwUInt32>(bytes[1] & 0xF); ++k) {
        if ((k & 3) == 0) {
            ++e;
        }
        e[3].x *= diffuseCoef;
        e[3].y *= diffuseCoef;
        e[3].z *= diffuseCoef;
        e += 4;
    }
    _RwD3DDevice->SetVertexShaderConstantF(4, reinterpret_cast<const float*>(base), static_cast<UINT>(e - base));
}

// 0x761820 (lote S30A, sin validar; 134 bytes; cdecl, (color*, despacho*); `_rpD3D9VertexShaderUpdateMaterialColor`): envia el color del material (r, g, b, a de 0 a 255 divididos por 255, multiplicando por 1/255 en float) como una
// constante de vertex shader en el registro offsetMaterialColor del despacho.
void _rpD3D9VertexShaderUpdateMaterialColor(const RwRGBA* color, const _rpD3D9VertexShaderDispatchDescriptor* dispatch) {
    const float k = F32(0x3B808081);
    const float values[4] = {
        static_cast<float>(static_cast<double>(color->red) * k),
        static_cast<float>(static_cast<double>(color->green) * k),
        static_cast<float>(static_cast<double>(color->blue) * k),
        static_cast<float>(static_cast<double>(color->alpha) * k),
    };
    _RwD3DDevice->SetVertexShaderConstantF(dispatch->offsetMaterialColor, values, 1);
}

// 0x7618B0 (lote S30A, sin validar; 156 bytes; cdecl, (descriptor*, despacho*); `_rpD3D9VertexShaderUpdateFogData`): constante de niebla {a, b, 0, 1} en el registro offsetFogRange: fogMode 1 (lineal): a = 1 / (near - far) de la camara actual
// y b = -(far * a); fogMode 2 y 3: a = densidad de niebla (estado 0x26) * 1.4426950 (1/ln 2) y b = 0; fogMode 0: el EXE envia pila sin inicializar (aqui ceros).
void _rpD3D9VertexShaderUpdateFogData(const _rpD3D9VertexShaderDescriptor* desc, const _rpD3D9VertexShaderDispatchDescriptor* dispatch) {
    float data[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    const RwUInt32 fogMode = reinterpret_cast<const RwUInt8*>(desc)[3] & 3;
    if (fogMode == 1) {
        const RwCamera* const camera = static_cast<const RwCamera*>(Globals()->curCamera);
        const double nearPlane = camera->nearPlane;
        const double farPlane = camera->farPlane;
        const float a = static_cast<float>(1.0 / (nearPlane - farPlane));
        data[0] = a;
        data[1] = static_cast<float>(-(static_cast<double>(a) * farPlane));
        data[2] = 0.0f;
        data[3] = 1.0f;
    } else if (fogMode != 0) {
        RwUInt32 density = 0;
        RwD3D9GetRenderState(0x26, &density);
        float densityValue;
        std::memcpy(&densityValue, &density, sizeof(densityValue));
        data[0] = static_cast<float>(static_cast<double>(densityValue) * static_cast<double>(F32(0x3FB8AA3B)));
        data[1] = 0.0f;
        data[2] = 0.0f;
        data[3] = 1.0f;
    }
    _RwD3DDevice->SetVertexShaderConstantF(dispatch->offsetFogRange, data, 1);
}

// 0x761950 (lote S30A, sin validar; 78 bytes; cdecl, (coef. de morphing, despacho*); `_rpD3D9VertexShaderUpdateMorphingCoef`): envia {coef, 1 - coef, 1, 0} al registro offsetMorphingCoef del despacho.
void _rpD3D9VertexShaderUpdateMorphingCoef(RwReal morphingCoef, const _rpD3D9VertexShaderDispatchDescriptor* dispatch) {
    const float data[4] = { morphingCoef, static_cast<float>(1.0 - static_cast<double>(morphingCoef)), 1.0f, 0.0f };
    _RwD3DDevice->SetVertexShaderConstantF(dispatch->offsetMorphingCoef, data, 1);
}

// 0x7619A0 (lote S30A, sin validar; 99 bytes; cdecl, (matriz*, despacho*); `_rpD3DVertexShaderSetUVAnimMatrix`): envia dos constantes (las filas x e y de la matriz de animacion UV transpuesta): {right.x, up.x, at.x, pos.x} y {right.y, up.y, at.y, pos.y}
// en el registro offsetEffect del despacho.
void _rpD3DVertexShaderSetUVAnimMatrix(RwMatrix* matrix, const _rpD3D9VertexShaderDispatchDescriptor* dispatch) {
    const float data[8] = {
        matrix->right.x, matrix->up.x, matrix->at.x, matrix->pos.x,
        matrix->right.y, matrix->up.y, matrix->at.y, matrix->pos.y,
    };
    _RwD3DDevice->SetVertexShaderConstantF(dispatch->offsetEffect, data, 2);
}

// 0x761C50 (lote S30A, sin validar; 11 bytes; cdecl, (sector); `RpWorldSectorRender`): llama al callback de render de sectores del mundo actual (mundo + 0x68) con el sector (en el EXE es un `jmp`) y devuelve su resultado.
RpWorldSector* RpWorldSectorRender(RpWorldSector* worldSector) {
    return static_cast<RpWorld*>(Globals()->curWorld)->renderCallBack(worldSector);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
// S31A: creacion, tamano y escritura de clumps, geometrias y atomos; caja del mundo; formato de normales; luces locales; mallas y callbacks de vertex shaders
// ---------------------------------------------------------------------------------------------------------------------------------------------

// Lista de geometrias de un clump (mismo tipo que usa rpworld.cpp) y su liberacion (rpworld.cpp, sin cabecera).
struct RpGeometryList {
    RpGeometry** geometries;
    RwInt32      numGeoms;
};
// 0x74AE50 (_rpGeometryListDeinitialize, funcion estatica en rpworld.cpp): destruye las geometrias y libera el vector.
static RpGeometryList* _rpGeometryListDeinitialize(RpGeometryList* geomList) {
    for (RwInt32 i = 0; i < geomList->numGeoms; ++i) {
        RpGeometryDestroy(geomList->geometries[i]);
    }
    if (geomList->geometries != nullptr) {
        RwFree(geomList->geometries);
        geomList->geometries = nullptr;
    }
    return geomList;
}

namespace {
// Contexto de escritura de un clump (24 bytes en la pila del EXE): flujo, lista de marcos, lista de geometrias y bandera de exito (la ponen a 0 los callbacks de escritura de luces, camaras y atomos).
struct ClumpWriteContext {
    RwStream*      stream;
    rwFrameList    frameList;
    RpGeometryList geomList;
    RwInt32        ok;
};
static_assert(sizeof(ClumpWriteContext) == 0x18);

// Funciones del EXE que no son raices del plan y se conservan por direccion.
using BuildGeometryListFn = RwBool(__cdecl*)(RpGeometryList*, RpClump*);                    // 0x74A770: reune las geometrias distintas de los atomos del clump
using ClumpAtomicWriteFn  = RpAtomic*(__cdecl*)(RpAtomic*, ClumpWriteContext*);             // 0x74A880: escribe un atomo (y sus datos) del clump
using ClumpLightWriteFn   = RpLight*(__cdecl*)(RpLight*, ClumpWriteContext*);               // 0x74AEA0
using ClumpCameraWriteFn  = RwCamera*(__cdecl*)(RwCamera*, ClumpWriteContext*);             // 0x74AF20
inline RwBool BuildGeometryList(RpGeometryList* list, RpClump* clump) { return reinterpret_cast<BuildGeometryListFn>(0x74A770)(list, clump); }

constexpr uintptr_t kAtomicPluginRegistry   = 0x8D624C;
constexpr uintptr_t kClumpPluginRegistry    = 0x8D6264;
constexpr uintptr_t kGeometryPluginRegistry = 0x8D628C;
constexpr RwUInt32  kStreamVersion          = 0x36003;
constexpr RwUInt32  kStreamBuild            = 0xFFFF;

inline RwStream* WriteChunkHeader(RwStream* stream, RwUInt32 type, RwUInt32 size) {
    return _rwStreamWriteVersionedChunkHeader(stream, type, size, kStreamVersion, kStreamBuild);
}

// Nodos de las listas de luces y camaras de un clump: la luz/camara esta en nodo - desplazamiento del plugin - 4 (desplazamientos 0xC924DC y 0xC924D8).
template<typename T> inline T* ClumpListObject(RwLLLink* node, uintptr_t offsetAddress) {
    return reinterpret_cast<T*>(reinterpret_cast<RwUInt8*>(node) - static_cast<uintptr_t>(StaticRef<RwInt32>(offsetAddress)) - 4);
}
inline RpAtomic* ClumpAtomicOfLink(RwLLLink* node) { return reinterpret_cast<RpAtomic*>(reinterpret_cast<RwUInt8*>(node) - 0x40); }
} // namespace

// 0x749970 (lote S31A, sin validar; 432 bytes; cdecl, (posicion*, radio)): crea un clump con un unico atomo y una geometria de espacio (RpGeometryCreateSpace), sin pintar: marco nuevo con la posicion, atomo con ese marco y la
// geometria (esfera = la del morph target 0), bandera de atomo +2 sin el bit 4, y el atomo metido al principio de la lista del clump. Si algo falla libera lo creado en orden inverso y devuelve NULL.
RpClump* RpClumpCreateSpace(const RwV3d* position, RwReal radius) {
    RpGeometry* const geometry = RpGeometryCreateSpace(radius);
    if (geometry == nullptr) {
        return nullptr;
    }
    RwFrame* const frame = RwFrameCreate();
    if (frame == nullptr) {
        RpGeometryDestroy(geometry);
        return nullptr;
    }
    std::memcpy(reinterpret_cast<RwUInt8*>(frame) + 0x40, position, sizeof(RwV3d));
    RpAtomic* const atomic = RpAtomicCreate();
    if (atomic == nullptr) {
        RwFrameDestroy(frame);
        RpGeometryDestroy(geometry);
        return nullptr;
    }
    RpClump* const clump = RpClumpCreate();
    if (clump == nullptr) {
        RpAtomicDestroy(atomic);
        RwFrameDestroy(frame);
        RpGeometryDestroy(geometry);
        return nullptr;
    }
    _rwObjectHasFrameSetFrame(atomic, frame);
    atomic->object.object.privateFlags |= 1;
    if (geometry != atomic->geometry) {
        RpGeometryAddRef(geometry);
        if (atomic->geometry != nullptr) {
            RpGeometryDestroy(atomic->geometry);
        }
        atomic->geometry = geometry;
        atomic->boundingSphere = geometry->morphTarget[0].boundingSphere;
        RwFrame* const parent = static_cast<RwFrame*>(atomic->object.object.parent);
        if (parent != nullptr && RpAtomicGetWorld(atomic) != nullptr) {
            RwFrameUpdateObjects(parent);
        }
    }
    atomic->object.object.flags &= static_cast<RwUInt8>(~4u);
    RpGeometryDestroy(geometry);
    RwLLLink* const link = &atomic->inClumpLink;
    link->next = clump->atomicList.link.next;
    link->prev = &clump->atomicList.link;
    clump->atomicList.link.next->prev = link;
    clump->atomicList.link.next = link;
    atomic->clump = clump;
    clump->object.parent = frame;
    return clump;
}

// 0x74A5E0 (lote S31A, sin validar; 390 bytes; cdecl, (clump)): tamano en flujo del clump: lista de marcos (_rwFrameListStreamGetSize + 0x24) + lista de geometrias (0x10 + 0xC y RpGeometryStreamGetSize por cada una, que se destruyen y
// liberan) + 0x28 + plugins del atomo + 0xC por cada atomo + 0x10 + RpLightStreamGetSize + 0xC por cada luz + 0x10 + RwCameraStreamGetSize + 0xC por cada camara + los plugins del clump + 0xC. Con fallo al preparar las listas devuelve 0.
RwUInt32 RpClumpStreamGetSize(RpClump* clump) {
    rwFrameList frameList;
    if (_rwFrameListInitialize(&frameList, static_cast<RwFrame*>(clump->object.parent)) == nullptr) {
        return 0;
    }
    RwUInt32 size = _rwFrameListStreamGetSize(&frameList) + 0x24;
    _rwFrameListDeinitialize(&frameList);
    RpGeometryList geomList{};
    geomList.numGeoms = 0;
    if (!BuildGeometryList(&geomList, clump)) {
        return 0;
    }
    RwUInt32 geometriesSize = 0x10;
    for (RwInt32 i = 0; i < geomList.numGeoms; ++i) {
        geometriesSize += RpGeometryStreamGetSize(geomList.geometries[i]) + 0xC;
    }
    size += geometriesSize + 0xC;
    for (RwInt32 i = 0; i < geomList.numGeoms; ++i) {
        RpGeometryDestroy(geomList.geometries[i]);
    }
    if (geomList.geometries != nullptr) {
        RwFree(geomList.geometries);
        geomList.geometries = nullptr;
    }
    RwLLLink* const atomics = &clump->atomicList.link;
    for (RwLLLink* node = atomics->next; node != atomics;) {
        RwLLLink* const next = node->next;
        RpAtomic* const atomic = ClumpAtomicOfLink(node);
        size += 0x28;
        size = size + _rwPluginRegistryGetSize(reinterpret_cast<const RwPluginRegistry*>(kAtomicPluginRegistry), atomic) + 0xC;
        if (atomic == nullptr) {
            break;
        }
        node = next;
    }
    RwLLLink* const lights = &clump->lightList.link;
    for (RwLLLink* node = lights->next; node != lights;) {
        RwLLLink* const next = node->next;
        RpLight* const light = ClumpListObject<RpLight>(node, 0xC924DC);
        size += 0x10;
        size = size + RpLightStreamGetSize(light) + 0xC;
        if (light == nullptr) {
            break;
        }
        node = next;
    }
    RwLLLink* const cameras = &clump->cameraList.link;
    for (RwLLLink* node = cameras->next; node != cameras;) {
        RwLLLink* const next = node->next;
        RwCamera* const camera = ClumpListObject<RwCamera>(node, 0xC924D8);
        size += 0x10;
        size = size + RwCameraStreamGetSize(camera) + 0xC;
        if (camera == nullptr) {
            break;
        }
        node = next;
    }
    return size + _rwPluginRegistryGetSize(reinterpret_cast<const RwPluginRegistry*>(kClumpPluginRegistry), clump) + 0xC;
}

// 0x74A850 (lote S31A, sin validar; 48 bytes; cdecl, (atomo, flujo)): escribe un atomo suelto: arma un contexto de escritura de clump (flujo, lista de marcos y de geometrias vacias, bandera de exito = 1) y llama a la escritura
// de atomo del clump del EXE (0x74A880, que no es raiz del plan). Devuelve su resultado.
RpAtomic* RpAtomicStreamWrite(RpAtomic* atomic, RwStream* stream) {
    ClumpWriteContext context{};
    context.stream = stream;
    context.frameList.numFrames = 0;
    context.geomList.numGeoms = 0;
    context.ok = 1;
    return reinterpret_cast<ClumpAtomicWriteFn>(0x74A880)(atomic, &context);
}

// --- Codigo vivo CAM (sin validar): callbacks de RpClumpStreamWrite ---
// 0x74AE40 (14 bytes; cdecl, (luz, contador)): suma uno al contador (dword apuntado por el dato) y devuelve la luz.
static RpLight* ClumpLightCountCallBack(RpLight* light, void* data) {
    ++*static_cast<RwUInt32*>(data);
    return light;
}

// 0x74B000 (45 bytes; cdecl, (atomo, total)): suma al total 0x28 y despues el tamano de los plugins del atomo (_rwPluginRegistryGetSize, registro 0x8D624C) + 0xC. Devuelve el atomo.
static RpAtomic* ClumpAtomicStreamSizeCallBack(RpAtomic* atomic, void* data) {
    RwUInt32& total = *static_cast<RwUInt32*>(data);
    total += 0x28;
    total += static_cast<RwUInt32>(_rwPluginRegistryGetSize(reinterpret_cast<const RwPluginRegistry*>(kAtomicPluginRegistry), atomic)) + 0xC;
    return atomic;
}

// 0x74AFA0 (40 bytes; cdecl, (luz, total)): suma al total 0x10 y despues RpLightStreamGetSize(luz) + 0xC. Devuelve la luz.
static RpLight* ClumpLightStreamSizeCallBack(RpLight* light, void* data) {
    RwUInt32& total = *static_cast<RwUInt32*>(data);
    total += 0x10;
    total += RpLightStreamGetSize(light) + 0xC;
    return light;
}

// 0x74AFD0 (40 bytes; cdecl, (camara, total)): suma al total 0x10 y despues RwCameraStreamGetSize(camara) + 0xC. Devuelve la camara.
static RwCamera* ClumpCameraStreamSizeCallBack(RwCamera* camera, void* data) {
    RwUInt32& total = *static_cast<RwUInt32*>(data);
    total += 0x10;
    total += RwCameraStreamGetSize(camera) + 0xC;
    return camera;
}

// 0x74AEA0 (119 bytes; cdecl, (luz, contexto de escritura)): busca el marco de la luz (+4) en la lista de marcos del contexto (_rwFrameListFindFrame); con indice: cabecera de estructura de 4 bytes (version 0x36003, build
// 0xFFFF), el indice (RwStreamWriteInt32, 4 bytes) y RpLightStreamWrite. Si algo falla pone el campo `ok` del contexto (+0x14) a cero y devuelve NULL; si no, la luz.
static RpLight* ClumpLightWriteCallBack(RpLight* light, ClumpWriteContext* context) {
    RwInt32 index;
    if (_rwFrameListFindFrame(&context->frameList, static_cast<RwFrame*>(light->object.object.parent), &index)
        && WriteChunkHeader(context->stream, 1, 4)
        && RwStreamWriteInt32(context->stream, &index, 4)
        && RpLightStreamWrite(light, context->stream)) {
        return light;
    }
    context->ok = 0;
    return nullptr;
}

// 0x74AF20 (119 bytes; cdecl, (camara, contexto de escritura)): igual para una camara (marco en +4) con RwCameraStreamWrite.
static RwCamera* ClumpCameraWriteCallBack(RwCamera* camera, ClumpWriteContext* context) {
    RwInt32 index;
    if (_rwFrameListFindFrame(&context->frameList, static_cast<RwFrame*>(camera->object.object.parent), &index)
        && WriteChunkHeader(context->stream, 1, 4)
        && RwStreamWriteInt32(context->stream, &index, 4)
        && RwCameraStreamWrite(camera, context->stream)) {
        return camera;
    }
    context->ok = 0;
    return nullptr;
}

// 0x74AA10 (lote S31A, sin validar; 1059 bytes; cdecl, (clump, flujo)): escribe el clump: cabecera de clump (tipo 0x10) con su tamano (calculado como RpClumpStreamGetSize pero con los callbacks 0x74B000, 0x74AFA0 y 0x74AFD0 (C++ de codigo vivo CAM); si
// falla la preparacion de listas el EXE escribe tamano 0 y sigue), cabecera de estructura (12 bytes: numero de atomos, luces y camaras), lista de marcos, lista de geometrias (cabecera 0x1A, estructura con el numero y cada RpGeometryStreamWrite),
// los atomos (0x74A880 del EXE), las luces (0x74AEA0) y las camaras (0x74AF20) y los plugins del clump. Si algo falla devuelve NULL (sin liberar la lista de marcos si falla despues, igual que el EXE); si va bien, el clump.
RpClump* RpClumpStreamWrite(RpClump* clump, RwStream* stream) {
    RwUInt32 size = 0;
    {
        rwFrameList frameList;
        if (_rwFrameListInitialize(&frameList, static_cast<RwFrame*>(clump->object.parent)) != nullptr) {
            RwUInt32 total = _rwFrameListStreamGetSize(&frameList) + 0x18 + 0xC;
            _rwFrameListDeinitialize(&frameList);
            RpGeometryList geomList{};
            geomList.numGeoms = 0;
            if (BuildGeometryList(&geomList, clump)) {
                RwUInt32 geometriesSize = 0x10;
                for (RwInt32 i = 0; i < geomList.numGeoms; ++i) {
                    geometriesSize += RpGeometryStreamGetSize(geomList.geometries[i]) + 0xC;
                }
                total = total + geometriesSize + 0xC;
                _rpGeometryListDeinitialize(&geomList);
                RpClumpForAllAtomics(clump, ClumpAtomicStreamSizeCallBack, &total);
                RpClumpForAllLights(clump, ClumpLightStreamSizeCallBack, &total);
                RpClumpForAllCameras(clump, ClumpCameraStreamSizeCallBack, &total);
                total = total + _rwPluginRegistryGetSize(reinterpret_cast<const RwPluginRegistry*>(kClumpPluginRegistry), clump) + 0xC;
                size = total;
            }
        }
    }
    if (!WriteChunkHeader(stream, 0x10, size)) {
        return nullptr;
    }
    if (!WriteChunkHeader(stream, 1, 0xC)) {
        return nullptr;
    }
    RwUInt32 counts[3] = { 0, 0, 0 };
    {
        RwLLLink* const sentinel = &clump->atomicList.link;
        for (RwLLLink* node = sentinel->next; node != sentinel; node = node->next) {
            ++counts[0];
            if (ClumpAtomicOfLink(node) == nullptr) {
                break;
            }
        }
    }
    RwUInt32 lightCount = 0;
    RpClumpForAllLights(clump, ClumpLightCountCallBack, &lightCount);
    counts[1] = lightCount;
    {
        RwLLLink* const sentinel = &clump->cameraList.link;
        for (RwLLLink* node = sentinel->next; node != sentinel; node = node->next) {
            ++counts[2];
            if (ClumpListObject<RwCamera>(node, 0xC924D8) == nullptr) {
                break;
            }
        }
    }
    if (!RwStreamWrite(stream, counts, 0xC)) {
        return nullptr;
    }
    ClumpWriteContext context{};
    if (_rwFrameListInitialize(&context.frameList, static_cast<RwFrame*>(clump->object.parent)) == nullptr) {
        return nullptr;
    }
    if (!_rwFrameListStreamWrite(&context.frameList, stream)) {
        _rwFrameListDeinitialize(&context.frameList);
        return nullptr;
    }
    context.geomList.numGeoms = 0;
    if (!BuildGeometryList(&context.geomList, clump)) {
        return nullptr;
    }
    const auto abortWithGeometries = [&]() -> RpClump* {
        for (RwInt32 i = 0; i < context.geomList.numGeoms; ++i) {
            RpGeometryDestroy(context.geomList.geometries[i]);
        }
        if (context.geomList.geometries != nullptr) {
            RwFree(context.geomList.geometries);
        }
        return nullptr;
    };
    RwUInt32 geometriesSize = 0x10;
    for (RwInt32 i = 0; i < context.geomList.numGeoms; ++i) {
        geometriesSize += RpGeometryStreamGetSize(context.geomList.geometries[i]) + 0xC;
    }
    if (!WriteChunkHeader(stream, 0x1A, geometriesSize)) {
        return abortWithGeometries();
    }
    if (!WriteChunkHeader(stream, 1, 4)) {
        return abortWithGeometries();
    }
    const RwInt32 numGeoms = context.geomList.numGeoms;
    if (!RwStreamWrite(stream, &numGeoms, 4)) {
        return abortWithGeometries();
    }
    for (RwInt32 i = 0; i < context.geomList.numGeoms; ++i) {
        if (!RpGeometryStreamWrite(context.geomList.geometries[i], stream)) {
            return abortWithGeometries();
        }
    }
    context.stream = stream;
    context.ok = 1;
    {
        RwLLLink* const sentinel = &clump->atomicList.link;
        for (RwLLLink* node = sentinel->next; node != sentinel;) {
            RwLLLink* const next = node->next;
            if (!reinterpret_cast<ClumpAtomicWriteFn>(0x74A880)(ClumpAtomicOfLink(node), &context)) {
                break;
            }
            node = next;
        }
    }
    if (counts[1] > 0 && context.ok != 0) {
        RwLLLink* const sentinel = &clump->lightList.link;
        for (RwLLLink* node = sentinel->next; node != sentinel;) {
            RwLLLink* const next = node->next;
            if (!ClumpLightWriteCallBack(ClumpListObject<RpLight>(node, 0xC924DC), &context)) {
                break;
            }
            node = next;
        }
    }
    if (counts[2] > 0 && context.ok != 0) {
        RwLLLink* const sentinel = &clump->cameraList.link;
        for (RwLLLink* node = sentinel->next; node != sentinel;) {
            RwLLLink* const next = node->next;
            if (!ClumpCameraWriteCallBack(ClumpListObject<RwCamera>(node, 0xC924D8), &context)) {
                break;
            }
            node = next;
        }
    }
    _rpGeometryListDeinitialize(&context.geomList);
    _rwFrameListDeinitialize(&context.frameList);
    if (context.ok == 0) {
        return nullptr;
    }
    return _rwPluginRegistryWriteDataChunks(reinterpret_cast<const RwPluginRegistry*>(kClumpPluginRegistry), stream, clump) != nullptr ? clump : nullptr;
}

// 0x74BFE0 (lote S31A, sin validar; 329 bytes; cdecl, (geometria, matriz*)): marca la geometria como modificada en vertices y normales (+0xC |= 6) y transforma cada morph target con la matriz: puntos (RwV3dTransformPoints), normales
// (RwV3dTransformVectors y _rwV3dNormalize una a una) y recalcula su esfera (RpMorphTargetCalcBoundingSphere). Despues RpGeometryUnlock: si falla destruye la geometria (RpGeometryDestroy) y devuelve NULL; si va bien, la geometria.
RpGeometry* RpGeometryTransform(RpGeometry* geometry, const RwMatrix* matrix) {
    *reinterpret_cast<RwUInt8*>(&geometry->lockedSinceLastInst) |= 6;
    for (RwInt32 i = 0; i < geometry->numMorphTargets; ++i) {
        RpMorphTarget* const morphTarget = &geometry->morphTarget[i];
        RwV3d* const verts = morphTarget->verts;
        RwV3d* const normals = morphTarget->normals;
        RwV3dTransformPoints(verts, verts, geometry->numVertices, matrix);
        if (normals != nullptr) {
            RwV3dTransformVectors(normals, normals, geometry->numVertices, matrix);
            for (RwInt32 n = 0; n < geometry->numVertices; ++n) {
                _rwV3dNormalize(&normals[n], &normals[n]);
            }
        }
        RpMorphTargetCalcBoundingSphere(morphTarget, &morphTarget->boundingSphere);
    }
    if (!RpGeometryUnlock(geometry)) {
        RpGeometryDestroy(geometry);
        return nullptr;
    }
    return geometry;
}

// 0x74CED0 (lote S31A, sin validar; 693 bytes; cdecl, (geometria, flujo)): escribe la geometria: cabecera 0xF con el tamano total (estructura 0x74CE60 del EXE + lista de materiales + plugins + 0xC por parte), cabecera de estructura y
// 16 bytes {texCoords << 16 | banderas, triangulos, vertices, morph targets}; sin la bandera nativa y con vertices: colores previos (banderas & 8), cada juego de coordenadas de textura, y los triangulos como 2 dwords
// {v0 << 16 | v1, v2 << 16 | material}; despues, por cada morph target 24 bytes {esfera, hayVertices, hayNormales} y sus vertices y normales; la lista de materiales y los plugins. Si algo falla devuelve NULL; si no, la geometria.
const RpGeometry* RpGeometryStreamWrite(const RpGeometry* geometry, RwStream* stream) {
    RwUInt32 total = static_cast<RwUInt32>(RpGeometryStreamStructSize(geometry)) + 0xC;
    total = total + _rpMaterialListStreamGetSize(&geometry->matList) + 0xC;
    total = total + _rwPluginRegistryGetSize(reinterpret_cast<const RwPluginRegistry*>(kGeometryPluginRegistry), geometry) + 0xC;
    if (!WriteChunkHeader(stream, 0xF, total)) {
        return nullptr;
    }
    if (!WriteChunkHeader(stream, 1, static_cast<RwUInt32>(RpGeometryStreamStructSize(geometry)))) {
        return nullptr;
    }
    RwUInt32 header[4];
    header[0] = ((static_cast<RwUInt32>(geometry->numTexCoordSets) & 0xFF) << 16) | geometry->flags;
    header[1] = geometry->numTriangles;
    header[2] = geometry->numVertices;
    header[3] = geometry->numMorphTargets;
    if (!RwStreamWrite(stream, header, 0x10)) {
        return nullptr;
    }
    if ((geometry->flags & 0x1000000) == 0 && geometry->numVertices != 0) {
        if (geometry->flags & 8) {
            if (!RwStreamWrite(stream, geometry->preLitLum, static_cast<RwUInt32>(geometry->numVertices) * 4)) {
                return nullptr;
            }
        }
        if (geometry->numTexCoordSets > 0) {
            const RwUInt32 texCoordBytes = static_cast<RwUInt32>(geometry->numVertices) * 8;
            for (RwInt32 i = 0; i < geometry->numTexCoordSets; ++i) {
                if (!RwStreamWriteReal(stream, reinterpret_cast<const RwReal*>(geometry->texCoords[i]), texCoordBytes)) {
                    return nullptr;
                }
            }
        }
        if (geometry->numTriangles != 0) {
            const RpTriangle* triangle = geometry->triangles;
            for (RwInt32 t = 0; t < geometry->numTriangles; ++t, ++triangle) {
                RwUInt32 words[2];
                words[0] = (static_cast<RwUInt32>(triangle->vertIndex[0]) << 16) | triangle->vertIndex[1];
                words[1] = (static_cast<RwUInt32>(triangle->vertIndex[2]) << 16) | static_cast<RwUInt16>(triangle->matIndex);
                if (!RwStreamWrite(stream, words, 8)) {
                    return nullptr;
                }
            }
        }
    }
    for (RwInt32 i = 0; i < geometry->numMorphTargets; ++i) {
        const RpMorphTarget* const morphTarget = &geometry->morphTarget[i];
        struct MorphHeader { RwSphere sphere; RwUInt32 hasVerts; RwUInt32 hasNormals; } morphHeader;
        morphHeader.sphere = morphTarget->boundingSphere;
        if (geometry->flags & 0x1000000) {
            morphHeader.hasVerts = 0;
            morphHeader.hasNormals = 0;
        } else {
            morphHeader.hasVerts = morphTarget->verts != nullptr ? 1 : 0;
            morphHeader.hasNormals = morphTarget->normals != nullptr ? 1 : 0;
        }
        if (!RwStreamWrite(stream, &morphHeader, 0x18)) {
            return nullptr;
        }
        if (morphHeader.hasVerts != 0) {
            if (!RwStreamWriteReal(stream, reinterpret_cast<const RwReal*>(morphTarget->verts), static_cast<RwUInt32>(geometry->numVertices) * 12)) {
                return nullptr;
            }
        }
        if (morphHeader.hasNormals != 0) {
            if (!RwStreamWriteReal(stream, reinterpret_cast<const RwReal*>(morphTarget->normals), static_cast<RwUInt32>(geometry->numVertices) * 12)) {
                return nullptr;
            }
        }
    }
    if (!_rpMaterialListStreamWrite(&geometry->matList, stream)) {
        return nullptr;
    }
    return _rwPluginRegistryWriteDataChunks(reinterpret_cast<const RwPluginRegistry*>(kGeometryPluginRegistry), stream, geometry) != nullptr ? geometry : nullptr;
}

// 0x74EFA0 (lote S31A, sin validar; 120 bytes; cdecl, (mundo, caja*)): recorre el arbol de sectores del mundo (pila local de 64 entradas: baja por la izquierda y apila la derecha de cada plano) y, en cada sector hoja (tipo < 0), mete
// en la caja los puntos inf y sup de su caja ajustada (la primera vez con RwBBoxInitialize y despues con RwBBoxAddPoint). Devuelve TRUE.
RwBool _rpWorldFindBBox(RpWorld* world, RwBBox* boundingBox) {
    RwUInt32 stack[65];
    RwInt32 depth = 0;
    bool initialized = false;
    RpSector* sector = world->rootSector;
    while (depth >= 0) {
        if (*reinterpret_cast<RwInt32*>(sector) < 0) {
            RpWorldSector* const leaf = reinterpret_cast<RpWorldSector*>(sector);
            if (!initialized) {
                RwBBoxInitialize(boundingBox, &leaf->tightBoundingBox.inf);
                initialized = true;
            } else {
                RwBBoxAddPoint(boundingBox, &leaf->tightBoundingBox.inf);
            }
            RwBBoxAddPoint(boundingBox, &leaf->tightBoundingBox.sup);
            sector = reinterpret_cast<RpSector*>(static_cast<uintptr_t>(stack[depth]));
            --depth;
        } else {
            RpPlaneSector* const plane = reinterpret_cast<RpPlaneSector*>(sector);
            sector = plane->leftSubTree;
            ++depth;
            stack[depth] = static_cast<RwUInt32>(reinterpret_cast<uintptr_t>(plane->rightSubTree));
        }
    }
    return TRUE;
}

// 0x755980 (lote S31A, sin validar; 283 bytes; cdecl, (vertices, numero); _rpD3D9FindFormatV3d del SDK): formato de declaracion D3D para un vector de posiciones/normales segun su minimo y maximo (_rpD3D9GetMinMaxValuesV3d): FLOAT3 (2) por defecto;
// con todo <= 1: si el minimo es >= 0 en las tres componentes -> DEC3N (0xE) si el bit 7 de las capacidades (byte 0xC9BFEC) esta puesto o USHORT4N (0xC) si el bit 0x20; si el minimo es >= -1 en las tres -> DEC3N con el bit 7 o SHORT4N (0xA) con el bit 8.
RwUInt32 _rpD3D9FindFormatV3d(const RwV3d* src, RwInt32 numElements) {
    RwV3d minimum, maximum;
    _rpD3D9GetMinMaxValuesV3d(src, numElements, &minimum, &maximum);
    RwUInt32 format = 2;
    if (!(maximum.x <= 1.0f) || !(maximum.y <= 1.0f) || !(maximum.z <= 1.0f)) {
        return format;
    }
    const RwUInt8 caps = StaticRef<RwUInt8>(0xC9BFEC);
    if (!(minimum.x < 0.0f) && !(minimum.y < 0.0f) && !(minimum.z < 0.0f)) {
        if (caps & 0x80) {
            return 0xE;
        }
        if (caps & 0x20) {
            return 0xC;
        }
        return format;
    }
    if (!(minimum.x < -1.0f) && !(minimum.y < -1.0f) && !(minimum.z < -1.0f)) {
        if (caps & 0x80) {
            return 0xE;
        }
        if (caps & 8) {
            format = 0xA;
        }
    }
    return format;
}

// 0x756390 (lote S31A, sin validar; 615 bytes; cdecl, (luz); _rwD3D9LightLocalEnable del SDK): activa en el dispositivo una luz local (puntual 0x80, foco 0x81 o foco suave 0x82): reserva su indice D3D como D3D9EnableDirectionalLight, rellena el
// D3DLIGHT9 de trabajo (0xC925E0): color, posicion del LTM, rango = radio (con radio 0 devuelve 0 sin enviarla), atenuacion {datos[1], datos[2] / radio, datos[3] / radio^2} y por subtipo: puntual (tipo 1, direccion y angulos 0), foco (tipo 2, direccion = eje
// at, Theta = 2 * angulo del cono, o 2 * angulo - 0.001 si pasa de pi, Phi = 2 * angulo) y foco suave (tipo 2, Theta 0, Phi = 2 * angulo). Otro subtipo devuelve 0. Envia la luz (RwD3D9SetLight) y la anota en la lista de activadas; devuelve 1.
RwBool _rwD3D9LightLocalEnable(RpLight* light) {
    RwInt32* const slot = reinterpret_cast<RwInt32*>(reinterpret_cast<RwUInt8*>(light) + StaticRef<RwInt32>(kLightIndexOffset));
    if (*slot < 0) {
        RwInt32& freeCount = StaticRef<RwInt32>(kFreeCount);
        if (freeCount != 0) {
            --freeCount;
            *slot = StaticRef<RwInt32*>(kFreeArray)[freeCount];
        } else {
            *slot = StaticRef<RwInt32>(kNextIndex);
            ++StaticRef<RwInt32>(kNextIndex);
        }
    }
    RwUInt32* const d3dLight = reinterpret_cast<RwUInt32*>(0xC925E0);
    d3dLight[1] = Bits(light->color.red);
    d3dLight[2] = Bits(light->color.green);
    d3dLight[3] = Bits(light->color.blue);
    const RwMatrix* const ltm = RwFrameGetLTM(static_cast<RwFrame*>(light->object.object.parent));
    d3dLight[0xD] = Bits(ltm->pos.x);
    d3dLight[0xE] = Bits(ltm->pos.y);
    d3dLight[0xF] = Bits(ltm->pos.z);
    const float radius = light->radius;
    d3dLight[0x13] = Bits(radius);
    if (radius == 0.0f) {
        return FALSE;
    }
    const RwUInt32* const data = reinterpret_cast<const RwUInt32*>(slot);
    d3dLight[0x15] = data[1];
    float dataFloat2, dataFloat3;
    std::memcpy(&dataFloat2, &data[2], sizeof(float));
    std::memcpy(&dataFloat3, &data[3], sizeof(float));
    d3dLight[0x16] = Bits(static_cast<float>(static_cast<double>(dataFloat2) / static_cast<double>(radius)));
    d3dLight[0x17] = Bits(static_cast<float>(static_cast<double>(dataFloat3) / (static_cast<double>(radius) * static_cast<double>(radius))));
    const RwUInt32 subType = light->object.object.subType;
    if (subType == 0x80) {
        d3dLight[0] = 1;
        d3dLight[0x10] = 0;
        d3dLight[0x11] = 0;
        d3dLight[0x12] = 0;
        d3dLight[0x18] = 0;
        d3dLight[0x19] = 0;
    } else if (subType == 0x81) {
        d3dLight[0] = 2;
        d3dLight[0x10] = Bits(ltm->at.x);
        d3dLight[0x11] = Bits(ltm->at.y);
        d3dLight[0x12] = Bits(ltm->at.z);
        const double doubled = static_cast<double>(RpLightGetConeAngle(light)) * 2.0;
        if (doubled < static_cast<double>(F32(0x40490FDB))) {
            d3dLight[0x18] = Bits(static_cast<float>(doubled));
        } else {
            d3dLight[0x18] = Bits(static_cast<float>(doubled - static_cast<double>(F32(0x3A83126F))));
        }
        d3dLight[0x19] = Bits(static_cast<float>(doubled));
    } else if (subType == 0x82) {
        d3dLight[0] = 2;
        d3dLight[0x10] = Bits(ltm->at.x);
        d3dLight[0x11] = Bits(ltm->at.y);
        d3dLight[0x12] = Bits(ltm->at.z);
        d3dLight[0x18] = 0;
        d3dLight[0x19] = Bits(static_cast<float>(static_cast<double>(RpLightGetConeAngle(light)) * 2.0));
    } else {
        return FALSE;
    }
    RwD3D9SetLight(*slot, d3dLight);
    const RwInt32 index = *slot;
    RwUInt32& capacity = StaticRef<RwUInt32>(kEnabledCapacity);
    if (StaticRef<RwUInt32>(kEnabledCount) >= capacity) {
        const RwUInt32 oldCapacity = capacity;
        capacity += 8;
        RwInt32*& array = StaticRef<RwInt32*>(kEnabledArray);
        if (array == nullptr) {
            array = static_cast<RwInt32*>(RwMalloc(capacity * 4, kLightArrayHint));
        } else {
            array = static_cast<RwInt32*>(RwRealloc(array, capacity * 4, kLightArrayHint));
        }
        for (RwUInt32 i = oldCapacity; i < StaticRef<RwUInt32>(kEnabledCapacity); ++i) {
            StaticRef<RwInt32*>(kEnabledArray)[i] = 0;
        }
    }
    StaticRef<RwInt32*>(kEnabledArray)[StaticRef<RwUInt32>(kEnabledCount)] = index;
    ++StaticRef<RwUInt32>(kEnabledCount);
    return TRUE;
}

// 0x75B4E0 (lote S31A, sin validar; 21 bytes; cdecl, (RpBuildMesh, datos)): como RpBuildMeshGenerateExhaustiveTriStrip (0x75A940) pero con el tercer argumento 0 del generador 0x75A960 del EXE (no es raiz del plan).
RpMeshHeader* RpBuildMeshGenerateExhaustiveIgnoreWindingTriStrip(RpBuildMesh* buildmesh, void* data) {
    return reinterpret_cast<RpMeshHeader*(__cdecl*)(RpBuildMesh*, void*, RwInt32)>(0x75A960)(buildmesh, data, 0);
}

// 0x761010 (lote S31A, sin validar; 19 bytes; cdecl, (material, descriptor*, despacho*)): el vertex shader por defecto de un material es _rpD3D9GetVertexShader(descriptor, despacho) (el material no se usa). Devuelve su resultado.
void* _rxD3D9VertexShaderDefaultGetMaterialShaderCallBack(const RpMaterial* material, _rpD3D9VertexShaderDescriptor* desc, _rpD3D9VertexShaderDispatchDescriptor* dispatch) {
    UNUSED(material);
    return _rpD3D9GetVertexShader(desc, dispatch);
}

// 0x764D20 (lote S31A, sin validar; 16 bytes; cdecl, (matriz de salida*); _rwD3D9VSGetProjectionTransposedMatrix del SDK): D3DXMatrixTranspose(salida, matriz de proyeccion del motor en 0x8E2458): la transpuesta de la matriz 4x4 de floats.
void _rwD3D9VSGetProjectionTransposedMatrix(void* matrix) {
    const float* const projection = reinterpret_cast<const float*>(0x8E2458);
    float transposed[16];
    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            transposed[row * 4 + column] = projection[column * 4 + row];
        }
    }
    std::memcpy(matrix, transposed, sizeof(transposed));
}

// 0x761170 (lote S31A, sin validar; 1452 bytes; cdecl, (objeto, tipo, constantes*, descriptor*); _rxD3D9VertexShaderDefaultLightingCallBack del SDK): prepara las constantes de iluminacion del vertex shader y devuelve el puntero siguiente.
// Pone a cero numDirectionales/numPuntuales y numFocos del descriptor; constante 0 = ambiente acumulado (w = 0 con colores previos, si no 1). Sin la bandera de luces (0x20) o sin mundo devuelve constantes + 1. Reune hasta 15 luces de cada clase:
// tipo 1 (atomo): direccionales del mundo (bandera de luz 1, solo con normales en la geometria) y las luces de los sectores donde esta el atomo (bandera 1, una vez por pasada con la marca lightFrame del motor y solo si la esfera del atomo
// toca la esfera de la luz: |d|^2 < (radio de luz + radio de esfera)^2); otro tipo (sector): igual con la bandera 2 y sin prueba de esfera. Las luces ambiente (subtipo 2) suman su color a la constante 0. Luego escribe: direccionales (normal y color
// por luz), puntuales (cada 4, una constante de radios inversos; por luz posicion, color y radio local, que se invierte) y focos (posicion, direccion, {a, b, 0.5, 0} segun el cono, color; el foco duro 0x81 y el suave estrecho usan pendiente 999.9999;
// el suave usa 1 / (1 - cos) y -cos * eso). Actualiza los contadores del descriptor.
RwV4d* _rxD3D9VertexShaderDefaultLightingCallBack(void* object, RwUInt32 type, RwV4d* shaderConstantPtr, _rpD3D9VertexShaderDescriptor* desc) {
    RwUInt8* const descriptorBytes = reinterpret_cast<RwUInt8*>(desc);
    descriptorBytes[0] = 0;
    descriptorBytes[1] = static_cast<RwUInt8>(descriptorBytes[1] & 0xF0);
    const RwBool isAtomic = (type == 1);
    const RwUInt32 flags = isAtomic ? static_cast<RpAtomic*>(object)->geometry->flags : static_cast<RpWorld*>(Globals()->curWorld)->flags;
    RwV4d* out = shaderConstantPtr;
    RwUInt32* const outBits = reinterpret_cast<RwUInt32*>(out);
    outBits[0] = 0;
    outBits[1] = 0;
    outBits[2] = 0;
    outBits[3] = (flags & 8) ? 0u : 0x3F800000u;
    RpWorld* const world = static_cast<RpWorld*>(Globals()->curWorld);
    if (!(flags & 0x20) || world == nullptr) {
        return out + 1;
    }
    RpLight* directionalLights[15];
    RpLight* pointLights[15];
    RpLight* spotLights[15];
    RwUInt32 numDirectional = 0, numPoint = 0, numSpot = 0;
    const RwUInt8 lightMask = isAtomic ? 1 : 2;
    bool any = false;
    const auto addAmbient = [&](const RpLight* light) {
        out->x = static_cast<float>(static_cast<double>(light->color.red) + static_cast<double>(out->x));
        out->y = static_cast<float>(static_cast<double>(light->color.green) + static_cast<double>(out->y));
        out->z = static_cast<float>(static_cast<double>(light->color.blue) + static_cast<double>(out->z));
    };
    const bool hasNormals = isAtomic ? ((flags >> 4) & 1) != 0 : true;
    RwLLLink* const directionalSentinel = &world->directionalLightList.link;
    for (RwLLLink* node = directionalSentinel->next; node != directionalSentinel; node = node->next) {
        RpLight* const light = reinterpret_cast<RpLight*>(reinterpret_cast<RwUInt8*>(node) - offsetof(RpLight, inWorld));
        if (light == nullptr || (light->object.object.flags & lightMask) == 0) {
            continue;
        }
        const RwUInt8 subType = light->object.object.subType;
        if (subType == 2) {
            addAmbient(light);
        } else if (subType == 1 && hasNormals) {
            if (numDirectional < 15) {
                directionalLights[numDirectional++] = light;
                any = true;
            }
        }
    }
    const auto collectLocalLight = [&](RpLight* light) {
        const RwUInt8 subType = light->object.object.subType;
        if (subType == 0x80) {
            if (numPoint < 15) {
                pointLights[numPoint++] = light;
                any = true;
            }
        } else if (subType > 0x80 && subType <= 0x82) {
            if (numSpot < 15) {
                spotLights[numSpot++] = light;
                any = true;
            }
        }
    };
    if (isAtomic) {
        if (hasNormals) {
            ++Globals()->lightFrame;
            RpAtomic* const atomic = static_cast<RpAtomic*>(object);
            RwLLLink* const sectorSentinel = &atomic->llWorldSectorsInAtomic.link;
            for (RwLLLink* sectorNode = sectorSentinel->next; sectorNode != sectorSentinel; sectorNode = sectorNode->next) {
                RpWorldSector* const sector = *reinterpret_cast<RpWorldSector**>(reinterpret_cast<RwUInt8*>(sectorNode) + 8);
                RwLLLink* const lightSentinel = &sector->lightsInWorldSector.link;
                for (RwLLLink* lightNode = lightSentinel->next; lightNode != lightSentinel; lightNode = lightNode->next) {
                    RpLight* const light = *reinterpret_cast<RpLight**>(reinterpret_cast<RwUInt8*>(lightNode) + 8);
                    if (light == nullptr || (light->object.object.flags & 1) == 0 || light->lightFrame == Globals()->lightFrame) {
                        continue;
                    }
                    light->lightFrame = Globals()->lightFrame;
                    const RwMatrix* const lightLtm = RwFrameGetLTM(static_cast<RwFrame*>(light->object.object.parent));
                    const RwSphere* const sphere = RpAtomicGetWorldBoundingSphere(atomic);
                    const double dx = static_cast<double>(sphere->center.x) - static_cast<double>(lightLtm->pos.x);
                    const double dy = static_cast<double>(sphere->center.y) - static_cast<double>(lightLtm->pos.y);
                    const double dz = static_cast<double>(sphere->center.z) - static_cast<double>(lightLtm->pos.z);
                    const double radiusSum = static_cast<double>(light->radius) + static_cast<double>(sphere->radius);
                    const double distanceSquared = (dy * dy + dx * dx) + dz * dz;
                    if (radiusSum * radiusSum > distanceSquared) {
                        collectLocalLight(light);
                    }
                }
            }
        }
    } else {
        ++Globals()->lightFrame;
        RpWorldSector* const sector = static_cast<RpWorldSector*>(object);
        RwLLLink* const lightSentinel = &sector->lightsInWorldSector.link;
        for (RwLLLink* lightNode = lightSentinel->next; lightNode != lightSentinel; lightNode = lightNode->next) {
            RpLight* const light = *reinterpret_cast<RpLight**>(reinterpret_cast<RwUInt8*>(lightNode) + 8);
            if (light == nullptr || (light->object.object.flags & 2) == 0 || light->lightFrame == Globals()->lightFrame) {
                continue;
            }
            light->lightFrame = Globals()->lightFrame;
            collectLocalLight(light);
        }
    }
    out += 1;
    if (!any) {
        return out;
    }
    const auto writeColor = [&](const RpLight* light) {
        RwUInt32* const bits = reinterpret_cast<RwUInt32*>(out);
        bits[0] = Bits(light->color.red);
        bits[1] = Bits(light->color.green);
        bits[2] = Bits(light->color.blue);
        bits[3] = 0;
    };
    if (numDirectional != 0) {
        for (RwUInt32 i = 0; i < numDirectional; ++i) {
            RpLight* const light = directionalLights[i];
            RwMatrix* const ltm = RwFrameGetLTM(static_cast<RwFrame*>(light->object.object.parent));
            _rwD3D9VSGetNormalInLocalSpace(&ltm->at, reinterpret_cast<RwV3d*>(out));
            reinterpret_cast<RwUInt32*>(out)[3] = 0;
            ++out;
            writeColor(light);
            ++out;
        }
        descriptorBytes[0] = static_cast<RwUInt8>((descriptorBytes[0] & 0xF0) | (numDirectional & 0xF));
    }
    float* inverseRadius = nullptr;
    if (numPoint != 0) {
        for (RwUInt32 i = 0; i < numPoint; ++i) {
            RpLight* const light = pointLights[i];
            if ((i & 3) == 0) {
                inverseRadius = reinterpret_cast<float*>(out);
                ++out;
            }
            RwMatrix* const ltm = RwFrameGetLTM(static_cast<RwFrame*>(light->object.object.parent));
            _rwD3D9VSGetPointInLocalSpace(&ltm->pos, reinterpret_cast<RwV3d*>(out));
            reinterpret_cast<RwUInt32*>(out)[3] = 0;
            ++out;
            writeColor(light);
            ++out;
            _rwD3D9VSGetRadiusInLocalSpace(light->radius, inverseRadius);
            *inverseRadius = static_cast<float>(1.0 / static_cast<double>(*inverseRadius));
            ++inverseRadius;
        }
        descriptorBytes[0] = static_cast<RwUInt8>((descriptorBytes[0] & 0x0F) | static_cast<RwUInt8>(numPoint << 4));
    }
    if (numSpot != 0) {
        for (RwUInt32 i = 0; i < numSpot; ++i) {
            RpLight* const light = spotLights[i];
            if ((i & 3) == 0) {
                inverseRadius = reinterpret_cast<float*>(out);
                ++out;
            }
            RwMatrix* const ltm = RwFrameGetLTM(static_cast<RwFrame*>(light->object.object.parent));
            _rwD3D9VSGetPointInLocalSpace(&ltm->pos, reinterpret_cast<RwV3d*>(out));
            reinterpret_cast<RwUInt32*>(out)[3] = 0;
            ++out;
            RwMatrix* const ltmDirection = RwFrameGetLTM(static_cast<RwFrame*>(light->object.object.parent));
            _rwD3D9VSGetNormalInLocalSpace(&ltmDirection->at, reinterpret_cast<RwV3d*>(out));
            reinterpret_cast<RwUInt32*>(out)[3] = 0;
            ++out;
            const double coneCosine = std::cos(static_cast<double>(RpLightGetConeAngle(light)));
            float* const cone = reinterpret_cast<float*>(out);
            if (light->object.object.subType == 0x81 || !(coneCosine < static_cast<double>(F32(0x3F7FBE77)))) {
                cone[0] = F32(0x4479FFFF);
                cone[1] = static_cast<float>(-coneCosine * static_cast<double>(F32(0x4479FFFF)));
            } else {
                const double inverse = 1.0 / (1.0 - coneCosine);
                cone[0] = static_cast<float>(inverse);
                cone[1] = static_cast<float>(-coneCosine * inverse);
            }
            cone[2] = F32(0x3F000000);
            cone[3] = 0.0f;
            ++out;
            writeColor(light);
            ++out;
            _rwD3D9VSGetRadiusInLocalSpace(light->radius, inverseRadius);
            *inverseRadius = static_cast<float>(1.0 / static_cast<double>(*inverseRadius));
            ++inverseRadius;
        }
        descriptorBytes[1] = static_cast<RwUInt8>((descriptorBytes[1] & 0xF0) | (numSpot & 0xF));
    }
    return out;
}

namespace {
// Funciones del EXE de lectura/escritura de sectores (no son raices del plan; se conservan por direccion).
using SectorSizeFn  = RwUInt32(__cdecl*)(void* sector, RpWorld* world, RwUInt32 flags);                 // RpWorldSectorStreamDataSize (rpworld.cpp) 0x762440 (sector atomico) y 0x762040 (sector plano): tamano en flujo
using SectorWriteFn = RwBool(__cdecl*)(void* sector, RwStream* stream, RpWorld* world, RwUInt32 flags); // 0x7624A0 (atomico) y 0x762740 (plano)
using SectorReadFn  = RpSector*(__cdecl*)(RwStream* stream, void** memoryCursor, RpWorld* world, RwUInt32 flags); // 0x762F30 (atomico) y 0x763310 (plano)

// RWERROR((codigo [, argumento])) del SDK: error de plugin 2 con el codigo que devuelve _rwerror (0x8088D0).
void RaiseWorldError(RwInt32 code) {
    RwError error;
    error.pluginID  = 2;
    error.errorCode = _rwerror(code);
    RwErrorSet(&error);
}

void RaiseWorldError(RwInt32 code, RwUInt32 argument) {
    RwError error;
    error.pluginID  = 2;
    error.errorCode = _rwerror(code, argument);
    RwErrorSet(&error);
}

constexpr uintptr_t kWorldPluginRegistry = 0x8D62D0;
constexpr uintptr_t kSectorPluginRegistry = 0x8D736C;

// Cabecera de estructura del mundo en el flujo (64 bytes, RpWorldChunkInfo del SDK).
struct WorldChunkInfo {
    RwInt32  rootIsWorldSector;
    RwV3d    invWorldOrigin;
    RwInt32  numTriangles;
    RwInt32  numVertices;
    RwInt32  numPlaneSectors;
    RwInt32  numWorldSectors;
    RwInt32  colSectorSize;
    RwUInt32 format;
    RwBBox   boundingBox;
};
static_assert(sizeof(WorldChunkInfo) == 0x40);
} // namespace

// 0x762150 (lote S31A, sin validar; 747 bytes; cdecl, (mundo, flujo)): escribe el mundo: cabecera 0xB con el tamano total (lista de materiales + 0x58, el sector raiz o los dos subarboles con las funciones de tamano del EXE 0x762440 y 0x762040 mas
// los plugins de sector, y los plugins del mundo), cabecera de estructura con 64 bytes {raiz es sector, origen invertido, triangulos, vertices, planos y sectores totales (recorrido del arbol con pila de 64 entradas), 0, formato | (texCoords | 0x4000) << 16, caja},
// la lista de materiales, el arbol de sectores (0x7624A0 / 0x762740 del EXE) y los plugins. Si algo falla devuelve NULL; si no, el mundo.
const RpWorld* RpWorldStreamWrite(const RpWorld* world, RwStream* stream) {
    RpWorld* const mutableWorld = const_cast<RpWorld*>(world);
    const RwUInt32 flags = world->flags;
    const RwPluginRegistry* const sectorRegistry = reinterpret_cast<const RwPluginRegistry*>(kSectorPluginRegistry);
    const SectorSizeFn atomicSectorSize = &RpWorldSectorStreamDataSize;
    const SectorSizeFn planeSectorSize = &RpWorldPlaneStreamSize;
    const RwUInt32 materialsSize = _rpMaterialListStreamGetSize(&world->matList) + 0x58;
    RpSector* const root = world->rootSector;
    RwUInt32 size;
    if (*reinterpret_cast<RwInt32*>(root) < 0) {
        RpWorldSector* const sector = reinterpret_cast<RpWorldSector*>(root);
        RwUInt32 sectorSize = 0x2C;
        if ((flags & 0x1000000) == 0) {
            const RwUInt32 numVertices = sector->numVertices;
            sectorSize = numVertices * 12 + 0x2C;
            if (flags & 0x10) {
                sectorSize += numVertices * 4;
            }
            if (flags & 8) {
                sectorSize += numVertices * 4;
            }
            sectorSize += (static_cast<RwUInt32>(world->numTexCoordSets) * numVertices + sector->numTriangles) * 8;
        }
        const RwUInt32 pluginsSize = _rwPluginRegistryGetSize(sectorRegistry, sector);
        size = materialsSize + (pluginsSize + sectorSize) + 0x24;
    } else {
        RpPlaneSector* const plane = reinterpret_cast<RpPlaneSector*>(root);
        RwUInt32 leftSize;
        void* const left = plane->leftSubTree;
        if (*reinterpret_cast<RwInt32*>(left) < 0) {
            const RwUInt32 sectorSize = atomicSectorSize(left, mutableWorld, flags) + 0xC;
            leftSize = _rwPluginRegistryGetSize(sectorRegistry, left) + sectorSize + 0x3C;
        } else {
            leftSize = planeSectorSize(left, mutableWorld, flags) + 0x30;
        }
        void* const right = plane->rightSubTree;
        RwUInt32 bothSize;
        if (*reinterpret_cast<RwInt32*>(right) < 0) {
            const RwUInt32 sectorSize = atomicSectorSize(right, mutableWorld, flags) + 0xC;
            bothSize = leftSize + (_rwPluginRegistryGetSize(sectorRegistry, right) + sectorSize) + 0x18;
        } else {
            bothSize = leftSize + planeSectorSize(right, mutableWorld, flags) + 0xC;
        }
        size = materialsSize + bothSize + 0xC;
    }
    size = size + _rwPluginRegistryGetSize(reinterpret_cast<const RwPluginRegistry*>(kWorldPluginRegistry), world) + 0xC;
    if (!_rwStreamWriteVersionedChunkHeader(stream, 0xB, size, 0x36003, 0xFFFF)) {
        return nullptr;
    }
    if (!_rwStreamWriteVersionedChunkHeader(stream, 1, 0x40, 0x36003, 0xFFFF)) {
        return nullptr;
    }
    RwUInt32 totalTriangles = 0, totalVertices = 0, numPlanes = 0, numSectors = 0;
    {
        RwUInt32 stack[65];
        RwInt32 depth = 0;
        RpSector* sector = world->rootSector;
        while (depth >= 0) {
            if (*reinterpret_cast<RwInt32*>(sector) < 0) {
                RpWorldSector* const leaf = reinterpret_cast<RpWorldSector*>(sector);
                totalTriangles += leaf->numTriangles;
                ++numSectors;
                totalVertices += leaf->numVertices;
                sector = reinterpret_cast<RpSector*>(static_cast<uintptr_t>(stack[depth]));
                --depth;
            } else {
                RpPlaneSector* const plane = reinterpret_cast<RpPlaneSector*>(sector);
                ++numPlanes;
                sector = plane->leftSubTree;
                ++depth;
                stack[depth] = static_cast<RwUInt32>(reinterpret_cast<uintptr_t>(plane->rightSubTree));
            }
        }
    }
    WorldChunkInfo info{};
    info.format = ((static_cast<RwUInt32>(world->numTexCoordSets) & 0xFF) | 0x4000) << 16 | flags;
    info.rootIsWorldSector = *reinterpret_cast<RwInt32*>(world->rootSector) < 0 ? 1 : 0;
    info.invWorldOrigin.x = static_cast<float>(static_cast<double>(world->worldOrigin.x) * -1.0);
    info.invWorldOrigin.y = static_cast<float>(static_cast<double>(world->worldOrigin.y) * -1.0);
    info.invWorldOrigin.z = static_cast<float>(static_cast<double>(world->worldOrigin.z) * -1.0);
    info.numTriangles = static_cast<RwInt32>(totalTriangles);
    info.numVertices = static_cast<RwInt32>(totalVertices);
    info.numPlaneSectors = static_cast<RwInt32>(numPlanes);
    info.numWorldSectors = static_cast<RwInt32>(numSectors);
    info.colSectorSize = 0;
    info.boundingBox = world->boundingBox;
    if (!RwStreamWrite(stream, &info, 0x40)) {
        return nullptr;
    }
    if (!_rpMaterialListStreamWrite(&world->matList, stream)) {
        return nullptr;
    }
    if (*reinterpret_cast<RwInt32*>(world->rootSector) < 0) {
        if (!reinterpret_cast<SectorWriteFn>(0x7624A0)(world->rootSector, stream, mutableWorld, flags)) {
            return nullptr;
        }
    } else {
        if (!reinterpret_cast<SectorWriteFn>(0x762740)(world->rootSector, stream, mutableWorld, flags)) {
            return nullptr;
        }
    }
    return _rwPluginRegistryWriteDataChunks(reinterpret_cast<const RwPluginRegistry*>(kWorldPluginRegistry), stream, world) != nullptr ? world : nullptr;
}

// 0x762960 (lote S31A, sin validar; 1476 bytes; cdecl, (flujo)): lee un mundo: busca la cabecera de estructura (version 0x34000..0x36003, si no error 0x80000004); lee el formato antiguo (13 dwords, antes de 0x34001) o el actual (64 bytes); con tamano de sector
// de colision > 0 error. Reserva de una vez el mundo, los sectores y planos y sus datos (tamanos de los registros 0x8D62D0 y 0x8D736C; pista 0x3000B; sin memoria: error 0x80000013 con el tamano) a cero, lo registra (_rpWorldRegisterWorld) y lo inicializa (tipo 7, orden de render 2,
// origen = -origen leido, listas vacias). Lee la lista de materiales y el sector raiz (0x762F30 atomico / 0x763310 plano del EXE), caja (leida si version >= 0x34003, si no calculada), cajas de sectores, callback de render por defecto, texturas de materiales, datos de
// plugins y derechos, y RpWorldUnlock. Cualquier fallo destruye el mundo y devuelve NULL.
RpWorld* RpWorldStreamRead(RwStream* stream) {
    RwUInt32 length = 0, version = 0;
    if (!RwStreamFindChunk(stream, 1, &length, &version)) {
        return nullptr;
    }
    if (version < 0x34000 || version > 0x36003) {
        RaiseWorldError(static_cast<RwInt32>(0x80000004));
        return nullptr;
    }
    WorldChunkInfo info{};
    if (version < 0x34001) {
        RwUInt32 legacy[13] = {};
        if (length > sizeof(legacy) || RwStreamRead(stream, legacy, length) != length) {   // el EXE no acota la longitud (desbordaria la pila)
            return nullptr;
        }
        std::memcpy(&info, &legacy[0], 4 * 4);   // d0..d3
        reinterpret_cast<RwUInt32*>(&info)[4] = legacy[7];
        reinterpret_cast<RwUInt32*>(&info)[5] = legacy[8];
        reinterpret_cast<RwUInt32*>(&info)[6] = legacy[9];
        reinterpret_cast<RwUInt32*>(&info)[7] = legacy[10];
        reinterpret_cast<RwUInt32*>(&info)[8] = legacy[11];
        reinterpret_cast<RwUInt32*>(&info)[9] = legacy[12];
    } else {
        if (length > sizeof(info) || RwStreamRead(stream, &info, length) != length) {      // idem
            return nullptr;
        }
    }
    if (info.colSectorSize > 0) {
        RaiseWorldError(static_cast<RwInt32>(0x80000004));
        return nullptr;
    }
    const RwUInt32 format = info.format;
    RwUInt32 memorySize = StaticRef<RwUInt32>(kWorldPluginRegistry) + StaticRef<RwUInt32>(kSectorPluginRegistry) * static_cast<RwUInt32>(info.numWorldSectors) + static_cast<RwUInt32>(info.numPlaneSectors) * 24;
    RwUInt32 numTexCoords;
    if (format & 0xFF0000) {
        numTexCoords = (format >> 16) & 0xFF;
    } else if (static_cast<RwInt8>(format) < 0) {
        numTexCoords = 2;
    } else {
        numTexCoords = (format >> 2) & 1;
    }
    if ((format & 0x1000000) == 0) {
        const RwUInt32 numVertices = static_cast<RwUInt32>(info.numVertices);
        memorySize += numVertices * 12;
        if (format & 0x10) {
            memorySize += numVertices * 4;
        }
        if (format & 8) {
            memorySize += numVertices * 4;
        }
        if (numTexCoords != 0) {
            memorySize += numTexCoords * numVertices * 8;
        }
        memorySize += static_cast<RwUInt32>(info.numTriangles) * 8;
    }
    RpWorld* const world = static_cast<RpWorld*>(RwMalloc(memorySize, 0x3000B));
    if (world == nullptr) {
        RaiseWorldError(static_cast<RwInt32>(0x80000013), memorySize);
        return nullptr;
    }
    std::memset(world, 0, memorySize);
    void* memoryCursor = reinterpret_cast<RwUInt8*>(world) + StaticRef<RwUInt32>(kWorldPluginRegistry);
    world->object.type = 7;
    world->object.subType = 0;
    world->object.flags = 0;
    world->object.parent = nullptr;
    world->object.privateFlags = 1;
    _rpWorldRegisterWorld(world, memorySize);
    world->renderOrder = static_cast<RpWorldRenderOrder>(2);
    world->flags = format;
    world->worldOrigin.x = static_cast<float>(static_cast<double>(info.invWorldOrigin.x) * -1.0);
    world->worldOrigin.y = static_cast<float>(static_cast<double>(info.invWorldOrigin.y) * -1.0);
    world->worldOrigin.z = static_cast<float>(static_cast<double>(info.invWorldOrigin.z) * -1.0);
    world->numTexCoordSets = static_cast<RwInt32>(numTexCoords);
    rwLinkListInitialize(&world->lightList);
    world->rootSector = nullptr;
    world->pipeline = nullptr;
    rwLinkListInitialize(&world->clumpList);
    rwLinkListInitialize(&world->directionalLightList);
    const auto destroyAndFail = [&]() -> RpWorld* {
        RpWorldDestroy(world);
        return nullptr;
    };
    RwUInt32 chunkVersion = 0;
    if (!RwStreamFindChunk(stream, 8, nullptr, &chunkVersion)) {
        return destroyAndFail();
    }
    if (chunkVersion < 0x34000 || chunkVersion > 0x36003) {
        RaiseWorldError(static_cast<RwInt32>(0x80000004));
        return destroyAndFail();
    }
    if (!_rpMaterialListStreamRead(stream, &world->matList)) {
        return destroyAndFail();
    }
    if (info.rootIsWorldSector != 0) {
        if (!RwStreamFindChunk(stream, 9, nullptr, &chunkVersion)) {
            return destroyAndFail();
        }
        if (chunkVersion < 0x34000 || chunkVersion > 0x36003) {
            RaiseWorldError(static_cast<RwInt32>(0x80000004));
            return destroyAndFail();
        }
        world->rootSector = reinterpret_cast<SectorReadFn>(0x762F30)(stream, &memoryCursor, world, format);
    } else {
        if (!RwStreamFindChunk(stream, 0xA, nullptr, &chunkVersion)) {
            return destroyAndFail();
        }
        if (chunkVersion < 0x34000 || chunkVersion > 0x36003) {
            RaiseWorldError(static_cast<RwInt32>(0x80000004));
            return destroyAndFail();
        }
        world->rootSector = reinterpret_cast<SectorReadFn>(0x763310)(stream, &memoryCursor, world, format);
    }
    if (world->rootSector == nullptr) {
        return destroyAndFail();
    }
    world->numClumpsInWorld = 0;
    world->currentClumpLink = &world->clumpList.link;
    if (version < 0x34003) {
        _rpWorldFindBBox(world, &world->boundingBox);
    } else {
        world->boundingBox = info.boundingBox;
    }
    _rpWorldSetupSectorBoundingBoxes(world);
    RpWorldSetSectorRenderCallBack(world, nullptr);
    for (RwInt32 i = 0; i < world->matList.numMaterials; ++i) {
        RpMaterial* const material = world->matList.materials[i];
        if (material->texture != nullptr) {
            RpMaterialSetTexture(material, material->texture);
        }
    }
    _rwPluginRegistryInitObject(reinterpret_cast<const RwPluginRegistry*>(kWorldPluginRegistry), world);
    StaticRef<RwUInt32>(0xC94C18) = 0;
    StaticRef<RwUInt32>(0xC94C14) = 0;
    if (!_rwPluginRegistryReadDataChunks(reinterpret_cast<const RwPluginRegistry*>(kWorldPluginRegistry), stream, world)) {
        _rpWorldUnregisterWorld(world);
        RwFree(world);
        return nullptr;
    }
    if (StaticRef<RwUInt32>(0xC94C18) != 0) {
        _rwPluginRegistryInvokeRights(reinterpret_cast<const RwPluginRegistry*>(kWorldPluginRegistry), StaticRef<RwUInt32>(0xC94C18), world, StaticRef<RwUInt32>(0xC94C14));
    }
    if (!RpWorldUnlock(world)) {
        return destroyAndFail();
    }
    return world;
}

// ---------------------------------------------------------------------------------------------------------------------
// Lote S31A (cierre; sin validar): las tres raices de tiras de triangulos que faltaban (RpTriStripMeshTunnel 0x75B780, RpBuildMeshGenerateTriStrip 0x75C380,
// RpGeometrySortByMaterial 0x75DAE0) y sus dependencias internas (0x75BB20, 0x75BB50, 0x75BB80, 0x75BC80, 0x75C660, 0x75C9D0, 0x75D240, 0x75E230, 0x74BFD0).
// Estructuras del SDK de RenderWare (RpTriStripPolygon 0x34 bytes, RpTriStripEdge 0x20, RpTriStripMesh 0x2C, RpTriStripTunnelData). Herramientas y desensamblados: reversing/FnS31AFILE.
// RpTriStripMeshTunnel y sus ayudantes estan escritos a mano; las demas (0x75C660, 0x75C9D0, 0x75D240, 0x75C380 y 0x75DAE0) son transcripciones literales (reversing/FnS31AFILE/lift.py)
// del desensamblado: registros como variables, la pila como la matriz `fr` (desplazamientos fijos desde el ESP de entrada) y las banderas calculadas en cada instruccion.
// ---------------------------------------------------------------------------------------------------------------------
static_assert(sizeof(RpTriStripPolygon) == 0x34 && offsetof(RpTriStripPolygon, numEdges) == 0x18 && offsetof(RpTriStripPolygon, edges) == 0x1C
              && offsetof(RpTriStripPolygon, vertIndex) == 0x28 && offsetof(RpTriStripPolygon, testFrame) == 0x30);
static_assert(sizeof(RpTriStripEdge) == 0x20 && offsetof(RpTriStripEdge, poly1) == 0x10 && offsetof(RpTriStripEdge, poly2) == 0x14
              && offsetof(RpTriStripEdge, vert1) == 0x18 && offsetof(RpTriStripEdge, vert2) == 0x1A && offsetof(RpTriStripEdge, strip) == 0x1C);
static_assert(sizeof(RpTriStripMesh) == 0x2C && offsetof(RpTriStripMesh, polygonEndList) == 4);

namespace {
constexpr RwUInt32 kTriStripHint = 0x10502; // pista de memoria de los bloques temporales del tunel (RwMalloc)

// Bloque del tunel (RwMalloc de 0x10 + 8 * limite + 4 bytes): `length` aristas/poligonos actuales, `capacity` es la longitud maxima del turno (empieza en 1 y sube uno por ronda),
// `edges` (limite entradas) y `polygons` (limite + 1 entradas) van detras de la cabecera.
struct TriStripTunnel {
    RwUInt32 length;
    RwUInt32 capacity;
    RpTriStripEdge** edges;
    RpTriStripPolygon** polygons;
};
static_assert(sizeof(TriStripTunnel) == 0x10);

// Entrada de la pila de busqueda del tunel (12 bytes): arista tomada, poligono al que lleva y longitud del tunel con ella.
struct TriStripSearchEntry {
    RpTriStripEdge* edge;
    RpTriStripPolygon* polygon;
    RwUInt32 depth;
};
static_assert(sizeof(TriStripSearchEntry) == 0xC);

// Marca de prueba global del coste de tiras: RpTriStripPolygon::testFrame de los poligonos de una tira debe igualarla (el callback de coste la escribe), y 0x75BC80 la incrementa en cada llamada.
inline RwUInt32& TriStripTestFrame() { return StaticRef<RwUInt32>(0x8D6390); }

// Siguiente poligono de `poly` por una arista de tira cuyo poligono vecino no es `excluded` (con `excluded` nulo, solo cuentan las aristas con los dos poligonos). Nulo si no hay.
inline RpTriStripPolygon* TriStripNextAlongStrip(const RpTriStripPolygon* poly, const RpTriStripPolygon* excluded) {
    for (RwUInt32 i = 0; i < poly->numEdges; ++i) {
        RpTriStripEdge* const edge = poly->edges[i];
        if (edge->strip == 0 || edge->poly1 == excluded || edge->poly2 == excluded) {
            continue;
        }
        return edge->poly1 == poly ? edge->poly2 : edge->poly1;
    }
    return nullptr;
}
} // namespace

// 0x75BB20 (lote S31A, sin validar; 48 bytes; cdecl, (tunel)): invierte la bandera de tira (0 <-> 1) de las `length` aristas del tunel.
void _rpTriStripTunnelToggle(TriStripTunnel* tunnel) {
    for (RwUInt32 i = 0; i < tunnel->length; ++i) {
        RpTriStripEdge* const edge = tunnel->edges[i];
        edge->strip = edge->strip == 0 ? 1 : 0;
    }
}

// 0x75BB50 (lote S31A, sin validar; 48 bytes; cdecl, (poligono)): numero de aristas del poligono que forman parte de una tira (bandera distinta de 0).
RwUInt32 _rpTriStripPolygonCountStripEdges(const RpTriStripPolygon* poly) {
    RwUInt32 count = 0;
    for (RwUInt32 i = 0; i < poly->numEdges; ++i) {
        if (poly->edges[i]->strip != 0) {
            ++count;
        }
    }
    return count;
}

// 0x75BB80 (lote S31A, sin validar; 256 bytes; cdecl, (tunel)): comprueba que el tunel no cierre un ciclo de tira. Por cada pareja (polygons[k], polygons[k + 1]) con k par recorre la tira desde
// el segundo poligono alejandose del primero; si vuelve a topar con el primero devuelve 0. Devuelve 1 si ninguna pareja cierra un ciclo.
RwBool _rpTriStripTunnelIsValid(const TriStripTunnel* tunnel) {
    for (RwUInt32 k = 0; k < tunnel->length; k += 2) {
        RpTriStripPolygon* const first = tunnel->polygons[k];
        const RpTriStripPolygon* previous = tunnel->polygons[k + 1];
        RpTriStripPolygon* current = TriStripNextAlongStrip(previous, first);
        while (current != nullptr) {
            if (current == first) {
                return FALSE;
            }
            RpTriStripPolygon* const next = TriStripNextAlongStrip(current, previous);
            previous = current;
            current = next;
        }
    }
    return TRUE;
}

// 0x75BC80 (lote S31A, sin validar; 256 bytes; cdecl, (tunel, datos del tunel)): suma el coste (costCB(final de la tira, marca de prueba, datos)) de las tiras que tocan los poligonos del
// tunel (polygons[0..length]): para cada poligono cuya marca de prueba no es la actual sigue la tira hasta su otro extremo. Incrementa la marca de prueba global (0x8D6390) al terminar.
RwUInt32 _rpTriStripTunnelCost(const TriStripTunnel* tunnel, const RpTriStripTunnelData* data) {
    RwUInt32 total = 0;
    for (RwUInt32 i = 0; i <= tunnel->length; ++i) {
        const RpTriStripPolygon* end = tunnel->polygons[i];
        if (end->testFrame == TriStripTestFrame()) {
            continue;
        }
        const RpTriStripPolygon* previous = end;
        const RpTriStripPolygon* next = TriStripNextAlongStrip(end, nullptr);
        while (next != nullptr) {
            const RpTriStripPolygon* const following = TriStripNextAlongStrip(next, previous);
            previous = next;
            end = next;
            next = following;
        }
        total += data->costCB(const_cast<RpTriStripPolygon*>(end), TriStripTestFrame(), data->data);
    }
    ++TriStripTestFrame();
    return total;
}

// 0x75B780 (lote S31A, sin validar; 922 bytes; cdecl, (RpTriStripMesh, RpTriStripTunnelData); RpTriStripMeshTunnel del SDK): mejora las tiras de la malla "tunelando": desde cada poligono extremo
// (lista polygonEndList) busca con una pila (arista fuera de tira -> poligono -> arista de tira -> ...) caminos alternos de hasta `capacity` aristas; si invertir las banderas de tira de un camino
// no cierra ciclos (0x75BB80) y su coste (0x75BC80) baja, lo aplica y actualiza la lista de extremos. `capacity` empieza en 1 y sube uno por ronda (una ronda termina al agotar la lista de
// extremos); acumulado = aciertos / poligonos + acumulado / 2 de la ronda; para cuando el acumulado cae por debajo de pow(0.001, calidad) o `capacity` llega a lengthLimit. Con lengthLimit 0 devuelve
// la malla sin tocarla. Devuelve la malla. (Sin comprobacion de memoria, como el EXE.)
RpTriStripMesh* RpTriStripMeshTunnel(RpTriStripMesh* mesh, void* data) {
    const RpTriStripTunnelData* const tunnelData = static_cast<const RpTriStripTunnelData*>(data);
    const RwUInt32 limit = tunnelData->lengthLimit;
    if (limit == 0) {
        return mesh;
    }
    TriStripTunnel* const tunnel = static_cast<TriStripTunnel*>(RwMalloc(limit * 8 + 0x14, kTriStripHint));
    tunnel->length = 0;
    tunnel->capacity = 1;
    tunnel->edges = reinterpret_cast<RpTriStripEdge**>(reinterpret_cast<RwUInt8*>(tunnel) + sizeof(TriStripTunnel));
    tunnel->polygons = reinterpret_cast<RpTriStripPolygon**>(tunnel->edges + limit);
    TriStripSearchEntry* const stackBase = static_cast<TriStripSearchEntry*>(RwMalloc(limit * 9 * 4, kTriStripHint));
    float accumulated = 0.0f;
    RwUInt32 successes = 0;
    const float threshold = static_cast<float>(ExeCrt::Pow(static_cast<double>(0.001f), static_cast<double>(tunnelData->quality)));
    RwLLLink* const endSentinel = &mesh->polygonEndList.link;

    for (;;) {
        // una ronda: recorre la lista de extremos desde el principio (se reinicia cada vez que se aplica un tunel)
        bool applied = false;
        for (RwLLLink* link = endSentinel->next; link != endSentinel && !applied; link = link->next) {
            RpTriStripPolygon* const start = reinterpret_cast<RpTriStripPolygon*>(link);
            tunnel->polygons[0] = start;
            TriStripSearchEntry* stackTop = stackBase;
            for (RwUInt32 i = 0; i < start->numEdges; ++i) {
                RpTriStripEdge* const edge = start->edges[i];
                if (edge->strip == 0) {
                    RpTriStripPolygon* const neighbour = edge->poly1 == start ? edge->poly2 : edge->poly1;
                    if (neighbour != nullptr) {
                        stackTop->edge = edge;
                        stackTop->polygon = neighbour;
                        stackTop->depth = 1;
                        ++stackTop;
                    }
                }
            }
            while (stackBase < stackTop) {
                --stackTop;
                const RwUInt32 depth = stackTop->depth;
                tunnel->length = depth;
                tunnel->edges[depth - 1] = stackTop->edge;
                tunnel->polygons[tunnel->length] = stackTop->polygon;
                const RwUInt32 parity = tunnel->length & 1;
                bool accept = false;
                // con longitud impar el ultimo poligono tiene que ser un extremo (estar en la lista de extremos) para poder cerrar el tunel
                if (parity == 0 || tunnel->polygons[tunnel->length]->inEndLink.next != nullptr) {
                    for (RwUInt32 i = 0; i < tunnel->length; ++i) {
                        RpTriStripEdge* const edge = tunnel->edges[i];
                        edge->strip = edge->strip == 0 ? 1 : 0;
                    }
                    RwUInt32 verdict = static_cast<RwUInt32>(_rpTriStripTunnelIsValid(tunnel));
                    if (verdict != 0 && tunnel->length >= 2) {
                        const RwUInt32 newCost = _rpTriStripTunnelCost(tunnel, tunnelData);
                        _rpTriStripTunnelToggle(tunnel);
                        const RwUInt32 oldCost = _rpTriStripTunnelCost(tunnel, tunnelData);
                        verdict = newCost < oldCost ? 1 : 0;
                    } else {
                        _rpTriStripTunnelToggle(tunnel);
                    }
                    accept = verdict != 0;
                }
                if (accept) {
                    // aplica el tunel: invierte las aristas y reajusta la pertenencia de cada poligono a la lista de extremos (menos de 2 aristas de tira = extremo)
                    for (RwUInt32 i = 0; i < tunnel->length; ++i) {
                        RpTriStripEdge* const edge = tunnel->edges[i];
                        edge->strip = edge->strip == 0 ? 1 : 0;
                    }
                    for (RwUInt32 b = 0; b <= tunnel->length; ++b) {
                        RpTriStripPolygon* const polygon = tunnel->polygons[b];
                        const RwUInt32 stripEdges = _rpTriStripPolygonCountStripEdges(polygon);
                        if (polygon->inEndLink.next != nullptr) {
                            if (stripEdges > 1) {
                                polygon->inEndLink.prev->next = polygon->inEndLink.next;
                                polygon->inEndLink.next->prev = polygon->inEndLink.prev;
                                polygon->inEndLink.prev = nullptr;
                                polygon->inEndLink.next = nullptr;
                            }
                        } else if (stripEdges < 2) {
                            RwLLLink* const head = endSentinel->next;
                            polygon->inEndLink.next = head;
                            polygon->inEndLink.prev = endSentinel;
                            head->prev = &polygon->inEndLink;
                            endSentinel->next = &polygon->inEndLink;
                        }
                    }
                    ++successes;
                    tunnel->length = 0;
                    applied = true;
                    break;
                }
                // sin exito: amplia la busqueda con las aristas del ultimo poligono cuya bandera de tira coincide con la paridad, que no repitan la ultima arista y cuyo poligono vecino
                // aun no este en el tunel
                if (tunnel->length < tunnel->capacity) {
                    RpTriStripPolygon* const last = tunnel->polygons[tunnel->length];
                    for (RwUInt32 j = 0; j < last->numEdges; ++j) {
                        RpTriStripEdge* const edge = last->edges[j];
                        if (static_cast<RwUInt32>(edge->strip) != parity || edge == tunnel->edges[tunnel->length - 1]) {
                            continue;
                        }
                        RpTriStripPolygon* neighbour = edge->poly1;
                        if (neighbour == last) {
                            neighbour = edge->poly2;
                        }
                        stackTop->polygon = neighbour;
                        if (neighbour == nullptr) {
                            continue;
                        }
                        RwUInt32 found = 0;
                        while (neighbour != tunnel->polygons[found]) {
                            ++found;
                            if (found > tunnel->length) {
                                break;
                            }
                        }
                        if (found > tunnel->length) {
                            stackTop->edge = edge;
                            stackTop->depth = tunnel->length + 1;
                            ++stackTop;
                        }
                    }
                }
            }
        }
        if (applied) {
            continue;
        }
        // fin de ronda: acumulado = aciertos / poligonos + acumulado / 2
        // (el EXE lo calcula con la x87 en precision de 24 bits: fild/fidiv, fmul 0.5, faddp y fstp a float; aqui el cociente se redondea a float y la suma se redondea una vez mas)
        const float ratio = static_cast<float>(static_cast<double>(successes) / static_cast<double>(static_cast<RwInt32>(mesh->numPolygons)));
        successes = 0;
        ++tunnel->capacity;
        accumulated = static_cast<float>(static_cast<double>(ratio) + static_cast<double>(accumulated) * 0.5);
        if (accumulated < threshold || tunnel->capacity >= tunnelData->lengthLimit) {
            break;
        }
    }
    RwFree(stackBase);
    RwFree(tunnel);
    return mesh;
}

// 0x75E230 (lote S31A, sin validar; 64 bytes; cdecl, (A, B); comparador de qsort de RpGeometrySortByMaterial): ordena registros de 8 bytes por el valor con signo de 16 bits de +4 y, si
// empatan, por el valor sin signo de 16 bits de +6.
int GeometrySortCompare(const void* first, const void* second) {
    const RwUInt8* const a = static_cast<const RwUInt8*>(first);
    const RwUInt8* const b = static_cast<const RwUInt8*>(second);
    const RwInt16 aKey = *reinterpret_cast<const RwInt16*>(a + 4);
    const RwInt16 bKey = *reinterpret_cast<const RwInt16*>(b + 4);
    if (aKey > bKey) {
        return 1;
    }
    if (aKey < bKey) {
        return -1;
    }
    const RwUInt16 aSub = *reinterpret_cast<const RwUInt16*>(a + 6);
    const RwUInt16 bSub = *reinterpret_cast<const RwUInt16*>(b + 6);
    if (aSub > bSub) {
        return 1;
    }
    return aSub < bSub ? -1 : 0;
}

// 0x74BFD0 (lote S31A, sin validar; 6 bytes; cdecl, sin argumentos): primera entrada del registro de plugins de geometria (geometryTKList, 0x8D628C + 0x10 = 0x8D629C).
RwPluginRegEntry* _rpGeometryPluginFirstEntry() {
    return geometryTKList.firstRegEntry;
}

namespace {
// Los llamados de las transcripciones reciben y devuelven enteros de 32 bits (registros): estos adaptadores los llevan a los tipos de las funciones reconstruidas.
template<class T> inline T* AsPtr(uint32_t value) { return reinterpret_cast<T*>(static_cast<uintptr_t>(value)); }
template<class T> inline uint32_t AsReg(T* pointer) { return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pointer)); }
}

void TriStripNeighbours(RpTriStripPolygon* poly, RpTriStripPolygon** current, RpTriStripPolygon** next, RpTriStripPolygon** afterNext, RxVertexIndex* vertex0, RxVertexIndex* vertex1, RxVertexIndex* vertex2);
RpTriStripMesh* TriStripMeshCreate(RpBuildMeshTriangle** triangles, RwUInt32 numTriangles);
void TriStripEmit(RpTriStripMesh* stripMesh, RpMesh* record, RpTriStripData* stripData);

namespace {
uint32_t T_TriStripNeighbours(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g) {
    TriStripNeighbours(AsPtr<RpTriStripPolygon>(a), AsPtr<RpTriStripPolygon*>(b), AsPtr<RpTriStripPolygon*>(c), AsPtr<RpTriStripPolygon*>(d), AsPtr<RxVertexIndex>(e), AsPtr<RxVertexIndex>(f), AsPtr<RxVertexIndex>(g));
    return 0;
}
uint32_t T_TriStripMeshCreate(uint32_t triangles, uint32_t count) { return AsReg(TriStripMeshCreate(AsPtr<RpBuildMeshTriangle*>(triangles), count)); }
uint32_t T_TriStripEmit(uint32_t stripMesh, uint32_t record, uint32_t stripData) {
    TriStripEmit(AsPtr<RpTriStripMesh>(stripMesh), AsPtr<RpMesh>(record), AsPtr<RpTriStripData>(stripData));
    return 0;
}
uint32_t T_Qsort(uint32_t base, uint32_t num, uint32_t width, uint32_t compare) {
    ExeCrt::Qsort(AsPtr<void>(base), num, width, reinterpret_cast<ExeCrt::QsortCompare>(static_cast<uintptr_t>(compare)));
    return 0;
}
uint32_t T_GeometryPluginFirst() { return AsReg(_rpGeometryPluginFirstEntry()); }
uint32_t T_MorphTargetCalcBoundingSphere(uint32_t morphTarget, uint32_t sphere) { return AsReg(RpMorphTargetCalcBoundingSphere(AsPtr<RpMorphTarget>(morphTarget), AsPtr<RwSphere>(sphere))); }
uint32_t T_GeometryAddMorphTarget(uint32_t geometry) { return static_cast<uint32_t>(RpGeometryAddMorphTarget(AsPtr<RpGeometry>(geometry))); }
uint32_t T_GeometryLock(uint32_t geometry, uint32_t mode) { return AsReg(RpGeometryLock(AsPtr<RpGeometry>(geometry), static_cast<RwInt32>(mode))); }
uint32_t T_GeometryUnlock(uint32_t geometry) { return AsReg(RpGeometryUnlock(AsPtr<RpGeometry>(geometry))); }
uint32_t T_GeometryCreate(uint32_t numVertices, uint32_t numTriangles, uint32_t format) { return AsReg(RpGeometryCreate(static_cast<RwInt32>(numVertices), static_cast<RwInt32>(numTriangles), format)); }
uint32_t T_GeometryDestroy(uint32_t geometry) { return static_cast<uint32_t>(RpGeometryDestroy(AsPtr<RpGeometry>(geometry))); }
uint32_t T_MaterialListCopy(uint32_t destination, uint32_t source) { return AsReg(_rpMaterialListCopy(AsPtr<RpMaterialList>(destination), AsPtr<const RpMaterialList>(source))); }
}

// 0x75D240
void TriStripNeighbours(RpTriStripPolygon* poly, RpTriStripPolygon** current, RpTriStripPolygon** next, RpTriStripPolygon** afterNext, RxVertexIndex* vertex0, RxVertexIndex* vertex1, RxVertexIndex* vertex2) {
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0, esi = 0, edi = 0, ebp = 0;
    bool cf = false, zf = false, sf = false, of = false;
    alignas(4) uint8_t fr[0x180] = {};
    *reinterpret_cast<uint32_t*>(fr + 260) = static_cast<uint32_t>((uintptr_t)(poly));
    *reinterpret_cast<uint32_t*>(fr + 264) = static_cast<uint32_t>((uintptr_t)(current));
    *reinterpret_cast<uint32_t*>(fr + 268) = static_cast<uint32_t>((uintptr_t)(next));
    *reinterpret_cast<uint32_t*>(fr + 272) = static_cast<uint32_t>((uintptr_t)(afterNext));
    *reinterpret_cast<uint32_t*>(fr + 276) = static_cast<uint32_t>((uintptr_t)(vertex0));
    *reinterpret_cast<uint32_t*>(fr + 280) = static_cast<uint32_t>((uintptr_t)(vertex1));
    *reinterpret_cast<uint32_t*>(fr + 284) = static_cast<uint32_t>((uintptr_t)(vertex2));
    (void)cf; (void)zf; (void)sf; (void)of;
    { uint32_t a_=ecx, b_=ecx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75d243 xor ecx, ecx
    *(uint32_t*)(fr+240) = ebx; // 75d245 push ebx
    *(uint32_t*)(fr+236) = ebp; // 75d246 push ebp
    ebp = *(uint32_t*)(fr+264); // 75d247 mov ebp, dword ptr [esp + 0x1c]
    *(uint32_t*)(fr+232) = esi; // 75d24b push esi
    esi = *(uint32_t*)(fr+260); // 75d24c mov esi, dword ptr [esp + 0x1c]
    { uint32_t a_=ebx, b_=ebx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ebx = r_; } // 75d250 xor ebx, ebx
    *(uint32_t*)(uintptr_t)(ebp) = esi; // 75d252 mov dword ptr [ebp], esi
    *(uint32_t*)(fr+228) = edi; // 75d255 push edi
    edx = *(uint32_t*)(uintptr_t)(esi+0x18); // 75d256 mov edx, dword ptr [esi + 0x18]
    { uint32_t r_=(uint32_t)(edx & edx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75d259 test edx, edx
    if ((cf||zf)) goto L_75d28c; // 75d25b jbe 0x75d28c
    edi = (esi+0x1c); // 75d25d lea edi, [esi + 0x1c]
    L_75d260:;
    eax = *(uint32_t*)(uintptr_t)(edi); // 75d260 mov eax, dword ptr [edi]
    { uint32_t a_=*(uint32_t*)(uintptr_t)(eax+0x1c), b_=0x0u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d262 cmp dword ptr [eax + 0x1c], 0
    if (zf) goto L_75d274; // 75d266 je 0x75d274
    { uint32_t a_=*(uint32_t*)(uintptr_t)(eax+0x10), b_=0x0u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d268 cmp dword ptr [eax + 0x10], 0
    if (zf) goto L_75d274; // 75d26c je 0x75d274
    { uint32_t a_=*(uint32_t*)(uintptr_t)(eax+0x14), b_=0x0u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d26e cmp dword ptr [eax + 0x14], 0
    if (!zf) goto L_75d27e; // 75d272 jne 0x75d27e
    L_75d274:;
    { uint32_t a_=ecx; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75d274 inc ecx
    { uint32_t a_=edi, b_=0x4u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75d275 add edi, 4
    { uint32_t a_=ecx, b_=edx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d278 cmp ecx, edx
    if (cf) goto L_75d260; // 75d27a jb 0x75d260
    goto L_75d28c; // 75d27c jmp 0x75d28c
    L_75d27e:;
    ecx = *(uint32_t*)(uintptr_t)(esi+ecx*4+0x1c); // 75d27e mov ecx, dword ptr [esi + ecx*4 + 0x1c]
    ebx = *(uint32_t*)(uintptr_t)(ecx+0x10); // 75d282 mov ebx, dword ptr [ecx + 0x10]
    { uint32_t a_=esi, b_=ebx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d285 cmp esi, ebx
    if (!zf) goto L_75d28c; // 75d287 jne 0x75d28c
    ebx = *(uint32_t*)(uintptr_t)(ecx+0x14); // 75d289 mov ebx, dword ptr [ecx + 0x14]
    L_75d28c:;
    eax = *(uint32_t*)(fr+268); // 75d28c mov eax, dword ptr [esp + 0x28]
    { uint32_t a_=ecx, b_=ecx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75d290 xor ecx, ecx
    { uint32_t a_=ebx, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d292 cmp ebx, ecx
    *(uint32_t*)(uintptr_t)(eax) = ebx; // 75d294 mov dword ptr [eax], ebx
    if (!zf) goto L_75d2d0; // 75d296 jne 0x75d2d0
    edx = *(uint32_t*)(uintptr_t)(ebp); // 75d298 mov edx, dword ptr [ebp]
    edi = *(uint32_t*)(fr+228); // 75d29b pop edi
    esi = *(uint32_t*)(fr+232); // 75d29c pop esi
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(edx+0x28)); // 75d29d mov ax, word ptr [edx + 0x28]
    edx = *(uint32_t*)(fr+276); // 75d2a1 mov edx, dword ptr [esp + 0x28]
    *(uint16_t*)(uintptr_t)(edx) = (uint16_t)eax; // 75d2a5 mov word ptr [edx], ax
    eax = *(uint32_t*)(uintptr_t)(ebp); // 75d2a8 mov eax, dword ptr [ebp]
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(eax+0x2a)); // 75d2ab mov dx, word ptr [eax + 0x2a]
    eax = *(uint32_t*)(fr+280); // 75d2af mov eax, dword ptr [esp + 0x2c]
    *(uint16_t*)(uintptr_t)(eax) = (uint16_t)edx; // 75d2b3 mov word ptr [eax], dx
    edx = *(uint32_t*)(uintptr_t)(ebp); // 75d2b6 mov edx, dword ptr [ebp]
    ebp = *(uint32_t*)(fr+236); // 75d2b9 pop ebp
    ebx = *(uint32_t*)(fr+240); // 75d2ba pop ebx
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(edx+0x2c)); // 75d2bb mov ax, word ptr [edx + 0x2c]
    edx = *(uint32_t*)(fr+284); // 75d2bf mov edx, dword ptr [esp + 0x28]
    *(uint16_t*)(uintptr_t)(edx) = (uint16_t)eax; // 75d2c3 mov word ptr [edx], ax
    eax = *(uint32_t*)(fr+272); // 75d2c6 mov eax, dword ptr [esp + 0x1c]
    *(uint32_t*)(uintptr_t)(eax) = ecx; // 75d2ca mov dword ptr [eax], ecx
    return void(); // 75d2cf ret 
    L_75d2d0:;
    edx = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75d2d0 mov edx, dword ptr [ebx + 0x18]
    edi = *(uint32_t*)(uintptr_t)(ebp); // 75d2d3 mov edi, dword ptr [ebp]
    { uint32_t a_=edx, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d2d6 cmp edx, ecx
    *(uint32_t*)(fr+260) = ecx; // 75d2d8 mov dword ptr [esp + 0x20], ecx
    if ((cf||zf)) goto L_75d315; // 75d2dc jbe 0x75d315
    esi = (ebx+0x1c); // 75d2de lea esi, [ebx + 0x1c]
    L_75d2e1:;
    eax = *(uint32_t*)(uintptr_t)(esi); // 75d2e1 mov eax, dword ptr [esi]
    { uint32_t a_=*(uint32_t*)(uintptr_t)(eax+0x1c), b_=0x0u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d2e3 cmp dword ptr [eax + 0x1c], 0
    if (zf) goto L_75d2f3; // 75d2e7 je 0x75d2f3
    { uint32_t a_=edi, b_=*(uint32_t*)(uintptr_t)(eax+0x10); uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d2e9 cmp edi, dword ptr [eax + 0x10]
    if (zf) goto L_75d2f3; // 75d2ec je 0x75d2f3
    { uint32_t a_=edi, b_=*(uint32_t*)(uintptr_t)(eax+0x14); uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d2ee cmp edi, dword ptr [eax + 0x14]
    if (!zf) goto L_75d2fd; // 75d2f1 jne 0x75d2fd
    L_75d2f3:;
    { uint32_t a_=ecx; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75d2f3 inc ecx
    { uint32_t a_=esi, b_=0x4u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75d2f4 add esi, 4
    { uint32_t a_=ecx, b_=edx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d2f7 cmp ecx, edx
    if (cf) goto L_75d2e1; // 75d2f9 jb 0x75d2e1
    goto L_75d315; // 75d2fb jmp 0x75d315
    L_75d2fd:;
    ecx = *(uint32_t*)(uintptr_t)(ebx+ecx*4+0x1c); // 75d2fd mov ecx, dword ptr [ebx + ecx*4 + 0x1c]
    eax = *(uint32_t*)(uintptr_t)(ecx+0x10); // 75d301 mov eax, dword ptr [ecx + 0x10]
    { uint32_t a_=ebx, b_=eax; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d304 cmp ebx, eax
    if (!zf) goto L_75d311; // 75d306 jne 0x75d311
    ecx = *(uint32_t*)(uintptr_t)(ecx+0x14); // 75d308 mov ecx, dword ptr [ecx + 0x14]
    *(uint32_t*)(fr+260) = ecx; // 75d30b mov dword ptr [esp + 0x20], ecx
    goto L_75d315; // 75d30f jmp 0x75d315
    L_75d311:;
    *(uint32_t*)(fr+260) = eax; // 75d311 mov dword ptr [esp + 0x20], eax
    L_75d315:;
    edx = *(uint32_t*)(fr+272); // 75d315 mov edx, dword ptr [esp + 0x2c]
    eax = *(uint32_t*)(fr+260); // 75d319 mov eax, dword ptr [esp + 0x20]
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75d31d test eax, eax
    *(uint32_t*)(uintptr_t)(edx) = eax; // 75d31f mov dword ptr [edx], eax
    if (!zf) goto L_75d37d; // 75d321 jne 0x75d37d
    ebx = *(uint32_t*)(uintptr_t)(ebp); // 75d323 mov ebx, dword ptr [ebp]
    eax = *(uint32_t*)(fr+268); // 75d326 mov eax, dword ptr [esp + 0x28]
    { uint32_t a_=edi, b_=edi; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75d32a xor edi, edi
    ebp = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75d32c mov ebp, dword ptr [ebx + 0x18]
    eax = *(uint32_t*)(uintptr_t)(eax); // 75d32f mov eax, dword ptr [eax]
    { uint32_t r_=(uint32_t)(ebp & ebp); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75d331 test ebp, ebp
    *(uint32_t*)(fr+272) = eax; // 75d333 mov dword ptr [esp + 0x2c], eax
    if ((cf||zf)) goto L_75d364; // 75d337 jbe 0x75d364
    edx = *(uint32_t*)(uintptr_t)(eax+0x18); // 75d339 mov edx, dword ptr [eax + 0x18]
    { uint32_t a_=ebx, b_=0x28u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ebx = r_; } // 75d33c add ebx, 0x28
    L_75d33f:;
    { uint32_t a_=ecx, b_=ecx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75d33f xor ecx, ecx
    { uint32_t r_=(uint32_t)(edx & edx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75d341 test edx, edx
    if ((cf||zf)) goto L_75d35c; // 75d343 jbe 0x75d35c
    esi = (esi & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(ebx)); // 75d345 mov si, word ptr [ebx]
    { uint32_t a_=eax, b_=0x28u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d348 add eax, 0x28
    L_75d34b:;
    { uint16_t a_=(uint16_t)esi, b_=*(uint16_t*)(uintptr_t)(eax); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75d34b cmp si, word ptr [eax]
    if (zf) goto L_75d364; // 75d34e je 0x75d364
    { uint32_t a_=ecx; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75d350 inc ecx
    { uint32_t a_=eax, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d351 add eax, 2
    { uint32_t a_=ecx, b_=edx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d354 cmp ecx, edx
    if (cf) goto L_75d34b; // 75d356 jb 0x75d34b
    eax = *(uint32_t*)(fr+272); // 75d358 mov eax, dword ptr [esp + 0x2c]
    L_75d35c:;
    { uint32_t a_=edi; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75d35c inc edi
    { uint32_t a_=ebx, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ebx = r_; } // 75d35d add ebx, 2
    { uint32_t a_=edi, b_=ebp; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d360 cmp edi, ebp
    if (cf) goto L_75d33f; // 75d362 jb 0x75d33f
    L_75d364:;
    ecx = *(uint32_t*)(fr+264); // 75d364 mov ecx, dword ptr [esp + 0x24]
    ebp = ecx; // 75d368 mov ebp, ecx
    eax = *(uint32_t*)(uintptr_t)(ecx); // 75d36a mov eax, dword ptr [ecx]
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(eax+edi*2+0x28)); // 75d36c mov dx, word ptr [eax + edi*2 + 0x28]
    eax = *(uint32_t*)(fr+284); // 75d371 mov eax, dword ptr [esp + 0x38]
    *(uint16_t*)(uintptr_t)(eax) = (uint16_t)edx; // 75d375 mov word ptr [eax], dx
    goto L_75d414; // 75d378 jmp 0x75d414
    L_75d37d:;
    ecx = *(uint32_t*)(fr+268); // 75d37d mov ecx, dword ptr [esp + 0x28]
    eax = *(uint32_t*)(uintptr_t)(ebp); // 75d381 mov eax, dword ptr [ebp]
    *(uint32_t*)(fr+272) = 0x0u; // 75d384 mov dword ptr [esp + 0x2c], 0
    esi = *(uint32_t*)(uintptr_t)(ecx); // 75d38c mov esi, dword ptr [ecx]
    ecx = *(uint32_t*)(uintptr_t)(eax+0x18); // 75d38e mov ecx, dword ptr [eax + 0x18]
    { uint32_t r_=(uint32_t)(ecx & ecx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75d391 test ecx, ecx
    *(uint32_t*)(fr+248) = esi; // 75d393 mov dword ptr [esp + 0x14], esi
    *(uint32_t*)(fr+252) = ecx; // 75d397 mov dword ptr [esp + 0x18], ecx
    if ((cf||zf)) goto L_75d401; // 75d39b jbe 0x75d401
    ebp = *(uint32_t*)(uintptr_t)(esi+0x18); // 75d39d mov ebp, dword ptr [esi + 0x18]
    { uint32_t a_=eax, b_=0x28u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d3a0 add eax, 0x28
    *(uint32_t*)(fr+244) = eax; // 75d3a3 mov dword ptr [esp + 0x10], eax
    L_75d3a7:;
    { uint32_t a_=ebx, b_=ebx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ebx = r_; } // 75d3a7 xor ebx, ebx
    { uint32_t r_=(uint32_t)(ebp & ebp); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75d3a9 test ebp, ebp
    if ((cf||zf)) goto L_75d3e5; // 75d3ab jbe 0x75d3e5
    edi = (edi & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(eax)); // 75d3ad mov di, word ptr [eax]
    { uint32_t a_=esi, b_=0x28u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75d3b0 add esi, 0x28
    L_75d3b3:;
    { uint16_t a_=(uint16_t)edi, b_=*(uint16_t*)(uintptr_t)(esi); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75d3b3 cmp di, word ptr [esi]
    if (!zf) goto L_75d3d5; // 75d3b6 jne 0x75d3d5
    ecx = *(uint32_t*)(fr+260); // 75d3b8 mov ecx, dword ptr [esp + 0x20]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d3bc xor eax, eax
    edx = *(uint32_t*)(uintptr_t)(ecx+0x18); // 75d3be mov edx, dword ptr [ecx + 0x18]
    { uint32_t r_=(uint32_t)(edx & edx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75d3c1 test edx, edx
    if ((cf||zf)) goto L_75d3d5; // 75d3c3 jbe 0x75d3d5
    { uint32_t a_=ecx, b_=0x28u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75d3c5 add ecx, 0x28
    L_75d3c8:;
    { uint16_t a_=(uint16_t)edi, b_=*(uint16_t*)(uintptr_t)(ecx); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75d3c8 cmp di, word ptr [ecx]
    if (zf) goto L_75d3fd; // 75d3cb je 0x75d3fd
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d3cd inc eax
    { uint32_t a_=ecx, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75d3ce add ecx, 2
    { uint32_t a_=eax, b_=edx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d3d1 cmp eax, edx
    if (cf) goto L_75d3c8; // 75d3d3 jb 0x75d3c8
    L_75d3d5:;
    { uint32_t a_=ebx; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; ebx = r_; } // 75d3d5 inc ebx
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75d3d6 add esi, 2
    { uint32_t a_=ebx, b_=ebp; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d3d9 cmp ebx, ebp
    if (cf) goto L_75d3b3; // 75d3db jb 0x75d3b3
    esi = *(uint32_t*)(fr+248); // 75d3dd mov esi, dword ptr [esp + 0x14]
    ecx = *(uint32_t*)(fr+252); // 75d3e1 mov ecx, dword ptr [esp + 0x18]
    L_75d3e5:;
    edx = *(uint32_t*)(fr+272); // 75d3e5 mov edx, dword ptr [esp + 0x2c]
    eax = *(uint32_t*)(fr+244); // 75d3e9 mov eax, dword ptr [esp + 0x10]
    { uint32_t a_=edx; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75d3ed inc edx
    { uint32_t a_=eax, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d3ee add eax, 2
    { uint32_t a_=edx, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d3f1 cmp edx, ecx
    *(uint32_t*)(fr+272) = edx; // 75d3f3 mov dword ptr [esp + 0x2c], edx
    *(uint32_t*)(fr+244) = eax; // 75d3f7 mov dword ptr [esp + 0x10], eax
    if (cf) goto L_75d3a7; // 75d3fb jb 0x75d3a7
    L_75d3fd:;
    ebp = *(uint32_t*)(fr+264); // 75d3fd mov ebp, dword ptr [esp + 0x24]
    L_75d401:;
    eax = *(uint32_t*)(uintptr_t)(ebp); // 75d401 mov eax, dword ptr [ebp]
    edx = *(uint32_t*)(fr+272); // 75d404 mov edx, dword ptr [esp + 0x2c]
    ecx = *(uint32_t*)(fr+284); // 75d408 mov ecx, dword ptr [esp + 0x38]
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(eax+edx*2+0x28)); // 75d40c mov ax, word ptr [eax + edx*2 + 0x28]
    *(uint16_t*)(uintptr_t)(ecx) = (uint16_t)eax; // 75d411 mov word ptr [ecx], ax
    L_75d414:;
    ebx = *(uint32_t*)(uintptr_t)(ebp); // 75d414 mov ebx, dword ptr [ebp]
    edx = *(uint32_t*)(fr+268); // 75d417 mov edx, dword ptr [esp + 0x28]
    { uint32_t a_=esi, b_=esi; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75d41b xor esi, esi
    eax = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75d41d mov eax, dword ptr [ebx + 0x18]
    ecx = *(uint32_t*)(uintptr_t)(edx); // 75d420 mov ecx, dword ptr [edx]
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75d422 test eax, eax
    *(uint32_t*)(fr+272) = ecx; // 75d424 mov dword ptr [esp + 0x2c], ecx
    if ((cf||zf)) goto L_75d470; // 75d428 jbe 0x75d470
    eax = *(uint32_t*)(fr+284); // 75d42a mov eax, dword ptr [esp + 0x38]
    ebp = (ebx+0x28); // 75d42e lea ebp, [ebx + 0x28]
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(eax)); // 75d431 mov dx, word ptr [eax]
    *(uint16_t*)(fr+268) = (uint16_t)edx; // 75d434 mov word ptr [esp + 0x28], dx
    L_75d439:;
    edi = (edi & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(ebp)); // 75d439 mov di, word ptr [ebp]
    { uint16_t a_=*(uint16_t*)(fr+268), b_=(uint16_t)edi; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75d43d cmp word ptr [esp + 0x28], di
    if (zf) goto L_75d461; // 75d442 je 0x75d461
    edx = *(uint32_t*)(uintptr_t)(ecx+0x18); // 75d444 mov edx, dword ptr [ecx + 0x18]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d447 xor eax, eax
    { uint32_t r_=(uint32_t)(edx & edx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75d449 test edx, edx
    if ((cf||zf)) goto L_75d461; // 75d44b jbe 0x75d461
    { uint32_t a_=ecx, b_=0x28u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75d44d add ecx, 0x28
    L_75d450:;
    { uint16_t a_=(uint16_t)edi, b_=*(uint16_t*)(uintptr_t)(ecx); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75d450 cmp di, word ptr [ecx]
    if (zf) goto L_75d46c; // 75d453 je 0x75d46c
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d455 inc eax
    { uint32_t a_=ecx, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75d456 add ecx, 2
    { uint32_t a_=eax, b_=edx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d459 cmp eax, edx
    if (cf) goto L_75d450; // 75d45b jb 0x75d450
    ecx = *(uint32_t*)(fr+272); // 75d45d mov ecx, dword ptr [esp + 0x2c]
    L_75d461:;
    eax = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75d461 mov eax, dword ptr [ebx + 0x18]
    { uint32_t a_=esi; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75d464 inc esi
    { uint32_t a_=ebp, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ebp = r_; } // 75d465 add ebp, 2
    { uint32_t a_=esi, b_=eax; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d468 cmp esi, eax
    if (cf) goto L_75d439; // 75d46a jb 0x75d439
    L_75d46c:;
    ebp = *(uint32_t*)(fr+264); // 75d46c mov ebp, dword ptr [esp + 0x24]
    L_75d470:;
    eax = *(uint32_t*)(fr+280); // 75d470 mov eax, dword ptr [esp + 0x34]
    esi = (esi & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(ebx+esi*2+0x28)); // 75d474 mov si, word ptr [ebx + esi*2 + 0x28]
    *(uint16_t*)(uintptr_t)(eax) = (uint16_t)esi; // 75d479 mov word ptr [eax], si
    ebx = *(uint32_t*)(uintptr_t)(ebp); // 75d47c mov ebx, dword ptr [ebp]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d47f xor eax, eax
    ecx = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75d481 mov ecx, dword ptr [ebx + 0x18]
    { uint32_t r_=(uint32_t)(ecx & ecx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75d484 test ecx, ecx
    if ((cf||zf)) goto L_75d4a5; // 75d486 jbe 0x75d4a5
    edi = (ebx+0x28); // 75d488 lea edi, [ebx + 0x28]
    L_75d48b:;
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(edi)); // 75d48b mov dx, word ptr [edi]
    { uint16_t a_=(uint16_t)esi, b_=(uint16_t)edx; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75d48e cmp si, dx
    if (zf) goto L_75d49d; // 75d491 je 0x75d49d
    ebp = *(uint32_t*)(fr+284); // 75d493 mov ebp, dword ptr [esp + 0x38]
    { uint16_t a_=*(uint16_t*)(uintptr_t)(ebp), b_=(uint16_t)edx; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75d497 cmp word ptr [ebp], dx
    if (!zf) goto L_75d4a5; // 75d49b jne 0x75d4a5
    L_75d49d:;
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d49d inc eax
    { uint32_t a_=edi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75d49e add edi, 2
    { uint32_t a_=eax, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d4a1 cmp eax, ecx
    if (cf) goto L_75d48b; // 75d4a3 jb 0x75d48b
    L_75d4a5:;
    edx = *(uint32_t*)(fr+276); // 75d4a5 mov edx, dword ptr [esp + 0x30]
    ecx = (ecx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(ebx+eax*2+0x28)); // 75d4a9 mov cx, word ptr [ebx + eax*2 + 0x28]
    edi = *(uint32_t*)(fr+228); // 75d4ae pop edi
    esi = *(uint32_t*)(fr+232); // 75d4af pop esi
    ebp = *(uint32_t*)(fr+236); // 75d4b0 pop ebp
    *(uint16_t*)(uintptr_t)(edx) = (uint16_t)ecx; // 75d4b1 mov word ptr [edx], cx
    ebx = *(uint32_t*)(fr+240); // 75d4b4 pop ebx
    return void(); // 75d4b8 ret 
}

// 0x75C660
RpTriStripMesh* TriStripMeshCreate(RpBuildMeshTriangle** triangles, RwUInt32 numTriangles) {
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0, esi = 0, edi = 0, ebp = 0;
    bool cf = false, zf = false, sf = false, of = false;
    alignas(4) uint8_t fr[0x180] = {};
    *reinterpret_cast<uint32_t*>(fr + 260) = static_cast<uint32_t>((uintptr_t)(triangles));
    *reinterpret_cast<uint32_t*>(fr + 264) = static_cast<uint32_t>((uintptr_t)(numTriangles));
    (void)cf; (void)zf; (void)sf; (void)of;
    *(uint32_t*)(fr+180) = ebx; // 75c663 push ebx
    *(uint32_t*)(fr+176) = ebp; // 75c664 push ebp
    ebp = *(uint32_t*)(fr+264); // 75c665 mov ebp, dword ptr [esp + 0x58]
    *(uint32_t*)(fr+172) = esi; // 75c669 push esi
    *(uint32_t*)(fr+168) = edi; // 75c66a push edi
    { uint32_t a_=esi, b_=esi; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75c66b xor esi, esi
    { uint32_t a_=edi, b_=edi; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75c66d xor edi, edi
    { uint32_t r_=(uint32_t)(ebp & ebp); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75c66f test ebp, ebp
    if ((cf||zf)) goto L_75c695; // 75c671 jbe 0x75c695
    L_75c673:;
    eax = *(uint32_t*)(fr+260); // 75c673 mov eax, dword ptr [esp + 0x5c]
    edx = 0x3u; // 75c677 mov edx, 3
    ecx = *(uint32_t*)(uintptr_t)(eax+edi*4); // 75c67c mov ecx, dword ptr [eax + edi*4]
    L_75c67f:;
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c67f xor eax, eax
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(ecx)); // 75c681 mov ax, word ptr [ecx]
    { uint32_t a_=eax, b_=esi; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c684 cmp eax, esi
    if ((cf||zf)) goto L_75c68a; // 75c686 jbe 0x75c68a
    esi = eax; // 75c688 mov esi, eax
    L_75c68a:;
    { uint32_t a_=ecx, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75c68a add ecx, 2
    { uint32_t a_=edx; uint32_t r_=(uint32_t)(a_-1); of=a_==(uint32_t)0x80000000; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75c68d dec edx
    if (!zf) goto L_75c67f; // 75c68e jne 0x75c67f
    { uint32_t a_=edi; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75c690 inc edi
    { uint32_t a_=edi, b_=ebp; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c691 cmp edi, ebp
    if (cf) goto L_75c673; // 75c693 jb 0x75c673
    L_75c695:;
    edx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75c695 mov edx, dword ptr [0xc97b24]
    ecx = (esi+esi*2); // 75c69b lea ecx, [esi + esi*2]
    ecx = (uint32_t)(ecx << 2); // 75c69e shl ecx, 2
    *(uint32_t*)(fr+164) = 0x10502u; // 75c6a1 push 0x10502
    *(uint32_t*)(fr+160) = ecx; // 75c6a6 push ecx
    eax = ((uint32_t(__cdecl*)(uint32_t,uint32_t))(*(uint32_t*)(uintptr_t)(edx+0x134)))(*(uint32_t*)(fr+160), *(uint32_t*)(fr+164)); // 75c6a7 call dword ptr [edx + 0x134]
    *(uint32_t*)(fr+236) = eax; // 75c6b0 mov dword ptr [esp + 0x44], eax
    { uint32_t r_=(uint32_t)(esi & esi); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75c6b4 test esi, esi
    if ((cf||zf)) goto L_75c6c6; // 75c6b6 jbe 0x75c6c6
    { uint32_t a_=eax, b_=0x4u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c6b8 add eax, 4
    L_75c6bb:;
    *(uint32_t*)(uintptr_t)(eax) = eax; // 75c6bb mov dword ptr [eax], eax
    *(uint32_t*)(uintptr_t)(eax+0x4) = eax; // 75c6bd mov dword ptr [eax + 4], eax
    { uint32_t a_=eax, b_=0xcu; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c6c0 add eax, 0xc
    { uint32_t a_=esi; uint32_t r_=(uint32_t)(a_-1); of=a_==(uint32_t)0x80000000; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75c6c3 dec esi
    if (!zf) goto L_75c6bb; // 75c6c4 jne 0x75c6bb
    L_75c6c6:;
    ecx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75c6c6 mov ecx, dword ptr [0xc97b24]
    eax = (ebp+ebp*8); // 75c6cc lea eax, [ebp + ebp*8]
    eax = (uint32_t)(eax << 2); // 75c6d0 shl eax, 2
    *(uint32_t*)(fr+164) = 0x10502u; // 75c6d3 push 0x10502
    *(uint32_t*)(fr+160) = eax; // 75c6d8 push eax
    eax = ((uint32_t(__cdecl*)(uint32_t,uint32_t))(*(uint32_t*)(uintptr_t)(ecx+0x134)))(*(uint32_t*)(fr+160), *(uint32_t*)(fr+164)); // 75c6d9 call dword ptr [ecx + 0x134]
    edx = (ebp+ebp*8); // 75c6df lea edx, [ebp + ebp*8]
    esi = eax; // 75c6e3 mov esi, eax
    *(uint32_t*)(fr+156) = 0x30502u; // 75c6e5 push 0x30502
    *(uint32_t*)(fr+252) = esi; // 75c6ea mov dword ptr [esp + 0x60], esi
    eax = (ebp+edx*4); // 75c6ee lea eax, [ebp + edx*4]
    edx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75c6f2 mov edx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+192) = esi; // 75c6f8 mov dword ptr [esp + 0x24], esi
    ecx = (eax*4+0x2c); // 75c6fc lea ecx, [eax*4 + 0x2c]
    *(uint32_t*)(fr+152) = ecx; // 75c703 push ecx
    eax = ((uint32_t(__cdecl*)(uint32_t,uint32_t))(*(uint32_t*)(uintptr_t)(edx+0x134)))(*(uint32_t*)(fr+152), *(uint32_t*)(fr+156)); // 75c704 call dword ptr [edx + 0x134]
    edi = eax; // 75c70a mov edi, eax
    *(uint32_t*)(fr+212) = edi; // 75c70f mov dword ptr [esp + 0x2c], edi
    eax = (edi+0x4); // 75c713 lea eax, [edi + 4]
    ebx = (edi+0x2c); // 75c716 lea ebx, [edi + 0x2c]
    *(uint32_t*)(uintptr_t)(edi+0x8) = eax; // 75c719 mov dword ptr [edi + 8], eax
    *(uint32_t*)(uintptr_t)(edi) = ebp; // 75c71c mov dword ptr [edi], ebp
    *(uint32_t*)(uintptr_t)(eax) = eax; // 75c71e mov dword ptr [eax], eax
    eax = (edi+0x14); // 75c720 lea eax, [edi + 0x14]
    *(uint32_t*)(uintptr_t)(edi+0x18) = eax; // 75c723 mov dword ptr [edi + 0x18], eax
    *(uint32_t*)(fr+200) = ebx; // 75c726 mov dword ptr [esp + 0x20], ebx
    *(uint32_t*)(uintptr_t)(eax) = eax; // 75c72a mov dword ptr [eax], eax
    eax = (edi+0xc); // 75c72c lea eax, [edi + 0xc]
    *(uint32_t*)(uintptr_t)(edi+0x10) = eax; // 75c72f mov dword ptr [edi + 0x10], eax
    *(uint32_t*)(uintptr_t)(eax) = eax; // 75c732 mov dword ptr [eax], eax
    eax = (edi+0x24); // 75c734 lea eax, [edi + 0x24]
    *(uint32_t*)(uintptr_t)(edi+0x28) = eax; // 75c737 mov dword ptr [edi + 0x28], eax
    *(uint32_t*)(uintptr_t)(eax) = eax; // 75c73a mov dword ptr [eax], eax
    eax = (edi+0x1c); // 75c73c lea eax, [edi + 0x1c]
    *(uint32_t*)(uintptr_t)(edi+0x20) = eax; // 75c73f mov dword ptr [edi + 0x20], eax
    *(uint32_t*)(uintptr_t)(eax) = eax; // 75c742 mov dword ptr [eax], eax
    eax = (ebp+ebp*2); // 75c744 lea eax, [ebp + ebp*2]
    ecx = (ebp+eax*4); // 75c748 lea ecx, [ebp + eax*4]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c74c xor eax, eax
    { uint32_t r_=(uint32_t)(ebp & ebp); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75c74e test ebp, ebp
    ecx = (ebx+ecx*4); // 75c750 lea ecx, [ebx + ecx*4]
    *(uint32_t*)(fr+224) = eax; // 75c753 mov dword ptr [esp + 0x38], eax
    if ((cf||zf)) goto L_75c981; // 75c757 jbe 0x75c981
    ebp = (ebx+0x8); // 75c75d lea ebp, [ebx + 8]
    goto L_75c766; // 75c760 jmp 0x75c766
    L_75c762:;
    eax = *(uint32_t*)(fr+224); // 75c762 mov eax, dword ptr [esp + 0x38]
    L_75c766:;
    esi = *(uint32_t*)(fr+260); // 75c766 mov esi, dword ptr [esp + 0x5c]
    edx = (ebp+0x20); // 75c76a lea edx, [ebp + 0x20]
    *(uint32_t*)(fr+220) = edx; // 75c76d mov dword ptr [esp + 0x34], edx
    esi = *(uint32_t*)(uintptr_t)(esi+eax*4); // 75c771 mov esi, dword ptr [esi + eax*4]
    esi = (esi & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(esi)); // 75c774 mov si, word ptr [esi]
    *(uint16_t*)(uintptr_t)(edx) = (uint16_t)esi; // 75c777 mov word ptr [edx], si
    esi = *(uint32_t*)(fr+260); // 75c77a mov esi, dword ptr [esp + 0x5c]
    esi = *(uint32_t*)(uintptr_t)(esi+eax*4); // 75c77e mov esi, dword ptr [esi + eax*4]
    esi = (esi & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(esi+0x2)); // 75c781 mov si, word ptr [esi + 2]
    *(uint16_t*)(uintptr_t)(ebp+0x22) = (uint16_t)esi; // 75c785 mov word ptr [ebp + 0x22], si
    esi = *(uint32_t*)(fr+260); // 75c789 mov esi, dword ptr [esp + 0x5c]
    eax = *(uint32_t*)(uintptr_t)(esi+eax*4); // 75c78d mov eax, dword ptr [esi + eax*4]
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(eax+0x4)); // 75c790 mov ax, word ptr [eax + 4]
    *(uint16_t*)(uintptr_t)(ebp+0x24) = (uint16_t)eax; // 75c794 mov word ptr [ebp + 0x24], ax
    esi = *(uint32_t*)(uintptr_t)(edi+0xc); // 75c798 mov esi, dword ptr [edi + 0xc]
    eax = (edi+0xc); // 75c79b lea eax, [edi + 0xc]
    *(uint32_t*)(uintptr_t)(ebp) = esi; // 75c79e mov dword ptr [ebp], esi
    *(uint32_t*)(uintptr_t)(ebp+0x4) = eax; // 75c7a1 mov dword ptr [ebp + 4], eax
    esi = *(uint32_t*)(uintptr_t)(eax); // 75c7a4 mov esi, dword ptr [eax]
    *(uint32_t*)(uintptr_t)(esi+0x4) = ebp; // 75c7a6 mov dword ptr [esi + 4], ebp
    esi = *(uint32_t*)(uintptr_t)(edi+0x4); // 75c7a9 mov esi, dword ptr [edi + 4]
    *(uint32_t*)(uintptr_t)(eax) = ebp; // 75c7ac mov dword ptr [eax], ebp
    eax = (edi+0x4); // 75c7ae lea eax, [edi + 4]
    *(uint32_t*)(uintptr_t)(ebx) = esi; // 75c7b1 mov dword ptr [ebx], esi
    *(uint32_t*)(uintptr_t)(ebp+0xfffffffc) = eax; // 75c7b3 mov dword ptr [ebp - 4], eax
    esi = *(uint32_t*)(uintptr_t)(eax); // 75c7b6 mov esi, dword ptr [eax]
    *(uint32_t*)(uintptr_t)(esi+0x4) = ebx; // 75c7b8 mov dword ptr [esi + 4], ebx
    esi = *(uint32_t*)(fr+192); // 75c7bb mov esi, dword ptr [esp + 0x18]
    *(uint32_t*)(uintptr_t)(eax) = ebx; // 75c7bf mov dword ptr [eax], ebx
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c7c1 xor eax, eax
    { uint32_t a_=esi, b_=0x4u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75c7c3 add esi, 4
    *(uint32_t*)(uintptr_t)(ebp+0x10) = 0x3u; // 75c7c6 mov dword ptr [ebp + 0x10], 3
    *(uint32_t*)(fr+232) = esi; // 75c7cd mov dword ptr [esp + 0x40], esi
    esi = (ebp+0x14); // 75c7d1 lea esi, [ebp + 0x14]
    *(uint32_t*)(uintptr_t)(ebp+0x28) = eax; // 75c7d4 mov dword ptr [ebp + 0x28], eax
    *(uint32_t*)(fr+196) = esi; // 75c7d7 mov dword ptr [esp + 0x1c], esi
    L_75c7db:;
    edx = *(uint32_t*)(fr+220); // 75c7db mov edx, dword ptr [esp + 0x34]
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c7df inc eax
    esi = 0x3u; // 75c7e0 mov esi, 3
    *(uint32_t*)(fr+248) = eax; // 75c7e5 mov dword ptr [esp + 0x50], eax
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(edx)); // 75c7e9 mov dx, word ptr [edx]
    *(uint16_t*)(fr+188) = (uint16_t)edx; // 75c7ec mov word ptr [esp + 0x14], dx
    { uint32_t a_=edx, b_=edx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75c7f1 xor edx, edx
    { uint64_t n_=((uint64_t)edx<<32)|eax; uint32_t d_=esi; eax=(uint32_t)(n_/d_); edx=(uint32_t)(n_%d_); } // 75c7f3 div esi
    eax = *(uint32_t*)(fr+188); // 75c7f5 mov eax, dword ptr [esp + 0x14]
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(ebx+edx*2+0x28)); // 75c7f9 mov dx, word ptr [ebx + edx*2 + 0x28]
    { uint16_t a_=(uint16_t)eax, b_=(uint16_t)edx; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75c7fe cmp ax, dx
    *(uint32_t*)(fr+216) = edx; // 75c801 mov dword ptr [esp + 0x30], edx
    if ((cf||zf)) goto L_75c80f; // 75c805 jbe 0x75c80f
    *(uint32_t*)(fr+188) = edx; // 75c807 mov dword ptr [esp + 0x14], edx
    *(uint32_t*)(fr+216) = eax; // 75c80b mov dword ptr [esp + 0x30], eax
    L_75c80f:;
    eax = *(uint32_t*)(fr+188); // 75c80f mov eax, dword ptr [esp + 0x14]
    edx = *(uint32_t*)(fr+236); // 75c813 mov edx, dword ptr [esp + 0x44]
    { uint32_t a_=eax, b_=0xffffu; uint32_t r_=(uint32_t)(a_ & b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c817 and eax, 0xffff
    eax = (eax+eax*2); // 75c81c lea eax, [eax + eax*2]
    eax = (edx+eax*4); // 75c81f lea eax, [edx + eax*4]
    edx = (eax+0x4); // 75c822 lea edx, [eax + 4]
    eax = *(uint32_t*)(uintptr_t)(eax+0x4); // 75c825 mov eax, dword ptr [eax + 4]
    { uint32_t a_=eax, b_=edx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c828 cmp eax, edx
    *(uint32_t*)(fr+184) = eax; // 75c82a mov dword ptr [esp + 0x10], eax
    if (zf) goto L_75c8d9; // 75c82e je 0x75c8d9
    L_75c834:;
    eax = *(uint32_t*)(fr+184); // 75c834 mov eax, dword ptr [esp + 0x10]
    esi = (esi & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+216)); // 75c838 mov si, word ptr [esp + 0x30]
    eax = *(uint32_t*)(uintptr_t)(eax+0xfffffffc); // 75c83d mov eax, dword ptr [eax - 4]
    *(uint32_t*)(fr+244) = eax; // 75c840 mov dword ptr [esp + 0x4c], eax
    { uint16_t a_=(uint16_t)esi, b_=*(uint16_t*)(uintptr_t)(eax+0x1a); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75c844 cmp si, word ptr [eax + 0x1a]
    if (!zf) goto L_75c8c7; // 75c848 jne 0x75c8c7
    esi = *(uint32_t*)(uintptr_t)(eax+0x14); // 75c84a mov esi, dword ptr [eax + 0x14]
    { uint32_t r_=(uint32_t)(esi & esi); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75c84d test esi, esi
    if (!zf) goto L_75c8c7; // 75c84f jne 0x75c8c7
    esi = *(uint32_t*)(uintptr_t)(eax+0x10); // 75c851 mov esi, dword ptr [eax + 0x10]
    eax = *(uint32_t*)(uintptr_t)(ebp+0x10); // 75c854 mov eax, dword ptr [ebp + 0x10]
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75c857 test eax, eax
    *(uint32_t*)(fr+240) = esi; // 75c859 mov dword ptr [esp + 0x48], esi
    *(uint32_t*)(fr+208) = 0x0u; // 75c85d mov dword ptr [esp + 0x28], 0
    if ((cf||zf)) goto L_75c8c7; // 75c865 jbe 0x75c8c7
    edi = *(uint32_t*)(uintptr_t)(esi+0x18); // 75c867 mov edi, dword ptr [esi + 0x18]
    esi = (ebp+0x20); // 75c86a lea esi, [ebp + 0x20]
    *(uint32_t*)(fr+204) = esi; // 75c86d mov dword ptr [esp + 0x24], esi
    *(uint32_t*)(fr+228) = eax; // 75c871 mov dword ptr [esp + 0x3c], eax
    L_75c875:;
    { uint32_t a_=esi, b_=esi; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75c875 xor esi, esi
    { uint32_t r_=(uint32_t)(edi & edi); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75c877 test edi, edi
    if ((cf||zf)) goto L_75c89c; // 75c879 jbe 0x75c89c
    eax = *(uint32_t*)(fr+204); // 75c87b mov eax, dword ptr [esp + 0x24]
    ebx = (ebx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(eax)); // 75c87f mov bx, word ptr [eax]
    eax = *(uint32_t*)(fr+240); // 75c882 mov eax, dword ptr [esp + 0x48]
    { uint32_t a_=eax, b_=0x28u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c886 add eax, 0x28
    L_75c889:;
    { uint16_t a_=(uint16_t)ebx, b_=*(uint16_t*)(uintptr_t)(eax); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75c889 cmp bx, word ptr [eax]
    if (zf) goto L_75c898; // 75c88c je 0x75c898
    { uint32_t a_=esi; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75c88e inc esi
    { uint32_t a_=eax, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c88f add eax, 2
    { uint32_t a_=esi, b_=edi; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c892 cmp esi, edi
    if (cf) goto L_75c889; // 75c894 jb 0x75c889
    goto L_75c89c; // 75c896 jmp 0x75c89c
    L_75c898:;
    { uint32_t a_=*(uint32_t*)(fr+208); uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; *(uint32_t*)(fr+208) = r_; } // 75c898 inc dword ptr [esp + 0x28]
    L_75c89c:;
    esi = *(uint32_t*)(fr+204); // 75c89c mov esi, dword ptr [esp + 0x24]
    eax = *(uint32_t*)(fr+228); // 75c8a0 mov eax, dword ptr [esp + 0x3c]
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75c8a4 add esi, 2
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_-1); of=a_==(uint32_t)0x80000000; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c8a7 dec eax
    *(uint32_t*)(fr+204) = esi; // 75c8a8 mov dword ptr [esp + 0x24], esi
    *(uint32_t*)(fr+228) = eax; // 75c8ac mov dword ptr [esp + 0x3c], eax
    if (!zf) goto L_75c875; // 75c8b0 jne 0x75c875
    eax = *(uint32_t*)(fr+208); // 75c8b2 mov eax, dword ptr [esp + 0x28]
    ebx = *(uint32_t*)(fr+200); // 75c8b6 mov ebx, dword ptr [esp + 0x20]
    edi = *(uint32_t*)(fr+212); // 75c8ba mov edi, dword ptr [esp + 0x2c]
    { uint32_t a_=eax, b_=0x2u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c8be cmp eax, 2
    if (zf) goto L_75c9ab; // 75c8c1 je 0x75c9ab
    L_75c8c7:;
    eax = *(uint32_t*)(fr+184); // 75c8c7 mov eax, dword ptr [esp + 0x10]
    eax = *(uint32_t*)(uintptr_t)(eax); // 75c8cb mov eax, dword ptr [eax]
    { uint32_t a_=eax, b_=edx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c8cd cmp eax, edx
    *(uint32_t*)(fr+184) = eax; // 75c8cf mov dword ptr [esp + 0x10], eax
    if (!zf) goto L_75c834; // 75c8d3 jne 0x75c834
    L_75c8d9:;
    esi = *(uint32_t*)(uintptr_t)(edi+0x1c); // 75c8d9 mov esi, dword ptr [edi + 0x1c]
    eax = (edi+0x1c); // 75c8dc lea eax, [edi + 0x1c]
    *(uint32_t*)(uintptr_t)(ecx) = esi; // 75c8df mov dword ptr [ecx], esi
    *(uint32_t*)(uintptr_t)(ecx+0x4) = eax; // 75c8e1 mov dword ptr [ecx + 4], eax
    esi = *(uint32_t*)(uintptr_t)(eax); // 75c8e4 mov esi, dword ptr [eax]
    *(uint32_t*)(uintptr_t)(esi+0x4) = ecx; // 75c8e6 mov dword ptr [esi + 4], ecx
    esi = (esi & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+188)); // 75c8e9 mov si, word ptr [esp + 0x14]
    *(uint32_t*)(uintptr_t)(eax) = ecx; // 75c8ee mov dword ptr [eax], ecx
    eax = *(uint32_t*)(fr+196); // 75c8f0 mov eax, dword ptr [esp + 0x1c]
    *(uint32_t*)(uintptr_t)(eax) = ecx; // 75c8f4 mov dword ptr [eax], ecx
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c8f6 xor eax, eax
    *(uint32_t*)(uintptr_t)(ecx+0x14) = eax; // 75c8f8 mov dword ptr [ecx + 0x14], eax
    *(uint16_t*)(uintptr_t)(ecx+0x18) = (uint16_t)esi; // 75c8fb mov word ptr [ecx + 0x18], si
    esi = (esi & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+216)); // 75c8ff mov si, word ptr [esp + 0x30]
    *(uint32_t*)(uintptr_t)(ecx+0x1c) = eax; // 75c904 mov dword ptr [ecx + 0x1c], eax
    eax = *(uint32_t*)(fr+192); // 75c907 mov eax, dword ptr [esp + 0x18]
    *(uint32_t*)(uintptr_t)(ecx+0x10) = ebx; // 75c90b mov dword ptr [ecx + 0x10], ebx
    *(uint16_t*)(uintptr_t)(ecx+0x1a) = (uint16_t)esi; // 75c90e mov word ptr [ecx + 0x1a], si
    *(uint32_t*)(uintptr_t)(eax) = ecx; // 75c912 mov dword ptr [eax], ecx
    eax = *(uint32_t*)(fr+232); // 75c914 mov eax, dword ptr [esp + 0x40]
    esi = *(uint32_t*)(uintptr_t)(edx); // 75c918 mov esi, dword ptr [edx]
    *(uint32_t*)(uintptr_t)(eax) = esi; // 75c91a mov dword ptr [eax], esi
    *(uint32_t*)(uintptr_t)(eax+0x4) = edx; // 75c91c mov dword ptr [eax + 4], edx
    esi = *(uint32_t*)(uintptr_t)(edx); // 75c91f mov esi, dword ptr [edx]
    *(uint32_t*)(uintptr_t)(esi+0x4) = eax; // 75c921 mov dword ptr [esi + 4], eax
    esi = *(uint32_t*)(fr+192); // 75c924 mov esi, dword ptr [esp + 0x18]
    *(uint32_t*)(uintptr_t)(edx) = eax; // 75c928 mov dword ptr [edx], eax
    { uint32_t a_=esi, b_=0xcu; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75c92a add esi, 0xc
    { uint32_t a_=eax, b_=0xcu; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c92d add eax, 0xc
    *(uint32_t*)(fr+192) = esi; // 75c930 mov dword ptr [esp + 0x18], esi
    *(uint32_t*)(fr+232) = eax; // 75c934 mov dword ptr [esp + 0x40], eax
    { uint32_t a_=ecx, b_=0x20u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75c938 add ecx, 0x20
    L_75c93b:;
    edx = *(uint32_t*)(fr+220); // 75c93b mov edx, dword ptr [esp + 0x34]
    esi = *(uint32_t*)(fr+196); // 75c93f mov esi, dword ptr [esp + 0x1c]
    eax = *(uint32_t*)(fr+248); // 75c943 mov eax, dword ptr [esp + 0x50]
    { uint32_t a_=edx, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75c947 add edx, 2
    { uint32_t a_=esi, b_=0x4u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75c94a add esi, 4
    { uint32_t a_=eax, b_=0x3u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c94d cmp eax, 3
    *(uint32_t*)(fr+220) = edx; // 75c950 mov dword ptr [esp + 0x34], edx
    *(uint32_t*)(fr+196) = esi; // 75c954 mov dword ptr [esp + 0x1c], esi
    if (cf) goto L_75c7db; // 75c958 jb 0x75c7db
    eax = *(uint32_t*)(fr+224); // 75c95e mov eax, dword ptr [esp + 0x38]
    edx = *(uint32_t*)(fr+264); // 75c962 mov edx, dword ptr [esp + 0x60]
    { uint32_t a_=ebx, b_=0x34u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ebx = r_; } // 75c966 add ebx, 0x34
    { uint32_t a_=ebp, b_=0x34u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ebp = r_; } // 75c969 add ebp, 0x34
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c96c inc eax
    *(uint32_t*)(fr+200) = ebx; // 75c96d mov dword ptr [esp + 0x20], ebx
    { uint32_t a_=eax, b_=edx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c971 cmp eax, edx
    *(uint32_t*)(fr+224) = eax; // 75c973 mov dword ptr [esp + 0x38], eax
    if (cf) goto L_75c762; // 75c977 jb 0x75c762
    esi = *(uint32_t*)(fr+252); // 75c97d mov esi, dword ptr [esp + 0x54]
    L_75c981:;
    ecx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75c981 mov ecx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+164) = esi; // 75c987 push esi
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(ecx+0x138)))(*(uint32_t*)(fr+164)); // 75c988 call dword ptr [ecx + 0x138]
    edx = *(uint32_t*)(fr+236); // 75c98e mov edx, dword ptr [esp + 0x48]
    eax = *(uint32_t*)(uintptr_t)(0xc97b24); // 75c992 mov eax, dword ptr [0xc97b24]
    *(uint32_t*)(fr+160) = edx; // 75c997 push edx
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(eax+0x138)))(*(uint32_t*)(fr+160)); // 75c998 call dword ptr [eax + 0x138]
    eax = edi; // 75c9a1 mov eax, edi
    edi = *(uint32_t*)(fr+168); // 75c9a3 pop edi
    esi = *(uint32_t*)(fr+172); // 75c9a4 pop esi
    ebp = *(uint32_t*)(fr+176); // 75c9a5 pop ebp
    ebx = *(uint32_t*)(fr+180); // 75c9a6 pop ebx
    return reinterpret_cast<RpTriStripMesh*>(static_cast<uintptr_t>(eax)); // 75c9aa ret 
    L_75c9ab:;
    { uint32_t a_=*(uint32_t*)(fr+184), b_=edx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c9ab cmp dword ptr [esp + 0x10], edx
    if (zf) goto L_75c8d9; // 75c9af je 0x75c8d9
    edx = *(uint32_t*)(fr+196); // 75c9b5 mov edx, dword ptr [esp + 0x1c]
    eax = *(uint32_t*)(fr+244); // 75c9b9 mov eax, dword ptr [esp + 0x4c]
    *(uint32_t*)(uintptr_t)(edx) = eax; // 75c9bd mov dword ptr [edx], eax
    *(uint32_t*)(uintptr_t)(eax+0x14) = ebx; // 75c9bf mov dword ptr [eax + 0x14], ebx
    goto L_75c93b; // 75c9c2 jmp 0x75c93b
}

// 0x75C9D0
void TriStripEmit(RpTriStripMesh* stripMesh, RpMesh* record, RpTriStripData* stripData) {
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0, esi = 0, edi = 0, ebp = 0;
    bool cf = false, zf = false, sf = false, of = false;
    alignas(4) uint8_t fr[0x180] = {};
    *reinterpret_cast<uint32_t*>(fr + 260) = static_cast<uint32_t>((uintptr_t)(stripMesh));
    *reinterpret_cast<uint32_t*>(fr + 264) = static_cast<uint32_t>((uintptr_t)(record));
    *reinterpret_cast<uint32_t*>(fr + 268) = static_cast<uint32_t>((uintptr_t)(stripData));
    (void)cf; (void)zf; (void)sf; (void)of;
    ecx = *(uint32_t*)(fr+268); // 75c9d3 mov ecx, dword ptr [esp + 0x34]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c9d7 xor eax, eax
    *(uint32_t*)(fr+212) = ebx; // 75c9d9 push ebx
    *(uint32_t*)(fr+208) = ebp; // 75c9da push ebp
    edx = *(uint32_t*)(uintptr_t)(ecx); // 75c9db mov edx, dword ptr [ecx]
    *(uint32_t*)(fr+204) = esi; // 75c9dd push esi
    { uint32_t r_=(uint32_t)(edx & edx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75c9de test edx, edx
    edx = *(uint32_t*)(fr+264); // 75c9e0 mov edx, dword ptr [esp + 0x3c]
    *(uint32_t*)(fr+200) = edi; // 75c9e4 push edi
    eax = (eax & 0xffffff00u) | (uint8_t)(zf?1:0); // 75c9e5 sete al
    esi = *(uint32_t*)(uintptr_t)(edx); // 75c9e8 mov esi, dword ptr [edx]
    *(uint32_t*)(fr+216) = eax; // 75c9ea mov dword ptr [esp + 0x10], eax
    eax = *(uint32_t*)(fr+260); // 75c9ee mov eax, dword ptr [esp + 0x3c]
    { uint32_t a_=eax, b_=0x4u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c9f2 add eax, 4
    *(uint32_t*)(fr+232) = eax; // 75c9f5 mov dword ptr [esp + 0x20], eax
    *(uint32_t*)(fr+220) = eax; // 75c9f9 mov dword ptr [esp + 0x14], eax
    L_75c9fd:;
    ecx = *(uint32_t*)(fr+232); // 75c9fd mov ecx, dword ptr [esp + 0x20]
    eax = *(uint32_t*)(fr+220); // 75ca01 mov eax, dword ptr [esp + 0x14]
    { uint32_t a_=eax, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75ca05 cmp eax, ecx
    if (!zf) goto L_75cb2d; // 75ca07 jne 0x75cb2d
    ebp = *(uint32_t*)(uintptr_t)(ecx); // 75ca0d mov ebp, dword ptr [ecx]
    { uint32_t a_=ebp, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75ca0f cmp ebp, ecx
    *(uint32_t*)(fr+220) = ebp; // 75ca11 mov dword ptr [esp + 0x14], ebp
    if (zf) goto L_75cad2; // 75ca15 je 0x75cad2
    ebx = *(uint32_t*)(fr+216); // 75ca1b mov ebx, dword ptr [esp + 0x10]
    L_75ca1f:;
    ecx = (uint32_t)(uintptr_t)(fr+260); // 75ca1f lea ecx, [esp + 0x3c]
    edx = (uint32_t)(uintptr_t)(fr+268); // 75ca23 lea edx, [esp + 0x44]
    *(uint32_t*)(fr+196) = ecx; // 75ca27 push ecx
    eax = (uint32_t)(uintptr_t)(fr+224); // 75ca28 lea eax, [esp + 0x1c]
    *(uint32_t*)(fr+192) = edx; // 75ca2c push edx
    ecx = (uint32_t)(uintptr_t)(fr+236); // 75ca2d lea ecx, [esp + 0x2c]
    *(uint32_t*)(fr+188) = eax; // 75ca31 push eax
    edx = (uint32_t)(uintptr_t)(fr+244); // 75ca32 lea edx, [esp + 0x38]
    *(uint32_t*)(fr+184) = ecx; // 75ca36 push ecx
    eax = (uint32_t)(uintptr_t)(fr+228); // 75ca37 lea eax, [esp + 0x2c]
    *(uint32_t*)(fr+180) = edx; // 75ca3b push edx
    *(uint32_t*)(fr+176) = eax; // 75ca3c push eax
    *(uint32_t*)(fr+172) = ebp; // 75ca3d push ebp
    eax = (uint32_t)(uintptr_t)T_TriStripNeighbours(*(uint32_t*)(fr+172), *(uint32_t*)(fr+176), *(uint32_t*)(fr+180), *(uint32_t*)(fr+184), *(uint32_t*)(fr+188), *(uint32_t*)(fr+192), *(uint32_t*)(fr+196)); // 75ca3e call 0x75d240
    { uint32_t r_=(uint32_t)(ebx & ebx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75ca46 test ebx, ebx
    if (zf) goto L_75caaf; // 75ca48 je 0x75caaf
    edi = *(uint32_t*)(fr+228); // 75ca4a mov edi, dword ptr [esp + 0x1c]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75ca4e xor eax, eax
    ecx = *(uint32_t*)(uintptr_t)(edi+0x18); // 75ca50 mov ecx, dword ptr [edi + 0x18]
    { uint32_t r_=(uint32_t)(ecx & ecx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75ca53 test ecx, ecx
    if ((cf||zf)) goto L_75ca6f; // 75ca55 jbe 0x75ca6f
    ebp = *(uint32_t*)(fr+224); // 75ca57 mov ebp, dword ptr [esp + 0x18]
    edx = (edi+0x28); // 75ca5b lea edx, [edi + 0x28]
    L_75ca5e:;
    { uint16_t a_=(uint16_t)ebp, b_=*(uint16_t*)(uintptr_t)(edx); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75ca5e cmp bp, word ptr [edx]
    if (zf) goto L_75ca6b; // 75ca61 je 0x75ca6b
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75ca63 inc eax
    { uint32_t a_=edx, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75ca64 add edx, 2
    { uint32_t a_=eax, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75ca67 cmp eax, ecx
    if (cf) goto L_75ca5e; // 75ca69 jb 0x75ca5e
    L_75ca6b:;
    ebp = *(uint32_t*)(fr+220); // 75ca6b mov ebp, dword ptr [esp + 0x14]
    L_75ca6f:;
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75ca6f inc eax
    { uint32_t a_=edx, b_=edx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75ca70 xor edx, edx
    { uint64_t n_=((uint64_t)edx<<32)|eax; uint32_t d_=ecx; eax=(uint32_t)(n_/d_); edx=(uint32_t)(n_%d_); } // 75ca72 div ecx
    ecx = (ecx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+268)); // 75ca74 mov cx, word ptr [esp + 0x44]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75ca79 xor eax, eax
    { uint16_t a_=(uint16_t)ecx, b_=*(uint16_t*)(uintptr_t)(edi+edx*2+0x28); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75ca7b cmp cx, word ptr [edi + edx*2 + 0x28]
    if (!zf) goto L_75ca87; // 75ca80 jne 0x75ca87
    { uint32_t a_=ebx, b_=0x1u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75ca82 cmp ebx, 1
    goto L_75ca8a; // 75ca85 jmp 0x75ca8a
    L_75ca87:;
    { uint32_t a_=ebx, b_=0x2u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75ca87 cmp ebx, 2
    L_75ca8a:;
    eax = (eax & 0xffffff00u) | (uint8_t)(zf?1:0); // 75ca8a sete al
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75ca8d test eax, eax
    if (!zf) goto L_75caa8; // 75ca8f jne 0x75caa8
    ebp = *(uint32_t*)(uintptr_t)(ebp); // 75ca91 mov ebp, dword ptr [ebp]
    eax = *(uint32_t*)(fr+232); // 75ca94 mov eax, dword ptr [esp + 0x20]
    { uint32_t a_=ebp, b_=eax; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75ca98 cmp ebp, eax
    *(uint32_t*)(fr+220) = ebp; // 75ca9a mov dword ptr [esp + 0x14], ebp
    if (!zf) goto L_75ca1f; // 75ca9e jne 0x75ca1f
    ecx = eax; // 75caa4 mov ecx, eax
    goto L_75cad2; // 75caa6 jmp 0x75cad2
    L_75caa8:;
    { uint32_t a_=ebx, b_=0x3u; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ebx = r_; } // 75caa8 xor ebx, 3
    *(uint32_t*)(fr+216) = ebx; // 75caab mov dword ptr [esp + 0x10], ebx
    L_75caaf:;
    edx = *(uint32_t*)(fr+264); // 75caaf mov edx, dword ptr [esp + 0x40]
    { uint32_t a_=esi, b_=*(uint32_t*)(uintptr_t)(edx); uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cab3 cmp esi, dword ptr [edx]
    if (zf) goto L_75caca; // 75cab5 je 0x75caca
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(esi+0xfffffffe)); // 75cab7 mov ax, word ptr [esi - 2]
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)eax; // 75cabb mov word ptr [esi], ax
    ecx = (ecx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+224)); // 75cabe mov cx, word ptr [esp + 0x18]
    *(uint16_t*)(uintptr_t)(esi+0x2) = (uint16_t)ecx; // 75cac3 mov word ptr [esi + 2], cx
    { uint32_t a_=esi, b_=0x4u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75cac7 add esi, 4
    L_75caca:;
    ecx = *(uint32_t*)(fr+232); // 75caca mov ecx, dword ptr [esp + 0x20]
    { uint32_t a_=ebp, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cace cmp ebp, ecx
    if (!zf) goto L_75cb2d; // 75cad0 jne 0x75cb2d
    L_75cad2:;
    eax = *(uint32_t*)(uintptr_t)(ecx); // 75cad2 mov eax, dword ptr [ecx]
    { uint32_t a_=eax, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cad4 cmp eax, ecx
    *(uint32_t*)(fr+220) = eax; // 75cad6 mov dword ptr [esp + 0x14], eax
    if (zf) goto L_75d228; // 75cada je 0x75d228
    edx = (uint32_t)(uintptr_t)(fr+260); // 75cae0 lea edx, [esp + 0x3c]
    ecx = (uint32_t)(uintptr_t)(fr+268); // 75cae4 lea ecx, [esp + 0x44]
    *(uint32_t*)(fr+196) = edx; // 75cae8 push edx
    *(uint32_t*)(fr+192) = ecx; // 75cae9 push ecx
    edx = (uint32_t)(uintptr_t)(fr+224); // 75caea lea edx, [esp + 0x20]
    ecx = (uint32_t)(uintptr_t)(fr+236); // 75caee lea ecx, [esp + 0x2c]
    *(uint32_t*)(fr+188) = edx; // 75caf2 push edx
    *(uint32_t*)(fr+184) = ecx; // 75caf3 push ecx
    edx = (uint32_t)(uintptr_t)(fr+244); // 75caf4 lea edx, [esp + 0x3c]
    ecx = (uint32_t)(uintptr_t)(fr+228); // 75caf8 lea ecx, [esp + 0x2c]
    *(uint32_t*)(fr+180) = edx; // 75cafc push edx
    *(uint32_t*)(fr+176) = ecx; // 75cafd push ecx
    *(uint32_t*)(fr+172) = eax; // 75cafe push eax
    eax = (uint32_t)(uintptr_t)T_TriStripNeighbours(*(uint32_t*)(fr+172), *(uint32_t*)(fr+176), *(uint32_t*)(fr+180), *(uint32_t*)(fr+184), *(uint32_t*)(fr+188), *(uint32_t*)(fr+192), *(uint32_t*)(fr+196)); // 75caff call 0x75d240
    edx = *(uint32_t*)(fr+264); // 75cb04 mov edx, dword ptr [esp + 0x5c]
    { uint32_t a_=esi, b_=*(uint32_t*)(uintptr_t)(edx); uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cb0b cmp esi, dword ptr [edx]
    if (zf) goto L_75cb22; // 75cb0d je 0x75cb22
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(esi+0xfffffffe)); // 75cb0f mov ax, word ptr [esi - 2]
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)eax; // 75cb13 mov word ptr [esi], ax
    ecx = (ecx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+224)); // 75cb16 mov cx, word ptr [esp + 0x18]
    *(uint16_t*)(uintptr_t)(esi+0x2) = (uint16_t)ecx; // 75cb1b mov word ptr [esi + 2], cx
    { uint32_t a_=esi, b_=0x4u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75cb1f add esi, 4
    L_75cb22:;
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+224)); // 75cb22 mov dx, word ptr [esp + 0x18]
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)edx; // 75cb27 mov word ptr [esi], dx
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75cb2a add esi, 2
    L_75cb2d:;
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+224)); // 75cb2d mov ax, word ptr [esp + 0x18]
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)eax; // 75cb32 mov word ptr [esi], ax
    ecx = (ecx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+268)); // 75cb35 mov cx, word ptr [esp + 0x44]
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75cb3a add esi, 2
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)ecx; // 75cb3d mov word ptr [esi], cx
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+260)); // 75cb40 mov dx, word ptr [esp + 0x3c]
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75cb45 add esi, 2
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)edx; // 75cb48 mov word ptr [esi], dx
    eax = *(uint32_t*)(fr+244); // 75cb4b mov eax, dword ptr [esp + 0x2c]
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75cb4f add esi, 2
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cb52 test eax, eax
    if (zf) goto L_75cef8; // 75cb54 je 0x75cef8
    L_75cb5a:;
    edx = *(uint32_t*)(fr+236); // 75cb5a mov edx, dword ptr [esp + 0x24]
    *(uint32_t*)(fr+228) = eax; // 75cb5e mov dword ptr [esp + 0x1c], eax
    { uint32_t r_=(uint32_t)(edx & edx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cb62 test edx, edx
    *(uint32_t*)(fr+244) = edx; // 75cb64 mov dword ptr [esp + 0x2c], edx
    if (zf) goto L_75cbba; // 75cb68 je 0x75cbba
    ebx = *(uint32_t*)(uintptr_t)(edx+0x18); // 75cb6a mov ebx, dword ptr [edx + 0x18]
    { uint32_t a_=ebp, b_=ebp; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ebp = r_; } // 75cb6d xor ebp, ebp
    { uint32_t a_=ecx, b_=ecx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75cb6f xor ecx, ecx
    *(uint32_t*)(fr+252) = ebp; // 75cb71 mov dword ptr [esp + 0x34], ebp
    { uint32_t r_=(uint32_t)(ebx & ebx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cb75 test ebx, ebx
    if ((cf||zf)) goto L_75cbb6; // 75cb77 jbe 0x75cbb6
    edi = (edx+0x1c); // 75cb79 lea edi, [edx + 0x1c]
    L_75cb7c:;
    eax = *(uint32_t*)(uintptr_t)(edi); // 75cb7c mov eax, dword ptr [edi]
    { uint32_t a_=*(uint32_t*)(uintptr_t)(eax+0x1c), b_=0x0u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cb7e cmp dword ptr [eax + 0x1c], 0
    if (zf) goto L_75cb96; // 75cb82 je 0x75cb96
    ebp = *(uint32_t*)(fr+228); // 75cb84 mov ebp, dword ptr [esp + 0x1c]
    { uint32_t a_=ebp, b_=*(uint32_t*)(uintptr_t)(eax+0x10); uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cb88 cmp ebp, dword ptr [eax + 0x10]
    if (zf) goto L_75cb92; // 75cb8b je 0x75cb92
    { uint32_t a_=ebp, b_=*(uint32_t*)(uintptr_t)(eax+0x14); uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cb8d cmp ebp, dword ptr [eax + 0x14]
    if (!zf) goto L_75cba2; // 75cb90 jne 0x75cba2
    L_75cb92:;
    ebp = *(uint32_t*)(fr+252); // 75cb92 mov ebp, dword ptr [esp + 0x34]
    L_75cb96:;
    { uint32_t a_=ecx; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75cb96 inc ecx
    { uint32_t a_=edi, b_=0x4u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75cb97 add edi, 4
    { uint32_t a_=ecx, b_=ebx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cb9a cmp ecx, ebx
    if (cf) goto L_75cb7c; // 75cb9c jb 0x75cb7c
    ebx = ebp; // 75cb9e mov ebx, ebp
    goto L_75cbbc; // 75cba0 jmp 0x75cbbc
    L_75cba2:;
    ecx = *(uint32_t*)(uintptr_t)(edx+ecx*4+0x1c); // 75cba2 mov ecx, dword ptr [edx + ecx*4 + 0x1c]
    eax = *(uint32_t*)(uintptr_t)(ecx+0x10); // 75cba6 mov eax, dword ptr [ecx + 0x10]
    { uint32_t a_=edx, b_=eax; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cba9 cmp edx, eax
    if (!zf) goto L_75cbb4; // 75cbab jne 0x75cbb4
    ebp = *(uint32_t*)(uintptr_t)(ecx+0x14); // 75cbad mov ebp, dword ptr [ecx + 0x14]
    ebx = ebp; // 75cbb0 mov ebx, ebp
    goto L_75cbbc; // 75cbb2 jmp 0x75cbbc
    L_75cbb4:;
    ebp = eax; // 75cbb4 mov ebp, eax
    L_75cbb6:;
    ebx = ebp; // 75cbb6 mov ebx, ebp
    goto L_75cbbc; // 75cbb8 jmp 0x75cbbc
    L_75cbba:;
    { uint32_t a_=ebx, b_=ebx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ebx = r_; } // 75cbba xor ebx, ebx
    L_75cbbc:;
    ebp = *(uint32_t*)(fr+228); // 75cbbc mov ebp, dword ptr [esp + 0x1c]
    *(uint32_t*)(fr+236) = ebx; // 75cbc0 mov dword ptr [esp + 0x24], ebx
    { uint32_t a_=ecx, b_=ecx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75cbc4 xor ecx, ecx
    eax = *(uint32_t*)(uintptr_t)(ebp); // 75cbc6 mov eax, dword ptr [ebp]
    { uint32_t a_=eax, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cbc9 cmp eax, ecx
    if (zf) goto L_75cbf3; // 75cbcb je 0x75cbf3
    edx = *(uint32_t*)(uintptr_t)(ebp+0x4); // 75cbcd mov edx, dword ptr [ebp + 4]
    *(uint32_t*)(uintptr_t)(edx) = eax; // 75cbd0 mov dword ptr [edx], eax
    eax = *(uint32_t*)(fr+228); // 75cbd2 mov eax, dword ptr [esp + 0x1c]
    edx = *(uint32_t*)(uintptr_t)(eax); // 75cbd6 mov edx, dword ptr [eax]
    eax = *(uint32_t*)(uintptr_t)(eax+0x4); // 75cbd8 mov eax, dword ptr [eax + 4]
    *(uint32_t*)(uintptr_t)(edx+0x4) = eax; // 75cbdb mov dword ptr [edx + 4], eax
    edx = *(uint32_t*)(fr+228); // 75cbde mov edx, dword ptr [esp + 0x1c]
    *(uint32_t*)(uintptr_t)(edx+0x4) = ecx; // 75cbe2 mov dword ptr [edx + 4], ecx
    eax = *(uint32_t*)(fr+228); // 75cbe5 mov eax, dword ptr [esp + 0x1c]
    *(uint32_t*)(uintptr_t)(eax) = ecx; // 75cbe9 mov dword ptr [eax], ecx
    ebx = *(uint32_t*)(fr+236); // 75cbeb mov ebx, dword ptr [esp + 0x24]
    ebp = *(uint32_t*)(fr+228); // 75cbef mov ebp, dword ptr [esp + 0x1c]
    L_75cbf3:;
    edi = *(uint32_t*)(uintptr_t)(ebp+0x18); // 75cbf3 mov edi, dword ptr [ebp + 0x18]
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(esi+0xfffffffa)); // 75cbf6 mov dx, word ptr [esi - 6]
    { uint32_t a_=edi, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cbfa cmp edi, ecx
    *(uint32_t*)(fr+252) = ecx; // 75cbfc mov dword ptr [esp + 0x34], ecx
    *(uint32_t*)(fr+240) = edx; // 75cc00 mov dword ptr [esp + 0x28], edx
    if ((cf||zf)) goto L_75ccc9; // 75cc04 jbe 0x75ccc9
    ecx = (ebp+0x28); // 75cc0a lea ecx, [ebp + 0x28]
    *(uint32_t*)(fr+248) = edi; // 75cc0d mov dword ptr [esp + 0x30], edi
    L_75cc11:;
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(ecx)); // 75cc11 mov ax, word ptr [ecx]
    { uint16_t a_=(uint16_t)edx, b_=(uint16_t)eax; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75cc14 cmp dx, ax
    if (zf) goto L_75cc2b; // 75cc17 je 0x75cc2b
    { uint16_t a_=*(uint16_t*)(uintptr_t)(esi+0xfffffffc), b_=(uint16_t)eax; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75cc19 cmp word ptr [esi - 4], ax
    if (zf) goto L_75cc31; // 75cc1d je 0x75cc31
    { uint16_t a_=*(uint16_t*)(uintptr_t)(esi+0xfffffffe), b_=(uint16_t)eax; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75cc1f cmp word ptr [esi - 2], ax
    if (zf) goto L_75cc2b; // 75cc23 je 0x75cc2b
    *(uint32_t*)(fr+240) = eax; // 75cc25 mov dword ptr [esp + 0x28], eax
    goto L_75cc39; // 75cc29 jmp 0x75cc39
    L_75cc2b:;
    { uint16_t a_=*(uint16_t*)(uintptr_t)(esi+0xfffffffc), b_=(uint16_t)eax; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75cc2b cmp word ptr [esi - 4], ax
    if (!zf) goto L_75cc39; // 75cc2f jne 0x75cc39
    L_75cc31:;
    *(uint32_t*)(fr+252) = 0x1u; // 75cc31 mov dword ptr [esp + 0x34], 1
    L_75cc39:;
    eax = *(uint32_t*)(fr+248); // 75cc39 mov eax, dword ptr [esp + 0x30]
    { uint32_t a_=ecx, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75cc3d add ecx, 2
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_-1); of=a_==(uint32_t)0x80000000; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75cc40 dec eax
    *(uint32_t*)(fr+248) = eax; // 75cc41 mov dword ptr [esp + 0x30], eax
    if (!zf) goto L_75cc11; // 75cc45 jne 0x75cc11
    eax = *(uint32_t*)(fr+252); // 75cc47 mov eax, dword ptr [esp + 0x34]
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cc4b test eax, eax
    if (zf) goto L_75ccc5; // 75cc4d je 0x75ccc5
    eax = *(uint32_t*)(fr+216); // 75cc4f mov eax, dword ptr [esp + 0x10]
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cc53 test eax, eax
    if (zf) goto L_75ccb8; // 75cc55 je 0x75ccb8
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75cc57 xor eax, eax
    ecx = (ebp+0x28); // 75cc59 lea ecx, [ebp + 0x28]
    L_75cc5c:;
    ebx = (ebx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(esi+0xfffffffc)); // 75cc5c mov bx, word ptr [esi - 4]
    { uint16_t a_=(uint16_t)ebx, b_=*(uint16_t*)(uintptr_t)(ecx); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75cc60 cmp bx, word ptr [ecx]
    if (zf) goto L_75cc6d; // 75cc63 je 0x75cc6d
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75cc65 inc eax
    { uint32_t a_=ecx, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75cc66 add ecx, 2
    { uint32_t a_=eax, b_=edi; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cc69 cmp eax, edi
    if (cf) goto L_75cc5c; // 75cc6b jb 0x75cc5c
    L_75cc6d:;
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75cc6d inc eax
    { uint32_t a_=edx, b_=edx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75cc6e xor edx, edx
    { uint64_t n_=((uint64_t)edx<<32)|eax; uint32_t d_=edi; eax=(uint32_t)(n_/d_); edx=(uint32_t)(n_%d_); } // 75cc70 div edi
    ecx = (ecx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(esi+0xfffffffe)); // 75cc72 mov cx, word ptr [esi - 2]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75cc76 xor eax, eax
    { uint16_t a_=(uint16_t)ecx, b_=*(uint16_t*)(uintptr_t)(ebp+edx*2+0x28); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75cc78 cmp cx, word ptr [ebp + edx*2 + 0x28]
    edx = *(uint32_t*)(fr+216); // 75cc7d mov edx, dword ptr [esp + 0x10]
    if (!zf) goto L_75cc88; // 75cc81 jne 0x75cc88
    { uint32_t a_=edx, b_=0x1u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cc83 cmp edx, 1
    goto L_75cc8b; // 75cc86 jmp 0x75cc8b
    L_75cc88:;
    { uint32_t a_=edx, b_=0x2u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cc88 cmp edx, 2
    L_75cc8b:;
    eax = (eax & 0xffffff00u) | (uint8_t)(zf?1:0); // 75cc8b sete al
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cc8e test eax, eax
    if (zf) goto L_75ccaa; // 75cc90 je 0x75ccaa
    eax = *(uint32_t*)(fr+216); // 75cc92 mov eax, dword ptr [esp + 0x10]
    ecx = (ecx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+240)); // 75cc96 mov cx, word ptr [esp + 0x28]
    { uint32_t a_=eax, b_=0x3u; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75cc9b xor eax, 3
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)ecx; // 75cc9e mov word ptr [esi], cx
    *(uint32_t*)(fr+216) = eax; // 75cca1 mov dword ptr [esp + 0x10], eax
    goto L_75cee9; // 75cca5 jmp 0x75cee9
    L_75ccaa:;
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)ebx; // 75ccaa mov word ptr [esi], bx
    *(uint16_t*)(uintptr_t)(esi+0x2) = (uint16_t)ebx; // 75ccad mov word ptr [esi + 2], bx
    *(uint16_t*)(uintptr_t)(esi+0x4) = (uint16_t)ecx; // 75ccb1 mov word ptr [esi + 4], cx
    { uint32_t a_=esi, b_=0x6u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75ccb5 add esi, 6
    L_75ccb8:;
    ecx = (ecx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+240)); // 75ccb8 mov cx, word ptr [esp + 0x28]
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)ecx; // 75ccbd mov word ptr [esi], cx
    goto L_75cee9; // 75ccc0 jmp 0x75cee9
    L_75ccc5:;
    ebx = *(uint32_t*)(fr+236); // 75ccc5 mov ebx, dword ptr [esp + 0x24]
    L_75ccc9:;
    ecx = *(uint32_t*)(fr+244); // 75ccc9 mov ecx, dword ptr [esp + 0x2c]
    { uint32_t r_=(uint32_t)(ecx & ecx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cccd test ecx, ecx
    if (zf) goto L_75ce6a; // 75cccf je 0x75ce6a
    eax = *(uint32_t*)(uintptr_t)(ecx+0x18); // 75ccd5 mov eax, dword ptr [ecx + 0x18]
    { uint32_t a_=ebp, b_=ebp; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ebp = r_; } // 75ccd8 xor ebp, ebp
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75ccda test eax, eax
    *(uint32_t*)(fr+252) = edx; // 75ccdc mov dword ptr [esp + 0x34], edx
    if ((cf||zf)) goto L_75cd1b; // 75cce0 jbe 0x75cd1b
    { uint32_t a_=ecx, b_=0x28u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75cce2 add ecx, 0x28
    edi = eax; // 75cce5 mov edi, eax
    L_75cce7:;
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(ecx)); // 75cce7 mov ax, word ptr [ecx]
    { uint16_t a_=(uint16_t)edx, b_=(uint16_t)eax; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75ccea cmp dx, ax
    if (zf) goto L_75cd02; // 75cced je 0x75cd02
    { uint16_t a_=*(uint16_t*)(uintptr_t)(esi+0xfffffffe), b_=(uint16_t)eax; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75ccef cmp word ptr [esi - 2], ax
    if (zf) goto L_75cd08; // 75ccf3 je 0x75cd08
    { uint16_t a_=*(uint16_t*)(fr+240), b_=(uint16_t)eax; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75ccf5 cmp word ptr [esp + 0x28], ax
    if (zf) goto L_75cd02; // 75ccfa je 0x75cd02
    *(uint32_t*)(fr+252) = eax; // 75ccfc mov dword ptr [esp + 0x34], eax
    goto L_75cd0d; // 75cd00 jmp 0x75cd0d
    L_75cd02:;
    { uint16_t a_=*(uint16_t*)(uintptr_t)(esi+0xfffffffe), b_=(uint16_t)eax; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75cd02 cmp word ptr [esi - 2], ax
    if (!zf) goto L_75cd0d; // 75cd06 jne 0x75cd0d
    L_75cd08:;
    ebp = 0x1u; // 75cd08 mov ebp, 1
    L_75cd0d:;
    { uint32_t a_=ecx, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75cd0d add ecx, 2
    { uint32_t a_=edi; uint32_t r_=(uint32_t)(a_-1); of=a_==(uint32_t)0x80000000; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75cd10 dec edi
    if (!zf) goto L_75cce7; // 75cd11 jne 0x75cce7
    { uint32_t r_=(uint32_t)(ebp & ebp); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cd13 test ebp, ebp
    if (!zf) goto L_75ce6a; // 75cd15 jne 0x75ce6a
    L_75cd1b:;
    { uint32_t r_=(uint32_t)(ebx & ebx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cd1b test ebx, ebx
    if (zf) goto L_75cde7; // 75cd1d je 0x75cde7
    eax = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75cd23 mov eax, dword ptr [ebx + 0x18]
    { uint32_t a_=ebp, b_=ebp; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ebp = r_; } // 75cd26 xor ebp, ebp
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cd28 test eax, eax
    if ((cf||zf)) goto L_75cd61; // 75cd2a jbe 0x75cd61
    ecx = (ebx+0x28); // 75cd2c lea ecx, [ebx + 0x28]
    edi = eax; // 75cd2f mov edi, eax
    L_75cd31:;
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(ecx)); // 75cd31 mov ax, word ptr [ecx]
    { uint16_t a_=(uint16_t)edx, b_=(uint16_t)eax; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75cd34 cmp dx, ax
    if (zf) goto L_75cd47; // 75cd37 je 0x75cd47
    { uint16_t a_=*(uint16_t*)(fr+240), b_=(uint16_t)eax; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75cd39 cmp word ptr [esp + 0x28], ax
    if (zf) goto L_75cd4e; // 75cd3e je 0x75cd4e
    { uint16_t a_=*(uint16_t*)(fr+252), b_=(uint16_t)eax; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75cd40 cmp word ptr [esp + 0x34], ax
    if (!zf) goto L_75cd53; // 75cd45 jne 0x75cd53
    L_75cd47:;
    { uint16_t a_=*(uint16_t*)(fr+240), b_=(uint16_t)eax; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75cd47 cmp word ptr [esp + 0x28], ax
    if (!zf) goto L_75cd53; // 75cd4c jne 0x75cd53
    L_75cd4e:;
    ebp = 0x1u; // 75cd4e mov ebp, 1
    L_75cd53:;
    { uint32_t a_=ecx, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75cd53 add ecx, 2
    { uint32_t a_=edi; uint32_t r_=(uint32_t)(a_-1); of=a_==(uint32_t)0x80000000; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75cd56 dec edi
    if (!zf) goto L_75cd31; // 75cd57 jne 0x75cd31
    { uint32_t r_=(uint32_t)(ebp & ebp); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cd59 test ebp, ebp
    if (!zf) goto L_75cde7; // 75cd5b jne 0x75cde7
    L_75cd61:;
    ebx = *(uint32_t*)(fr+216); // 75cd61 mov ebx, dword ptr [esp + 0x10]
    *(uint32_t*)(fr+224) = edx; // 75cd65 mov dword ptr [esp + 0x18], edx
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(esi+0xfffffffe)); // 75cd69 mov dx, word ptr [esi - 2]
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)edx; // 75cd6d mov word ptr [esi], dx
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75cd70 add esi, 2
    { uint32_t r_=(uint32_t)(ebx & ebx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cd73 test ebx, ebx
    if (zf) goto L_75cdcf; // 75cd75 je 0x75cdcf
    ebp = *(uint32_t*)(fr+228); // 75cd77 mov ebp, dword ptr [esp + 0x1c]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75cd7b xor eax, eax
    edi = *(uint32_t*)(uintptr_t)(ebp+0x18); // 75cd7d mov edi, dword ptr [ebp + 0x18]
    { uint32_t r_=(uint32_t)(edi & edi); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cd80 test edi, edi
    if ((cf||zf)) goto L_75cd98; // 75cd82 jbe 0x75cd98
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(esi+0xfffffffe)); // 75cd84 mov dx, word ptr [esi - 2]
    ecx = (ebp+0x28); // 75cd88 lea ecx, [ebp + 0x28]
    L_75cd8b:;
    { uint16_t a_=(uint16_t)edx, b_=*(uint16_t*)(uintptr_t)(ecx); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75cd8b cmp dx, word ptr [ecx]
    if (zf) goto L_75cd98; // 75cd8e je 0x75cd98
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75cd90 inc eax
    { uint32_t a_=ecx, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75cd91 add ecx, 2
    { uint32_t a_=eax, b_=edi; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cd94 cmp eax, edi
    if (cf) goto L_75cd8b; // 75cd96 jb 0x75cd8b
    L_75cd98:;
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75cd98 inc eax
    { uint32_t a_=edx, b_=edx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75cd99 xor edx, edx
    { uint64_t n_=((uint64_t)edx<<32)|eax; uint32_t d_=edi; eax=(uint32_t)(n_/d_); edx=(uint32_t)(n_%d_); } // 75cd9b div edi
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+240)); // 75cd9d mov ax, word ptr [esp + 0x28]
    { uint16_t a_=(uint16_t)eax, b_=*(uint16_t*)(uintptr_t)(ebp+edx*2+0x28); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75cda2 cmp ax, word ptr [ebp + edx*2 + 0x28]
    if (!zf) goto L_75cdb0; // 75cda7 jne 0x75cdb0
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75cda9 xor eax, eax
    { uint32_t a_=ebx, b_=0x1u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cdab cmp ebx, 1
    goto L_75cdb5; // 75cdae jmp 0x75cdb5
    L_75cdb0:;
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75cdb0 xor eax, eax
    { uint32_t a_=ebx, b_=0x2u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cdb2 cmp ebx, 2
    L_75cdb5:;
    eax = (eax & 0xffffff00u) | (uint8_t)(zf?1:0); // 75cdb5 sete al
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cdb8 test eax, eax
    if (zf) goto L_75cdc5; // 75cdba je 0x75cdc5
    { uint32_t a_=ebx, b_=0x3u; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ebx = r_; } // 75cdbc xor ebx, 3
    *(uint32_t*)(fr+216) = ebx; // 75cdbf mov dword ptr [esp + 0x10], ebx
    goto L_75cdcf; // 75cdc3 jmp 0x75cdcf
    L_75cdc5:;
    ecx = (ecx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(esi+0xfffffffe)); // 75cdc5 mov cx, word ptr [esi - 2]
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)ecx; // 75cdc9 mov word ptr [esi], cx
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75cdcc add esi, 2
    L_75cdcf:;
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+240)); // 75cdcf mov dx, word ptr [esp + 0x28]
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)edx; // 75cdd4 mov word ptr [esi], dx
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+224)); // 75cdd7 mov ax, word ptr [esp + 0x18]
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75cddc add esi, 2
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)eax; // 75cddf mov word ptr [esi], ax
    goto L_75cee9; // 75cde2 jmp 0x75cee9
    L_75cde7:;
    ebx = *(uint32_t*)(fr+216); // 75cde7 mov ebx, dword ptr [esp + 0x10]
    *(uint32_t*)(fr+224) = edx; // 75cdeb mov dword ptr [esp + 0x18], edx
    ecx = (ecx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(esi+0xfffffffe)); // 75cdef mov cx, word ptr [esi - 2]
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)ecx; // 75cdf3 mov word ptr [esi], cx
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75cdf6 add esi, 2
    { uint32_t r_=(uint32_t)(ebx & ebx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cdf9 test ebx, ebx
    if (zf) goto L_75ce55; // 75cdfb je 0x75ce55
    ebp = *(uint32_t*)(fr+228); // 75cdfd mov ebp, dword ptr [esp + 0x1c]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75ce01 xor eax, eax
    edi = *(uint32_t*)(uintptr_t)(ebp+0x18); // 75ce03 mov edi, dword ptr [ebp + 0x18]
    { uint32_t r_=(uint32_t)(edi & edi); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75ce06 test edi, edi
    if ((cf||zf)) goto L_75ce1e; // 75ce08 jbe 0x75ce1e
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(esi+0xfffffffe)); // 75ce0a mov dx, word ptr [esi - 2]
    ecx = (ebp+0x28); // 75ce0e lea ecx, [ebp + 0x28]
    L_75ce11:;
    { uint16_t a_=(uint16_t)edx, b_=*(uint16_t*)(uintptr_t)(ecx); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75ce11 cmp dx, word ptr [ecx]
    if (zf) goto L_75ce1e; // 75ce14 je 0x75ce1e
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75ce16 inc eax
    { uint32_t a_=ecx, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75ce17 add ecx, 2
    { uint32_t a_=eax, b_=edi; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75ce1a cmp eax, edi
    if (cf) goto L_75ce11; // 75ce1c jb 0x75ce11
    L_75ce1e:;
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75ce1e inc eax
    { uint32_t a_=edx, b_=edx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75ce1f xor edx, edx
    { uint64_t n_=((uint64_t)edx<<32)|eax; uint32_t d_=edi; eax=(uint32_t)(n_/d_); edx=(uint32_t)(n_%d_); } // 75ce21 div edi
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+224)); // 75ce23 mov ax, word ptr [esp + 0x18]
    { uint16_t a_=(uint16_t)eax, b_=*(uint16_t*)(uintptr_t)(ebp+edx*2+0x28); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75ce28 cmp ax, word ptr [ebp + edx*2 + 0x28]
    if (!zf) goto L_75ce36; // 75ce2d jne 0x75ce36
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75ce2f xor eax, eax
    { uint32_t a_=ebx, b_=0x1u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75ce31 cmp ebx, 1
    goto L_75ce3b; // 75ce34 jmp 0x75ce3b
    L_75ce36:;
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75ce36 xor eax, eax
    { uint32_t a_=ebx, b_=0x2u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75ce38 cmp ebx, 2
    L_75ce3b:;
    eax = (eax & 0xffffff00u) | (uint8_t)(zf?1:0); // 75ce3b sete al
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75ce3e test eax, eax
    if (zf) goto L_75ce4b; // 75ce40 je 0x75ce4b
    { uint32_t a_=ebx, b_=0x3u; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ebx = r_; } // 75ce42 xor ebx, 3
    *(uint32_t*)(fr+216) = ebx; // 75ce45 mov dword ptr [esp + 0x10], ebx
    goto L_75ce55; // 75ce49 jmp 0x75ce55
    L_75ce4b:;
    ecx = (ecx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(esi+0xfffffffe)); // 75ce4b mov cx, word ptr [esi - 2]
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)ecx; // 75ce4f mov word ptr [esi], cx
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75ce52 add esi, 2
    L_75ce55:;
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+224)); // 75ce55 mov dx, word ptr [esp + 0x18]
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+240)); // 75ce5a mov ax, word ptr [esp + 0x28]
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)edx; // 75ce5f mov word ptr [esi], dx
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75ce62 add esi, 2
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)eax; // 75ce65 mov word ptr [esi], ax
    goto L_75cee9; // 75ce68 jmp 0x75cee9
    L_75ce6a:;
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(esi+0xfffffffe)); // 75ce6a mov ax, word ptr [esi - 2]
    *(uint16_t*)(uintptr_t)(esi+0xfffffffe) = (uint16_t)edx; // 75ce6e mov word ptr [esi - 2], dx
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)eax; // 75ce72 mov word ptr [esi], ax
    eax = *(uint32_t*)(fr+216); // 75ce75 mov eax, dword ptr [esp + 0x10]
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75ce79 add esi, 2
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75ce7c test eax, eax
    if (zf) goto L_75cee1; // 75ce7e je 0x75cee1
    ebp = *(uint32_t*)(fr+228); // 75ce80 mov ebp, dword ptr [esp + 0x1c]
    ebx = eax; // 75ce84 mov ebx, eax
    { uint32_t a_=ebx, b_=0x3u; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ebx = r_; } // 75ce86 xor ebx, 3
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75ce89 xor eax, eax
    ecx = *(uint32_t*)(uintptr_t)(ebp+0x18); // 75ce8b mov ecx, dword ptr [ebp + 0x18]
    { uint32_t r_=(uint32_t)(ecx & ecx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75ce8e test ecx, ecx
    if ((cf||zf)) goto L_75cea6; // 75ce90 jbe 0x75cea6
    edi = (edi & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(esi+0xfffffffc)); // 75ce92 mov di, word ptr [esi - 4]
    edx = (ebp+0x28); // 75ce96 lea edx, [ebp + 0x28]
    L_75ce99:;
    { uint16_t a_=(uint16_t)edi, b_=*(uint16_t*)(uintptr_t)(edx); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75ce99 cmp di, word ptr [edx]
    if (zf) goto L_75cea6; // 75ce9c je 0x75cea6
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75ce9e inc eax
    { uint32_t a_=edx, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75ce9f add edx, 2
    { uint32_t a_=eax, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cea2 cmp eax, ecx
    if (cf) goto L_75ce99; // 75cea4 jb 0x75ce99
    L_75cea6:;
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75cea6 inc eax
    { uint32_t a_=edx, b_=edx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75cea7 xor edx, edx
    { uint64_t n_=((uint64_t)edx<<32)|eax; uint32_t d_=ecx; eax=(uint32_t)(n_/d_); edx=(uint32_t)(n_%d_); } // 75cea9 div ecx
    edi = (edi & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(esi+0xfffffffe)); // 75ceab mov di, word ptr [esi - 2]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75ceaf xor eax, eax
    { uint16_t a_=(uint16_t)edi, b_=*(uint16_t*)(uintptr_t)(ebp+edx*2+0x28); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75ceb1 cmp di, word ptr [ebp + edx*2 + 0x28]
    if (!zf) goto L_75cebd; // 75ceb6 jne 0x75cebd
    { uint32_t a_=ebx, b_=0x1u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75ceb8 cmp ebx, 1
    goto L_75cec0; // 75cebb jmp 0x75cec0
    L_75cebd:;
    { uint32_t a_=ebx, b_=0x2u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cebd cmp ebx, 2
    L_75cec0:;
    eax = (eax & 0xffffff00u) | (uint8_t)(zf?1:0); // 75cec0 sete al
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cec3 test eax, eax
    if (!zf) goto L_75cee1; // 75cec5 jne 0x75cee1
    ecx = (ecx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(esi+0xfffffffc)); // 75cec7 mov cx, word ptr [esi - 4]
    *(uint16_t*)(uintptr_t)(esi+0x4) = (uint16_t)edi; // 75cecb mov word ptr [esi + 4], di
    *(uint16_t*)(uintptr_t)(esi+0x2) = (uint16_t)ecx; // 75cecf mov word ptr [esi + 2], cx
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)edi; // 75ced3 mov word ptr [esi], di
    *(uint16_t*)(uintptr_t)(esi+0xfffffffc) = (uint16_t)edi; // 75ced6 mov word ptr [esi - 4], di
    { uint32_t a_=esi, b_=0x6u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75ceda add esi, 6
    *(uint32_t*)(fr+216) = ebx; // 75cedd mov dword ptr [esp + 0x10], ebx
    L_75cee1:;
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+240)); // 75cee1 mov dx, word ptr [esp + 0x28]
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)edx; // 75cee6 mov word ptr [esi], dx
    L_75cee9:;
    eax = *(uint32_t*)(fr+244); // 75cee9 mov eax, dword ptr [esp + 0x2c]
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75ceed add esi, 2
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cef0 test eax, eax
    if (!zf) goto L_75cb5a; // 75cef2 jne 0x75cb5a
    L_75cef8:;
    eax = *(uint32_t*)(fr+220); // 75cef8 mov eax, dword ptr [esp + 0x14]
    ebx = *(uint32_t*)(fr+232); // 75cefc mov ebx, dword ptr [esp + 0x20]
    ebp = *(uint32_t*)(fr+216); // 75cf00 mov ebp, dword ptr [esp + 0x10]
    ecx = *(uint32_t*)(uintptr_t)(eax+0x4); // 75cf04 mov ecx, dword ptr [eax + 4]
    edx = *(uint32_t*)(uintptr_t)(eax); // 75cf07 mov edx, dword ptr [eax]
    *(uint32_t*)(uintptr_t)(ecx) = edx; // 75cf09 mov dword ptr [ecx], edx
    ecx = *(uint32_t*)(uintptr_t)(eax); // 75cf0b mov ecx, dword ptr [eax]
    edx = *(uint32_t*)(uintptr_t)(eax+0x4); // 75cf0d mov edx, dword ptr [eax + 4]
    *(uint32_t*)(uintptr_t)(ecx+0x4) = edx; // 75cf10 mov dword ptr [ecx + 4], edx
    *(uint32_t*)(uintptr_t)(eax+0x4) = 0x0u; // 75cf13 mov dword ptr [eax + 4], 0
    *(uint32_t*)(uintptr_t)(eax) = 0x0u; // 75cf1a mov dword ptr [eax], 0
    edi = *(uint32_t*)(uintptr_t)(ebx); // 75cf20 mov edi, dword ptr [ebx]
    { uint32_t a_=edi, b_=ebx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cf22 cmp edi, ebx
    *(uint32_t*)(fr+220) = edi; // 75cf24 mov dword ptr [esp + 0x14], edi
    if (zf) goto L_75cfd6; // 75cf28 je 0x75cfd6
    L_75cf2e:;
    eax = (uint32_t)(uintptr_t)(fr+260); // 75cf2e lea eax, [esp + 0x3c]
    ecx = (uint32_t)(uintptr_t)(fr+268); // 75cf32 lea ecx, [esp + 0x44]
    *(uint32_t*)(fr+196) = eax; // 75cf36 push eax
    edx = (uint32_t)(uintptr_t)(fr+224); // 75cf37 lea edx, [esp + 0x1c]
    *(uint32_t*)(fr+192) = ecx; // 75cf3b push ecx
    eax = (uint32_t)(uintptr_t)(fr+236); // 75cf3c lea eax, [esp + 0x2c]
    *(uint32_t*)(fr+188) = edx; // 75cf40 push edx
    ecx = (uint32_t)(uintptr_t)(fr+244); // 75cf41 lea ecx, [esp + 0x38]
    *(uint32_t*)(fr+184) = eax; // 75cf45 push eax
    edx = (uint32_t)(uintptr_t)(fr+228); // 75cf46 lea edx, [esp + 0x2c]
    *(uint32_t*)(fr+180) = ecx; // 75cf4a push ecx
    *(uint32_t*)(fr+176) = edx; // 75cf4b push edx
    *(uint32_t*)(fr+172) = edi; // 75cf4c push edi
    eax = (uint32_t)(uintptr_t)T_TriStripNeighbours(*(uint32_t*)(fr+172), *(uint32_t*)(fr+176), *(uint32_t*)(fr+180), *(uint32_t*)(fr+184), *(uint32_t*)(fr+188), *(uint32_t*)(fr+192), *(uint32_t*)(fr+196)); // 75cf4d call 0x75d240
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+224)); // 75cf52 mov ax, word ptr [esp + 0x34]
    { uint16_t a_=(uint16_t)eax, b_=*(uint16_t*)(uintptr_t)(esi+0xfffffffe); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75cf5a cmp ax, word ptr [esi - 2]
    if (!zf) goto L_75cfaf; // 75cf5e jne 0x75cfaf
    { uint32_t r_=(uint32_t)(ebp & ebp); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cf60 test ebp, ebp
    if (zf) goto L_75d035; // 75cf62 je 0x75d035
    ebx = *(uint32_t*)(fr+228); // 75cf68 mov ebx, dword ptr [esp + 0x1c]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75cf6c xor eax, eax
    ecx = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75cf6e mov ecx, dword ptr [ebx + 0x18]
    { uint32_t r_=(uint32_t)(ecx & ecx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cf71 test ecx, ecx
    if ((cf||zf)) goto L_75cf8d; // 75cf73 jbe 0x75cf8d
    ebp = *(uint32_t*)(fr+224); // 75cf75 mov ebp, dword ptr [esp + 0x18]
    edx = (ebx+0x28); // 75cf79 lea edx, [ebx + 0x28]
    L_75cf7c:;
    { uint16_t a_=(uint16_t)ebp, b_=*(uint16_t*)(uintptr_t)(edx); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75cf7c cmp bp, word ptr [edx]
    if (zf) goto L_75cf89; // 75cf7f je 0x75cf89
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75cf81 inc eax
    { uint32_t a_=edx, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75cf82 add edx, 2
    { uint32_t a_=eax, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cf85 cmp eax, ecx
    if (cf) goto L_75cf7c; // 75cf87 jb 0x75cf7c
    L_75cf89:;
    ebp = *(uint32_t*)(fr+216); // 75cf89 mov ebp, dword ptr [esp + 0x10]
    L_75cf8d:;
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75cf8d inc eax
    { uint32_t a_=edx, b_=edx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75cf8e xor edx, edx
    { uint64_t n_=((uint64_t)edx<<32)|eax; uint32_t d_=ecx; eax=(uint32_t)(n_/d_); edx=(uint32_t)(n_%d_); } // 75cf90 div ecx
    ecx = (ecx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+268)); // 75cf92 mov cx, word ptr [esp + 0x44]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75cf97 xor eax, eax
    { uint16_t a_=(uint16_t)ecx, b_=*(uint16_t*)(uintptr_t)(ebx+edx*2+0x28); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75cf99 cmp cx, word ptr [ebx + edx*2 + 0x28]
    if (!zf) goto L_75cfa5; // 75cf9e jne 0x75cfa5
    { uint32_t a_=ebp, b_=0x1u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cfa0 cmp ebp, 1
    goto L_75cfa8; // 75cfa3 jmp 0x75cfa8
    L_75cfa5:;
    { uint32_t a_=ebp, b_=0x2u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cfa5 cmp ebp, 2
    L_75cfa8:;
    eax = (eax & 0xffffff00u) | (uint8_t)(zf?1:0); // 75cfa8 sete al
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cfab test eax, eax
    if (!zf) goto L_75cfc3; // 75cfad jne 0x75cfc3
    L_75cfaf:;
    edi = *(uint32_t*)(uintptr_t)(edi); // 75cfaf mov edi, dword ptr [edi]
    ebx = *(uint32_t*)(fr+232); // 75cfb1 mov ebx, dword ptr [esp + 0x20]
    { uint32_t a_=edi, b_=ebx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cfb5 cmp edi, ebx
    if (!zf) goto L_75cf2e; // 75cfb7 jne 0x75cf2e
    *(uint32_t*)(fr+220) = edi; // 75cfbd mov dword ptr [esp + 0x14], edi
    goto L_75cfd6; // 75cfc1 jmp 0x75cfd6
    L_75cfc3:;
    { uint32_t r_=(uint32_t)(ebp & ebp); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cfc3 test ebp, ebp
    *(uint32_t*)(fr+220) = edi; // 75cfc5 mov dword ptr [esp + 0x14], edi
    if (zf) goto L_75d039; // 75cfc9 je 0x75d039
    ebx = *(uint32_t*)(fr+232); // 75cfcb mov ebx, dword ptr [esp + 0x20]
    { uint32_t a_=ebp, b_=0x3u; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ebp = r_; } // 75cfcf xor ebp, 3
    *(uint32_t*)(fr+216) = ebp; // 75cfd2 mov dword ptr [esp + 0x10], ebp
    L_75cfd6:;
    { uint32_t r_=(uint32_t)(ebp & ebp); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75cfd6 test ebp, ebp
    if (zf) goto L_75d03d; // 75cfd8 je 0x75d03d
    { uint32_t a_=edi, b_=ebx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cfda cmp edi, ebx
    if (!zf) goto L_75d167; // 75cfdc jne 0x75d167
    edi = *(uint32_t*)(uintptr_t)(ebx); // 75cfe2 mov edi, dword ptr [ebx]
    { uint32_t a_=edi, b_=ebx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75cfe4 cmp edi, ebx
    *(uint32_t*)(fr+220) = edi; // 75cfe6 mov dword ptr [esp + 0x14], edi
    if (zf) goto L_75d045; // 75cfea je 0x75d045
    L_75cfec:;
    edx = (uint32_t)(uintptr_t)(fr+260); // 75cfec lea edx, [esp + 0x3c]
    eax = (uint32_t)(uintptr_t)(fr+268); // 75cff0 lea eax, [esp + 0x44]
    *(uint32_t*)(fr+196) = edx; // 75cff4 push edx
    ecx = (uint32_t)(uintptr_t)(fr+224); // 75cff5 lea ecx, [esp + 0x1c]
    *(uint32_t*)(fr+192) = eax; // 75cff9 push eax
    edx = (uint32_t)(uintptr_t)(fr+236); // 75cffa lea edx, [esp + 0x2c]
    *(uint32_t*)(fr+188) = ecx; // 75cffe push ecx
    eax = (uint32_t)(uintptr_t)(fr+244); // 75cfff lea eax, [esp + 0x38]
    *(uint32_t*)(fr+184) = edx; // 75d003 push edx
    ecx = (uint32_t)(uintptr_t)(fr+228); // 75d004 lea ecx, [esp + 0x2c]
    *(uint32_t*)(fr+180) = eax; // 75d008 push eax
    *(uint32_t*)(fr+176) = ecx; // 75d009 push ecx
    *(uint32_t*)(fr+172) = edi; // 75d00a push edi
    eax = (uint32_t)(uintptr_t)T_TriStripNeighbours(*(uint32_t*)(fr+172), *(uint32_t*)(fr+176), *(uint32_t*)(fr+180), *(uint32_t*)(fr+184), *(uint32_t*)(fr+188), *(uint32_t*)(fr+192), *(uint32_t*)(fr+196)); // 75d00b call 0x75d240
    eax = *(uint32_t*)(fr+224); // 75d010 mov eax, dword ptr [esp + 0x34]
    { uint16_t a_=(uint16_t)eax, b_=*(uint16_t*)(uintptr_t)(esi+0xfffffffe); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75d017 cmp ax, word ptr [esi - 2]
    if (zf) goto L_75d029; // 75d01b je 0x75d029
    edi = *(uint32_t*)(uintptr_t)(edi); // 75d01d mov edi, dword ptr [edi]
    { uint32_t a_=edi, b_=ebx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d01f cmp edi, ebx
    if (!zf) goto L_75cfec; // 75d021 jne 0x75cfec
    *(uint32_t*)(fr+220) = edi; // 75d023 mov dword ptr [esp + 0x14], edi
    goto L_75d045; // 75d027 jmp 0x75d045
    L_75d029:;
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)eax; // 75d029 mov word ptr [esi], ax
    *(uint32_t*)(fr+220) = edi; // 75d02c mov dword ptr [esp + 0x14], edi
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75d030 add esi, 2
    goto L_75d03d; // 75d033 jmp 0x75d03d
    L_75d035:;
    *(uint32_t*)(fr+220) = edi; // 75d035 mov dword ptr [esp + 0x14], edi
    L_75d039:;
    ebx = *(uint32_t*)(fr+232); // 75d039 mov ebx, dword ptr [esp + 0x20]
    L_75d03d:;
    { uint32_t a_=edi, b_=ebx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d03d cmp edi, ebx
    if (!zf) goto L_75d167; // 75d03f jne 0x75d167
    L_75d045:;
    eax = *(uint32_t*)(uintptr_t)(ebx); // 75d045 mov eax, dword ptr [ebx]
    { uint32_t a_=eax, b_=ebx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d047 cmp eax, ebx
    *(uint32_t*)(fr+220) = eax; // 75d049 mov dword ptr [esp + 0x14], eax
    if (zf) goto L_75d167; // 75d04d je 0x75d167
    edi = eax; // 75d053 mov edi, eax
    L_75d055:;
    edx = (uint32_t)(uintptr_t)(fr+260); // 75d055 lea edx, [esp + 0x3c]
    eax = (uint32_t)(uintptr_t)(fr+268); // 75d059 lea eax, [esp + 0x44]
    *(uint32_t*)(fr+196) = edx; // 75d05d push edx
    ecx = (uint32_t)(uintptr_t)(fr+224); // 75d05e lea ecx, [esp + 0x1c]
    *(uint32_t*)(fr+192) = eax; // 75d062 push eax
    edx = (uint32_t)(uintptr_t)(fr+236); // 75d063 lea edx, [esp + 0x2c]
    *(uint32_t*)(fr+188) = ecx; // 75d067 push ecx
    eax = (uint32_t)(uintptr_t)(fr+244); // 75d068 lea eax, [esp + 0x38]
    *(uint32_t*)(fr+184) = edx; // 75d06c push edx
    ecx = (uint32_t)(uintptr_t)(fr+228); // 75d06d lea ecx, [esp + 0x2c]
    *(uint32_t*)(fr+180) = eax; // 75d071 push eax
    *(uint32_t*)(fr+176) = ecx; // 75d072 push ecx
    *(uint32_t*)(fr+172) = edi; // 75d073 push edi
    eax = (uint32_t)(uintptr_t)T_TriStripNeighbours(*(uint32_t*)(fr+172), *(uint32_t*)(fr+176), *(uint32_t*)(fr+180), *(uint32_t*)(fr+184), *(uint32_t*)(fr+188), *(uint32_t*)(fr+192), *(uint32_t*)(fr+196)); // 75d074 call 0x75d240
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+224)); // 75d079 mov dx, word ptr [esp + 0x34]
    ebx = *(uint32_t*)(fr+228); // 75d07e mov ebx, dword ptr [esp + 0x38]
    { uint16_t a_=(uint16_t)edx, b_=*(uint16_t*)(uintptr_t)(esi+0xfffffffc); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75d085 cmp dx, word ptr [esi - 4]
    if (!zf) goto L_75d0e1; // 75d089 jne 0x75d0e1
    { uint32_t r_=(uint32_t)(ebp & ebp); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75d08b test ebp, ebp
    if (zf) goto L_75d152; // 75d08d je 0x75d152
    ecx = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75d093 mov ecx, dword ptr [ebx + 0x18]
    edi = ebp; // 75d096 mov edi, ebp
    { uint32_t a_=edi, b_=0x3u; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75d098 xor edi, 3
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d09b xor eax, eax
    { uint32_t r_=(uint32_t)(ecx & ecx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75d09d test ecx, ecx
    if ((cf||zf)) goto L_75d0b9; // 75d09f jbe 0x75d0b9
    ebp = *(uint32_t*)(fr+224); // 75d0a1 mov ebp, dword ptr [esp + 0x18]
    edx = (ebx+0x28); // 75d0a5 lea edx, [ebx + 0x28]
    L_75d0a8:;
    { uint16_t a_=(uint16_t)ebp, b_=*(uint16_t*)(uintptr_t)(edx); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75d0a8 cmp bp, word ptr [edx]
    if (zf) goto L_75d0b5; // 75d0ab je 0x75d0b5
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d0ad inc eax
    { uint32_t a_=edx, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75d0ae add edx, 2
    { uint32_t a_=eax, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d0b1 cmp eax, ecx
    if (cf) goto L_75d0a8; // 75d0b3 jb 0x75d0a8
    L_75d0b5:;
    ebp = *(uint32_t*)(fr+216); // 75d0b5 mov ebp, dword ptr [esp + 0x10]
    L_75d0b9:;
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d0b9 inc eax
    { uint32_t a_=edx, b_=edx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75d0ba xor edx, edx
    { uint64_t n_=((uint64_t)edx<<32)|eax; uint32_t d_=ecx; eax=(uint32_t)(n_/d_); edx=(uint32_t)(n_%d_); } // 75d0bc div ecx
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+268)); // 75d0be mov ax, word ptr [esp + 0x44]
    { uint16_t a_=(uint16_t)eax, b_=*(uint16_t*)(uintptr_t)(ebx+edx*2+0x28); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75d0c3 cmp ax, word ptr [ebx + edx*2 + 0x28]
    if (!zf) goto L_75d0d1; // 75d0c8 jne 0x75d0d1
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d0ca xor eax, eax
    { uint32_t a_=edi, b_=0x1u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d0cc cmp edi, 1
    goto L_75d0d6; // 75d0cf jmp 0x75d0d6
    L_75d0d1:;
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d0d1 xor eax, eax
    { uint32_t a_=edi, b_=0x2u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d0d3 cmp edi, 2
    L_75d0d6:;
    eax = (eax & 0xffffff00u) | (uint8_t)(zf?1:0); // 75d0d6 sete al
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75d0d9 test eax, eax
    if (!zf) goto L_75d152; // 75d0db jne 0x75d152
    edi = *(uint32_t*)(fr+220); // 75d0dd mov edi, dword ptr [esp + 0x14]
    L_75d0e1:;
    ecx = (ecx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+268)); // 75d0e1 mov cx, word ptr [esp + 0x44]
    { uint16_t a_=(uint16_t)ecx, b_=*(uint16_t*)(uintptr_t)(esi+0xfffffffe); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75d0e6 cmp cx, word ptr [esi - 2]
    if (!zf) goto L_75d13e; // 75d0ea jne 0x75d13e
    { uint32_t r_=(uint32_t)(ebp & ebp); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75d0ec test ebp, ebp
    if (zf) goto L_75d15c; // 75d0ee je 0x75d15c
    ecx = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75d0f0 mov ecx, dword ptr [ebx + 0x18]
    edi = ebp; // 75d0f3 mov edi, ebp
    { uint32_t a_=edi, b_=0x3u; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75d0f5 xor edi, 3
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d0f8 xor eax, eax
    { uint32_t r_=(uint32_t)(ecx & ecx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75d0fa test ecx, ecx
    if ((cf||zf)) goto L_75d116; // 75d0fc jbe 0x75d116
    ebp = *(uint32_t*)(fr+224); // 75d0fe mov ebp, dword ptr [esp + 0x18]
    edx = (ebx+0x28); // 75d102 lea edx, [ebx + 0x28]
    L_75d105:;
    { uint16_t a_=(uint16_t)ebp, b_=*(uint16_t*)(uintptr_t)(edx); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75d105 cmp bp, word ptr [edx]
    if (zf) goto L_75d112; // 75d108 je 0x75d112
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d10a inc eax
    { uint32_t a_=edx, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75d10b add edx, 2
    { uint32_t a_=eax, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d10e cmp eax, ecx
    if (cf) goto L_75d105; // 75d110 jb 0x75d105
    L_75d112:;
    ebp = *(uint32_t*)(fr+216); // 75d112 mov ebp, dword ptr [esp + 0x10]
    L_75d116:;
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d116 inc eax
    { uint32_t a_=edx, b_=edx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75d117 xor edx, edx
    { uint64_t n_=((uint64_t)edx<<32)|eax; uint32_t d_=ecx; eax=(uint32_t)(n_/d_); edx=(uint32_t)(n_%d_); } // 75d119 div ecx
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+268)); // 75d11b mov ax, word ptr [esp + 0x44]
    { uint16_t a_=(uint16_t)eax, b_=*(uint16_t*)(uintptr_t)(ebx+edx*2+0x28); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75d120 cmp ax, word ptr [ebx + edx*2 + 0x28]
    if (!zf) goto L_75d12e; // 75d125 jne 0x75d12e
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d127 xor eax, eax
    { uint32_t a_=edi, b_=0x1u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d129 cmp edi, 1
    goto L_75d133; // 75d12c jmp 0x75d133
    L_75d12e:;
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75d12e xor eax, eax
    { uint32_t a_=edi, b_=0x2u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d130 cmp edi, 2
    L_75d133:;
    eax = (eax & 0xffffff00u) | (uint8_t)(zf?1:0); // 75d133 sete al
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75d136 test eax, eax
    if (!zf) goto L_75d15c; // 75d138 jne 0x75d15c
    edi = *(uint32_t*)(fr+220); // 75d13a mov edi, dword ptr [esp + 0x14]
    L_75d13e:;
    edi = *(uint32_t*)(uintptr_t)(edi); // 75d13e mov edi, dword ptr [edi]
    eax = *(uint32_t*)(fr+232); // 75d140 mov eax, dword ptr [esp + 0x20]
    { uint32_t a_=edi, b_=eax; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d144 cmp edi, eax
    *(uint32_t*)(fr+220) = edi; // 75d146 mov dword ptr [esp + 0x14], edi
    if (!zf) goto L_75d055; // 75d14a jne 0x75d055
    goto L_75d167; // 75d150 jmp 0x75d167
    L_75d152:;
    ecx = (ecx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+224)); // 75d152 mov cx, word ptr [esp + 0x18]
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)ecx; // 75d157 mov word ptr [esi], cx
    goto L_75d164; // 75d15a jmp 0x75d164
    L_75d15c:;
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+268)); // 75d15c mov dx, word ptr [esp + 0x44]
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)edx; // 75d161 mov word ptr [esi], dx
    L_75d164:;
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75d164 add esi, 2
    L_75d167:;
    { uint32_t r_=(uint32_t)(ebp & ebp); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75d167 test ebp, ebp
    if (zf) goto L_75c9fd; // 75d169 je 0x75c9fd
    eax = *(uint32_t*)(fr+232); // 75d16f mov eax, dword ptr [esp + 0x20]
    ecx = *(uint32_t*)(fr+220); // 75d173 mov ecx, dword ptr [esp + 0x14]
    { uint32_t a_=ecx, b_=eax; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d177 cmp ecx, eax
    if (!zf) goto L_75c9fd; // 75d179 jne 0x75c9fd
    ecx = eax; // 75d17f mov ecx, eax
    eax = *(uint32_t*)(uintptr_t)(ecx); // 75d181 mov eax, dword ptr [ecx]
    { uint32_t a_=eax, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d183 cmp eax, ecx
    *(uint32_t*)(fr+220) = eax; // 75d185 mov dword ptr [esp + 0x14], eax
    if (zf) goto L_75c9fd; // 75d189 je 0x75c9fd
    ebx = ecx; // 75d18f mov ebx, ecx
    edi = eax; // 75d191 mov edi, eax
    L_75d193:;
    ecx = (uint32_t)(uintptr_t)(fr+260); // 75d193 lea ecx, [esp + 0x3c]
    edx = (uint32_t)(uintptr_t)(fr+268); // 75d197 lea edx, [esp + 0x44]
    *(uint32_t*)(fr+196) = ecx; // 75d19b push ecx
    eax = (uint32_t)(uintptr_t)(fr+224); // 75d19c lea eax, [esp + 0x1c]
    *(uint32_t*)(fr+192) = edx; // 75d1a0 push edx
    ecx = (uint32_t)(uintptr_t)(fr+236); // 75d1a1 lea ecx, [esp + 0x2c]
    *(uint32_t*)(fr+188) = eax; // 75d1a5 push eax
    edx = (uint32_t)(uintptr_t)(fr+244); // 75d1a6 lea edx, [esp + 0x38]
    *(uint32_t*)(fr+184) = ecx; // 75d1aa push ecx
    eax = (uint32_t)(uintptr_t)(fr+228); // 75d1ab lea eax, [esp + 0x2c]
    *(uint32_t*)(fr+180) = edx; // 75d1af push edx
    *(uint32_t*)(fr+176) = eax; // 75d1b0 push eax
    *(uint32_t*)(fr+172) = edi; // 75d1b1 push edi
    eax = (uint32_t)(uintptr_t)T_TriStripNeighbours(*(uint32_t*)(fr+172), *(uint32_t*)(fr+176), *(uint32_t*)(fr+180), *(uint32_t*)(fr+184), *(uint32_t*)(fr+188), *(uint32_t*)(fr+192), *(uint32_t*)(fr+196)); // 75d1b2 call 0x75d240
    eax = *(uint32_t*)(fr+224); // 75d1b7 mov eax, dword ptr [esp + 0x34]
    { uint16_t a_=(uint16_t)eax, b_=*(uint16_t*)(uintptr_t)(esi+0xfffffffc); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75d1be cmp ax, word ptr [esi - 4]
    if (zf) goto L_75d1de; // 75d1c2 je 0x75d1de
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+268)); // 75d1c4 mov ax, word ptr [esp + 0x44]
    { uint16_t a_=(uint16_t)eax, b_=*(uint16_t*)(uintptr_t)(esi+0xfffffffe); uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75d1c9 cmp ax, word ptr [esi - 2]
    if (zf) goto L_75d203; // 75d1cd je 0x75d203
    edi = *(uint32_t*)(uintptr_t)(edi); // 75d1cf mov edi, dword ptr [edi]
    { uint32_t a_=edi, b_=ebx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75d1d1 cmp edi, ebx
    if (!zf) goto L_75d193; // 75d1d3 jne 0x75d193
    *(uint32_t*)(fr+220) = edi; // 75d1d5 mov dword ptr [esp + 0x14], edi
    goto L_75c9fd; // 75d1d9 jmp 0x75c9fd
    L_75d1de:;
    edx = *(uint32_t*)(fr+216); // 75d1de mov edx, dword ptr [esp + 0x10]
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)eax; // 75d1e2 mov word ptr [esi], ax
    ecx = (ecx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+224)); // 75d1e5 mov cx, word ptr [esp + 0x18]
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75d1ea add esi, 2
    { uint32_t a_=edx, b_=0x3u; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75d1ed xor edx, 3
    *(uint32_t*)(fr+220) = edi; // 75d1f0 mov dword ptr [esp + 0x14], edi
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)ecx; // 75d1f4 mov word ptr [esi], cx
    *(uint32_t*)(fr+216) = edx; // 75d1f7 mov dword ptr [esp + 0x10], edx
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75d1fb add esi, 2
    goto L_75c9fd; // 75d1fe jmp 0x75c9fd
    L_75d203:;
    edx = *(uint32_t*)(fr+216); // 75d203 mov edx, dword ptr [esp + 0x10]
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)eax; // 75d207 mov word ptr [esi], ax
    { uint32_t a_=edx, b_=0x3u; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75d20a xor edx, 3
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75d20d add esi, 2
    *(uint32_t*)(fr+216) = edx; // 75d210 mov dword ptr [esp + 0x10], edx
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(fr+268)); // 75d214 mov dx, word ptr [esp + 0x44]
    *(uint16_t*)(uintptr_t)(esi) = (uint16_t)edx; // 75d219 mov word ptr [esi], dx
    *(uint32_t*)(fr+220) = edi; // 75d21c mov dword ptr [esp + 0x14], edi
    { uint32_t a_=esi, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75d220 add esi, 2
    goto L_75c9fd; // 75d223 jmp 0x75c9fd
    L_75d228:;
    eax = *(uint32_t*)(fr+264); // 75d228 mov eax, dword ptr [esp + 0x40]
    edi = *(uint32_t*)(fr+200); // 75d22c pop edi
    { uint32_t a_=esi, b_=*(uint32_t*)(uintptr_t)(eax); uint32_t r_=(uint32_t)(a_ - b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75d22d sub esi, dword ptr [eax]
    esi = (uint32_t)((int32_t)(esi) >> 1); // 75d22f sar esi, 1
    *(uint32_t*)(uintptr_t)(eax+0x4) = esi; // 75d231 mov dword ptr [eax + 4], esi
    esi = *(uint32_t*)(fr+204); // 75d234 pop esi
    ebp = *(uint32_t*)(fr+208); // 75d235 pop ebp
    ebx = *(uint32_t*)(fr+212); // 75d236 pop ebx
    return void(); // 75d23a ret 
}

// 0x75DAE0
RpGeometry* RpGeometrySortByMaterial(const RpGeometry* geometry, RpGeometrySortByMaterialCallBack callback) {
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0, esi = 0, edi = 0, ebp = 0;
    bool cf = false, zf = false, sf = false, of = false;
    alignas(4) uint8_t fr[0x180] = {};
    *reinterpret_cast<uint32_t*>(fr + 260) = static_cast<uint32_t>((uintptr_t)(geometry));
    *reinterpret_cast<uint32_t*>(fr + 264) = static_cast<uint32_t>((uintptr_t)(callback));
    (void)cf; (void)zf; (void)sf; (void)of;
    ecx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75dae3 mov ecx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+176) = ebx; // 75dae9 push ebx
    ebx = *(uint32_t*)(fr+260); // 75daea mov ebx, dword ptr [esp + 0x54]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75daee xor eax, eax
    *(uint32_t*)(fr+216) = eax; // 75daf0 mov dword ptr [esp + 0x28], eax
    *(uint32_t*)(fr+220) = eax; // 75daf4 mov dword ptr [esp + 0x2c], eax
    eax = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75daf8 mov eax, dword ptr [ebx + 0x18]
    *(uint32_t*)(fr+172) = ebp; // 75dafb push ebp
    *(uint32_t*)(fr+168) = esi; // 75dafc push esi
    *(uint32_t*)(fr+164) = edi; // 75dafd push edi
    edi = *(uint32_t*)(uintptr_t)(ebx+0x8); // 75dafe mov edi, dword ptr [ebx + 8]
    *(uint32_t*)(fr+160) = 0x1050du; // 75db01 push 0x1050d
    eax = (uint32_t)(eax << 2); // 75db06 shl eax, 2
    *(uint32_t*)(fr+156) = eax; // 75db09 push eax
    *(uint32_t*)(fr+212) = edi; // 75db0a mov dword ptr [esp + 0x38], edi
    eax = ((uint32_t(__cdecl*)(uint32_t,uint32_t))(*(uint32_t*)(uintptr_t)(ecx+0x134)))(*(uint32_t*)(fr+156), *(uint32_t*)(fr+160)); // 75db0e call dword ptr [ecx + 0x134]
    edx = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75db14 mov edx, dword ptr [ebx + 0x18]
    *(uint32_t*)(fr+188) = eax; // 75db17 mov dword ptr [esp + 0x20], eax
    eax = *(uint32_t*)(uintptr_t)(0xc97b24); // 75db1b mov eax, dword ptr [0xc97b24]
    *(uint32_t*)(fr+152) = 0x1050du; // 75db20 push 0x1050d
    edx = (uint32_t)(edx << 2); // 75db25 shl edx, 2
    *(uint32_t*)(fr+148) = edx; // 75db28 push edx
    eax = ((uint32_t(__cdecl*)(uint32_t,uint32_t))(*(uint32_t*)(uintptr_t)(eax+0x134)))(*(uint32_t*)(fr+148), *(uint32_t*)(fr+152)); // 75db29 call dword ptr [eax + 0x134]
    ecx = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75db2f mov ecx, dword ptr [ebx + 0x18]
    edx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75db32 mov edx, dword ptr [0xc97b24]
    ecx = (uint32_t)(ecx << 2); // 75db38 shl ecx, 2
    *(uint32_t*)(fr+144) = 0x1050du; // 75db3b push 0x1050d
    *(uint32_t*)(fr+140) = ecx; // 75db40 push ecx
    *(uint32_t*)(fr+204) = eax; // 75db41 mov dword ptr [esp + 0x40], eax
    eax = ((uint32_t(__cdecl*)(uint32_t,uint32_t))(*(uint32_t*)(uintptr_t)(edx+0x134)))(*(uint32_t*)(fr+140), *(uint32_t*)(fr+144)); // 75db45 call dword ptr [edx + 0x134]
    ecx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75db4b mov ecx, dword ptr [0xc97b24]
    esi = eax; // 75db51 mov esi, eax
    eax = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75db53 mov eax, dword ptr [ebx + 0x18]
    *(uint32_t*)(fr+136) = 0x1050du; // 75db56 push 0x1050d
    eax = (uint32_t)(eax << 2); // 75db5b shl eax, 2
    *(uint32_t*)(fr+132) = eax; // 75db5e push eax
    *(uint32_t*)(fr+200) = esi; // 75db5f mov dword ptr [esp + 0x44], esi
    eax = ((uint32_t(__cdecl*)(uint32_t,uint32_t))(*(uint32_t*)(uintptr_t)(ecx+0x134)))(*(uint32_t*)(fr+132), *(uint32_t*)(fr+136)); // 75db63 call dword ptr [ecx + 0x134]
    ebp = eax; // 75db69 mov ebp, eax
    eax = *(uint32_t*)(uintptr_t)(ebx+0x10); // 75db6b mov eax, dword ptr [ebx + 0x10]
    *(uint32_t*)(fr+128) = 0x1050du; // 75db6e push 0x1050d
    *(uint32_t*)(fr+196) = ebp; // 75db73 mov dword ptr [esp + 0x44], ebp
    edx = (eax+eax*2); // 75db77 lea edx, [eax + eax*2]
    eax = *(uint32_t*)(uintptr_t)(0xc97b24); // 75db7a mov eax, dword ptr [0xc97b24]
    edx = (uint32_t)(edx << 3); // 75db7f shl edx, 3
    *(uint32_t*)(fr+124) = edx; // 75db82 push edx
    eax = ((uint32_t(__cdecl*)(uint32_t,uint32_t))(*(uint32_t*)(uintptr_t)(eax+0x134)))(*(uint32_t*)(fr+124), *(uint32_t*)(fr+128)); // 75db83 call dword ptr [eax + 0x134]
    ecx = edi; // 75db89 mov ecx, edi
    edi = *(uint32_t*)(fr+188); // 75db8b mov edi, dword ptr [esp + 0x40]
    { uint32_t a_=ecx, b_=0x10u; uint32_t r_=(uint32_t)(a_ & b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75db8f and ecx, 0x10
    { uint32_t a_=ecx, b_=0x10u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75db95 cmp ecx, 0x10
    *(uint32_t*)(fr+184) = eax; // 75db98 mov dword ptr [esp + 0x14], eax
    *(uint32_t*)(fr+228) = ecx; // 75db9c mov dword ptr [esp + 0x40], ecx
    if (!zf) goto L_75dbb6; // 75dba0 jne 0x75dbb6
    { uint32_t r_=(uint32_t)(edi & edi); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75dba2 test edi, edi
    if (zf) goto L_75e1b2; // 75dba4 je 0x75e1b2
    ecx = *(uint32_t*)(fr+204); // 75dbaa mov ecx, dword ptr [esp + 0x28]
    { uint32_t r_=(uint32_t)(ecx & ecx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75dbae test ecx, ecx
    if (zf) goto L_75e1b2; // 75dbb0 je 0x75e1b2
    L_75dbb6:;
    { uint32_t r_=(uint32_t)(esi & esi); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75dbb6 test esi, esi
    if (zf) goto L_75e1b2; // 75dbb8 je 0x75e1b2
    { uint32_t r_=(uint32_t)(ebp & ebp); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75dbbe test ebp, ebp
    if (zf) goto L_75e1b2; // 75dbc0 je 0x75e1b2
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75dbc6 test eax, eax
    if (zf) goto L_75e1c5; // 75dbc8 je 0x75e1c5
    eax = *(uint32_t*)(uintptr_t)(ebx+0x10); // 75dbce mov eax, dword ptr [ebx + 0x10]
    { uint32_t a_=esi, b_=esi; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75dbd1 xor esi, esi
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75dbd3 test eax, eax
    if ((zf||sf!=of)) goto L_75dc2b; // 75dbd5 jle 0x75dc2b
    ecx = *(uint32_t*)(fr+184); // 75dbd7 mov ecx, dword ptr [esp + 0x14]
    ebp = (ecx+0x6); // 75dbdb lea ebp, [ecx + 6]
    goto L_75dbe4; // 75dbde jmp 0x75dbe4
    L_75dbe0:;
    ebp = *(uint32_t*)(fr+260); // 75dbe0 mov ebp, dword ptr [esp + 0x60]
    L_75dbe4:;
    eax = *(uint32_t*)(uintptr_t)(ebx+0x2c); // 75dbe4 mov eax, dword ptr [ebx + 0x2c]
    edx = (esi*8); // 75dbe7 lea edx, [esi*8]
    { uint32_t a_=ecx, b_=ecx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75dbee xor ecx, ecx
    edi = (edi & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(eax+edx+0x6)); // 75dbf0 mov di, word ptr [eax + edx + 6]
    eax = ebp; // 75dbf5 mov eax, ebp
    { uint32_t a_=ebp, b_=0x18u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ebp = r_; } // 75dbf7 add ebp, 0x18
    *(uint32_t*)(fr+260) = ebp; // 75dbfa mov dword ptr [esp + 0x60], ebp
    L_75dbfe:;
    *(uint16_t*)(uintptr_t)(eax+0xfffffffe) = (uint16_t)edi; // 75dbfe mov word ptr [eax - 2], di
    ebp = *(uint32_t*)(uintptr_t)(ebx+0x2c); // 75dc02 mov ebp, dword ptr [ebx + 0x2c]
    { uint32_t a_=eax, b_=0x8u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75dc05 add eax, 8
    ebp = (ebp & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(edx+ebp)); // 75dc08 mov bp, word ptr [edx + ebp]
    { uint32_t a_=edx, b_=0x2u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75dc0c add edx, 2
    *(uint16_t*)(uintptr_t)(eax+0xfffffff8) = (uint16_t)ebp; // 75dc0f mov word ptr [eax - 8], bp
    ebp = ecx; // 75dc13 mov ebp, ecx
    ebp = (uint32_t)(ebp << 30); // 75dc15 shl ebp, 0x1e
    { uint32_t a_=ebp, b_=esi; uint32_t r_=(uint32_t)(a_ | b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ebp = r_; } // 75dc18 or ebp, esi
    { uint32_t a_=ecx; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75dc1a inc ecx
    *(uint32_t*)(uintptr_t)(eax+0xfffffff2) = ebp; // 75dc1b mov dword ptr [eax - 0xe], ebp
    { uint32_t a_=ecx, b_=0x3u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75dc1e cmp ecx, 3
    if ((sf!=of)) goto L_75dbfe; // 75dc21 jl 0x75dbfe
    eax = *(uint32_t*)(uintptr_t)(ebx+0x10); // 75dc23 mov eax, dword ptr [ebx + 0x10]
    { uint32_t a_=esi; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75dc26 inc esi
    { uint32_t a_=esi, b_=eax; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75dc27 cmp esi, eax
    if ((sf!=of)) goto L_75dbe0; // 75dc29 jl 0x75dbe0
    L_75dc2b:;
    eax = *(uint32_t*)(uintptr_t)(ebx+0x10); // 75dc2b mov eax, dword ptr [ebx + 0x10]
    esi = *(uint32_t*)(fr+184); // 75dc2e mov esi, dword ptr [esp + 0x14]
    *(uint32_t*)(fr+160) = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&GeometrySortCompare)); // 75dc32 push 0x75e230
    *(uint32_t*)(fr+156) = 0x8u; // 75dc37 push 8
    ecx = (eax+eax*2); // 75dc39 lea ecx, [eax + eax*2]
    *(uint32_t*)(fr+152) = ecx; // 75dc3c push ecx
    *(uint32_t*)(fr+148) = esi; // 75dc3d push esi
    eax = (uint32_t)(uintptr_t)T_Qsort(*(uint32_t*)(fr+148), *(uint32_t*)(fr+152), *(uint32_t*)(fr+156), *(uint32_t*)(fr+160)); // 75dc3e call 0x8247e0
    eax = *(uint32_t*)(uintptr_t)(ebx+0x10); // 75dc43 mov eax, dword ptr [ebx + 0x10]
    { uint32_t a_=ebp, b_=0xffffffffu; uint32_t r_=(uint32_t)(a_ | b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ebp = r_; } // 75dc49 or ebp, 0xffffffff
    { uint32_t a_=ecx, b_=ecx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75dc4c xor ecx, ecx
    edi = (eax+eax*2); // 75dc4e lea edi, [eax + eax*2]
    { uint32_t r_=(uint32_t)(edi & edi); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75dc51 test edi, edi
    if ((zf||sf!=of)) goto L_75dc82; // 75dc53 jle 0x75dc82
    eax = (esi+0xfffffffe); // 75dc55 lea eax, [esi - 2]
    L_75dc58:;
    { uint32_t r_=(uint32_t)(ecx & ecx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75dc58 test ecx, ecx
    if (zf) goto L_75dc79; // 75dc5a je 0x75dc79
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(eax+0xfffffffe)); // 75dc5c mov dx, word ptr [eax - 2]
    esi = (esi & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(eax+0x6)); // 75dc60 mov si, word ptr [eax + 6]
    { uint16_t a_=(uint16_t)edx, b_=(uint16_t)esi; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75dc64 cmp dx, si
    if ((!zf&&sf==of)) goto L_75dc79; // 75dc67 jg 0x75dc79
    if ((sf!=of)) goto L_75dc79; // 75dc69 jl 0x75dc79
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(eax)); // 75dc6b mov dx, word ptr [eax]
    esi = (esi & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(eax+0x8)); // 75dc6e mov si, word ptr [eax + 8]
    { uint16_t a_=(uint16_t)edx, b_=(uint16_t)esi; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75dc72 cmp dx, si
    if ((!cf&&!zf)) goto L_75dc79; // 75dc75 ja 0x75dc79
    if (!cf) goto L_75dc7a; // 75dc77 jae 0x75dc7a
    L_75dc79:;
    { uint32_t a_=ebp; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; ebp = r_; } // 75dc79 inc ebp
    L_75dc7a:;
    { uint32_t a_=ecx; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75dc7a inc ecx
    { uint32_t a_=eax, b_=0x8u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75dc7b add eax, 8
    { uint32_t a_=ecx, b_=edi; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75dc7e cmp ecx, edi
    if ((sf!=of)) goto L_75dc58; // 75dc80 jl 0x75dc58
    L_75dc82:;
    eax = *(uint32_t*)(uintptr_t)(0xc97b24); // 75dc82 mov eax, dword ptr [0xc97b24]
    esi = (ebp+0x1); // 75dc87 lea esi, [ebp + 1]
    *(uint32_t*)(fr+160) = 0x1050du; // 75dc8a push 0x1050d
    *(uint32_t*)(fr+192) = esi; // 75dc8f mov dword ptr [esp + 0x20], esi
    edx = (esi+esi); // 75dc93 lea edx, [esi + esi]
    *(uint32_t*)(fr+156) = edx; // 75dc96 push edx
    eax = ((uint32_t(__cdecl*)(uint32_t,uint32_t))(*(uint32_t*)(uintptr_t)(eax+0x134)))(*(uint32_t*)(fr+156), *(uint32_t*)(fr+160)); // 75dc97 call dword ptr [eax + 0x134]
    ebp = *(uint32_t*)(fr+212); // 75dc9d mov ebp, dword ptr [esp + 0x38]
    edi = eax; // 75dca1 mov edi, eax
    eax = *(uint32_t*)(uintptr_t)(ebx+0x1c); // 75dca3 mov eax, dword ptr [ebx + 0x1c]
    ecx = *(uint32_t*)(uintptr_t)(ebx+0x10); // 75dca6 mov ecx, dword ptr [ebx + 0x10]
    { uint32_t a_=eax, b_=0xffu; uint32_t r_=(uint32_t)(a_ & b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75dca9 and eax, 0xff
    eax = (uint32_t)(eax << 16); // 75dcb1 shl eax, 0x10
    { uint32_t a_=eax, b_=ebp; uint32_t r_=(uint32_t)(a_ | b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75dcb4 or eax, ebp
    *(uint32_t*)(fr+160) = 0xfffu; // 75dcb6 push 0xfff
    *(uint32_t*)(fr+156) = eax; // 75dcbb push eax
    *(uint32_t*)(fr+152) = ecx; // 75dcbc push ecx
    *(uint32_t*)(fr+148) = esi; // 75dcbd push esi
    *(uint32_t*)(fr+260) = edi; // 75dcbe mov dword ptr [esp + 0x70], edi
    eax = (uint32_t)(uintptr_t)T_GeometryCreate(*(uint32_t*)(fr+148), *(uint32_t*)(fr+152), *(uint32_t*)(fr+156)); // 75dcc2 call 0x74ca90
    *(uint32_t*)(fr+156) = eax; // 75dcca push eax
    eax = (uint32_t)(uintptr_t)T_GeometryLock(*(uint32_t*)(fr+156), *(uint32_t*)(fr+160)); // 75dccb call 0x74c7d0
    ebp = eax; // 75dcd3 mov ebp, eax
    { uint32_t r_=(uint32_t)(edi & edi); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75dcd5 test edi, edi
    *(uint32_t*)(fr+180) = ebp; // 75dcd7 mov dword ptr [esp + 0x10], ebp
    if (zf) goto L_75e152; // 75dcdb je 0x75e152
    { uint32_t r_=(uint32_t)(ebp & ebp); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75dce1 test ebp, ebp
    if (zf) goto L_75e152; // 75dce3 je 0x75e152
    eax = *(uint32_t*)(uintptr_t)(ebx+0x10); // 75dce9 mov eax, dword ptr [ebx + 0x10]
    { uint32_t a_=edi, b_=edi; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75dcec xor edi, edi
    *(uint32_t*)(fr+208) = 0xffffffffu; // 75dcee mov dword ptr [esp + 0x2c], 0xffffffff
    edx = (eax+eax*2); // 75dcf6 lea edx, [eax + eax*2]
    { uint32_t r_=(uint32_t)(edx & edx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75dcf9 test edx, edx
    if ((zf||sf!=of)) goto L_75dd88; // 75dcfb jle 0x75dd88
    eax = *(uint32_t*)(fr+184); // 75dd01 mov eax, dword ptr [esp + 0x14]
    ecx = (eax+0xfffffffe); // 75dd05 lea ecx, [eax - 2]
    L_75dd08:;
    eax = *(uint32_t*)(uintptr_t)(ecx+0x2); // 75dd08 mov eax, dword ptr [ecx + 2]
    edx = eax; // 75dd0b mov edx, eax
    { uint32_t a_=edx, b_=0xc0000000u; uint32_t r_=(uint32_t)(a_ & b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75dd0d and edx, 0xc0000000
    esi = edx; // 75dd13 mov esi, edx
    esi = (uint32_t)~esi; // 75dd15 not esi
    { uint32_t a_=eax, b_=esi; uint32_t r_=(uint32_t)(a_ & b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75dd17 and eax, esi
    edx = (uint32_t)(edx >> 30); // 75dd19 shr edx, 0x1e
    { uint32_t r_=(uint32_t)(edi & edi); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75dd1c test edi, edi
    if (zf) goto L_75dd41; // 75dd1e je 0x75dd41
    esi = (esi & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(ecx+0xfffffffe)); // 75dd20 mov si, word ptr [ecx - 2]
    ebp = (ebp & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(ecx+0x6)); // 75dd24 mov bp, word ptr [ecx + 6]
    { uint16_t a_=(uint16_t)esi, b_=(uint16_t)ebp; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75dd28 cmp si, bp
    if ((!zf&&sf==of)) goto L_75dd3d; // 75dd2b jg 0x75dd3d
    if ((sf!=of)) goto L_75dd3d; // 75dd2d jl 0x75dd3d
    esi = (esi & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(ecx)); // 75dd2f mov si, word ptr [ecx]
    ebp = (ebp & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(ecx+0x8)); // 75dd32 mov bp, word ptr [ecx + 8]
    { uint16_t a_=(uint16_t)esi, b_=(uint16_t)ebp; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75dd36 cmp si, bp
    if ((!cf&&!zf)) goto L_75dd3d; // 75dd39 ja 0x75dd3d
    if (!cf) goto L_75dd47; // 75dd3b jae 0x75dd47
    L_75dd3d:;
    ebp = *(uint32_t*)(fr+180); // 75dd3d mov ebp, dword ptr [esp + 0x10]
    L_75dd41:;
    { uint32_t a_=*(uint32_t*)(fr+208); uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; *(uint32_t*)(fr+208) = r_; } // 75dd41 inc dword ptr [esp + 0x2c]
    goto L_75dd4b; // 75dd45 jmp 0x75dd4b
    L_75dd47:;
    ebp = *(uint32_t*)(fr+180); // 75dd47 mov ebp, dword ptr [esp + 0x10]
    L_75dd4b:;
    esi = (eax*8); // 75dd4b lea esi, [eax*8]
    eax = *(uint32_t*)(uintptr_t)(ebp+0x2c); // 75dd52 mov eax, dword ptr [ebp + 0x2c]
    ebp = *(uint32_t*)(fr+208); // 75dd55 mov ebp, dword ptr [esp + 0x2c]
    { uint32_t a_=eax, b_=esi; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75dd59 add eax, esi
    { uint32_t a_=edi; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75dd5b inc edi
    { uint32_t a_=ecx, b_=0x8u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75dd5c add ecx, 8
    *(uint16_t*)(uintptr_t)(eax+edx*2) = (uint16_t)ebp; // 75dd5f mov word ptr [eax + edx*2], bp
    edx = *(uint32_t*)(uintptr_t)(ebx+0x2c); // 75dd63 mov edx, dword ptr [ebx + 0x2c]
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(edx+esi+0x6)); // 75dd66 mov dx, word ptr [edx + esi + 6]
    *(uint16_t*)(uintptr_t)(eax+0x6) = (uint16_t)edx; // 75dd6b mov word ptr [eax + 6], dx
    edx = *(uint32_t*)(fr+260); // 75dd6f mov edx, dword ptr [esp + 0x60]
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(ecx)); // 75dd73 mov ax, word ptr [ecx]
    *(uint16_t*)(uintptr_t)(edx+ebp*2) = (uint16_t)eax; // 75dd76 mov word ptr [edx + ebp*2], ax
    eax = *(uint32_t*)(uintptr_t)(ebx+0x10); // 75dd7a mov eax, dword ptr [ebx + 0x10]
    ebp = *(uint32_t*)(fr+180); // 75dd7d mov ebp, dword ptr [esp + 0x10]
    eax = (eax+eax*2); // 75dd81 lea eax, [eax + eax*2]
    { uint32_t a_=edi, b_=eax; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75dd84 cmp edi, eax
    if ((sf!=of)) goto L_75dd08; // 75dd86 jl 0x75dd08
    L_75dd88:;
    ecx = *(uint32_t*)(fr+184); // 75dd88 mov ecx, dword ptr [esp + 0x14]
    edx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75dd8c mov edx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+160) = ecx; // 75dd92 push ecx
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(edx+0x138)))(*(uint32_t*)(fr+160)); // 75dd93 call dword ptr [edx + 0x138]
    eax = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75dd99 mov eax, dword ptr [ebx + 0x18]
    { uint32_t a_=esi, b_=esi; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75dd9f xor esi, esi
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_-1); of=a_==(uint32_t)0x80000000; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75dda1 dec eax
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75dda2 test eax, eax
    if ((zf||sf!=of)) goto L_75ddb8; // 75dda4 jle 0x75ddb8
    L_75dda6:;
    *(uint32_t*)(fr+160) = ebp; // 75dda6 push ebp
    eax = (uint32_t)(uintptr_t)T_GeometryAddMorphTarget(*(uint32_t*)(fr+160)); // 75dda7 call 0x74c4d0
    ecx = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75ddac mov ecx, dword ptr [ebx + 0x18]
    { uint32_t a_=esi; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75ddb2 inc esi
    { uint32_t a_=ecx; uint32_t r_=(uint32_t)(a_-1); of=a_==(uint32_t)0x80000000; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75ddb3 dec ecx
    { uint32_t a_=esi, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75ddb4 cmp esi, ecx
    if ((sf!=of)) goto L_75dda6; // 75ddb6 jl 0x75dda6
    L_75ddb8:;
    eax = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75ddb8 mov eax, dword ptr [ebx + 0x18]
    ecx = *(uint32_t*)(uintptr_t)(ebp+0x18); // 75ddbb mov ecx, dword ptr [ebp + 0x18]
    { uint32_t a_=ecx, b_=eax; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75ddbe cmp ecx, eax
    if (zf) goto L_75de07; // 75ddc0 je 0x75de07
    *(uint32_t*)(fr+160) = ebp; // 75ddc2 push ebp
    eax = (uint32_t)(uintptr_t)T_GeometryDestroy(*(uint32_t*)(fr+160)); // 75ddc3 call 0x74ccc0
    edx = *(uint32_t*)(fr+196); // 75ddc8 mov edx, dword ptr [esp + 0x24]
    eax = *(uint32_t*)(uintptr_t)(0xc97b24); // 75ddcc mov eax, dword ptr [0xc97b24]
    *(uint32_t*)(fr+156) = edx; // 75ddd1 push edx
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(eax+0x138)))(*(uint32_t*)(fr+156)); // 75ddd2 call dword ptr [eax + 0x138]
    ecx = *(uint32_t*)(fr+200); // 75ddd8 mov ecx, dword ptr [esp + 0x2c]
    edx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75dddc mov edx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+152) = ecx; // 75dde2 push ecx
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(edx+0x138)))(*(uint32_t*)(fr+152)); // 75dde3 call dword ptr [edx + 0x138]
    eax = *(uint32_t*)(fr+260); // 75dde9 mov eax, dword ptr [esp + 0x6c]
    ecx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75dded mov ecx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+148) = eax; // 75ddf3 push eax
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(ecx+0x138)))(*(uint32_t*)(fr+148)); // 75ddf4 call dword ptr [ecx + 0x138]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75ddfd xor eax, eax
    edi = *(uint32_t*)(fr+164); // 75ddff pop edi
    esi = *(uint32_t*)(fr+168); // 75de00 pop esi
    ebp = *(uint32_t*)(fr+172); // 75de01 pop ebp
    ebx = *(uint32_t*)(fr+176); // 75de02 pop ebx
    return reinterpret_cast<RpGeometry*>(static_cast<uintptr_t>(eax)); // 75de06 ret 
    L_75de07:;
    { uint32_t a_=edx, b_=edx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75de07 xor edx, edx
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75de09 test eax, eax
    if ((zf||sf!=of)) goto L_75de3c; // 75de0b jle 0x75de3c
    ecx = *(uint32_t*)(fr+196); // 75de0d mov ecx, dword ptr [esp + 0x20]
    esi = *(uint32_t*)(fr+200); // 75de11 mov esi, dword ptr [esp + 0x24]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75de15 xor eax, eax
    { uint32_t a_=esi, b_=ecx; uint32_t r_=(uint32_t)(a_ - b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75de17 sub esi, ecx
    L_75de19:;
    edi = *(uint32_t*)(uintptr_t)(ebx+0x5c); // 75de19 mov edi, dword ptr [ebx + 0x5c]
    { uint32_t a_=edx; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75de1c inc edx
    { uint32_t a_=ecx, b_=0x4u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75de1d add ecx, 4
    edi = *(uint32_t*)(uintptr_t)(edi+eax+0x14); // 75de20 mov edi, dword ptr [edi + eax + 0x14]
    { uint32_t a_=eax, b_=0x1cu; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75de24 add eax, 0x1c
    *(uint32_t*)(uintptr_t)(esi+ecx+0xfffffffc) = edi; // 75de27 mov dword ptr [esi + ecx - 4], edi
    edi = *(uint32_t*)(uintptr_t)(ebp+0x5c); // 75de2b mov edi, dword ptr [ebp + 0x5c]
    edi = *(uint32_t*)(uintptr_t)(eax+edi+0xfffffff8); // 75de2e mov edi, dword ptr [eax + edi - 8]
    *(uint32_t*)(uintptr_t)(ecx+0xfffffffc) = edi; // 75de32 mov dword ptr [ecx - 4], edi
    edi = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75de35 mov edi, dword ptr [ebx + 0x18]
    { uint32_t a_=edx, b_=edi; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75de38 cmp edx, edi
    if ((sf!=of)) goto L_75de19; // 75de3a jl 0x75de19
    L_75de3c:;
    { uint32_t a_=*(uint32_t*)(fr+228), b_=0x10u; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75de3c cmp dword ptr [esp + 0x40], 0x10
    if (!zf) goto L_75de85; // 75de41 jne 0x75de85
    eax = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75de43 mov eax, dword ptr [ebx + 0x18]
    { uint32_t a_=edx, b_=edx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75de46 xor edx, edx
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75de48 test eax, eax
    *(uint32_t*)(fr+224) = 0x1u; // 75de4a mov dword ptr [esp + 0x3c], 1
    if ((zf||sf!=of)) goto L_75de8d; // 75de52 jle 0x75de8d
    ecx = *(uint32_t*)(fr+204); // 75de54 mov ecx, dword ptr [esp + 0x28]
    esi = *(uint32_t*)(fr+188); // 75de58 mov esi, dword ptr [esp + 0x18]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75de5c xor eax, eax
    { uint32_t a_=esi, b_=ecx; uint32_t r_=(uint32_t)(a_ - b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75de5e sub esi, ecx
    L_75de60:;
    edi = *(uint32_t*)(uintptr_t)(ebx+0x5c); // 75de60 mov edi, dword ptr [ebx + 0x5c]
    { uint32_t a_=edx; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75de63 inc edx
    { uint32_t a_=ecx, b_=0x4u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75de64 add ecx, 4
    edi = *(uint32_t*)(uintptr_t)(edi+eax+0x18); // 75de67 mov edi, dword ptr [edi + eax + 0x18]
    { uint32_t a_=eax, b_=0x1cu; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75de6b add eax, 0x1c
    *(uint32_t*)(uintptr_t)(esi+ecx+0xfffffffc) = edi; // 75de6e mov dword ptr [esi + ecx - 4], edi
    edi = *(uint32_t*)(uintptr_t)(ebp+0x5c); // 75de72 mov edi, dword ptr [ebp + 0x5c]
    edi = *(uint32_t*)(uintptr_t)(eax+edi+0xfffffffc); // 75de75 mov edi, dword ptr [eax + edi - 4]
    *(uint32_t*)(uintptr_t)(ecx+0xfffffffc) = edi; // 75de79 mov dword ptr [ecx - 4], edi
    edi = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75de7c mov edi, dword ptr [ebx + 0x18]
    { uint32_t a_=edx, b_=edi; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75de7f cmp edx, edi
    if ((sf!=of)) goto L_75de60; // 75de81 jl 0x75de60
    goto L_75de8d; // 75de83 jmp 0x75de8d
    L_75de85:;
    *(uint32_t*)(fr+224) = 0x0u; // 75de85 mov dword ptr [esp + 0x3c], 0
    L_75de8d:;
    edx = *(uint32_t*)(fr+212); // 75de8d mov edx, dword ptr [esp + 0x30]
    { uint32_t a_=edx, b_=0x8u; uint32_t r_=(uint32_t)(a_ & b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75de91 and edx, 8
    { uint8_t a_=(uint8_t)edx, b_=0x8u; uint8_t r_=(uint8_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>7)&1; zf=r_==0; sf=(r_>>7)&1; } // 75de94 cmp dl, 8
    if (!zf) goto L_75deb1; // 75de97 jne 0x75deb1
    eax = *(uint32_t*)(uintptr_t)(ebx+0x30); // 75de99 mov eax, dword ptr [ebx + 0x30]
    ecx = *(uint32_t*)(uintptr_t)(ebp+0x30); // 75de9c mov ecx, dword ptr [ebp + 0x30]
    *(uint32_t*)(fr+212) = 0x1u; // 75de9f mov dword ptr [esp + 0x30], 1
    *(uint32_t*)(fr+216) = eax; // 75dea7 mov dword ptr [esp + 0x34], eax
    *(uint32_t*)(fr+220) = ecx; // 75deab mov dword ptr [esp + 0x38], ecx
    goto L_75deb9; // 75deaf jmp 0x75deb9
    L_75deb1:;
    *(uint32_t*)(fr+212) = 0x0u; // 75deb1 mov dword ptr [esp + 0x30], 0
    L_75deb9:;
    eax = *(uint32_t*)(fr+192); // 75deb9 mov eax, dword ptr [esp + 0x1c]
    { uint32_t a_=ecx, b_=ecx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75debd xor ecx, ecx
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75debf test eax, eax
    *(uint32_t*)(fr+184) = ecx; // 75dec1 mov dword ptr [esp + 0x14], ecx
    if ((zf||sf!=of)) goto L_75dfa8; // 75dec5 jle 0x75dfa8
    *(uint32_t*)(fr+208) = ecx; // 75decb mov dword ptr [esp + 0x2c], ecx
    L_75decf:;
    edx = *(uint32_t*)(fr+260); // 75decf mov edx, dword ptr [esp + 0x60]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75ded3 xor eax, eax
    eax = (eax & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(edx+ecx*2)); // 75ded5 mov ax, word ptr [edx + ecx*2]
    edx = *(uint32_t*)(fr+212); // 75ded9 mov edx, dword ptr [esp + 0x30]
    { uint32_t r_=(uint32_t)(edx & edx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75dedd test edx, edx
    if (zf) goto L_75deef; // 75dedf je 0x75deef
    edx = *(uint32_t*)(fr+216); // 75dee1 mov edx, dword ptr [esp + 0x34]
    esi = *(uint32_t*)(fr+220); // 75dee5 mov esi, dword ptr [esp + 0x38]
    edx = *(uint32_t*)(uintptr_t)(edx+eax*4); // 75dee9 mov edx, dword ptr [edx + eax*4]
    *(uint32_t*)(uintptr_t)(esi+ecx*4) = edx; // 75deec mov dword ptr [esi + ecx*4], edx
    L_75deef:;
    edx = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75deef mov edx, dword ptr [ebx + 0x18]
    *(uint32_t*)(fr+228) = 0x0u; // 75def2 mov dword ptr [esp + 0x40], 0
    { uint32_t r_=(uint32_t)(edx & edx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75defa test edx, edx
    if ((zf||sf!=of)) goto L_75df8c; // 75defc jle 0x75df8c
    esi = *(uint32_t*)(fr+188); // 75df02 mov esi, dword ptr [esp + 0x18]
    edx = *(uint32_t*)(fr+200); // 75df06 mov edx, dword ptr [esp + 0x24]
    ecx = (eax+eax*2); // 75df0a lea ecx, [eax + eax*2]
    eax = *(uint32_t*)(fr+196); // 75df0d mov eax, dword ptr [esp + 0x20]
    { uint32_t a_=esi, b_=eax; uint32_t r_=(uint32_t)(a_ - b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75df11 sub esi, eax
    { uint32_t a_=edx, b_=eax; uint32_t r_=(uint32_t)(a_ - b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75df13 sub edx, eax
    *(uint32_t*)(fr+232) = esi; // 75df15 mov dword ptr [esp + 0x44], esi
    esi = *(uint32_t*)(fr+204); // 75df19 mov esi, dword ptr [esp + 0x28]
    ecx = (uint32_t)(ecx << 2); // 75df1d shl ecx, 2
    { uint32_t a_=esi, b_=eax; uint32_t r_=(uint32_t)(a_ - b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75df20 sub esi, eax
    *(uint32_t*)(fr+236) = esi; // 75df22 mov dword ptr [esp + 0x48], esi
    L_75df26:;
    esi = *(uint32_t*)(uintptr_t)(eax+edx); // 75df26 mov esi, dword ptr [eax + edx]
    edi = *(uint32_t*)(uintptr_t)(eax); // 75df29 mov edi, dword ptr [eax]
    ebp = *(uint32_t*)(fr+208); // 75df2b mov ebp, dword ptr [esp + 0x2c]
    { uint32_t a_=esi, b_=ecx; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75df2f add esi, ecx
    { uint32_t a_=edi, b_=ebp; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75df31 add edi, ebp
    ebp = *(uint32_t*)(uintptr_t)(esi); // 75df33 mov ebp, dword ptr [esi]
    *(uint32_t*)(uintptr_t)(edi) = ebp; // 75df35 mov dword ptr [edi], ebp
    ebp = *(uint32_t*)(uintptr_t)(esi+0x4); // 75df37 mov ebp, dword ptr [esi + 4]
    *(uint32_t*)(uintptr_t)(edi+0x4) = ebp; // 75df3a mov dword ptr [edi + 4], ebp
    esi = *(uint32_t*)(uintptr_t)(esi+0x8); // 75df3d mov esi, dword ptr [esi + 8]
    *(uint32_t*)(uintptr_t)(edi+0x8) = esi; // 75df40 mov dword ptr [edi + 8], esi
    esi = *(uint32_t*)(fr+224); // 75df43 mov esi, dword ptr [esp + 0x3c]
    { uint32_t r_=(uint32_t)(esi & esi); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75df47 test esi, esi
    if (zf) goto L_75df71; // 75df49 je 0x75df71
    esi = *(uint32_t*)(fr+232); // 75df4b mov esi, dword ptr [esp + 0x44]
    edi = *(uint32_t*)(fr+236); // 75df4f mov edi, dword ptr [esp + 0x48]
    ebp = *(uint32_t*)(fr+208); // 75df53 mov ebp, dword ptr [esp + 0x2c]
    esi = *(uint32_t*)(uintptr_t)(esi+eax); // 75df57 mov esi, dword ptr [esi + eax]
    edi = *(uint32_t*)(uintptr_t)(edi+eax); // 75df5a mov edi, dword ptr [edi + eax]
    { uint32_t a_=esi, b_=ecx; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75df5d add esi, ecx
    { uint32_t a_=edi, b_=ebp; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75df5f add edi, ebp
    ebp = *(uint32_t*)(uintptr_t)(esi); // 75df61 mov ebp, dword ptr [esi]
    *(uint32_t*)(uintptr_t)(edi) = ebp; // 75df63 mov dword ptr [edi], ebp
    ebp = *(uint32_t*)(uintptr_t)(esi+0x4); // 75df65 mov ebp, dword ptr [esi + 4]
    *(uint32_t*)(uintptr_t)(edi+0x4) = ebp; // 75df68 mov dword ptr [edi + 4], ebp
    esi = *(uint32_t*)(uintptr_t)(esi+0x8); // 75df6b mov esi, dword ptr [esi + 8]
    *(uint32_t*)(uintptr_t)(edi+0x8) = esi; // 75df6e mov dword ptr [edi + 8], esi
    L_75df71:;
    esi = *(uint32_t*)(fr+228); // 75df71 mov esi, dword ptr [esp + 0x40]
    edi = *(uint32_t*)(uintptr_t)(ebx+0x18); // 75df75 mov edi, dword ptr [ebx + 0x18]
    { uint32_t a_=esi; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75df78 inc esi
    { uint32_t a_=eax, b_=0x4u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75df79 add eax, 4
    { uint32_t a_=esi, b_=edi; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75df7c cmp esi, edi
    *(uint32_t*)(fr+228) = esi; // 75df7e mov dword ptr [esp + 0x40], esi
    if ((sf!=of)) goto L_75df26; // 75df82 jl 0x75df26
    ebp = *(uint32_t*)(fr+180); // 75df84 mov ebp, dword ptr [esp + 0x10]
    ecx = *(uint32_t*)(fr+184); // 75df88 mov ecx, dword ptr [esp + 0x14]
    L_75df8c:;
    edx = *(uint32_t*)(fr+208); // 75df8c mov edx, dword ptr [esp + 0x2c]
    eax = *(uint32_t*)(fr+192); // 75df90 mov eax, dword ptr [esp + 0x1c]
    { uint32_t a_=ecx; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75df94 inc ecx
    { uint32_t a_=edx, b_=0xcu; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75df95 add edx, 0xc
    { uint32_t a_=ecx, b_=eax; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75df98 cmp ecx, eax
    *(uint32_t*)(fr+184) = ecx; // 75df9a mov dword ptr [esp + 0x14], ecx
    *(uint32_t*)(fr+208) = edx; // 75df9e mov dword ptr [esp + 0x2c], edx
    if ((sf!=of)) goto L_75decf; // 75dfa2 jl 0x75decf
    L_75dfa8:;
    eax = *(uint32_t*)(uintptr_t)(ebx+0x1c); // 75dfa8 mov eax, dword ptr [ebx + 0x1c]
    *(uint32_t*)(fr+184) = 0x0u; // 75dfab mov dword ptr [esp + 0x14], 0
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75dfb3 test eax, eax
    if ((zf||sf!=of)) goto L_75e015; // 75dfb5 jle 0x75e015
    edi = ebx; // 75dfb7 mov edi, ebx
    esi = (ebp+0x34); // 75dfb9 lea esi, [ebp + 0x34]
    { uint32_t a_=edi, b_=ebp; uint32_t r_=(uint32_t)(a_ - b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75dfbc sub edi, ebp
    *(uint32_t*)(fr+228) = esi; // 75dfbe mov dword ptr [esp + 0x40], esi
    *(uint32_t*)(fr+236) = edi; // 75dfc2 mov dword ptr [esp + 0x48], edi
    L_75dfc6:;
    edx = *(uint32_t*)(uintptr_t)(edi+esi); // 75dfc6 mov edx, dword ptr [edi + esi]
    ecx = *(uint32_t*)(uintptr_t)(esi); // 75dfc9 mov ecx, dword ptr [esi]
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75dfcb xor eax, eax
    { uint32_t a_=*(uint32_t*)(fr+192), b_=eax; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75dfcd cmp dword ptr [esp + 0x1c], eax
    if ((zf||sf!=of)) goto L_75dffe; // 75dfd1 jle 0x75dffe
    L_75dfd3:;
    edi = *(uint32_t*)(fr+260); // 75dfd3 mov edi, dword ptr [esp + 0x60]
    { uint32_t a_=esi, b_=esi; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75dfd7 xor esi, esi
    { uint32_t a_=ecx, b_=0x8u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75dfd9 add ecx, 8
    esi = (esi & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(edi+eax*2)); // 75dfdc mov si, word ptr [edi + eax*2]
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75dfe0 inc eax
    edi = *(uint32_t*)(uintptr_t)(edx+esi*8); // 75dfe1 mov edi, dword ptr [edx + esi*8]
    *(uint32_t*)(uintptr_t)(ecx+0xfffffff8) = edi; // 75dfe4 mov dword ptr [ecx - 8], edi
    esi = *(uint32_t*)(uintptr_t)(edx+esi*8+0x4); // 75dfe7 mov esi, dword ptr [edx + esi*8 + 4]
    *(uint32_t*)(uintptr_t)(ecx+0xfffffffc) = esi; // 75dfeb mov dword ptr [ecx - 4], esi
    esi = *(uint32_t*)(fr+192); // 75dfee mov esi, dword ptr [esp + 0x1c]
    { uint32_t a_=eax, b_=esi; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75dff2 cmp eax, esi
    if ((sf!=of)) goto L_75dfd3; // 75dff4 jl 0x75dfd3
    esi = *(uint32_t*)(fr+228); // 75dff6 mov esi, dword ptr [esp + 0x40]
    edi = *(uint32_t*)(fr+236); // 75dffa mov edi, dword ptr [esp + 0x48]
    L_75dffe:;
    eax = *(uint32_t*)(fr+184); // 75dffe mov eax, dword ptr [esp + 0x14]
    ecx = *(uint32_t*)(uintptr_t)(ebx+0x1c); // 75e002 mov ecx, dword ptr [ebx + 0x1c]
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75e005 inc eax
    { uint32_t a_=esi, b_=0x4u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75e006 add esi, 4
    { uint32_t a_=eax, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75e009 cmp eax, ecx
    *(uint32_t*)(fr+184) = eax; // 75e00b mov dword ptr [esp + 0x14], eax
    *(uint32_t*)(fr+228) = esi; // 75e00f mov dword ptr [esp + 0x40], esi
    if ((sf!=of)) goto L_75dfc6; // 75e013 jl 0x75dfc6
    L_75e015:;
    eax = *(uint32_t*)(uintptr_t)(ebp+0x18); // 75e015 mov eax, dword ptr [ebp + 0x18]
    { uint32_t a_=edi, b_=edi; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75e018 xor edi, edi
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75e01a test eax, eax
    if ((zf||sf!=of)) goto L_75e06a; // 75e01c jle 0x75e06a
    *(uint32_t*)(fr+208) = edi; // 75e01e mov dword ptr [esp + 0x2c], edi
    L_75e022:;
    esi = *(uint32_t*)(uintptr_t)(ebp+0x5c); // 75e022 mov esi, dword ptr [ebp + 0x5c]
    eax = *(uint32_t*)(fr+208); // 75e025 mov eax, dword ptr [esp + 0x2c]
    { uint32_t a_=esi, b_=eax; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75e029 add esi, eax
    eax = (uint32_t)(uintptr_t)(fr+240); // 75e02b lea eax, [esp + 0x4c]
    *(uint32_t*)(fr+160) = eax; // 75e02f push eax
    *(uint32_t*)(fr+156) = esi; // 75e030 push esi
    eax = (uint32_t)(uintptr_t)T_MorphTargetCalcBoundingSphere(*(uint32_t*)(fr+156), *(uint32_t*)(fr+160)); // 75e031 call 0x74c200
    ecx = *(uint32_t*)(fr+240); // 75e036 mov ecx, dword ptr [esp + 0x54]
    { uint32_t a_=esi, b_=0x4u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75e03a add esi, 4
    { uint32_t a_=edi; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75e040 inc edi
    *(uint32_t*)(uintptr_t)(esi) = ecx; // 75e041 mov dword ptr [esi], ecx
    edx = *(uint32_t*)(fr+244); // 75e043 mov edx, dword ptr [esp + 0x50]
    *(uint32_t*)(uintptr_t)(esi+0x4) = edx; // 75e047 mov dword ptr [esi + 4], edx
    eax = *(uint32_t*)(fr+248); // 75e04a mov eax, dword ptr [esp + 0x54]
    *(uint32_t*)(uintptr_t)(esi+0x8) = eax; // 75e04e mov dword ptr [esi + 8], eax
    ecx = *(uint32_t*)(fr+252); // 75e051 mov ecx, dword ptr [esp + 0x58]
    *(uint32_t*)(uintptr_t)(esi+0xc) = ecx; // 75e055 mov dword ptr [esi + 0xc], ecx
    ecx = *(uint32_t*)(fr+208); // 75e058 mov ecx, dword ptr [esp + 0x2c]
    eax = *(uint32_t*)(uintptr_t)(ebp+0x18); // 75e05c mov eax, dword ptr [ebp + 0x18]
    { uint32_t a_=ecx, b_=0x1cu; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75e05f add ecx, 0x1c
    { uint32_t a_=edi, b_=eax; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75e062 cmp edi, eax
    *(uint32_t*)(fr+208) = ecx; // 75e064 mov dword ptr [esp + 0x2c], ecx
    if ((sf!=of)) goto L_75e022; // 75e068 jl 0x75e022
    L_75e06a:;
    edx = *(uint32_t*)(fr+196); // 75e06a mov edx, dword ptr [esp + 0x20]
    eax = *(uint32_t*)(uintptr_t)(0xc97b24); // 75e06e mov eax, dword ptr [0xc97b24]
    *(uint32_t*)(fr+160) = edx; // 75e073 push edx
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(eax+0x138)))(*(uint32_t*)(fr+160)); // 75e074 call dword ptr [eax + 0x138]
    ecx = *(uint32_t*)(fr+200); // 75e07a mov ecx, dword ptr [esp + 0x28]
    edx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75e07e mov edx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+156) = ecx; // 75e084 push ecx
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(edx+0x138)))(*(uint32_t*)(fr+156)); // 75e085 call dword ptr [edx + 0x138]
    eax = *(uint32_t*)(fr+204); // 75e08b mov eax, dword ptr [esp + 0x30]
    ecx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75e08f mov ecx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+152) = eax; // 75e095 push eax
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(ecx+0x138)))(*(uint32_t*)(fr+152)); // 75e096 call dword ptr [ecx + 0x138]
    edx = *(uint32_t*)(fr+188); // 75e09c mov edx, dword ptr [esp + 0x24]
    eax = *(uint32_t*)(uintptr_t)(0xc97b24); // 75e0a0 mov eax, dword ptr [0xc97b24]
    *(uint32_t*)(fr+148) = edx; // 75e0a5 push edx
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(eax+0x138)))(*(uint32_t*)(fr+148)); // 75e0a6 call dword ptr [eax + 0x138]
    ecx = (ebx+0x20); // 75e0ac lea ecx, [ebx + 0x20]
    edx = (ebp+0x20); // 75e0af lea edx, [ebp + 0x20]
    *(uint32_t*)(fr+144) = ecx; // 75e0b2 push ecx
    *(uint32_t*)(fr+140) = edx; // 75e0b3 push edx
    eax = (uint32_t)(uintptr_t)T_MaterialListCopy(*(uint32_t*)(fr+140), *(uint32_t*)(fr+144)); // 75e0b4 call 0x74e1f0
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75e0bc test eax, eax
    if (!zf) goto L_75e0e4; // 75e0be jne 0x75e0e4
    eax = *(uint32_t*)(fr+260); // 75e0c0 mov eax, dword ptr [esp + 0x60]
    ecx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75e0c4 mov ecx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+160) = eax; // 75e0ca push eax
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(ecx+0x138)))(*(uint32_t*)(fr+160)); // 75e0cb call dword ptr [ecx + 0x138]
    *(uint32_t*)(fr+156) = ebp; // 75e0d1 push ebp
    eax = (uint32_t)(uintptr_t)T_GeometryDestroy(*(uint32_t*)(fr+156)); // 75e0d2 call 0x74ccc0
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75e0da xor eax, eax
    edi = *(uint32_t*)(fr+164); // 75e0dc pop edi
    esi = *(uint32_t*)(fr+168); // 75e0dd pop esi
    ebp = *(uint32_t*)(fr+172); // 75e0de pop ebp
    ebx = *(uint32_t*)(fr+176); // 75e0df pop ebx
    return reinterpret_cast<RpGeometry*>(static_cast<uintptr_t>(eax)); // 75e0e3 ret 
    L_75e0e4:;
    eax = (uint32_t)(uintptr_t)T_GeometryPluginFirst(); // 75e0e4 call 0x74bfd0
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75e0e9 test eax, eax
    if (zf) goto L_75e111; // 75e0eb je 0x75e111
    L_75e0ed:;
    ecx = *(uint32_t*)(uintptr_t)(eax+0x4); // 75e0ed mov ecx, dword ptr [eax + 4]
    { uint32_t r_=(uint32_t)(ecx & ecx); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75e0f0 test ecx, ecx
    if ((zf||sf!=of)) goto L_75e10a; // 75e0f2 jle 0x75e10a
    edx = *(uint32_t*)(uintptr_t)(eax); // 75e0f4 mov edx, dword ptr [eax]
    esi = (edx+ebx); // 75e0f6 lea esi, [edx + ebx]
    edi = (edx+ebp); // 75e0f9 lea edi, [edx + ebp]
    edx = ecx; // 75e0fc mov edx, ecx
    ecx = (uint32_t)(ecx >> 2); // 75e0fe shr ecx, 2
    while (ecx) { *(uint32_t*)(uintptr_t)edi = *(uint32_t*)(uintptr_t)esi; esi += 4; edi += 4; --ecx; } // 75e101 rep movsd dword ptr es:[edi], dword ptr [esi] dword ptr es:[edi], dword ptr [esi]
    ecx = edx; // 75e103 mov ecx, edx
    { uint32_t a_=ecx, b_=0x3u; uint32_t r_=(uint32_t)(a_ & b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75e105 and ecx, 3
    while (ecx) { *(uint8_t*)(uintptr_t)edi = *(uint8_t*)(uintptr_t)esi; ++esi; ++edi; --ecx; } // 75e108 rep movsb byte ptr es:[edi], byte ptr [esi] byte ptr es:[edi], byte ptr [esi]
    L_75e10a:;
    eax = *(uint32_t*)(uintptr_t)(eax+0x30); // 75e10a mov eax, dword ptr [eax + 0x30]
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75e10d test eax, eax
    if (!zf) goto L_75e0ed; // 75e10f jne 0x75e0ed
    L_75e111:;
    eax = *(uint32_t*)(fr+264); // 75e111 mov eax, dword ptr [esp + 0x64]
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75e115 test eax, eax
    if (zf) goto L_75e12a; // 75e117 je 0x75e12a
    ecx = *(uint32_t*)(fr+192); // 75e119 mov ecx, dword ptr [esp + 0x1c]
    edx = *(uint32_t*)(fr+260); // 75e11d mov edx, dword ptr [esp + 0x60]
    *(uint32_t*)(fr+160) = ecx; // 75e121 push ecx
    *(uint32_t*)(fr+156) = edx; // 75e122 push edx
    *(uint32_t*)(fr+152) = ebp; // 75e123 push ebp
    *(uint32_t*)(fr+148) = ebx; // 75e124 push ebx
    eax = ((uint32_t(__cdecl*)(uint32_t,uint32_t,uint32_t,uint32_t))(eax))(*(uint32_t*)(fr+148), *(uint32_t*)(fr+152), *(uint32_t*)(fr+156), *(uint32_t*)(fr+160)); // 75e125 call eax
    L_75e12a:;
    eax = *(uint32_t*)(fr+260); // 75e12a mov eax, dword ptr [esp + 0x60]
    ecx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75e12e mov ecx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+160) = eax; // 75e134 push eax
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(ecx+0x138)))(*(uint32_t*)(fr+160)); // 75e135 call dword ptr [ecx + 0x138]
    *(uint32_t*)(fr+156) = ebp; // 75e13b push ebp
    eax = (uint32_t)(uintptr_t)T_GeometryUnlock(*(uint32_t*)(fr+156)); // 75e13c call 0x74c800
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75e144 test eax, eax
    if (zf) goto L_75e19f; // 75e146 je 0x75e19f
    edi = *(uint32_t*)(fr+164); // 75e148 pop edi
    eax = ebp; // 75e149 mov eax, ebp
    esi = *(uint32_t*)(fr+168); // 75e14b pop esi
    ebp = *(uint32_t*)(fr+172); // 75e14c pop ebp
    ebx = *(uint32_t*)(fr+176); // 75e14d pop ebx
    return reinterpret_cast<RpGeometry*>(static_cast<uintptr_t>(eax)); // 75e151 ret 
    L_75e152:;
    edx = *(uint32_t*)(fr+184); // 75e152 mov edx, dword ptr [esp + 0x14]
    eax = *(uint32_t*)(uintptr_t)(0xc97b24); // 75e156 mov eax, dword ptr [0xc97b24]
    *(uint32_t*)(fr+160) = edx; // 75e15b push edx
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(eax+0x138)))(*(uint32_t*)(fr+160)); // 75e15c call dword ptr [eax + 0x138]
    ecx = *(uint32_t*)(fr+196); // 75e162 mov ecx, dword ptr [esp + 0x24]
    edx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75e166 mov edx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+156) = ecx; // 75e16c push ecx
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(edx+0x138)))(*(uint32_t*)(fr+156)); // 75e16d call dword ptr [edx + 0x138]
    eax = *(uint32_t*)(fr+200); // 75e173 mov eax, dword ptr [esp + 0x2c]
    ecx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75e177 mov ecx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+152) = eax; // 75e17d push eax
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(ecx+0x138)))(*(uint32_t*)(fr+152)); // 75e17e call dword ptr [ecx + 0x138]
    { uint32_t r_=(uint32_t)(edi & edi); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75e187 test edi, edi
    if (zf) goto L_75e19b; // 75e189 je 0x75e19b
    edx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75e18b mov edx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+160) = edi; // 75e191 push edi
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(edx+0x138)))(*(uint32_t*)(fr+160)); // 75e192 call dword ptr [edx + 0x138]
    L_75e19b:;
    { uint32_t r_=(uint32_t)(ebp & ebp); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75e19b test ebp, ebp
    if (zf) goto L_75e218; // 75e19d je 0x75e218
    L_75e19f:;
    *(uint32_t*)(fr+160) = ebp; // 75e19f push ebp
    eax = (uint32_t)(uintptr_t)T_GeometryDestroy(*(uint32_t*)(fr+160)); // 75e1a0 call 0x74ccc0
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75e1a8 xor eax, eax
    edi = *(uint32_t*)(fr+164); // 75e1aa pop edi
    esi = *(uint32_t*)(fr+168); // 75e1ab pop esi
    ebp = *(uint32_t*)(fr+172); // 75e1ac pop ebp
    ebx = *(uint32_t*)(fr+176); // 75e1ad pop ebx
    return reinterpret_cast<RpGeometry*>(static_cast<uintptr_t>(eax)); // 75e1b1 ret 
    L_75e1b2:;
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75e1b2 test eax, eax
    if (zf) goto L_75e1c5; // 75e1b4 je 0x75e1c5
    *(uint32_t*)(fr+160) = eax; // 75e1b6 push eax
    eax = *(uint32_t*)(uintptr_t)(0xc97b24); // 75e1b7 mov eax, dword ptr [0xc97b24]
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(eax+0x138)))(*(uint32_t*)(fr+160)); // 75e1bc call dword ptr [eax + 0x138]
    L_75e1c5:;
    { uint32_t r_=(uint32_t)(ebp & ebp); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75e1c5 test ebp, ebp
    if (zf) goto L_75e1d9; // 75e1c7 je 0x75e1d9
    ecx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75e1c9 mov ecx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+160) = ebp; // 75e1cf push ebp
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(ecx+0x138)))(*(uint32_t*)(fr+160)); // 75e1d0 call dword ptr [ecx + 0x138]
    L_75e1d9:;
    { uint32_t r_=(uint32_t)(esi & esi); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75e1d9 test esi, esi
    if (zf) goto L_75e1ed; // 75e1db je 0x75e1ed
    edx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75e1dd mov edx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+160) = esi; // 75e1e3 push esi
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(edx+0x138)))(*(uint32_t*)(fr+160)); // 75e1e4 call dword ptr [edx + 0x138]
    L_75e1ed:;
    eax = *(uint32_t*)(fr+204); // 75e1ed mov eax, dword ptr [esp + 0x28]
    { uint32_t r_=(uint32_t)(eax & eax); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75e1f1 test eax, eax
    if (zf) goto L_75e204; // 75e1f3 je 0x75e204
    *(uint32_t*)(fr+160) = eax; // 75e1f5 push eax
    eax = *(uint32_t*)(uintptr_t)(0xc97b24); // 75e1f6 mov eax, dword ptr [0xc97b24]
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(eax+0x138)))(*(uint32_t*)(fr+160)); // 75e1fb call dword ptr [eax + 0x138]
    L_75e204:;
    { uint32_t r_=(uint32_t)(edi & edi); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75e204 test edi, edi
    if (zf) goto L_75e218; // 75e206 je 0x75e218
    ecx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75e208 mov ecx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+160) = edi; // 75e20e push edi
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(ecx+0x138)))(*(uint32_t*)(fr+160)); // 75e20f call dword ptr [ecx + 0x138]
    L_75e218:;
    edi = *(uint32_t*)(fr+164); // 75e218 pop edi
    esi = *(uint32_t*)(fr+168); // 75e219 pop esi
    ebp = *(uint32_t*)(fr+172); // 75e21a pop ebp
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75e21b xor eax, eax
    ebx = *(uint32_t*)(fr+176); // 75e21d pop ebx
    return reinterpret_cast<RpGeometry*>(static_cast<uintptr_t>(eax)); // 75e221 ret 
}

// 0x75C380
RpMeshHeader* RpBuildMeshGenerateTriStrip(RpBuildMesh* buildMesh, void* data) {
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0, esi = 0, edi = 0, ebp = 0;
    bool cf = false, zf = false, sf = false, of = false;
    alignas(4) uint8_t fr[0x180] = {};
    *reinterpret_cast<uint32_t*>(fr + 260) = static_cast<uint32_t>((uintptr_t)(buildMesh));
    *reinterpret_cast<uint32_t*>(fr + 264) = static_cast<uint32_t>((uintptr_t)(data));
    (void)cf; (void)zf; (void)sf; (void)of;
    *(uint32_t*)(fr+228) = ebx; // 75c383 push ebx
    *(uint32_t*)(fr+224) = ebp; // 75c384 push ebp
    ecx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75c385 mov ecx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+220) = esi; // 75c38b push esi
    esi = *(uint32_t*)(fr+260); // 75c38c mov esi, dword ptr [esp + 0x28]
    *(uint32_t*)(fr+216) = edi; // 75c390 push edi
    { uint32_t a_=ebp, b_=ebp; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ebp = r_; } // 75c391 xor ebp, ebp
    *(uint32_t*)(fr+212) = 0x10502u; // 75c393 push 0x10502
    eax = *(uint32_t*)(uintptr_t)(esi+0x4); // 75c398 mov eax, dword ptr [esi + 4]
    { uint32_t a_=edi, b_=edi; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75c39b xor edi, edi
    eax = (uint32_t)(eax << 2); // 75c39d shl eax, 2
    *(uint32_t*)(fr+208) = eax; // 75c3a0 push eax
    *(uint32_t*)(fr+232) = ebp; // 75c3a1 mov dword ptr [esp + 0x18], ebp
    eax = ((uint32_t(__cdecl*)(uint32_t,uint32_t))(*(uint32_t*)(uintptr_t)(ecx+0x134)))(*(uint32_t*)(fr+208), *(uint32_t*)(fr+212)); // 75c3a5 call dword ptr [ecx + 0x134]
    *(uint32_t*)(fr+260) = eax; // 75c3ab mov dword ptr [esp + 0x34], eax
    eax = *(uint32_t*)(uintptr_t)(esi+0x4); // 75c3af mov eax, dword ptr [esi + 4]
    { uint32_t a_=eax, b_=edi; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c3b5 cmp eax, edi
    *(uint32_t*)(fr+236) = edi; // 75c3b7 mov dword ptr [esp + 0x14], edi
    if ((cf||zf)) goto L_75c3fc; // 75c3bb jbe 0x75c3fc
    L_75c3bd:;
    eax = *(uint32_t*)(uintptr_t)(esi+0x8); // 75c3bd mov eax, dword ptr [esi + 8]
    { uint32_t a_=eax, b_=edi; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c3c0 add eax, edi
    edx = (edx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(eax)); // 75c3c2 mov dx, word ptr [eax]
    ecx = (ecx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(eax+0x2)); // 75c3c5 mov cx, word ptr [eax + 2]
    { uint16_t a_=(uint16_t)edx, b_=(uint16_t)ecx; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75c3c9 cmp dx, cx
    if (zf) goto L_75c3e5; // 75c3cc je 0x75c3e5
    ebx = (ebx & 0xffff0000u) | (uint16_t)(*(uint16_t*)(uintptr_t)(eax+0x4)); // 75c3ce mov bx, word ptr [eax + 4]
    { uint16_t a_=(uint16_t)ecx, b_=(uint16_t)ebx; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75c3d2 cmp cx, bx
    if (zf) goto L_75c3e5; // 75c3d5 je 0x75c3e5
    { uint16_t a_=(uint16_t)ebx, b_=(uint16_t)edx; uint16_t r_=(uint16_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>15)&1; zf=r_==0; sf=(r_>>15)&1; } // 75c3d7 cmp bx, dx
    if (zf) goto L_75c3e5; // 75c3da je 0x75c3e5
    edx = *(uint32_t*)(fr+260); // 75c3dc mov edx, dword ptr [esp + 0x2c]
    { uint32_t a_=ebp; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; ebp = r_; } // 75c3e0 inc ebp
    *(uint32_t*)(uintptr_t)(edx+ebp*4+0xfffffffc) = eax; // 75c3e1 mov dword ptr [edx + ebp*4 - 4], eax
    L_75c3e5:;
    eax = *(uint32_t*)(fr+236); // 75c3e5 mov eax, dword ptr [esp + 0x14]
    ecx = *(uint32_t*)(uintptr_t)(esi+0x4); // 75c3e9 mov ecx, dword ptr [esi + 4]
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c3ec inc eax
    { uint32_t a_=edi, b_=0x14u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75c3ed add edi, 0x14
    { uint32_t a_=eax, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c3f0 cmp eax, ecx
    *(uint32_t*)(fr+236) = eax; // 75c3f2 mov dword ptr [esp + 0x14], eax
    if (cf) goto L_75c3bd; // 75c3f6 jb 0x75c3bd
    *(uint32_t*)(fr+232) = ebp; // 75c3f8 mov dword ptr [esp + 0x10], ebp
    L_75c3fc:;
    eax = *(uint32_t*)(fr+260); // 75c3fc mov eax, dword ptr [esp + 0x2c]
    *(uint32_t*)(fr+212) = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&BuildMeshTriangleCompare)); // 75c400 push 0x759640
    *(uint32_t*)(fr+208) = 0x4u; // 75c405 push 4
    *(uint32_t*)(fr+204) = ebp; // 75c407 push ebp
    *(uint32_t*)(fr+200) = eax; // 75c408 push eax
    eax = (uint32_t)(uintptr_t)T_Qsort(*(uint32_t*)(fr+200), *(uint32_t*)(fr+204), *(uint32_t*)(fr+208), *(uint32_t*)(fr+212)); // 75c409 call 0x8247e0
    ebx = 0x1u; // 75c40e mov ebx, 1
    { uint32_t a_=ebp, b_=ebx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c416 cmp ebp, ebx
    *(uint32_t*)(fr+248) = ebx; // 75c418 mov dword ptr [esp + 0x20], ebx
    eax = ebx; // 75c41c mov eax, ebx
    if ((cf||zf)) goto L_75c43f; // 75c41e jbe 0x75c43f
    L_75c420:;
    ecx = *(uint32_t*)(fr+260); // 75c420 mov ecx, dword ptr [esp + 0x2c]
    edx = *(uint32_t*)(uintptr_t)(ecx+eax*4+0xfffffffc); // 75c424 mov edx, dword ptr [ecx + eax*4 - 4]
    ecx = *(uint32_t*)(uintptr_t)(ecx+eax*4); // 75c428 mov ecx, dword ptr [ecx + eax*4]
    edx = *(uint32_t*)(uintptr_t)(edx+0x8); // 75c42b mov edx, dword ptr [edx + 8]
    esi = *(uint32_t*)(uintptr_t)(ecx+0x8); // 75c42e mov esi, dword ptr [ecx + 8]
    { uint32_t a_=edx, b_=esi; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c431 cmp edx, esi
    if (zf) goto L_75c436; // 75c433 je 0x75c436
    { uint32_t a_=ebx; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; ebx = r_; } // 75c435 inc ebx
    L_75c436:;
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c436 inc eax
    { uint32_t a_=eax, b_=ebp; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c437 cmp eax, ebp
    if (cf) goto L_75c420; // 75c439 jb 0x75c420
    *(uint32_t*)(fr+248) = ebx; // 75c43b mov dword ptr [esp + 0x20], ebx
    L_75c43f:;
    ecx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75c43f mov ecx, dword ptr [0xc97b24]
    eax = (ebx*4); // 75c445 lea eax, [ebx*4]
    *(uint32_t*)(fr+212) = 0x10502u; // 75c44c push 0x10502
    *(uint32_t*)(fr+208) = eax; // 75c451 push eax
    eax = ((uint32_t(__cdecl*)(uint32_t,uint32_t))(*(uint32_t*)(uintptr_t)(ecx+0x134)))(*(uint32_t*)(fr+208), *(uint32_t*)(fr+212)); // 75c452 call dword ptr [ecx + 0x134]
    *(uint32_t*)(fr+240) = eax; // 75c458 mov dword ptr [esp + 0x20], eax
    esi = 0x1u; // 75c45c mov esi, 1
    { uint32_t a_=edi, b_=edi; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75c464 xor edi, edi
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c466 xor eax, eax
    { uint32_t a_=ebp, b_=esi; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c468 cmp ebp, esi
    ebp = *(uint32_t*)(fr+264); // 75c46a mov ebp, dword ptr [esp + 0x30]
    if ((cf||zf)) goto L_75c4b3; // 75c46e jbe 0x75c4b3
    L_75c470:;
    ecx = *(uint32_t*)(fr+260); // 75c470 mov ecx, dword ptr [esp + 0x2c]
    edx = *(uint32_t*)(uintptr_t)(ecx+esi*4+0xfffffffc); // 75c474 mov edx, dword ptr [ecx + esi*4 - 4]
    ecx = *(uint32_t*)(uintptr_t)(ecx+esi*4); // 75c478 mov ecx, dword ptr [ecx + esi*4]
    edx = *(uint32_t*)(uintptr_t)(edx+0x8); // 75c47b mov edx, dword ptr [edx + 8]
    { uint32_t a_=edx, b_=*(uint32_t*)(uintptr_t)(ecx+0x8); uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c47e cmp edx, dword ptr [ecx + 8]
    if (zf) goto L_75c4aa; // 75c481 je 0x75c4aa
    edx = *(uint32_t*)(fr+260); // 75c483 mov edx, dword ptr [esp + 0x2c]
    ecx = esi; // 75c487 mov ecx, esi
    { uint32_t a_=ecx, b_=eax; uint32_t r_=(uint32_t)(a_ - b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75c489 sub ecx, eax
    eax = (edx+eax*4); // 75c48b lea eax, [edx + eax*4]
    *(uint32_t*)(fr+212) = ecx; // 75c48e push ecx
    *(uint32_t*)(fr+208) = eax; // 75c48f push eax
    eax = (uint32_t)(uintptr_t)T_TriStripMeshCreate(*(uint32_t*)(fr+208), *(uint32_t*)(fr+212)); // 75c490 call 0x75c660
    ecx = *(uint32_t*)(fr+240); // 75c495 mov ecx, dword ptr [esp + 0x20]
    *(uint32_t*)(uintptr_t)(ecx+edi*4) = eax; // 75c499 mov dword ptr [ecx + edi*4], eax
    edx = *(uint32_t*)(uintptr_t)(ebp+0x8); // 75c49c mov edx, dword ptr [ebp + 8]
    *(uint32_t*)(fr+204) = edx; // 75c49f push edx
    *(uint32_t*)(fr+200) = eax; // 75c4a0 push eax
    eax = ((uint32_t(__cdecl*)(uint32_t,uint32_t))(*(uint32_t*)(uintptr_t)(ebp+0x4)))(*(uint32_t*)(fr+200), *(uint32_t*)(fr+204)); // 75c4a1 call dword ptr [ebp + 4]
    { uint32_t a_=edi; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75c4a7 inc edi
    eax = esi; // 75c4a8 mov eax, esi
    L_75c4aa:;
    ecx = *(uint32_t*)(fr+232); // 75c4aa mov ecx, dword ptr [esp + 0x10]
    { uint32_t a_=esi; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75c4ae inc esi
    { uint32_t a_=esi, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c4af cmp esi, ecx
    if (cf) goto L_75c470; // 75c4b1 jb 0x75c470
    L_75c4b3:;
    ecx = *(uint32_t*)(fr+260); // 75c4b3 mov ecx, dword ptr [esp + 0x2c]
    { uint32_t a_=esi, b_=eax; uint32_t r_=(uint32_t)(a_ - b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; esi = r_; } // 75c4b7 sub esi, eax
    *(uint32_t*)(fr+212) = esi; // 75c4b9 push esi
    edx = (ecx+eax*4); // 75c4ba lea edx, [ecx + eax*4]
    *(uint32_t*)(fr+208) = edx; // 75c4bd push edx
    eax = (uint32_t)(uintptr_t)T_TriStripMeshCreate(*(uint32_t*)(fr+208), *(uint32_t*)(fr+212)); // 75c4be call 0x75c660
    ecx = *(uint32_t*)(fr+240); // 75c4c3 mov ecx, dword ptr [esp + 0x20]
    *(uint32_t*)(uintptr_t)(ecx+edi*4) = eax; // 75c4c7 mov dword ptr [ecx + edi*4], eax
    edx = *(uint32_t*)(uintptr_t)(ebp+0x8); // 75c4ca mov edx, dword ptr [ebp + 8]
    *(uint32_t*)(fr+204) = edx; // 75c4cd push edx
    *(uint32_t*)(fr+200) = eax; // 75c4ce push eax
    eax = ((uint32_t(__cdecl*)(uint32_t,uint32_t))(*(uint32_t*)(uintptr_t)(ebp+0x4)))(*(uint32_t*)(fr+200), *(uint32_t*)(fr+204)); // 75c4cf call dword ptr [ebp + 4]
    eax = *(uint32_t*)(fr+232); // 75c4d2 mov eax, dword ptr [esp + 0x20]
    *(uint32_t*)(fr+196) = 0x30502u; // 75c4d6 push 0x30502
    { uint32_t a_=eax, b_=ebx; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c4db add eax, ebx
    ecx = (eax+eax*2); // 75c4dd lea ecx, [eax + eax*2]
    eax = *(uint32_t*)(uintptr_t)(0xc97b24); // 75c4e0 mov eax, dword ptr [0xc97b24]
    edx = (ecx*4+0x10); // 75c4e5 lea edx, [ecx*4 + 0x10]
    *(uint32_t*)(fr+192) = edx; // 75c4ec push edx
    eax = ((uint32_t(__cdecl*)(uint32_t,uint32_t))(*(uint32_t*)(uintptr_t)(eax+0x134)))(*(uint32_t*)(fr+192), *(uint32_t*)(fr+196)); // 75c4ed call dword ptr [eax + 0x134]
    esi = eax; // 75c4f3 mov esi, eax
    edx = *(uint32_t*)(fr+232); // 75c4f5 mov edx, dword ptr [esp + 0x28]
    ecx = 0x1u; // 75c4f9 mov ecx, 1
    *(uint16_t*)(uintptr_t)(esi+0x4) = (uint16_t)ebx; // 75c501 mov word ptr [esi + 4], bx
    ebx = (ebx+ebx*2); // 75c505 lea ebx, [ebx + ebx*2]
    ebx = (uint32_t)(ebx << 2); // 75c508 shl ebx, 2
    edi = (esi+0x10); // 75c50b lea edi, [esi + 0x10]
    *(uint32_t*)(fr+252) = ebx; // 75c50e mov dword ptr [esp + 0x24], ebx
    { uint32_t a_=eax, b_=eax; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c512 xor eax, eax
    { uint32_t a_=ebx, b_=edi; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ebx = r_; } // 75c514 add ebx, edi
    { uint32_t a_=edx, b_=ecx; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c516 cmp edx, ecx
    *(uint32_t*)(uintptr_t)(esi) = 0x1u; // 75c518 mov dword ptr [esi], 1
    *(uint16_t*)(uintptr_t)(esi+0x6) = (uint16_t)eax; // 75c51e mov word ptr [esi + 6], ax
    *(uint32_t*)(uintptr_t)(esi+0x8) = eax; // 75c522 mov dword ptr [esi + 8], eax
    *(uint32_t*)(uintptr_t)(esi+0xc) = eax; // 75c525 mov dword ptr [esi + 0xc], eax
    *(uint32_t*)(fr+244) = eax; // 75c528 mov dword ptr [esp + 0x1c], eax
    if ((cf||zf)) goto L_75c5a7; // 75c52c jbe 0x75c5a7
    ecx = *(uint32_t*)(fr+260); // 75c52e mov ecx, dword ptr [esp + 0x2c]
    { uint32_t a_=edx; uint32_t r_=(uint32_t)(a_-1); of=a_==(uint32_t)0x80000000; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75c532 dec edx
    *(uint32_t*)(fr+232) = edx; // 75c533 mov dword ptr [esp + 0x10], edx
    { uint32_t a_=edx; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75c537 inc edx
    *(uint32_t*)(fr+264) = ecx; // 75c538 mov dword ptr [esp + 0x30], ecx
    *(uint32_t*)(fr+236) = edx; // 75c53c mov dword ptr [esp + 0x14], edx
    goto L_75c546; // 75c540 jmp 0x75c546
    L_75c542:;
    ecx = *(uint32_t*)(fr+264); // 75c542 mov ecx, dword ptr [esp + 0x30]
    L_75c546:;
    edx = *(uint32_t*)(uintptr_t)(ecx); // 75c546 mov edx, dword ptr [ecx]
    ecx = *(uint32_t*)(uintptr_t)(ecx+0x4); // 75c548 mov ecx, dword ptr [ecx + 4]
    edx = *(uint32_t*)(uintptr_t)(edx+0x8); // 75c54b mov edx, dword ptr [edx + 8]
    { uint32_t a_=edx, b_=*(uint32_t*)(uintptr_t)(ecx+0x8); uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c54e cmp edx, dword ptr [ecx + 8]
    if (zf) goto L_75c58d; // 75c551 je 0x75c58d
    ecx = *(uint32_t*)(fr+264); // 75c553 mov ecx, dword ptr [esp + 0x30]
    *(uint32_t*)(uintptr_t)(edi) = ebx; // 75c557 mov dword ptr [edi], ebx
    *(uint32_t*)(fr+212) = ebp; // 75c559 push ebp
    *(uint32_t*)(fr+208) = edi; // 75c55a push edi
    edx = *(uint32_t*)(uintptr_t)(ecx); // 75c55b mov edx, dword ptr [ecx]
    ecx = *(uint32_t*)(uintptr_t)(edx+0x8); // 75c55d mov ecx, dword ptr [edx + 8]
    edx = *(uint32_t*)(fr+240); // 75c560 mov edx, dword ptr [esp + 0x20]
    *(uint32_t*)(uintptr_t)(edi+0x8) = ecx; // 75c564 mov dword ptr [edi + 8], ecx
    eax = *(uint32_t*)(uintptr_t)(edx+eax*4); // 75c567 mov eax, dword ptr [edx + eax*4]
    *(uint32_t*)(fr+204) = eax; // 75c56a push eax
    eax = (uint32_t)(uintptr_t)T_TriStripEmit(*(uint32_t*)(fr+204), *(uint32_t*)(fr+208), *(uint32_t*)(fr+212)); // 75c56b call 0x75c9d0
    eax = *(uint32_t*)(uintptr_t)(edi+0x4); // 75c570 mov eax, dword ptr [edi + 4]
    edx = *(uint32_t*)(uintptr_t)(esi+0x8); // 75c573 mov edx, dword ptr [esi + 8]
    { uint32_t a_=edx, b_=eax; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75c576 add edx, eax
    ebx = (ebx+eax*2); // 75c57b lea ebx, [ebx + eax*2]
    eax = *(uint32_t*)(fr+244); // 75c57e mov eax, dword ptr [esp + 0x1c]
    { uint32_t a_=edi, b_=0xcu; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; edi = r_; } // 75c582 add edi, 0xc
    { uint32_t a_=eax; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c585 inc eax
    *(uint32_t*)(uintptr_t)(esi+0x8) = edx; // 75c586 mov dword ptr [esi + 8], edx
    *(uint32_t*)(fr+244) = eax; // 75c589 mov dword ptr [esp + 0x1c], eax
    L_75c58d:;
    ecx = *(uint32_t*)(fr+264); // 75c58d mov ecx, dword ptr [esp + 0x30]
    { uint32_t a_=ecx, b_=0x4u; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75c591 add ecx, 4
    *(uint32_t*)(fr+264) = ecx; // 75c594 mov dword ptr [esp + 0x30], ecx
    ecx = *(uint32_t*)(fr+232); // 75c598 mov ecx, dword ptr [esp + 0x10]
    { uint32_t a_=ecx; uint32_t r_=(uint32_t)(a_-1); of=a_==(uint32_t)0x80000000; zf=r_==0; sf=(r_>>31)&1; ecx = r_; } // 75c59c dec ecx
    *(uint32_t*)(fr+232) = ecx; // 75c59d mov dword ptr [esp + 0x10], ecx
    if (!zf) goto L_75c542; // 75c5a1 jne 0x75c542
    ecx = *(uint32_t*)(fr+236); // 75c5a3 mov ecx, dword ptr [esp + 0x14]
    L_75c5a7:;
    edx = *(uint32_t*)(fr+260); // 75c5a7 mov edx, dword ptr [esp + 0x2c]
    *(uint32_t*)(uintptr_t)(edi) = ebx; // 75c5ab mov dword ptr [edi], ebx
    *(uint32_t*)(fr+212) = ebp; // 75c5ad push ebp
    ebp = *(uint32_t*)(fr+240); // 75c5ae mov ebp, dword ptr [esp + 0x1c]
    ecx = *(uint32_t*)(uintptr_t)(edx+ecx*4+0xfffffffc); // 75c5b2 mov ecx, dword ptr [edx + ecx*4 - 4]
    *(uint32_t*)(fr+208) = edi; // 75c5b6 push edi
    edx = *(uint32_t*)(uintptr_t)(ecx+0x8); // 75c5b7 mov edx, dword ptr [ecx + 8]
    *(uint32_t*)(uintptr_t)(edi+0x8) = edx; // 75c5ba mov dword ptr [edi + 8], edx
    eax = *(uint32_t*)(uintptr_t)(ebp+eax*4); // 75c5bd mov eax, dword ptr [ebp + eax*4]
    *(uint32_t*)(fr+204) = eax; // 75c5c1 push eax
    eax = (uint32_t)(uintptr_t)T_TriStripEmit(*(uint32_t*)(fr+204), *(uint32_t*)(fr+208), *(uint32_t*)(fr+212)); // 75c5c2 call 0x75c9d0
    edi = *(uint32_t*)(uintptr_t)(edi+0x4); // 75c5c7 mov edi, dword ptr [edi + 4]
    eax = *(uint32_t*)(uintptr_t)(esi+0x8); // 75c5ca mov eax, dword ptr [esi + 8]
    { uint32_t a_=eax, b_=edi; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c5cd add eax, edi
    *(uint32_t*)(fr+200) = 0x30502u; // 75c5cf push 0x30502
    ebx = (ebx+edi*2); // 75c5d4 lea ebx, [ebx + edi*2]
    *(uint32_t*)(uintptr_t)(esi+0x8) = eax; // 75c5d7 mov dword ptr [esi + 8], eax
    ecx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75c5da mov ecx, dword ptr [0xc97b24]
    { uint32_t a_=ebx, b_=esi; uint32_t r_=(uint32_t)(a_ - b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; ebx = r_; } // 75c5e0 sub ebx, esi
    *(uint32_t*)(fr+196) = ebx; // 75c5e2 push ebx
    *(uint32_t*)(fr+192) = esi; // 75c5e3 push esi
    eax = ((uint32_t(__cdecl*)(uint32_t,uint32_t,uint32_t))(*(uint32_t*)(uintptr_t)(ecx+0x13c)))(*(uint32_t*)(fr+192), *(uint32_t*)(fr+196), *(uint32_t*)(fr+200)); // 75c5e4 call dword ptr [ecx + 0x13c]
    edx = *(uint32_t*)(fr+252); // 75c5ea mov edx, dword ptr [esp + 0x3c]
    edi = *(uint32_t*)(fr+248); // 75c5ee mov edi, dword ptr [esp + 0x38]
    esi = eax; // 75c5f2 mov esi, eax
    { uint32_t r_=(uint32_t)(edi & edi); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75c5f7 test edi, edi
    eax = (esi+0x10); // 75c5f9 lea eax, [esi + 0x10]
    ecx = (edx+eax); // 75c5fc lea ecx, [edx + eax]
    if ((cf||zf)) goto L_75c611; // 75c5ff jbe 0x75c611
    edx = edi; // 75c601 mov edx, edi
    L_75c603:;
    ebx = *(uint32_t*)(uintptr_t)(eax+0x4); // 75c603 mov ebx, dword ptr [eax + 4]
    *(uint32_t*)(uintptr_t)(eax) = ecx; // 75c606 mov dword ptr [eax], ecx
    { uint32_t a_=eax, b_=0xcu; uint32_t r_=(uint32_t)(a_ + b_); cf=r_<a_; of=(((a_^r_)&(b_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; eax = r_; } // 75c608 add eax, 0xc
    { uint32_t a_=edx; uint32_t r_=(uint32_t)(a_-1); of=a_==(uint32_t)0x80000000; zf=r_==0; sf=(r_>>31)&1; edx = r_; } // 75c60b dec edx
    ecx = (ecx+ebx*2); // 75c60c lea ecx, [ecx + ebx*2]
    if (!zf) goto L_75c603; // 75c60f jne 0x75c603
    L_75c611:;
    { uint32_t a_=ebx, b_=ebx; uint32_t r_=(uint32_t)(a_ ^ b_); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; ebx = r_; } // 75c611 xor ebx, ebx
    { uint32_t r_=(uint32_t)(edi & edi); cf=0; of=0; zf=r_==0; sf=(r_>>31)&1; } // 75c613 test edi, edi
    if ((cf||zf)) goto L_75c630; // 75c615 jbe 0x75c630
    L_75c617:;
    eax = *(uint32_t*)(uintptr_t)(ebp+ebx*4); // 75c617 mov eax, dword ptr [ebp + ebx*4]
    ecx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75c61b mov ecx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+212) = eax; // 75c621 push eax
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(ecx+0x138)))(*(uint32_t*)(fr+212)); // 75c622 call dword ptr [ecx + 0x138]
    { uint32_t a_=ebx; uint32_t r_=(uint32_t)(a_+1); of=a_==(uint32_t)0x7fffffff; zf=r_==0; sf=(r_>>31)&1; ebx = r_; } // 75c62b inc ebx
    { uint32_t a_=ebx, b_=edi; uint32_t r_=(uint32_t)(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>31)&1; zf=r_==0; sf=(r_>>31)&1; } // 75c62c cmp ebx, edi
    if (cf) goto L_75c617; // 75c62e jb 0x75c617
    L_75c630:;
    edx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75c630 mov edx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+212) = ebp; // 75c636 push ebp
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(edx+0x138)))(*(uint32_t*)(fr+212)); // 75c637 call dword ptr [edx + 0x138]
    eax = *(uint32_t*)(fr+260); // 75c63d mov eax, dword ptr [esp + 0x30]
    ecx = *(uint32_t*)(uintptr_t)(0xc97b24); // 75c641 mov ecx, dword ptr [0xc97b24]
    *(uint32_t*)(fr+208) = eax; // 75c647 push eax
    eax = ((uint32_t(__cdecl*)(uint32_t))(*(uint32_t*)(uintptr_t)(ecx+0x138)))(*(uint32_t*)(fr+208)); // 75c648 call dword ptr [ecx + 0x138]
    eax = esi; // 75c651 mov eax, esi
    edi = *(uint32_t*)(fr+216); // 75c653 pop edi
    esi = *(uint32_t*)(fr+220); // 75c654 pop esi
    ebp = *(uint32_t*)(fr+224); // 75c655 pop ebp
    ebx = *(uint32_t*)(fr+228); // 75c656 pop ebx
    return reinterpret_cast<RpMeshHeader*>(static_cast<uintptr_t>(eax)); // 75c65a ret 
}


// Lotes S30A y S31A (sin validar): ganchos de las raices de RpWorld reconstruidas en este archivo.
void RpWorldInjectHooksS30A31A() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("RpWorld");

    RH_ScopedGlobalInstall(RpClumpCreateSpace, 0x749970);                                      // lote S31A (sin validar)
    RH_ScopedGlobalInstall(RpAtomicStreamGetSize, 0x74A5B0);                                   // lote S30A (sin validar)
    RH_ScopedGlobalInstall(RpClumpStreamGetSize, 0x74A5E0);                                    // lote S31A (sin validar)
    RH_ScopedGlobalInstall(RpAtomicStreamWrite, 0x74A850);                                     // lote S31A (sin validar)
    RH_ScopedGlobalInstall(RpClumpStreamWrite, 0x74AA10);                                      // lote S31A (sin validar)
    RH_ScopedGlobalInstall(RpGeometryTransform, 0x74BFE0);                                     // lote S31A (sin validar)
    RH_ScopedGlobalInstall(RpGeometryStreamWrite, 0x74CED0);                                   // lote S31A (sin validar)
    RH_ScopedGlobalInstall(_rpMaterialListStreamWrite, 0x74E4B0);                              // lote S30A (sin validar)
    RH_ScopedGlobalInstall(_rpWorldFindBBox, 0x74EFA0);                                        // lote S31A (sin validar)
    RH_ScopedGlobalInstall(RpWorldRender, 0x74F570);                                           // lote S30A (sin validar)
    RH_ScopedGlobalInstall(RwCameraForAllClumpsInFrustum, 0x7516C0);                           // lote S30A (sin validar)
    RH_ScopedGlobalInstall(RwCameraForAllAtomicsInFrustum, 0x7517F0);                          // lote S30A (sin validar)
    RH_ScopedGlobalInstall(RpLightGetConeAngle, 0x751AE0);                                     // lote S30A (sin validar)
    RH_ScopedGlobalInstall(RpLightSetConeAngle, 0x751D20);                                     // lote S30A (sin validar)
    RH_ScopedGlobalInstall(_rpD3D9VertexDeclarationInstV3dComp, 0x7531B0);                     // lote S30A (sin validar)
    RH_ScopedGlobalInstall(_rpD3D9GetMinMaxValuesV3d, 0x755830);                               // lote S30A (sin validar)
    RH_ScopedGlobalInstall(_rpD3D9FindFormatV3d, 0x755980);                                    // lote S31A (sin validar)
    RH_ScopedGlobalInstall(_rwD3D9LightPluginDestruct, 0x755BF0);                              // lote S30A (sin validar)
    RH_ScopedGlobalInstall(_rwD3D9LightsGlobalEnable, 0x756070);                               // lote S30A (sin validar)
    RH_ScopedGlobalInstall(_rwD3D9LightDirectionalEnable, 0x756260);                           // lote S30A (sin validar)
    RH_ScopedGlobalInstall(_rwD3D9LightLocalEnable, 0x756390);                                 // lote S31A (sin validar)
    RH_ScopedGlobalInstall(_rpMeshHeaderDestroy, 0x758910);                                    // lote S30A (sin validar)
    RH_ScopedGlobalInstall(RpBuildMeshGenerateExhaustiveTriStrip, 0x75A940);                   // lote S30A (sin validar)
    RH_ScopedGlobalInstall(RpBuildMeshGenerateExhaustiveIgnoreWindingTriStrip, 0x75B4E0);      // lote S31A (sin validar)
    RH_ScopedGlobalInstall(_rpTriListMeshOptimize, 0x75D730);                                  // lote S30A (sin validar)
    RH_ScopedGlobalInstall(RpGeometryIsCorrectlySorted, 0x75D9D0);                             // lote S30A (sin validar)
    RH_ScopedGlobalInstall(_rxD3D9VertexShaderDefaultBeginCallBack, 0x760DF0);                 // lote S30A (sin validar)
    RH_ScopedGlobalInstall(_rxD3D9VertexShaderDefaultGetMaterialShaderCallBack, 0x761010);     // lote S31A (sin validar)
    RH_ScopedGlobalInstall(_rxD3D9VertexShaderDefaultMeshRenderCallBack, 0x761030);            // lote S30A (sin validar)
    RH_ScopedGlobalInstall(_rxD3D9VertexShaderDefaultLightingCallBack, 0x761170);              // lote S31A (sin validar)
    RH_ScopedGlobalInstall(_rpD3D9VertexShaderUpdateLightsColors, 0x761720);                   // lote S30A (sin validar)
    RH_ScopedGlobalInstall(_rpD3D9VertexShaderUpdateMaterialColor, 0x761820);                  // lote S30A (sin validar)
    RH_ScopedGlobalInstall(_rpD3D9VertexShaderUpdateFogData, 0x7618B0);                        // lote S30A (sin validar)
    RH_ScopedGlobalInstall(_rpD3D9VertexShaderUpdateMorphingCoef, 0x761950);                   // lote S30A (sin validar)
    RH_ScopedGlobalInstall(_rpD3DVertexShaderSetUVAnimMatrix, 0x7619A0);                       // lote S30A (sin validar)
    RH_ScopedGlobalInstall(RpWorldSectorRender, 0x761C50);                                     // lote S30A (sin validar)
    RH_ScopedGlobalInstall(WorldSectorRenderCallBack, 0x74F590); // codigo vivo CAM (sin validar)
    RH_ScopedGlobalInstall(ClumpLightCountCallBack, 0x74AE40); // codigo vivo CAM (sin validar)
    RH_ScopedGlobalInstall(ClumpAtomicStreamSizeCallBack, 0x74B000); // codigo vivo CAM (sin validar)
    RH_ScopedGlobalInstall(ClumpLightStreamSizeCallBack, 0x74AFA0); // codigo vivo CAM (sin validar)
    RH_ScopedGlobalInstall(ClumpCameraStreamSizeCallBack, 0x74AFD0); // codigo vivo CAM (sin validar)
    RH_ScopedGlobalInstall(ClumpLightWriteCallBack, 0x74AEA0); // codigo vivo CAM (sin validar)
    RH_ScopedGlobalInstall(ClumpCameraWriteCallBack, 0x74AF20); // codigo vivo CAM (sin validar)
    RH_ScopedGlobalInstall(RpWorldStreamWrite, 0x762150);                                      // lote S31A (sin validar)
    RH_ScopedGlobalInstall(RpWorldStreamRead, 0x762960);                                       // lote S31A (sin validar)
    RH_ScopedGlobalInstall(_rwD3D9VSGetProjectionTransposedMatrix, 0x764D20);                  // lote S31A (sin validar)
    RH_ScopedGlobalInstall(_rpTriStripTunnelToggle, 0x75BB20);                                 // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(_rpTriStripPolygonCountStripEdges, 0x75BB50);                       // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(_rpTriStripTunnelIsValid, 0x75BB80);                                // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(_rpTriStripTunnelCost, 0x75BC80);                                   // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(RpTriStripMeshTunnel, 0x75B780);                                    // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(TriStripMeshCreate, 0x75C660);                                      // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(TriStripEmit, 0x75C9D0);                                            // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(TriStripNeighbours, 0x75D240);                                      // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(RpBuildMeshGenerateTriStrip, 0x75C380);                             // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(GeometrySortCompare, 0x75E230);                                     // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(RpGeometrySortByMaterial, 0x75DAE0);                                // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(_rpGeometryPluginFirstEntry, 0x74BFD0);                             // lote S31A, cierre (sin validar)
}
