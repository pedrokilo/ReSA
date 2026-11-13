/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: D3DTextureBuffer
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

// Este archivo se compila completo con /O2 /Oi /arch:IA32 /fp:fast /Zi, sin PCH ni unity (ver source/CMakeLists.txt): GetTotalDataSize (0x7300A0) usa la x87 del EXE
// (fild, fadd 2^32, fmul 4/3, fiadd y __ftol) con la precision de 24 bits del dispositivo D3D; con SSE y /Od cada paso se redondearia distinto.
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include "StdInc.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "D3DTextureBuffer.h"
#include "D3DResourceSystem.h"

namespace {
// D3DSURFACE_DESC (IDirect3DTexture9::GetLevelDesc, ranura 17 de la vtable): 32 bytes que el dispositivo escribe enteros.
struct SurfaceDesc {
    uint32 Format, Type, Usage, Pool, MultiSampleType, MultiSampleQuality, Width, Height;
};
using GetLevelCountFn = uint32(__stdcall*)(void*);
using GetLevelDescFn  = long(__stdcall*)(void*, uint32, SurfaceDesc*);

// IDirect3DBaseTexture9::GetLevelCount (ranura 13): el `call dword ptr [ecx+34h]` del EXE.
inline uint32 GetLevelCount(void* texture) {
    return reinterpret_cast<GetLevelCountFn>(D3DResourceCom::VTable(texture)[13])(texture);
}
// IDirect3DTexture9::GetLevelDesc (ranura 17): el `call dword ptr [edx+44h]` del EXE.
inline void GetLevelDesc(void* texture, uint32 level, SurfaceDesc* desc) {
    reinterpret_cast<GetLevelDescFn>(D3DResourceCom::VTable(texture)[17])(texture, level, desc);
}
// 4/3 como float (0x85F09C): 1.3333333730697632.
constexpr double kMipChainFactor = 1.3333333730697632;
// 2^32 (0x858C54): corrige el `fild` con signo de un valor sin signo.
constexpr double kTwoPow32 = 4294967296.0;
} // namespace

void D3DTextureBuffer::InjectHooks() {
    RH_ScopedClass(D3DTextureBuffer);
    RH_ScopedCategory("RenderWare");

    RH_ScopedInstall(Setup, 0x72FE80);
    RH_ScopedOverloadedInstall(Pop, "Match", 0x72FF60, Texture * (D3DTextureBuffer::*)(uint32, int32, int32, int32));
    RH_ScopedInstall(Push, 0x72FFF0);
    RH_ScopedInstall(Resize, 0x730020);
    RH_ScopedInstall(GetTotalDataSize, 0x7300A0);
    RH_ScopedInstall(PushWithoutIncreasingCounter, 0x730AD0);
    RH_ScopedInstall(Clear, 0x72FEC0);   // lote 182A (sin validar)
    RH_ScopedInstall(Destroy, 0x72FEF0); // lote 182A (sin validar)
}

// 0x72FEC0 (lote 182A, sin validar; thiscall, sin argumentos, `ret`, 46 bytes, sin llamadores en el EXE)
// Libera (Release, ranura 2 de la vtable COM) las texturas guardadas de la primera a la ultima y deja a cero el numero de texturas guardadas (+0x14) y despues el contador (+0x10). La pila (+0x18) se conserva.
// El numero de texturas (con signo) y el puntero de la pila se vuelven a leer de memoria en cada vuelta (Release puede reentrar), como el EXE.
void D3DTextureBuffer::Clear() {
    for (int32 i = 0; i < static_cast<int32>(m_nSize); ++i) {
        D3DResourceCom::Release(m_apTextures[i]);
    }
    m_nSize = 0;
    m_nNumTexturesInBuffer = 0;
}

// 0x72FEF0 (lote 182A, sin validar; thiscall, sin argumentos, `ret`, 66 bytes, sin llamadores en el EXE)
// Como Clear, pero ademas libera la pila con free (0x8214BD, el operator delete de la DLL) si no es nula y la deja a nulo (solo en ese caso). El puntero se lee tras el bucle y antes de poner a cero los dos contadores.
void D3DTextureBuffer::Destroy() {
    for (int32 i = 0; i < static_cast<int32>(m_nSize); ++i) {
        D3DResourceCom::Release(m_apTextures[i]);
    }
    Texture** const stack = m_apTextures;
    m_nSize = 0;
    m_nNumTexturesInBuffer = 0;
    if (stack) {
        ::operator delete(stack);
        m_apTextures = nullptr;
    }
}

// 0x72FE80
// Prepara la pila: guarda formato, ancho y niveles, reserva `capacity` punteros con operator new (0x82119A; el tamano es `capacity * 4` en 32 bits) y deja a cero el
// contador y el numero de texturas guardadas. Los tres primeros campos se escriben antes de reservar, como el EXE.
void D3DTextureBuffer::Setup(uint32 format, int32 width, int32 levels, uint32 capacity) {
    m_nFormat = format;
    m_nWidth  = static_cast<uint32>(width);
    m_nLevels = levels;
    m_apTextures            = static_cast<Texture**>(::operator new(capacity * 4u));
    m_nCapcacity            = capacity;
    m_nSize                 = 0;
    m_nNumTexturesInBuffer  = 0;
}

// 0x72FF60
// Busca desde la ultima textura guardada hacia la primera una cuyo GetLevelCount sea 1 (`levels` = 1) o distinto de 1 (`levels` = 0) y que tenga el mismo formato, ancho y
// alto del nivel 0 (GetLevelDesc). Si la encuentra la saca de la pila (el hueco lo ocupa la ultima, sin comprobar el rango; m_nNumTexturesInBuffer no cambia) y la
// devuelve; si no, devuelve nulo. El numero de texturas se lee una sola vez y el arreglo se vuelve a leer en cada paso.
D3DTextureBuffer::Texture* D3DTextureBuffer::Pop(uint32 format, int32 width, int32 height, int32 levels) {
    uint32 index = m_nSize;
    if (index == 0) {
        return nullptr;
    }
    do {
        Texture* const texture = m_apTextures[index - 1];
        --index;
        const int32 oneLevel = GetLevelCount(texture) == 1 ? 1 : 0;
        if (oneLevel == levels) {
            SurfaceDesc desc;
            GetLevelDesc(texture, 0, &desc);
            if (desc.Format == format && desc.Width == static_cast<uint32>(width) && desc.Height == static_cast<uint32>(height)) {
                const uint32 last = m_nSize - 1;
                m_nSize = last;
                if (static_cast<int32>(last) > 0 && last != index) {
                    m_apTextures[index] = m_apTextures[last];
                }
                return texture;
            }
        }
    } while (index != 0);
    return nullptr;
}

// 0x72FFF0
// Cuenta la devolucion (m_nNumTexturesInBuffer + 1, aunque no quepa) y guarda la textura si hay sitio (m_nSize < m_nCapcacity, con signo); devuelve si la guardo.
bool D3DTextureBuffer::Push(Texture* texture) {
    const uint32 size = m_nSize;
    const uint32 capacity = m_nCapcacity;
    m_nNumTexturesInBuffer = m_nNumTexturesInBuffer + 1;
    if (static_cast<int32>(size) >= static_cast<int32>(capacity)) {
        return false;
    }
    m_apTextures[size] = texture;
    m_nSize = m_nSize + 1;
    return true;
}

// 0x730020
// Cambia la capacidad: reserva la pila nueva (operator new, `newCapacity * 4` en 32 bits), copia las texturas que caben (m_nSize se relee en cada paso, comparaciones con
// signo), libera (Release) las que no caben, libera la pila vieja (operator delete 0x8214BD, tambien si es nula) y guarda el puntero y la capacidad. NO actualiza m_nSize: con
// una capacidad menor que m_nSize quedan mas texturas contadas que las que caben (como el EXE). Sin cambios si la capacidad es la misma.
void D3DTextureBuffer::Resize(uint32 newCapacity) {
    if (newCapacity == m_nCapcacity) {
        return;
    }
    Texture** const newStack = static_cast<Texture**>(::operator new(newCapacity * 4u));
    const int32 capacity = static_cast<int32>(newCapacity);
    for (int32 i = 0; static_cast<int32>(m_nSize) > i && i < capacity; ++i) {
        newStack[i] = m_apTextures[i];
    }
    if (capacity < static_cast<int32>(m_nSize)) {
        for (int32 i = capacity; i < static_cast<int32>(m_nSize); ++i) {
            D3DResourceCom::Release(m_apTextures[i]);
        }
    }
    ::operator delete(m_apTextures);
    m_apTextures = newStack;
    m_nCapcacity = newCapacity;
}

// 0x7300A0
// Suma de pixeles de las texturas guardadas (sin contar bytes por pixel). Formato 0 (buffer generico): para cada textura, ancho * alto del nivel 0 (en 32 bits); si
// GetLevelCount es 0 la suma pasa por la x87 de 24 bits (`fild` del producto, `fadd` 2^32 si es negativo, `fmul` 4/3, `fiadd` del total y `__ftol`) y entonces el total se
// REEMPLAZA por el resultado (el producto escalado mas el total); si no, se suma el producto. Otro formato: m_nSize * alto * ancho de la primera textura (en 32 bits), escalado
// por 4/3 con `__ftol` si su GetLevelCount es 0. Sin texturas devuelve 0. Los contadores y los punteros se vuelven a leer de memoria tras cada llamada COM.
uint32 D3DTextureBuffer::GetTotalDataSize() {
    int32 total = 0;
    if (m_nFormat == 0) {
        for (int32 i = 0; i < static_cast<int32>(m_nSize); ++i) {
            SurfaceDesc desc;
            GetLevelDesc(m_apTextures[i], 0, &desc);
            if (GetLevelCount(m_apTextures[i]) == 0) {
                const int32 pixels = static_cast<int32>(desc.Height * desc.Width);
                double scaled = static_cast<double>(pixels);
                if (pixels < 0) {
                    scaled += kTwoPow32;
                }
                scaled *= kMipChainFactor;
                total = static_cast<int32>(static_cast<int64>(scaled + total));
            } else {
                total += static_cast<int32>(desc.Height * desc.Width);
            }
        }
        return static_cast<uint32>(total);
    }
    if (m_nSize == 0) {
        return 0;
    }
    SurfaceDesc desc;
    GetLevelDesc(m_apTextures[0], 0, &desc);
    const int32 size = static_cast<int32>(m_nSize * desc.Height * desc.Width);
    if (GetLevelCount(m_apTextures[0]) == 0) {
        return static_cast<uint32>(static_cast<int32>(static_cast<int64>(static_cast<double>(size) * kMipChainFactor)));
    }
    return static_cast<uint32>(size);
}

// 0x730AD0
// Como Push pero sin tocar m_nNumTexturesInBuffer (el EXE lo relee y lo vuelve a escribir igual): guarda la textura si hay sitio y devuelve si la guardo.
bool D3DTextureBuffer::PushWithoutIncreasingCounter(Texture* texture) {
    const uint32 size = m_nSize;
    if (static_cast<int32>(size) >= static_cast<int32>(m_nCapcacity)) {
        return false;
    }
    m_apTextures[size] = texture;
    m_nSize = m_nSize + 1;
    return true;
}
