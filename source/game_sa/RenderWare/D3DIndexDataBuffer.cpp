#include "StdInc.h"

#include "D3DIndexDataBuffer.h"
#include "D3DResourceSystem.h"

namespace {
// D3DINDEXBUFFER_DESC (IDirect3DIndexBuffer9::GetDesc, ranura 13 de la vtable): 20 bytes que el dispositivo escribe enteros.
struct IndexBufferDesc {
    uint32 Format, Type, Usage, Pool, Size;
};
using GetDescFn = long(__stdcall*)(void*, IndexBufferDesc*);

// IDirect3DIndexBuffer9::GetDesc (ranura 13): el `call dword ptr [ecx+34h]` del EXE.
inline void GetDesc(void* indexBuffer, IndexBufferDesc* desc) {
    reinterpret_cast<GetDescFn>(D3DResourceCom::VTable(indexBuffer)[13])(indexBuffer, desc);
}
} // namespace

void D3DIndexDataBuffer::InjectHooks() {
    RH_ScopedClass(D3DIndexDataBuffer);
    RH_ScopedCategory("RenderWare");

    RH_ScopedOverloadedInstall(Pop, "Size", 0x730270, IndexData * (D3DIndexDataBuffer::*)(uint32));
    RH_ScopedInstall(Resize, 0x730330);
    RH_ScopedInstall(GetTotalDataSize, 0x7303B0);
}

// 0x730270
// Entrega el index buffer guardado MAS PEQUENO cuyo tamano (GetDesc.Size, en bytes) sea al menos `indexCount * 2` (32 bits) y menor que 0x7FFFFFFF; recorre la pila de la ultima a la
// primera (el numero de buffers se lee una sola vez) y, ante un empate de tamano, se queda con el primero que encontro (el de mayor posicion). Lo saca de la pila (el hueco lo ocupa
// el ultimo, sin comprobar el rango; m_nNumDatasInBuffer no cambia) y lo devuelve; si ninguno sirve, devuelve nulo.
D3DIndexDataBuffer::IndexData* D3DIndexDataBuffer::Pop(uint32 indexCount) {
    uint32 index = m_nSize;
    int32 best = -1;
    uint32 bestSize = 0x7FFFFFFFu;
    const uint32 minimumSize = indexCount * 2u;
    if (index == 0) {
        return nullptr;
    }
    do {
        IndexData* const indexBuffer = m_apIndexData[index - 1];
        --index;
        IndexBufferDesc desc;
        GetDesc(indexBuffer, &desc);
        if (desc.Size < bestSize && desc.Size >= minimumSize) {
            bestSize = desc.Size;
            best = static_cast<int32>(index);
        }
    } while (index != 0);
    if (best == -1) {
        return nullptr;
    }
    IndexData** const stack = m_apIndexData;
    IndexData* const chosen = stack[best];
    const uint32 last = m_nSize - 1;
    m_nSize = last;
    if (static_cast<int32>(last) > 0 && static_cast<int32>(last) != best) {
        stack[best] = stack[last];
    }
    return chosen;
}

// 0x730330
// Cambia la capacidad: reserva la pila nueva (operator new, `newCapacity * 4` en 32 bits), copia los index buffers que caben (m_nSize se relee en cada paso, comparaciones con
// signo), libera (Release) los que no caben, libera la pila vieja (operator delete 0x8214BD, tambien si es nula) y guarda el puntero y la capacidad. NO actualiza m_nSize.
// Sin cambios si la capacidad es la misma.
void D3DIndexDataBuffer::Resize(uint32 newCapacity) {
    if (newCapacity == m_nCapcacity) {
        return;
    }
    IndexData** const newStack = static_cast<IndexData**>(::operator new(newCapacity * 4u));
    const int32 capacity = static_cast<int32>(newCapacity);
    for (int32 i = 0; static_cast<int32>(m_nSize) > i && i < capacity; ++i) {
        newStack[i] = m_apIndexData[i];
    }
    if (capacity < static_cast<int32>(m_nSize)) {
        for (int32 i = capacity; i < static_cast<int32>(m_nSize); ++i) {
            D3DResourceCom::Release(m_apIndexData[i]);
        }
    }
    ::operator delete(m_apIndexData);
    m_apIndexData = newStack;
    m_nCapcacity = newCapacity;
}

// 0x7303B0
// Suma de bytes (GetDesc.Size) de los index buffers guardados. Formato 0: la de todos, en 32 bits. Otro formato: m_nSize por el tamano del primero (en 32 bits; se supone que
// todos son iguales). Sin buffers devuelve 0. El numero de buffers y los punteros se vuelven a leer de memoria tras cada llamada COM.
uint32 D3DIndexDataBuffer::GetTotalDataSize() {
    uint32 total = 0;
    if (m_nFormat == 0) {
        for (int32 i = 0; i < static_cast<int32>(m_nSize); ++i) {
            IndexBufferDesc desc;
            GetDesc(m_apIndexData[i], &desc);
            total += desc.Size;
        }
        return total;
    }
    if (m_nSize == 0) {
        return 0;
    }
    IndexBufferDesc desc;
    GetDesc(m_apIndexData[0], &desc);
    return m_nSize * desc.Size;
}
