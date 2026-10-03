/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CGridRef
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "GridRef.h"

// todo: remove magic numbers
auto& GridRefList = StaticRef<char[10][10][32]>(0xC72FB0);

void CGridRef::InjectHooks() {
    RH_ScopedClass(CGridRef);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x71D4E0);
    RH_ScopedInstall(GetAreaName, 0x71D670);
    RH_ScopedInstall(GetArtistBugstarID, 0x71D650);
    RH_ScopedOverloadedInstall(GetGridRefPositions, "vec", 0x71D5A0, void(*)(CVector, uint8*, uint8*));
    RH_ScopedOverloadedInstall(GetGridRefPositions, "", 0x71D5E0, void(*)(uint8*, uint8*));
}

// 0x71D4E0
void CGridRef::Init() {
    // Como el original: x/y se inicializan una sola vez y conservan el último valor leído entre líneas;
    // sscanf/_strlwr/strcpy sin comprobaciones (índice plano (x - 'A') * 10 + y - 1 con x sin signo).
    char str[32];
    char x_ch = 0;
    int32 y = 0;
    CFileMgr::SetDir("");
    auto* file = CFileMgr::OpenFile("DATA\\GRIDREF.DAT", "rb");
    for (auto line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == '#' || !*line)
            continue;

        sscanf(line, "%c%d %s", &x_ch, &y, str);
        const char* lower = _strlwr(str);
        strcpy(reinterpret_cast<char(*)[32]>(&GridRefList[0][0][0])[(int32)(uint8)x_ch * 10 + y - ('A' * 10 + 1)], lower);
    }
    CFileMgr::CloseFile(file);
}

// 0x71D670
int32 CGridRef::GetAreaName(uint8 x, uint8 y) {
    if (x >= 10 || y >= 10)
        return 0;
    else
        return atoi(GridRefList[x][y]);
}

// 0x71D650
char* CGridRef::GetArtistBugstarID(uint8 x, uint8 y) {
    if (x >= 10 || y >= 10)
        return nullptr;
    else
        return &StaticRef<char>(0x858B54); // Como el original: cadena vacía del EXE
}

// 0x71D5A0
void CGridRef::GetGridRefPositions(CVector pos, uint8* x, uint8* y) {
    // Como el original: (p + 3000) * (1/600 en float, 0x3ADA740E) en float y truncado con _ftol2.
    const float inv600 = std::bit_cast<float>(0x3ADA740Eu);
    *x =     (uint8)(int64)((pos.x + 3000.0f) * inv600);
    *y = 9 - (uint8)(int64)((pos.y + 3000.0f) * inv600);
}

// 0x71D5E0
void CGridRef::GetGridRefPositions(uint8* x, uint8* y) {
    // Como el original: FindPlayerCoors se llama dos veces y se usa la segunda; el cálculo va en línea.
    FindPlayerCoors();
    const CVector coors = FindPlayerCoors();
    const float inv600 = std::bit_cast<float>(0x3ADA740Eu);
    *x =     (uint8)(int64)((coors.x + 3000.0f) * inv600);
    *y = 9 - (uint8)(int64)((coors.y + 3000.0f) * inv600);
}
