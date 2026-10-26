/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: D3DResourceSystem
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 12/12 entradas.
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
    RH_ScopedInstall(CancelBuffering, 0x730900);
    RH_ScopedInstall(CreateIndexBuffer, 0x7306A0);
    RH_ScopedInstall(CreateTexture, 0x730510);
    RH_ScopedInstall(DestroyIndexBuffer, 0x730D30);
    RH_ScopedInstall(DestroyTexture, 0x730B70);
    RH_ScopedInstall(Init, 0x730830);
    RH_ScopedInstall(Shutdown, 0x730A00);
    RH_ScopedInstall(GetTotalIndexDataSize, 0x7307F0);
    RH_ScopedInstall(GetTotalPixelsSize, 0x730660);
}

namespace {
// IUnknown::Release (ranura 2 de la vtable, stdcall con `this` en la pila) de un recurso D3D9 (textura o index buffer): el `call dword ptr [edx+8]` del EXE.
// Se llama por la vtable para no depender de d3d9.h (las caches guardan void*).
inline void ReleaseD3DResource(void* resource) {
    using ReleaseFn = unsigned long(__stdcall*)(void*);
    (*static_cast<ReleaseFn**>(resource))[2](resource);
}
// Tabla de metodos de un objeto COM (cada metodo es stdcall con `this` como primer argumento).
inline void** VTable(void* object) {
    return *static_cast<void***>(object);
}
// IDirect3DDevice9 global del EXE (_RwD3DDevice, 0xC97C28).
inline void* D3DDevice() {
    return *reinterpret_cast<void**>(0xC97C28);
}
// free() de la CRT del EXE (0x8214BD), con el que se liberan las pilas de los buffers.
inline void ExeFree(void* memory) {
    plugin::Call<0x8214BD, void*>(memory);
}
// Metodos de D3DTextureBuffer / D3DIndexDataBuffer que siguen siendo del EXE (sin definicion en C++): colaboradores.
inline void* TextureBufferPop(D3DTextureBuffer* buffer, uint32 format, int32 width, int32 height, int32 levels) { // 0x72FF60
    return plugin::CallMethodAndReturn<void*, 0x72FF60, D3DTextureBuffer*, uint32, int32, int32, int32>(buffer, format, width, height, levels);
}
inline void TextureBufferSetup(D3DTextureBuffer* buffer, uint32 format, int32 width, int32 levels, uint32 capacity) { // 0x72FE80
    plugin::CallMethod<0x72FE80, D3DTextureBuffer*, uint32, int32, int32, uint32>(buffer, format, width, levels, capacity);
}
inline bool TextureBufferPush(D3DTextureBuffer* buffer, void* texture) { // 0x72FFF0
    return plugin::CallMethodAndReturn<bool, 0x72FFF0, D3DTextureBuffer*, void*>(buffer, texture);
}
inline bool TextureBufferPushWithoutIncreasingCounter(D3DTextureBuffer* buffer, void* texture) { // 0x730AD0
    return plugin::CallMethodAndReturn<bool, 0x730AD0, D3DTextureBuffer*, void*>(buffer, texture);
}
inline void TextureBufferResize(D3DTextureBuffer* buffer, uint32 capacity) { // 0x730020
    plugin::CallMethod<0x730020, D3DTextureBuffer*, uint32>(buffer, capacity);
}
inline void* IndexDataBufferPop(D3DIndexDataBuffer* buffer, uint32 indexCount) { // 0x730270
    return plugin::CallMethodAndReturn<void*, 0x730270, D3DIndexDataBuffer*, uint32>(buffer, indexCount);
}
inline void IndexDataBufferResize(D3DIndexDataBuffer* buffer, uint32 capacity) { // 0x730330
    plugin::CallMethod<0x730330, D3DIndexDataBuffer*, uint32>(buffer, capacity);
}
// Buffer i-esimo con la aritmetica de 32 bits del EXE (`imul eax,edx,0x1C`; `lea eax,[edx+edx*2]` y `*8`): sin comprobar el rango, como el original.
inline D3DTextureBuffer* TextureBufferAt(int32 index) {
    return reinterpret_cast<D3DTextureBuffer*>(reinterpret_cast<uintptr_t>(&D3DResourceSystem::TextureBuffers[0]) + static_cast<uint32>(index) * sizeof(D3DTextureBuffer));
}
inline D3DIndexDataBuffer* IndexDataBufferAt(int32 index) {
    return reinterpret_cast<D3DIndexDataBuffer*>(reinterpret_cast<uintptr_t>(&D3DResourceSystem::IndexDataBuffers[0]) + static_cast<uint32>(index) * sizeof(D3DIndexDataBuffer));
}
inline uint32 TextureBufferGetTotalDataSize(D3DTextureBuffer* buffer) { // 0x7300A0
    return plugin::CallMethodAndReturn<uint32, 0x7300A0, D3DTextureBuffer*>(buffer);
}
inline uint32 IndexDataBufferGetTotalDataSize(D3DIndexDataBuffer* buffer) { // 0x7303B0
    return plugin::CallMethodAndReturn<uint32, 0x7303B0, D3DIndexDataBuffer*>(buffer);
}
inline void StoreResource(void* array, int32 index, void* resource) {
    static_cast<void**>(array)[index] = resource;
}
}

// 0x730900
// Libera todos los recursos D3D guardados en las caches (texturas, index buffers por tamano y generico), pone a cero sus contadores y libera las pilas
// con free(). Las longitudes y el numero de buffers se vuelven a leer de memoria en cada paso (Release puede reentrar), como el EXE.
void D3DResourceSystem::CancelBuffering() {
    for (int32 i = 0; i < static_cast<int32>(NumTextureBuffers); ++i) {
        D3DTextureBuffer* const buffer = TextureBufferAt(i);
        for (int32 j = 0; j < static_cast<int32>(buffer->m_nSize); ++j) {
            ReleaseD3DResource(buffer->m_apTextures[j]);
        }
        buffer->m_nSize = 0;
        buffer->m_nNumTexturesInBuffer = 0;
        if (buffer->m_apTextures) {
            ExeFree(buffer->m_apTextures);
            buffer->m_apTextures = nullptr;
        }
    }
    NumTextureBuffers = 0;
    for (int32 i = 0; i < 16; ++i) {
        D3DIndexDataBuffer* const buffer = IndexDataBufferAt(i);
        for (int32 j = 0; j < static_cast<int32>(buffer->m_nSize); ++j) {
            ReleaseD3DResource(buffer->m_apIndexData[j]);
        }
        buffer->m_nSize = 0;
        buffer->m_nNumDatasInBuffer = 0;
        if (buffer->m_apIndexData) {
            ExeFree(buffer->m_apIndexData);
            buffer->m_apIndexData = nullptr;
        }
    }
    for (int32 j = 0; j < static_cast<int32>(IndexDataBuffer.m_nSize); ++j) {
        ReleaseD3DResource(IndexDataBuffer.m_apIndexData[j]);
    }
    auto* const genericStack = IndexDataBuffer.m_apIndexData;
    IndexDataBuffer.m_nSize = 0;
    IndexDataBuffer.m_nNumDatasInBuffer = 0;
    if (genericStack) {
        ExeFree(genericStack);
        IndexDataBuffer.m_apIndexData = nullptr;
    }
    UseD3DResourceBuffering = false;
}

// 0x7307F0
// Suma D3DIndexDataBuffer::GetTotalDataSize (0x7303B0, del EXE) de los 16 buffers por tamano, del ultimo (0xC87FB0) al primero (0xC87E48), y despues la del generico (0xC87FC8).
// El EXE no devuelve nada mas que la suma (EAX); ningun llamador directo (codigo muerto en el EXE).
uint32 D3DResourceSystem::GetTotalIndexDataSize() {
    uint32 total = 0;
    for (int32 i = 15; i >= 0; --i) {
        total += IndexDataBufferGetTotalDataSize(IndexDataBufferAt(i));
    }
    return IndexDataBufferGetTotalDataSize(&IndexDataBuffer) + total;
}

// 0x730660
// Suma D3DTextureBuffer::GetTotalDataSize (0x7300A0, del EXE) de los NumTextureBuffers buffers, del ultimo al primero. El contador se lee una sola vez (ebx) y el bucle es un
// `dec`/`jne` sin signo: con 0 no hay llamadas. El primer puntero es 0xC87C68 + n * 0x1C con la aritmetica de 32 bits del EXE (sin comprobar el rango). Ningun llamador directo.
uint32 D3DResourceSystem::GetTotalPixelsSize() {
    uint32 count = NumTextureBuffers;
    uint32 total = 0;
    uintptr_t buffer = reinterpret_cast<uintptr_t>(&TextureBuffers[0]) + count * static_cast<uint32>(sizeof(D3DTextureBuffer));
    if (count) {
        do {
            buffer -= sizeof(D3DTextureBuffer);
            total += TextureBufferGetTotalDataSize(reinterpret_cast<D3DTextureBuffer*>(buffer));
        } while (--count);
    }
    return total;
}

// 0x730830
// Prepara las caches: un buffer de texturas generico (formato 0, ancho 0, niveles -1) y 16 buffers de index por tamano (100, 200, ... 1600 indices)
// mas el generico, todos con una pila inicial de 16 punteros (0x40 bytes).
void D3DResourceSystem::Init() {
    NumTextureBuffers = 1;
    UseD3DResourceBuffering = true;
    TextureBuffers[0].m_nFormat = 0;
    TextureBuffers[0].m_nWidth = 0;
    TextureBuffers[0].m_nLevels = -1;
    TextureBuffers[0].m_apTextures = static_cast<decltype(TextureBuffers[0].m_apTextures)>(::operator new(0x40));
    TextureBuffers[0].m_nCapcacity = 0x10;
    TextureBuffers[0].m_nSize = 0;
    TextureBuffers[0].m_nNumTexturesInBuffer = 0;
    int32 size = 100;
    for (int32 i = 0; i < 16; ++i) {
        D3DIndexDataBuffer* const buffer = IndexDataBufferAt(i);
        buffer->m_nFormat = 0x65;
        buffer->field_4 = size;
        buffer->m_apIndexData = static_cast<decltype(buffer->m_apIndexData)>(::operator new(0x40));
        buffer->m_nCapcacity = 0x10;
        buffer->m_nSize = 0;
        buffer->m_nNumDatasInBuffer = 0;
        size += 100;
    }
    IndexDataBuffer.m_nFormat = 0x65;
    IndexDataBuffer.field_4 = 0;
    IndexDataBuffer.m_apIndexData = static_cast<decltype(IndexDataBuffer.m_apIndexData)>(::operator new(0x40));
    IndexDataBuffer.m_nCapcacity = 0x10;
    IndexDataBuffer.m_nSize = 0;
    IndexDataBuffer.m_nNumDatasInBuffer = 0;
}

// 0x730AC0
// Writes one byte and preserves EAX high bits, ECX, EDX and the flags as in the EXE.
void D3DResourceSystem::SetUseD3DResourceBuffering(bool enabled) {
    *reinterpret_cast<volatile unsigned char*>(0x8D6084) = enabled;
}

// 0x730A00
// Como CancelBuffering, pero conserva las pilas (sin free) y el indicador de buffering.
void D3DResourceSystem::Shutdown() {
    for (int32 i = 0; i < static_cast<int32>(NumTextureBuffers); ++i) {
        D3DTextureBuffer* const buffer = TextureBufferAt(i);
        for (int32 j = 0; j < static_cast<int32>(buffer->m_nSize); ++j) {
            ReleaseD3DResource(buffer->m_apTextures[j]);
        }
        buffer->m_nSize = 0;
        buffer->m_nNumTexturesInBuffer = 0;
    }
    NumTextureBuffers = 0;
    for (int32 i = 0; i < 16; ++i) {
        D3DIndexDataBuffer* const buffer = IndexDataBufferAt(i);
        for (int32 j = 0; j < static_cast<int32>(buffer->m_nSize); ++j) {
            ReleaseD3DResource(buffer->m_apIndexData[j]);
        }
        buffer->m_nSize = 0;
        buffer->m_nNumDatasInBuffer = 0;
    }
    for (int32 j = 0; j < static_cast<int32>(IndexDataBuffer.m_nSize); ++j) {
        ReleaseD3DResource(IndexDataBuffer.m_apIndexData[j]);
    }
    IndexDataBuffer.m_nSize = 0;
    IndexDataBuffer.m_nNumDatasInBuffer = 0;
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
// Entrega un index buffer de la cache del tamano pedido (grupos de 100 indices; a partir de 16 grupos usa el buffer generico, 0x730270) o crea uno
// nuevo en el dispositivo (Length = indices * 2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED). `format` no se usa. Con un buffer de la
// cache por tamano el EXE llama a GetDesc y descarta el resultado (se conserva la llamada); con uno del buffer generico no.
int32 D3DResourceSystem::CreateIndexBuffer(uint32 numIndices, uint32 format, void** ppIndexBuffer) {
    (void)format;
    using GetDescFn = long(__stdcall*)(void*, void*);
    // `dec ecx` + division con signo por 100 (imul 0x51EB851F) y `imul esi,esi,100`: aritmetica de 32 bits con envoltura, sin comportamiento indefinido.
    const int32 group = static_cast<int32>(numIndices - 1u) / 100;
    const int32 indices = static_cast<int32>(static_cast<uint32>(group + 1) * 100u);
    void* indexBuffer;
    if (group < 16) {
        D3DIndexDataBuffer* const buffer = IndexDataBufferAt(group);
        if (buffer->m_nSize != 0) {
            buffer->m_nNumDatasInBuffer = buffer->m_nNumDatasInBuffer - 1;
            const uint32 top = buffer->m_nSize - 1;
            buffer->m_nSize = top;
            indexBuffer = buffer->m_apIndexData[top];
        } else {
            indexBuffer = nullptr;
        }
        *ppIndexBuffer = indexBuffer;
    } else {
        indexBuffer = IndexDataBufferPop(&IndexDataBuffer, static_cast<uint32>(indices));
        *ppIndexBuffer = indexBuffer;
        if (indexBuffer) {
            return 0;   // 0x730714: `jne 0x730700` salta el GetDesc (solo lo llama la ruta de la cache por tamano)
        }
    }
    if (indexBuffer) {
        uint32 desc[5];
        reinterpret_cast<GetDescFn>(VTable(indexBuffer)[13])(indexBuffer, desc);
        return 0;
    }
    void* const device = D3DDevice();
    using CreateIndexBufferFn = long(__stdcall*)(void*, uint32, uint32, uint32, uint32, void**, void*);
    return reinterpret_cast<CreateIndexBufferFn>(VTable(device)[27])(device, static_cast<uint32>(indices) * 2, 8, 0x65, 1, ppIndexBuffer, nullptr);
}

// 0x730510
// Entrega una textura de la cache (buffer con el mismo ancho, formato y niveles cuando la textura es cuadrada; si no, el buffer generico) o la crea en el
// dispositivo (D3DUSAGE 0, D3DPOOL_MANAGED). `levels` > 1 se trata como 0 (cadena completa). Si el buffer coincidente esta vacio no se prueba el generico.
int32 D3DResourceSystem::CreateTexture(int32 width, int32 height, int32 levels, uint32 format, void** ppTexture) {
    if (levels > 1) {
        levels = 0;
    }
    D3DTextureBuffer* match = nullptr;
    if (width == height) {
        const int32 count = static_cast<int32>(NumTextureBuffers);
        if (count > 1) {
            // `fild width` + `fstp dword`: el ancho se redondea a float EN MEMORIA (volatile: /fp:fast eliminaria el redondeo); el del buffer entra con `fild` (exacto) y `fcomp` compara
            // sin redondear: se compara en double.
            volatile float widthF = static_cast<float>(width);
            for (int32 i = 1; i < count; ++i) {
                D3DTextureBuffer* const buffer = TextureBufferAt(i);
                if (static_cast<double>(static_cast<int32>(buffer->m_nWidth)) == static_cast<double>(widthF)
                    && levels == buffer->m_nLevels
                    && format == buffer->m_nFormat) {
                    match = buffer;
                    break;
                }
            }
        }
    }
    void* texture;
    if (match) {
        if (match->m_nSize != 0) {
            const uint32 top = match->m_nSize - 1;
            match->m_nNumTexturesInBuffer = match->m_nNumTexturesInBuffer - 1;
            match->m_nSize = top;
            texture = match->m_apTextures[top];
        } else {
            texture = nullptr;
        }
    } else {
        texture = TextureBufferPop(&TextureBuffer, format, width, height, levels);
    }
    *ppTexture = texture;
    if (texture) {
        return 0;
    }
    void* const device = D3DDevice();
    using CreateTextureFn = long(__stdcall*)(void*, int32, int32, int32, uint32, uint32, uint32, void**, void*);
    return reinterpret_cast<CreateTextureFn>(VTable(device)[23])(device, width, height, levels, 0, format, 1, ppTexture, nullptr);
}

// 0x730D30
// Devuelve un index buffer a la cache del grupo que le corresponde por su tamano (GetDesc.Size / 2, en grupos de 100 indices) o al generico a partir de
// 16 grupos; si la cache esta desactivada lo libera. Crece la pila (Resize, 0x730330) cuando esta llena y, si sigue llena, lo descarta sin liberarlo.
void D3DResourceSystem::DestroyIndexBuffer(void* pIndexBuffer) {
    if (!UseD3DResourceBuffering) {
        ReleaseD3DResource(pIndexBuffer);
        return;
    }
    uint32 desc[5];
    using GetDescFn = long(__stdcall*)(void*, void*);
    reinterpret_cast<GetDescFn>(VTable(pIndexBuffer)[13])(pIndexBuffer, desc);
    const uint32 group = ((desc[4] >> 1) - 1) / 100;
    if (static_cast<int32>(group) < 16) {
        D3DIndexDataBuffer* const buffer = IndexDataBufferAt(static_cast<int32>(group));
        buffer->m_nNumDatasInBuffer = buffer->m_nNumDatasInBuffer + 1;
        if (static_cast<int32>(buffer->m_nSize) >= static_cast<int32>(buffer->m_nCapcacity)) {
            IndexDataBufferResize(buffer, buffer->m_nCapcacity * 2);
            if (static_cast<int32>(buffer->m_nSize) >= static_cast<int32>(buffer->m_nCapcacity)) {
                return;
            }
        }
        StoreResource(buffer->m_apIndexData, static_cast<int32>(buffer->m_nSize), pIndexBuffer);
        buffer->m_nSize = buffer->m_nSize + 1;
        return;
    }
    IndexDataBuffer.m_nNumDatasInBuffer = IndexDataBuffer.m_nNumDatasInBuffer + 1;
    if (static_cast<int32>(IndexDataBuffer.m_nSize) >= static_cast<int32>(IndexDataBuffer.m_nCapcacity)) {
        IndexDataBufferResize(&IndexDataBuffer, IndexDataBuffer.m_nCapcacity * 2);
        if (static_cast<int32>(IndexDataBuffer.m_nSize) >= static_cast<int32>(IndexDataBuffer.m_nCapcacity)) {
            return;
        }
    }
    StoreResource(IndexDataBuffer.m_apIndexData, static_cast<int32>(IndexDataBuffer.m_nSize), pIndexBuffer);
    IndexDataBuffer.m_nSize = IndexDataBuffer.m_nSize + 1;
}

// 0x730B70
// Devuelve una textura a la cache: la de un buffer con su mismo ancho/formato/niveles (cuadrada, 32..256 de lado), uno nuevo si hay menos de 17, o el
// buffer generico (lados <= 256). El resto, y todo con la cache desactivada, se libera.
void D3DResourceSystem::DestroyTexture(void* texture) {
    if (!UseD3DResourceBuffering) {
        ReleaseD3DResource(texture);
        return;
    }
    using GetLevelCountFn = uint32(__stdcall*)(void*);
    using GetLevelDescFn = long(__stdcall*)(void*, uint32, void*);
    const uint32 levelCount = reinterpret_cast<GetLevelCountFn>(VTable(texture)[13])(texture);
    const int32 singleLevel = levelCount > 1 ? 0 : 1;
    uint32 desc[8]; // D3DSURFACE_DESC: Format (0) ... Width (6), Height (7)
    reinterpret_cast<GetLevelDescFn>(VTable(texture)[17])(texture, 0, desc);
    const uint32 format = desc[0];
    const uint32 width  = desc[6];
    const uint32 height = desc[7];
    const int32 count = static_cast<int32>(NumTextureBuffers);
    if (width == height && count > 1) {
        // `fild width`, `fadd 2^32` si es negativo (0x858C54) y un unico `fstp dword`: el redondeo a float es unico sobre el valor sin signo; el ancho del buffer se compara
        // exacto (`fild` + `fcomp`), por lo que se compara en double.
        volatile float widthF = static_cast<float>(static_cast<double>(width));   // volatile: el `fstp dword` redondea a float en memoria (/fp:fast lo eliminaria)
        for (int32 i = 1; i < count; ++i) {
            D3DTextureBuffer* const buffer = TextureBufferAt(i);
            if (static_cast<double>(static_cast<int32>(buffer->m_nWidth)) == static_cast<double>(widthF)
                && format == buffer->m_nFormat
                && singleLevel == buffer->m_nLevels) {
                buffer->m_nNumTexturesInBuffer = buffer->m_nNumTexturesInBuffer + 1;
                if (static_cast<int32>(buffer->m_nSize) < static_cast<int32>(buffer->m_nCapcacity)) {
                    StoreResource(buffer->m_apTextures, static_cast<int32>(buffer->m_nSize), texture);
                    buffer->m_nSize = buffer->m_nSize + 1;
                } else {
                    TextureBufferResize(buffer, buffer->m_nCapcacity * 2);
                    TextureBufferPushWithoutIncreasingCounter(buffer, texture);
                }
                return;
            }
        }
    }
    if (count < 17 && width == height && width >= 0x20) {
        if (width > 0x100) {
            ReleaseD3DResource(texture);
            return;
        }
        TextureBufferSetup(TextureBufferAt(count), format, static_cast<int32>(width), singleLevel, 0x10);
        const int32 next = static_cast<int32>(NumTextureBuffers);
        TextureBufferPush(TextureBufferAt(next), texture);
        NumTextureBuffers = static_cast<uint32>(next + 1);
        return;
    }
    if (width > 0x100 || height > 0x100) {
        ReleaseD3DResource(texture);
        return;
    }
    TextureBuffer.m_nNumTexturesInBuffer = TextureBuffer.m_nNumTexturesInBuffer + 1;
    if (static_cast<int32>(TextureBuffer.m_nSize) >= static_cast<int32>(TextureBuffer.m_nCapcacity)) {
        TextureBufferResize(&TextureBuffer, TextureBuffer.m_nCapcacity * 2);
        if (static_cast<int32>(TextureBuffer.m_nSize) >= static_cast<int32>(TextureBuffer.m_nCapcacity)) {
            return;
        }
    }
    StoreResource(TextureBuffer.m_apTextures, static_cast<int32>(TextureBuffer.m_nSize), texture);
    TextureBuffer.m_nSize = TextureBuffer.m_nSize + 1;
}
