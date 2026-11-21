#pragma once

// Registro de errores del ensamblador de shaders de D3DX9 del EXE (lote NEWDEL, ver reversing/FnNEWDEL/README.md). Vive en +4 del objeto fuente
// (D3dxShaderSource). 0x76E3FD lo inicializa y 0x76E41D lo destruye (sin reconstruir; se llaman por direccion). Los textos de error se encadenan por la cabeza:
// el primer dword de cada bloque apunta al anterior y la cadena C empieza en +4.
struct D3dxErrorLog {
    void*  head;       // +0x00 ultimo texto anadido
    uint32 totalSize;  // +0x04 longitud total de los textos (sin terminador)
    uint32 errorCount; // +0x08 numero de errores (0x76E664 lo devuelve: 4 bytes, sin gancho)
    uint32 pad[5];

    // 0x76E5FB (__thiscall, ret 4): copia los textos (en orden cronologico, el bloque mas reciente al final) a un ID3DXBuffer nuevo con terminador nulo.
    // Sin ppBuffer devuelve 0 sin hacer nada; sin textos deja *ppBuffer = nullptr. Devuelve HRESULT (D3DXCreateBuffer, 0x780E8C, si falla).
    int32 ToBuffer(void** ppBuffer);
    // 0x76E664 (mov eax,[ecx+8]; ret): 4 bytes seguidos de otra funcion de 4 bytes: no admite gancho de 5 bytes. Reconstruida solo como ayudante.
    uint32 GetErrorCount() const { return errorCount; }

    static void InjectHooks();
};
static_assert(sizeof(D3dxErrorLog) == 0x20);
