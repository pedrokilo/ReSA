"""Prueba negativa del banco Fn109A: aplica (o revierte) una ronda de mutaciones a las fuentes de produccion (una por funcion y ronda).

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto buscado, texto nuevo); el texto buscado debe aparecer exactamente una vez.
"""
import hashlib
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
G = 'source/game_sa/'

R1 = [
    # accesores de SetPiece: cada hook apunta al accesor siguiente (cableado equivocado)
    (G + 'SetPiece.cpp', 'RH_ScopedInstall(GetSpawnCoord1, 0x4997A0);', 'RH_ScopedNamedInstall(GetSpawnCoord2, "GetSpawnCoord1", 0x4997A0);'),
    (G + 'SetPiece.cpp', 'RH_ScopedInstall(GetSpawnCoord2, 0x4997E0);', 'RH_ScopedNamedInstall(GetTargetCoord1, "GetSpawnCoord2", 0x4997E0);'),
    (G + 'SetPiece.cpp', 'RH_ScopedInstall(GetTargetCoord1, 0x499820);', 'RH_ScopedNamedInstall(GetTargetCoord2, "GetTargetCoord1", 0x499820);'),
    (G + 'SetPiece.cpp', 'RH_ScopedInstall(GetTargetCoord2, 0x499860);', 'RH_ScopedNamedInstall(GetSpawnCoord1, "GetTargetCoord2", 0x499860);'),
    (G + 'Cam.cpp', 'm_fHorizontalAngle -= CamExeFloat(0x858CBC);', 'm_fHorizontalAngle += CamExeFloat(0x858CBC);'),
    (G + 'Cam.cpp', 'const int32 slot = int8(ped->m_nActiveWeaponSlot);', 'const int32 slot = uint8(ped->m_nActiveWeaponSlot) & 0xF;'),
    (G + 'Cam.cpp', 'static_cast<uint32>(static_cast<int32>(modelId));', 'static_cast<uint32>(static_cast<int16>(modelId));'),
    (G + 'common.cpp', 'static bool VectorsDifferExe(const CVector* a, const CVector* b) {\n    return !(a->x == b->x && a->y == b->y && a->z == b->z);', 'static bool VectorsDifferExe(const CVector* a, const CVector* b) {\n    return !(a->x == b->x && a->y == b->y);'),
    (G + 'common.cpp', 'return a->x == b->x && a->y == b->y && a->z == b->z;\n}\n\n// 0x509760', 'return a->x == b->x && a->y == b->y && a->z != b->z;\n}\n\n// 0x509760'),
    (G + 'Events/EventDamage.cpp', 'RH_ScopedInstall(HasKilledPed, 0x4ABCA0);', 'RH_ScopedNamedInstall(IsCriminalEvent, "HasKilledPed", 0x4ABCA0);'),
    (G + 'VehicleScanner.cpp', 'RH_ScopedInstall(GetClosestVehicleInRange, 0x5FFEE0);', 'RH_ScopedNamedInstall(ScanForVehiclesInRange, "GetClosestVehicleInRange", 0x5FFEE0);'),
    (G + 'PedScanner.cpp', 'RH_ScopedInstall(GetClosestPedInRange, 0x5FFF20);', 'RH_ScopedNamedInstall(ScanForPedsInRange, "GetClosestPedInRange", 0x5FFF20);'),
    (G + 'PedGeometryAnalyser.cpp', 'RH_ScopedInstall(operator new, 0x41B7C0);', 'RH_ScopedNamedInstall(operator delete, "operator new", 0x41B7C0);'),
    (G + 'PedGeometryAnalyser.cpp', 'RH_ScopedInstall(operator delete, 0x41B7D0);', 'RH_ScopedNamedInstall(operator new, "operator delete", 0x41B7D0);'),
    (G + 'PedClothesDesc.cpp', 'm_anTextureKeys.data()[texturePart] = texture;', 'm_anTextureKeys.data()[texturePart] = model;'),
    (G + 'PedClothesDesc.cpp', 'm_anTextureKeys.data()[texturePart] = textureKey;', 'm_anTextureKeys.data()[texturePart] = modelKey;'),
]
R2 = [
    (G + 'SetPiece.cpp', 'RH_ScopedInstall(GetSpawnCoord1, 0x4997A0);', 'RH_ScopedNamedInstall(GetTargetCoord2, "GetSpawnCoord1", 0x4997A0);'),
    (G + 'SetPiece.cpp', 'RH_ScopedInstall(GetSpawnCoord2, 0x4997E0);', 'RH_ScopedNamedInstall(GetSpawnCoord1, "GetSpawnCoord2", 0x4997E0);'),
    (G + 'SetPiece.cpp', 'RH_ScopedInstall(GetTargetCoord1, 0x499820);', 'RH_ScopedNamedInstall(GetSpawnCoord2, "GetTargetCoord1", 0x499820);'),
    (G + 'SetPiece.cpp', 'RH_ScopedInstall(GetTargetCoord2, 0x499860);', 'RH_ScopedNamedInstall(GetTargetCoord1, "GetTargetCoord2", 0x499860);'),
    (G + 'Cam.cpp', 'if (m_fHorizontalAngle > CamExeFloat(0x858CB8)) {', 'if (m_fHorizontalAngle >= CamExeFloat(0x858CB8)) {'),
    (G + 'Cam.cpp', 'return ped->m_aWeapons.data()[slot].m_IsFirstPersonWeaponModeSelected;', 'return !ped->m_aWeapons.data()[slot].m_IsFirstPersonWeaponModeSelected;'),
    (G + 'Cam.cpp', '0x8CD4FC, 0x8CD504, 0x8CD508, 0x8CD50C, 0x8CD514, 0x8CD510,', '0x8CD4FC, 0x8CD504, 0x8CD508, 0x8CD50C, 0x8CD514, 0x8CD514,'),
    (G + 'common.cpp', 'static bool VectorsDifferExe(const CVector* a, const CVector* b) {\n    return !(a->x == b->x && a->y == b->y && a->z == b->z);', 'static bool VectorsDifferExe(const CVector* a, const CVector* b) {\n    return !(a->x == b->x && a->y == b->y && a->z == b->z) && a->x == a->x;'),
    (G + 'common.cpp', 'return a->x == b->x && a->y == b->y && a->z == b->z;\n}\n\n// 0x509760', 'return a->x == b->x && a->z == b->z;\n}\n\n// 0x509760'),
    (G + 'Events/EventDamage.cpp', 'RH_ScopedInstall(HasKilledPed, 0x4ABCA0);', 'RH_ScopedNamedInstall(IsCriminalEvent, "HasKilledPed", 0x4ABCA0);'),
    (G + 'VehicleScanner.cpp', 'RH_ScopedInstall(GetClosestVehicleInRange, 0x5FFEE0);', 'RH_ScopedNamedInstall(ScanForVehiclesInRange, "GetClosestVehicleInRange", 0x5FFEE0);'),
    (G + 'PedScanner.cpp', 'RH_ScopedInstall(GetClosestPedInRange, 0x5FFF20);', 'RH_ScopedNamedInstall(ScanForPedsInRange, "GetClosestPedInRange", 0x5FFF20);'),
    (G + 'PedGeometryAnalyser.cpp', 'RH_ScopedInstall(operator new, 0x41B7C0);', 'RH_ScopedNamedInstall(operator delete, "operator new", 0x41B7C0);'),
    (G + 'PedGeometryAnalyser.cpp', 'RH_ScopedInstall(operator delete, 0x41B7D0);', 'RH_ScopedNamedInstall(operator new, "operator delete", 0x41B7D0);'),
    (G + 'PedClothesDesc.cpp', 'm_anModelKeys.data()[modelPart] = model;', 'm_anModelKeys.data()[modelPart] = texture;'),
    (G + 'PedClothesDesc.cpp', 'const uint32 modelKey   = modelName ? CKeyGen::GetUppercaseKey(modelName) : 0u;', 'const uint32 modelKey   = modelName ? CKeyGen::GetUppercaseKey(textureName ? textureName : modelName) : 0u;'),
]
R3 = [
    (G + 'Events/EventDamage.cpp', 'RH_ScopedInstall(HasKilledPed, 0x4ABCA0);', 'RH_ScopedNamedInstall(GetAnimAdded, "HasKilledPed", 0x4ABCA0);'),
    (G + 'VehicleScanner.cpp', 'return static_cast<CVehicle*>(m_pClosestEntityInRange);', 'return nullptr;'),
    (G + 'PedScanner.cpp', 'return static_cast<CPed*>(m_pClosestEntityInRange);', 'return nullptr;'),
    (G + 'PedGeometryAnalyser.cpp', 'return GetPointRoutePool()->New();', 'return nullptr;'),
    (G + 'PedGeometryAnalyser.cpp', 'GetPointRoutePool()->Delete(reinterpret_cast<CPointRoute*>(ptr));', '(void)ptr;'),
    (G + 'PedClothesDesc.cpp', 'm_anTextureKeys.data()[texturePart] = texture;', 'm_anTextureKeys.data()[texturePart] = model;'),
    (G + 'PedClothesDesc.cpp', 'm_anTextureKeys.data()[texturePart] = textureKey;', 'm_anTextureKeys.data()[texturePart] = modelKey;'),
]
R4 = [
    (G + 'VehicleScanner.cpp', 'return static_cast<CVehicle*>(m_pClosestEntityInRange);', 'return reinterpret_cast<CVehicle*>(reinterpret_cast<char*>(m_pClosestEntityInRange) + 4);'),
    (G + 'PedScanner.cpp', 'return static_cast<CPed*>(m_pClosestEntityInRange);', 'return reinterpret_cast<CPed*>(reinterpret_cast<char*>(m_pClosestEntityInRange) + 4);'),
    (G + 'PedGeometryAnalyser.cpp', 'return GetPointRoutePool()->New();', 'return reinterpret_cast<char*>(GetPointRoutePool()->New()) + 4;'),
    (G + 'PedGeometryAnalyser.cpp', 'GetPointRoutePool()->Delete(reinterpret_cast<CPointRoute*>(ptr));', 'GetPointRoutePool()->Delete(reinterpret_cast<CPointRoute*>(static_cast<char*>(ptr) - (reinterpret_cast<uintptr_t>(ptr) % 100 >= 50 ? 100 : 0)));'),
    (G + 'PedClothesDesc.cpp', 'm_anModelKeys.data()[modelPart] = model;', 'm_anModelKeys.data()[modelPart] = texture;'),
    (G + 'PedClothesDesc.cpp', 'const uint32 modelKey   = modelName ? CKeyGen::GetUppercaseKey(modelName) : 0u;', 'const uint32 modelKey   = modelName ? CKeyGen::GetUppercaseKey(textureName ? textureName : modelName) : 0u;'),
]
R5 = [
    (G + 'PedGeometryAnalyser.cpp', 'GetPointRoutePool()->Delete(reinterpret_cast<CPointRoute*>(ptr));', 'GetPointRoutePool()->New(); GetPointRoutePool()->Delete(reinterpret_cast<CPointRoute*>(ptr));'),
    (G + 'PedClothesDesc.cpp', 'm_anModelKeys.data()[modelPart] = model;', 'm_anModelKeys.data()[modelPart] = texture;'),
    (G + 'PedClothesDesc.cpp', 'const uint32 modelKey   = modelName ? CKeyGen::GetUppercaseKey(modelName) : 0u;', 'const uint32 modelKey   = modelName ? CKeyGen::GetUppercaseKey(textureName ? textureName : modelName) : 0u;'),
]
SOURCES = sorted({f for f, _, _ in R1 + R2 + R3 + R4 + R5})


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def apply(rnd):
    GOOD.mkdir(parents=True, exist_ok=True)
    for f in SOURCES:
        dst = GOOD / f.replace('/', '__')
        if not dst.exists():
            shutil.copyfile(ROOT / f, dst)
        assert sha(dst) == sha(ROOT / f), 'la fuente ya esta mutada: ' + f
    texts = {f: (ROOT / f).read_bytes().decode('utf-8') for f in SOURCES}
    muts = {1: R1, 2: R2, 3: R3, 4: R4, 5: R5}[rnd]
    for f, old, new in muts:
        assert texts[f].count(old) == 1, (f, old, texts[f].count(old))
        texts[f] = texts[f].replace(old, new, 1)
    for f, t in texts.items():
        (ROOT / f).write_bytes(t.encode('utf-8'))
    print('mutadas', len(muts), 'funciones, ronda', rnd)


def restore():
    for f in SOURCES:
        src = GOOD / f.replace('/', '__')
        shutil.copyfile(src, ROOT / f)
        assert sha(src) == sha(ROOT / f), f
    print('restauradas', len(SOURCES), 'fuentes')


if __name__ == '__main__':
    if sys.argv[1] == 'apply':
        apply(int(sys.argv[2]))
    elif sys.argv[1] == 'restore':
        restore()
    else:
        for f in SOURCES:
            print(sha(ROOT / f), f)
