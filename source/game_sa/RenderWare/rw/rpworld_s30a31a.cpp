// Lotes S30A y S31A del plan de sustitucion (docs/ExeIndependiente/plan-sustitucion.md): raices de RpWorld que antes ejecutaba el EXE (envolturas por direccion y callbacks).
// Cada funcion esta enganchada con RH_ScopedGlobalInstall (sin validar, validated=0 en docs/hooks.csv) en RpWorldInjectHooksS30A31A, al final del archivo.
// Herramientas y desensamblados: reversing/FnS30A33A. Las funciones del EXE que aun no estan reconstruidas se llaman por direccion y se anotan en cada una.
#include "StdInc.h"

#include "rpworld.h"
#include "X87Math.h"
#include <rpdbgerr.h>
#include <bit>
#include <cmath>
#include <cstring>

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

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

// 0x74F570 (lote S30A, sin validar; 31 bytes; cdecl, (mundo)): recorre los sectores en el frustum de la camara actual (RwCameraForAllSectorsInFrustum, 0x751660; lote de otro agente, se llama por su funcion) con el callback
// 0x74F590 del EXE (renderiza el sector y sus atomos; NO es raiz del plan, se conserva por direccion) y el mundo como datos. Devuelve el mundo.
RpWorld* RpWorldRender(RpWorld* world) {
    RwCameraForAllSectorsInFrustum(static_cast<RwCamera*>(Globals()->curCamera), reinterpret_cast<RpWorldSectorCallBack>(0x74F590), world);
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

// 0x74AA10 (lote S31A, sin validar; 1059 bytes; cdecl, (clump, flujo)): escribe el clump: cabecera de clump (tipo 0x10) con su tamano (calculado como RpClumpStreamGetSize pero con los callbacks 0x74B000, 0x74AFA0 y 0x74AFD0 del EXE; si
// falla la preparacion de listas el EXE escribe tamano 0 y sigue), cabecera de estructura (12 bytes: numero de atomos, luces y camaras), lista de marcos, lista de geometrias (cabecera 0x1A, estructura con el numero y cada RpGeometryStreamWrite),
// los atomos (0x74A880), las luces (0x74AEA0) y las camaras (0x74AF20) del EXE y los plugins del clump. Si algo falla devuelve NULL (sin liberar la lista de marcos si falla despues, igual que el EXE); si va bien, el clump.
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
                RpClumpForAllAtomics(clump, reinterpret_cast<RpAtomicCallBack>(0x74B000), &total);
                RpClumpForAllLights(clump, reinterpret_cast<RpLightCallBack>(0x74AFA0), &total);
                RpClumpForAllCameras(clump, reinterpret_cast<RwCameraCallBack>(0x74AFD0), &total);
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
    RpClumpForAllLights(clump, reinterpret_cast<RpLightCallBack>(0x74AE40), &lightCount);
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
            if (!reinterpret_cast<ClumpLightWriteFn>(0x74AEA0)(ClumpListObject<RpLight>(node, 0xC924DC), &context)) {
                break;
            }
            node = next;
        }
    }
    if (counts[2] > 0 && context.ok != 0) {
        RwLLLink* const sentinel = &clump->cameraList.link;
        for (RwLLLink* node = sentinel->next; node != sentinel;) {
            RwLLLink* const next = node->next;
            if (!reinterpret_cast<ClumpCameraWriteFn>(0x74AF20)(ClumpListObject<RwCamera>(node, 0xC924D8), &context)) {
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
    using StructSizeFn = RwUInt32(__cdecl*)(const RpGeometry*);
    const StructSizeFn structSize = reinterpret_cast<StructSizeFn>(0x74CE60);
    RwUInt32 total = structSize(geometry) + 0xC;
    total = total + _rpMaterialListStreamGetSize(&geometry->matList) + 0xC;
    total = total + _rwPluginRegistryGetSize(reinterpret_cast<const RwPluginRegistry*>(kGeometryPluginRegistry), geometry) + 0xC;
    if (!WriteChunkHeader(stream, 0xF, total)) {
        return nullptr;
    }
    if (!WriteChunkHeader(stream, 1, structSize(geometry))) {
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
using SectorSizeFn  = RwUInt32(__cdecl*)(void* sector, RpWorld* world, RwUInt32 flags);                 // 0x762440 (sector atomico) y 0x762040 (sector plano): tamano en flujo
using SectorWriteFn = RwBool(__cdecl*)(void* sector, RwStream* stream, RpWorld* world, RwUInt32 flags); // 0x7624A0 (atomico) y 0x762740 (plano)
using SectorReadFn  = RpSector*(__cdecl*)(RwStream* stream, void** memoryCursor, RpWorld* world, RwUInt32 flags); // 0x762F30 (atomico) y 0x763310 (plano)

// RWERROR((codigo [, argumento])) del SDK: error de plugin 2 con el codigo que devuelve _rwerror (0x8088D0).
void RaiseWorldError(RwInt32 code) {
    RwError error;
    error.pluginID  = 2;
    error.errorCode = reinterpret_cast<RwInt32(__cdecl*)(RwInt32, ...)>(0x8088D0)(code);
    RwErrorSet(&error);
}

void RaiseWorldError(RwInt32 code, RwUInt32 argument) {
    RwError error;
    error.pluginID  = 2;
    error.errorCode = reinterpret_cast<RwInt32(__cdecl*)(RwInt32, ...)>(0x8088D0)(code, argument);
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
    const SectorSizeFn atomicSectorSize = reinterpret_cast<SectorSizeFn>(0x762440);
    const SectorSizeFn planeSectorSize = reinterpret_cast<SectorSizeFn>(0x762040);
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
    RH_ScopedGlobalInstall(RpWorldStreamWrite, 0x762150);                                      // lote S31A (sin validar)
    RH_ScopedGlobalInstall(RpWorldStreamRead, 0x762960);                                       // lote S31A (sin validar)
    RH_ScopedGlobalInstall(_rwD3D9VSGetProjectionTransposedMatrix, 0x764D20);                  // lote S31A (sin validar)
}
