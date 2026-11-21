/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAudioZones
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "game_sa/Crt/CrtState.h"
#include "game_sa/Crt/CrtMath.h"
#include "game_sa/Crt/CrtFile.h"

#include "AudioZones.h"

#include <intrin.h>

namespace {
// Cadena de texto del volcado de depuracion de las zonas de audio (0x5089B0..0x509600 del EXE): puntero a un bloque del CRT y longitud. Sin nombre en el EXE: el listado de pendientes atribuye estas funciones
// a CAudioZones por cercania.
struct TextBufferRaw {
    char*  data;
    uint32 length;
};

// 0x508930 (lote 172A, banco Fn172A; thiscall, dos dwords de pila, `ret 8`, 124 bytes): agrega `count` bytes de `source` al final de la cadena. Como el original: reserva length + count bytes con operator new
// (0x82119A), los pone a cero (dwords y luego bytes sueltos: rep stosd / rep stosb), copia la cadena actual y despues `source` (rep movsd / rep movsb; se usan los intrinsecos para conservar el orden y la
// granularidad aunque los rangos se solapen), libera el bloque anterior con operator delete (0x8214BD) y deja bloque y longitud (la longitud se relee de la cadena y suma `count`).
__declspec(noinline) void __fastcall AudioZonesTextBufferAppend(TextBufferRaw* self, void* /*edx*/, const void* source, uint32 count) {
    const uint32 total = self->length + count;
    uint8* const block = static_cast<uint8*>(::operator new(total));
    __stosd(reinterpret_cast<unsigned long*>(block), 0, total >> 2);
    __stosb(block + (total >> 2) * 4, 0, total & 3);
    {
        const uint32 length = self->length;
        __movsd(reinterpret_cast<unsigned long*>(block), reinterpret_cast<const unsigned long*>(self->data), length >> 2);
        __movsb(block + (length >> 2) * 4, reinterpret_cast<const unsigned char*>(self->data) + (length >> 2) * 4, length & 3);
    }
    {
        uint8* const tail = block + self->length;
        __movsd(reinterpret_cast<unsigned long*>(tail), static_cast<const unsigned long*>(source), count >> 2);
        __movsb(tail + (count >> 2) * 4, static_cast<const unsigned char*>(source) + (count >> 2) * 4, count & 3);
    }
    ::operator delete(self->data);
    const uint32 length = self->length;
    self->data = reinterpret_cast<char*>(block);
    self->length = length + count;
}

// 0x508A10 y, copia identica, 0x509140 (lote 172A, banco Fn172A; thiscall, sin argumentos de pila, `ret`, 17 bytes cada una): vacia la cadena. Como el original: solo si el puntero no es nulo, libera el bloque
// con operator delete (0x8214BD) y pone puntero y longitud a cero (con puntero nulo no toca la longitud).
__declspec(noinline) void __fastcall AudioZonesTextBufferClear(TextBufferRaw* self, void* /*edx*/) {
    if (self->data) {
        ::operator delete(self->data);
        self->data = nullptr;
        self->length = 0;
    }
}

// 0x5089B0 (lote S2A, sin validar; thiscall, tres argumentos de pila (etiqueta, texto, longitud), `ret 0Ch`, 85 bytes): agrega una entrada {etiqueta, longitud, texto sin terminador} a la cadena. Como el
// original: reserva longitud + 8 bytes con operator new (0x82119A), escribe etiqueta (+0) y longitud (+4), copia el texto desde +8 (rep movsd / rep movsb), agrega el bloque entero con 0x508930
// (AudioZonesTextBufferAppend) y lo libera con operator delete (0x8214BD).
__declspec(noinline) void __fastcall AudioZonesTextBufferAppendTagged(TextBufferRaw* self, void* /*edx*/, uint32 tag, const void* text, uint32 length) {
    uint8* const block = static_cast<uint8*>(::operator new(length + 8));
    *reinterpret_cast<uint32*>(block + 4) = length;
    *reinterpret_cast<uint32*>(block)     = tag;
    __movsd(reinterpret_cast<unsigned long*>(block + 8), static_cast<const unsigned long*>(text), length >> 2);
    __movsb(block + 8 + (length >> 2) * 4, static_cast<const unsigned char*>(text) + (length >> 2) * 4, length & 3);
    AudioZonesTextBufferAppend(self, nullptr, block, length + 8);
    ::operator delete(block);
}

// 0x508B80 (lote S2A, sin validar; thiscall, un entero de pila, `ret 4`, 142 bytes): formatea el entero con el sprintf del EXE (0x821BB5, "%d" de 0x859508) en un bufer local de 0x800 bytes y agrega
// la cadena como entrada de tipo 0xB (AudioZonesTextBufferAppendTagged, con la longitud hasta el terminador).
__declspec(noinline) void __fastcall AudioZonesTextBufferAddInt(TextBufferRaw* self, void* /*edx*/, int32 value) {
    char formatted[0x800];
    reinterpret_cast<int(__cdecl*)(char*, const char*, ...)>(0x821BB5)(formatted, "%d", value);
    AudioZonesTextBufferAppendTagged(self, nullptr, 0xB, formatted, static_cast<uint32>(strlen(formatted)));
}

// 0x508D30 (lote S2A, sin validar; thiscall, dos cadenas de pila (clave, tipo), `ret 8`, 156 bytes): agrega una entrada de tipo 0x11 con el bloque {longitud de la clave, clave, longitud del tipo, tipo}
// (sin terminadores; ambas longitudes se miden primero, luego operator new de suma + 8 bytes, copias con rep movsd / rep movsb), con AudioZonesTextBufferAppendTagged, y libera el bloque.
__declspec(noinline) void __fastcall AudioZonesTextBufferAddTyped(TextBufferRaw* self, void* /*edx*/, const char* key, const char* type) {
    const uint32 keyLength  = static_cast<uint32>(strlen(key));
    const uint32 typeLength = static_cast<uint32>(strlen(type));
    const uint32 size       = typeLength + keyLength + 8;
    uint8* const block      = static_cast<uint8*>(::operator new(size));
    *reinterpret_cast<uint32*>(block) = keyLength;
    __movsd(reinterpret_cast<unsigned long*>(block + 4), reinterpret_cast<const unsigned long*>(key), keyLength >> 2);
    __movsb(block + 4 + (keyLength >> 2) * 4, reinterpret_cast<const unsigned char*>(key) + (keyLength >> 2) * 4, keyLength & 3);
    *reinterpret_cast<uint32*>(block + keyLength + 4) = typeLength;
    __movsd(reinterpret_cast<unsigned long*>(block + keyLength + 8), reinterpret_cast<const unsigned long*>(type), typeLength >> 2);
    __movsb(block + keyLength + 8 + (typeLength >> 2) * 4, reinterpret_cast<const unsigned char*>(type) + (typeLength >> 2) * 4, typeLength & 3);
    AudioZonesTextBufferAppendTagged(self, nullptr, 0x11, block, size);
    ::operator delete(block);
}

// Registro de informes de "bugstar.cnf" (objeto distinto de TextBufferRaw: lo que se le pasa a 0x509190 es el objeto de informe): +4 = objeto de E/S con vtabla (ranura 1 abrir, ranura 2 cerrar,
// ranura 3 escribir; thiscall con ECX = el objeto de E/S, los argumentos de pila los limpia el llamado), +8 = argumento de apertura, +0xC = manejador, +0x9A = cadena (nombre).
struct BugstarReportRaw {
    void* unknown0;
    struct IoRaw {
        struct VTable {
            void* slot0;
            int32 (__thiscall* open)(IoRaw* io, int32 argument, char* name);                     // +4
            int32 (__thiscall* close)(IoRaw* io, int32 handle);                                  // +8
            int32 (__thiscall* write)(IoRaw* io, int32 handle, const void* buffer, int32 size);  // +0xC
        }* vtable;
    }* io;
    int32 openArgument; // +8
    int32 handle;       // +0xC
    uint8 padding[0x9A - 0x10];
    char  name[1];      // +0x9A
};
static_assert(offsetof(BugstarReportRaw, io) == 4 && offsetof(BugstarReportRaw, handle) == 0xC && offsetof(BugstarReportRaw, name) == 0x9A);

// 0x509190 (lote S2A, sin validar; thiscall, tres argumentos de pila (entero, bloque, longitud), `ret 0Ch`, 152 bytes): abre el informe con io->abrir(+8 del informe, nombre en +0x9A) y guarda el
// manejador en +0xC; si es 0 devuelve 0 sin mas. Si no escribe tres dwords con io->escribir(manejador, buffer, 4): el valor 4, el primer argumento de pila (con tamano igual al valor local, que sigue
// siendo 4) y 0; despues llama a 0x508EB0 (thiscall con el informe: bloque y longitud, `ret 8`, sin reconstruir), cierra con io->cerrar(manejador), pone +0xC a 0 y devuelve 0.
__declspec(noinline) int32 __fastcall BugstarFlushReport(BugstarReportRaw* self, void* /*edx*/, int32 firstValue, const char* data, uint32 length) {
    self->handle = self->io->vtable->open(self->io, self->openArgument, self->name);
    if (self->handle == 0) {
        return 0;
    }
    int32 four = 4;
    self->io->vtable->write(self->io, self->handle, &four, 4);
    self->io->vtable->write(self->io, self->handle, &firstValue, four);
    int32 zero = 0;
    self->io->vtable->write(self->io, self->handle, &zero, 4);
    plugin::CallMethod<0x508EB0, BugstarReportRaw*, const char*, uint32>(self, data, length);
    self->io->vtable->close(self->io, self->handle);
    self->handle = 0;
    return 0;
}

// 0x509230 (lote F1A, sin validar; el listado decia CAudioZones por cercania y daba 139 bytes: la funcion real llega hasta 0x50962E, `ret 1Ch`, unos 1022 bytes con marco SEH que solo destruye la cadena local;
// ningun llamador en el EXE: sin `call`/`jmp` ni referencias). thiscall, siete argumentos de pila (tres floats, dos `char*`, un bloque (puntero y tamano)). Es parte del volcado de informes de "bugstar.cnf"
// (cadenas "XCoord", "YCoord", "ZCoord", "grid", "ScreenShot", "image", "text", "SA"): serializa un registro en una cadena local TextBufferRaw (0x508930 agregar, 0x508A10 vaciar) y la entrega a 0x509190. `self` (ECX)
// solo se pasa a 0x509190. Colaboradores del EXE sin reconstruir, por direccion fija: 0x508B80 (thiscall, un entero: agrega una entrada de tipo 0xB con "%d"), 0x508D30 (thiscall, clave y tipo: agrega una entrada
// de tipo 0x11) y 0x509190 (thiscall, tres argumentos: 0, bloque y longitud). Orden del EXE:
//  1. Entrada con cabecera de 8 bytes {0, longitud} y la cadena `title` (+0x18 de pila); luego 0x508B80(1); luego la misma entrada con cabecera {0xA, 2} y "SA" (0x862EF4).
//  2. Para x, y, z (en ese orden): sprintf("%f", (double)v) en un bufer local; 0x508D30("XCoord"/"YCoord"/"ZCoord", "text"); entrada con cabecera de 4 bytes {longitud} y el texto formateado; cuatro bytes a cero.
//  3. 0x508D30("grid", "text"); entrada {longitud} con `grid` (+0x14); cuatro bytes a cero.
//  4. 0x508D30("ScreenShot", "image"); entrada {imageSize} con los `imageSize` bytes de `image` (sin strlen); cuatro bytes a cero.
//  5. 0x509190(self, 0, datos, longitud) y, si hay datos, operator delete de la cadena.
// Cada entrada se arma en un bloque temporal (operator new, 0x82119A; copia con rep movsd/movsb) que se agrega y se libera (0x8214BD) antes de seguir; los bytes a cero se agregan desde una variable local (4 bytes).
// sprintf del EXE (0x821BB5): se llama por su direccion (el de la biblioteca de C de la DLL daba textos distintos para NaN, infinitos, cero negativo y valores de mas de 17 digitos: lo encontro el banco FnF1A).
__declspec(noinline) void __fastcall BugstarWriteScreenShotEntry(void* self, void* /*edx*/, float x, float y, float z, const char* grid, const char* title, const void* image, uint32 imageSize) {
    TextBufferRaw text{ nullptr, 0 };

    // Entrada con cabecera de 8 bytes {tipo, longitud}: la cadena se copia sin terminador
    const auto appendTagged = [&](uint32 tag, const char* str) {
        const uint32 length = (uint32)strlen(str);
        const uint32 size   = length + 8;
        uint8* const block  = static_cast<uint8*>(::operator new(size));
        *reinterpret_cast<uint32*>(block + 4) = length;
        *reinterpret_cast<uint32*>(block)     = tag;
        memcpy(block + 8, str, length);
        AudioZonesTextBufferAppend(&text, nullptr, block, size);
        ::operator delete(block);
    };
    // Entrada con cabecera de 4 bytes {longitud}
    const auto appendSized = [&](const void* data, uint32 length) {
        const uint32 size  = length + 4;
        uint8* const block = static_cast<uint8*>(::operator new(size));
        *reinterpret_cast<uint32*>(block) = length;
        memcpy(block + 4, data, length);
        AudioZonesTextBufferAppend(&text, nullptr, block, size);
        ::operator delete(block);
    };
    const auto appendZero = [&] {
        uint32 zero = 0;
        AudioZonesTextBufferAppend(&text, nullptr, &zero, 4);
    };
    const auto addTyped = [&](const char* key, const char* type) { // 0x508D30
        AudioZonesTextBufferAddTyped(&text, nullptr, key, type);
    };
    const auto appendCoordinate = [&](float value, const char* key) {
        char formatted[0x44];
        // sprintf del CRT del EXE (0x821BB5), por direccion fija: el de la biblioteca de C de la DLL (UCRT) formatea distinto (NaN como "nan" y no "1.#QNAN0", cero negativo con signo, todos los digitos exactos en vez de 17 significativos y ceros)
        ExeCrt::Sprintf(formatted, "%f", (double)value);
        addTyped(key, "text");
        appendSized(formatted, (uint32)strlen(formatted));
        appendZero();
    };

    appendTagged(0, title);
    AudioZonesTextBufferAddInt(&text, nullptr, 1); // 0x508B80
    appendTagged(0xA, "SA");

    appendCoordinate(x, "XCoord");
    appendCoordinate(y, "YCoord");
    appendCoordinate(z, "ZCoord");

    addTyped("grid", "text");
    appendSized(grid, (uint32)strlen(grid));
    appendZero();

    addTyped("ScreenShot", "image");
    appendSized(image, imageSize);
    appendZero();

    const char* const data   = text.data;
    const uint32      length = text.length;
    // plugin::CallMethod exige un puntero a clase: `self` (ECX del EXE) se pasa tal cual, solo cambia el tipo del puntero
    BugstarFlushReport(static_cast<BugstarReportRaw*>(self), nullptr, 0, data, length); // 0x509190
    if (data) {
        ::operator delete(const_cast<char*>(data));
    }
}
} // namespace

/**
 * Registra el catálogo y actualización espacial de AudioZones.
 */
void CAudioZones::InjectHooks() {
    RH_ScopedClass(CAudioZones);
    RH_ScopedCategory("Audio");

    RH_ScopedInstall(Init, 0x5081A0);
    RH_ScopedInstall(RegisterAudioSphere, 0x5081C0);
    RH_ScopedInstall(RegisterAudioBox, 0x508240);
    RH_ScopedInstall(SwitchAudioZone, 0x508320);
    RH_ScopedInstall(Update, 0x5083C0);
    RH_ScopedNamedGlobalInstall(AudioZonesTextBufferAppend, "TextBufferAppend", 0x508930); // lote 172A (banco Fn172A)
    RH_ScopedNamedGlobalInstall(AudioZonesTextBufferClear, "TextBufferClear", 0x508A10);   // lote 172A (banco Fn172A)
    RH_ScopedNamedGlobalInstall(AudioZonesTextBufferClear, "TextBufferClear-2", 0x509140); // lote 172A (banco Fn172A): copia identica
    RH_ScopedNamedGlobalInstall(AudioZonesTextBufferAppendTagged, "TextBufferAppendTagged", 0x5089B0); // lote S2A (sin validar)
    RH_ScopedNamedGlobalInstall(AudioZonesTextBufferAddInt, "TextBufferAddInt", 0x508B80);            // lote S2A (sin validar)
    RH_ScopedNamedGlobalInstall(AudioZonesTextBufferAddTyped, "TextBufferAddTyped", 0x508D30);        // lote S2A (sin validar)
    RH_ScopedNamedGlobalInstall(BugstarFlushReport, "BugstarFlushReport", 0x509190);                  // lote S2A (sin validar)
    RH_ScopedNamedGlobalInstall(BugstarWriteScreenShotEntry, "BugstarWriteScreenShotEntry", 0x509230); // lote F1A (sin validar)
}

// 0x5081A0
void CAudioZones::Init() {
    ZoneScoped;

    m_NumSpheres = 0;
    m_NumBoxes = 0;
    m_NumActiveSpheres = 0;
    m_NumActiveBoxes = 0;
}

// 0x508240
// Añade una zona AABB comprimida al siguiente slot disponible del array global.
void CAudioZones::RegisterAudioBox(char name[8], int32 id, bool isActive, CVector min, CVector max) {
    // Como el original: se escribe en el sitio (strcpy sin rellenar el resto del nombre, palabra de flags
    // con solo el bit 0 modificado) y cada coordenada se comprime como (int16)_ftol2(v * 8.0f).
    auto& box = m_aBoxes.data()[m_NumBoxes];
    strcpy(box.m_szName, name);
    box.m_nAudioZone = (int16)id;
    box.m_IsActive = isActive;
    const auto Compress = [](float v) { return (int16)(int64)(v * 8.0f); };
    auto* packed = reinterpret_cast<int16*>(&box.m_Box);
    packed[0] = Compress(min.x);
    packed[1] = Compress(min.y);
    packed[2] = Compress(min.z);
    packed[3] = Compress(max.x);
    packed[4] = Compress(max.y);
    packed[5] = Compress(max.z);
    m_NumBoxes++;
}

// 0x5081C0
// Añade una zona esférica al catálogo global conservando nombre, ID y estado activo.
void CAudioZones::RegisterAudioSphere(char name[8], int32 id, bool isActive, CVector position, float radius) {
    // Como el original: se escribe en el sitio, sin copiar una estructura local completa.
    auto& sphere = m_aSpheres.data()[m_NumSpheres];
    strcpy(sphere.m_szName, name);
    sphere.m_nAudioZone = (int16)id;
    sphere.m_IsActive = isActive;
    sphere.m_Sphere.m_vecCenter = position;
    sphere.m_Sphere.m_fRadius = radius;
    m_NumSpheres++;
}

// 0x508320
// Activa/desactiva todas las zonas cuyo nombre coincide sin distinguir mayúsculas.
void CAudioZones::SwitchAudioZone(const char* zoneName, bool enable) {
    // Como el original: contadores con signo releídos en cada vuelta y sin límite por tamaño del array.
    for (int32 i = 0; i < (int32)m_NumSpheres; i++) {
        auto& sphere = m_aSpheres.data()[i];
        if (!_stricmp(zoneName, sphere.m_szName))
            sphere.m_IsActive = enable;
    }

    for (int32 i = 0; i < (int32)m_NumBoxes; i++) {
        auto& box = m_aBoxes.data()[i];
        if (!_stricmp(zoneName, box.m_szName))
            box.m_IsActive = enable;
    }
}

// 0x5083C0
/**
 * Recalcula qué zonas contienen la posición dada.
 *
 * Salvo forceUpdate, limita el coste mediante una comprobación periódica y por
 * distancia recorrida. Los arrays activos guardan índices hacia los catálogos fijos.
 *
 * @addr 0x5083C0
 */
void CAudioZones::Update(bool forceUpdate, CVector posn) {
    ZoneScoped;

    // Como el original: la última posición es un global del EXE (0xB6EBFC).
    auto& LastUpdateCoors = StaticRef<CVector>(0xB6EBFC);

    // Como el original: distancia = sqrt(dz*dz + dy*dy + dx*dx); con NaN se actualiza.
    if (!forceUpdate && (CTimer::m_FrameCounter & 0xF) != 6) {
        const float dx = posn.x - LastUpdateCoors.x;
        const float dy = posn.y - LastUpdateCoors.y;
        const float dz = posn.z - LastUpdateCoors.z;
        if (std::sqrt(dz * dz + dy * dy + dx * dx) < 20.0f)
            return;
    }

    LastUpdateCoors = posn;
    m_NumActiveSpheres = m_NumActiveBoxes = 0;

    // Como el original: esfera estricta sqrt(dz*dz + dy*dy + dx*dx) < radio, con d = posición - centro.
    int32 numActive = 0;
    for (int32 i = 0; i < (int32)m_NumSpheres; i++) {
        const auto& sphere = m_aSpheres.data()[i];
        if (!sphere.m_IsActive)
            continue;

        const float dx = posn.x - sphere.m_Sphere.m_vecCenter.x;
        const float dy = posn.y - sphere.m_Sphere.m_vecCenter.y;
        const float dz = posn.z - sphere.m_Sphere.m_vecCenter.z;
        if (std::sqrt(dz * dz + dy * dy + dx * dx) < sphere.m_Sphere.m_fRadius && numActive < 10)
            m_aActiveSpheres.data()[numActive++] = i;
    }
    m_NumActiveSpheres = numActive;

    // Como el original: caja abierta (min*0.125 < p < max*0.125 en cada eje, x, y, z).
    numActive = 0;
    for (int32 i = 0; i < (int32)m_NumBoxes; i++) {
        const auto& box = m_aBoxes.data()[i];
        if (!box.m_IsActive)
            continue;

        const auto* packed = reinterpret_cast<const int16*>(&box.m_Box);
        if (!((float)packed[0] * 0.125f < posn.x && (float)packed[3] * 0.125f > posn.x))
            continue;
        if (!((float)packed[1] * 0.125f < posn.y && (float)packed[4] * 0.125f > posn.y))
            continue;
        if (!((float)packed[2] * 0.125f < posn.z && (float)packed[5] * 0.125f > posn.z))
            continue;
        if (numActive < 10)
            m_aActiveBoxes.data()[numActive++] = i;
    }
    m_NumActiveBoxes = numActive;
}
