/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CIniFile
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "IniFile.h"
#include "CarCtrl.h"

void CIniFile::InjectHooks() {
    RH_ScopedClass(CIniFile);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(LoadIniFile, 0x56D070);
}

// 0x56D070
void CIniFile::LoadIniFile() {
    ZoneScoped;

    CFileMgr::SetDir("");
    auto file = CFileMgr::OpenFile("gta3.ini", "r");
    if (file) {
        CFileMgr::ReadLine(file, gString, 200);
        VERIFY(sscanf_s(gString, "%f", &PedNumberMultiplier) == 1);
        if (PedNumberMultiplier >= 0.5f) {
            if (PedNumberMultiplier > 3.0f)
                PedNumberMultiplier = 3.0f;
        } else {
            PedNumberMultiplier = 0.5f;
        }

        CFileMgr::ReadLine(file, gString, 200);
        VERIFY(sscanf_s(gString, "%f", &CarNumberMultiplier) == 1);
        if (CarNumberMultiplier >= 0.5f) {
            if (CarNumberMultiplier > 3.0f)
                CarNumberMultiplier = 3.0f;
        } else {
            CarNumberMultiplier = 0.5f;
        }
        CFileMgr::CloseFile(file);
    }
    CPopulation::MaxNumberOfPedsInUse = 25;
    CCarCtrl::MaxNumberOfCarsInUse = 12;
}
