/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: D3DResourceSystem
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

// Este archivo se compila completo con /O2 /Oi /Oy /arch:IA32 /fp:fast /Zi, sin PCH ni unity (ver source/CMakeLists.txt):
// SetUseD3DResourceBuffering debe coincidir byte a byte con el EXE (10 bytes, sin prólogo ni relocaciones).
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include "StdInc.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

// Globales del EXE (0x730830 Init las inicializa); declaradas en D3DResourceSystem.h.
bool&                D3DResourceSystem::UseD3DResourceBuffering = *reinterpret_cast<bool*>(0x8D6084);
uint32&              D3DResourceSystem::NumTextureBuffers       = *reinterpret_cast<uint32*>(0xC87C60);
D3DTextureBuffer&    D3DResourceSystem::TextureBuffer           = *reinterpret_cast<D3DTextureBuffer*>(0xC87C68); // TextureBuffers[0]
D3DTextureBuffer (&D3DResourceSystem::TextureBuffers)[17]       = *reinterpret_cast<D3DTextureBuffer(*)[17]>(0xC87C68);
D3DIndexDataBuffer (&D3DResourceSystem::IndexDataBuffers)[16]   = *reinterpret_cast<D3DIndexDataBuffer(*)[16]>(0xC87E48);
D3DIndexDataBuffer&  D3DResourceSystem::IndexDataBuffer         = *reinterpret_cast<D3DIndexDataBuffer*>(0xC87FC8);
uint32&              D3DResourceSystem::FreeTextureBufferIndex  = *reinterpret_cast<uint32*>(0xC87FE0);
uint32&              D3DResourceSystem::FreeIndexBufferIndex    = *reinterpret_cast<uint32*>(0xC87FE4);

void D3DResourceSystem::InjectHooks() {
    RH_ScopedClass(D3DResourceSystem);
    RH_ScopedCategory("RenderWare");
    RH_ScopedInstall(SetUseD3DResourceBuffering, 0x730AC0);
    RH_ScopedInstall(TidyUpD3DIndexBuffers, 0x730740);
    RH_ScopedInstall(TidyUpD3DTextures, 0x7305E0);
}

// 0x730900
void D3DResourceSystem::CancelBuffering() {
    plugin::Call<0x730900>();
}

// 0x7307F0
uint32 D3DResourceSystem::GetTotalIndexDataSize() {
    return plugin::CallAndReturn<uint32, 0x7307F0>();
}

// 0x730660
uint32 D3DResourceSystem::GetTotalPixelsSize() {
    return plugin::CallAndReturn<uint32, 0x730660>();
}

// 0x730830
void D3DResourceSystem::Init() {
    plugin::Call<0x730830>();
}

// 0x730AC0
// Writes one byte and preserves EAX high bits, ECX, EDX and the flags as in the EXE.
void D3DResourceSystem::SetUseD3DResourceBuffering(bool enabled) {
    *reinterpret_cast<volatile unsigned char*>(0x8D6084) = enabled;
}

// 0x730A00
void D3DResourceSystem::Shutdown() {
    plugin::Call<0x730A00>();
}

namespace {
// IUnknown::Release (ranura 2 de la vtable, stdcall con `this` en la pila) de un recurso D3D9 (textura o index buffer): el `call dword ptr [edx+8]` del EXE.
// Se llama por la vtable para no depender de d3d9.h (las caches guardan void*).
inline void ReleaseD3DResource(void* resource) {
    using ReleaseFn = unsigned long(__stdcall*)(void*);
    (*static_cast<ReleaseFn**>(resource))[2](resource);
}
// Buffer i-esimo con la aritmetica de 32 bits del EXE (`imul eax,edx,0x1C`; `lea eax,[edx+edx*2]` y `*8`): sin comprobar el rango, como el original.
inline D3DTextureBuffer* TextureBufferAt(int32 index) {
    return reinterpret_cast<D3DTextureBuffer*>(reinterpret_cast<uintptr_t>(&D3DResourceSystem::TextureBuffers[0]) + static_cast<uint32>(index) * sizeof(D3DTextureBuffer));
}
inline D3DIndexDataBuffer* IndexDataBufferAt(int32 index) {
    return reinterpret_cast<D3DIndexDataBuffer*>(reinterpret_cast<uintptr_t>(&D3DResourceSystem::IndexDataBuffers[0]) + static_cast<uint32>(index) * sizeof(D3DIndexDataBuffer));
}
}

// 0x730740
// Libera hasta `count` index buffers. Primero vacia la pila del buffer generico (IndexDataBuffer, 0xC87FC8); cuando esa pila se agota (o encuentra un
// puntero nulo, cuyos contadores ya estan decrementados) recorre en circulo los 16 buffers por tamano (IndexDataBuffers, 0xC87E48) desde
// FreeIndexBufferIndex, que se guarda en cada paso. Tras cada Release el indice se vuelve a leer de la memoria (la llamada puede cambiarlo).
// Termina tras `count` Release o tras 16 pasos seguidos sin liberar nada. No reproduce el residuo de EAX/ECX/EDX: ningun llamador lo lee (todos son C++).
void D3DResourceSystem::TidyUpD3DIndexBuffers(uint32 count) {
    if (count == 0) {
        return;
    }
    uint32 remaining = count;
    // Fase 1: buffer generico.
    while (IndexDataBuffer.m_nSize != 0) {
        const uint32 top = IndexDataBuffer.m_nSize - 1;
        IndexDataBuffer.m_nNumDatasInBuffer = IndexDataBuffer.m_nNumDatasInBuffer - 1;
        IndexDataBuffer.m_nSize = top;
        void* const indexBuffer = IndexDataBuffer.m_apIndexData[top];
        if (!indexBuffer) {
            break;
        }
        ReleaseD3DResource(indexBuffer);
        if (--remaining == 0) {
            return;
        }
    }
    // Fase 2: los 16 buffers por tamano.
    int32 index = static_cast<int32>(FreeIndexBufferIndex);
    int32 emptyScans = 0;
    while (emptyScans < 16) {
        D3DIndexDataBuffer* const buffer = IndexDataBufferAt(index);
        bool released = false;
        if (buffer->m_nSize != 0) {
            const uint32 top = buffer->m_nSize - 1;
            buffer->m_nNumDatasInBuffer = buffer->m_nNumDatasInBuffer - 1;
            buffer->m_nSize = top;
            void* const indexBuffer = buffer->m_apIndexData[top];
            if (indexBuffer) {
                ReleaseD3DResource(indexBuffer);
                index = static_cast<int32>(FreeIndexBufferIndex);
                emptyScans = 0;
                --remaining;
                released = true;
            }
        }
        if (!released) {
            ++emptyScans;
        }
        index = static_cast<int32>(static_cast<uint32>(index) + 1u) % 16;
        FreeIndexBufferIndex = static_cast<uint32>(index);
        if (remaining == 0) {
            return;
        }
    }
}

// 0x7305E0
// Libera hasta `count` texturas: recorre en circulo los NumTextureBuffers buffers de TextureBuffers (0xC87C68) desde FreeTextureBufferIndex y saca de cada
// uno la ultima textura de su pila (decrementa m_nNumTexturesInBuffer y m_nSize antes de liberar, tambien si el puntero es nulo). El indice avanza
// (modulo NumTextureBuffers) tras cada buffer y se guarda en cada paso; tras cada Release el numero de buffers y el indice se vuelven a leer de la memoria.
// Termina tras `count` Release o tras NumTextureBuffers pasos seguidos sin liberar nada (con NumTextureBuffers <= 0 no hace nada). Sin residuo de
// registros: ningun llamador (todos ya en C++) lee EAX/ECX/EDX.
void D3DResourceSystem::TidyUpD3DTextures(uint32 count) {
    if (count == 0) {
        return;
    }
    uint32 remaining = count;
    int32 index = static_cast<int32>(FreeTextureBufferIndex);
    int32 numBuffers = static_cast<int32>(NumTextureBuffers);
    int32 emptyScans = 0;
    while (emptyScans < numBuffers) {
        D3DTextureBuffer* const buffer = TextureBufferAt(index);
        bool released = false;
        if (buffer->m_nSize != 0) {
            const uint32 top = buffer->m_nSize - 1;
            buffer->m_nNumTexturesInBuffer = buffer->m_nNumTexturesInBuffer - 1;
            buffer->m_nSize = top;
            void* const texture = buffer->m_apTextures[top];
            if (texture) {
                ReleaseD3DResource(texture);
                numBuffers = static_cast<int32>(NumTextureBuffers);
                index = static_cast<int32>(FreeTextureBufferIndex);
                emptyScans = 0;
                --remaining;
                released = true;
            }
        }
        if (!released) {
            ++emptyScans;
        }
        index = static_cast<int32>(static_cast<uint32>(index) + 1u) % numBuffers;
        FreeTextureBufferIndex = static_cast<uint32>(index);
        if (remaining == 0) {
            return;
        }
    }
}

// 0x7306A0
int32 D3DResourceSystem::CreateIndexBuffer(uint32 numIndices, uint32 format, void** ppIndexBuffer) {
    return plugin::CallAndReturn<int32, 0x7306A0, uint32, uint32, void**>(numIndices, format, ppIndexBuffer);
}

// 0x730510
int32 D3DResourceSystem::CreateTexture(int32 width, int32 height, uint32 format, void** ppTexture) {
    return plugin::CallAndReturn<int32, 0x730510, int32, int32, uint32, void**>(width, height, format, ppTexture);
}

// 0x730D30
void D3DResourceSystem::DestroyIndexBuffer(void* pIndexBuffer) {
    plugin::Call<0x730D30, void*>(pIndexBuffer);
}

// 0x730B70
void D3DResourceSystem::DestroyTexture(void* texture) {
    plugin::Call<0x730B70, void*>(texture);
}
