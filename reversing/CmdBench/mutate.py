"""Prueba negativa del banco por comando: introduce errores conocidos en manejadores C++ y comprueba que el banco los detecta.

Uso: python -I mutate.py apply <ronda> | restore | list
Cada mutacion es (archivo, texto original, texto nuevo, comando afectado). `apply` guarda copias en artifacts/good/ y `restore` las devuelve
(comprobando el SHA256). La ronda 1 contiene la regresion REAL que motivo el banco (GET_CHAR_AREA_VISIBLE devolviendo un booleano).
Tras `apply` hay que compilar la sonda (build_probe.py) y correr el banco con --ids de los comandos mutados; debe FALLAR en todos.
"""
import hashlib
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
GOOD = HERE / 'artifacts' / 'good'
CH = 'source/game_sa/Scripts/Commands/CharacterCommands.cpp'

ROUNDS = {
    1: [
        (CH, "    return (uint32)(uint8)ped.GetAreaCode();\n", "    return ped.GetAreaCode() != eAreaCodes::AREA_CODE_NORMAL_WORLD;\n", 2536),  # la regresion real
        (CH, "auto IsCharInAnyCar(CPed& ped) {\n    return ped.IsInVehicle();\n}", "auto IsCharInAnyCar(CPed& ped) {\n    return ped.bInVehicle;\n}", 223),
        (CH, "    return FixAngleDegrees(RadiansToDegrees(GetPedOrItsVehicle(ped).GetHeading()));\n", "    return FixAngleDegrees(RadiansToDegrees(ped.GetHeading()));\n", 370),
        (CH, "auto GetCharModel(CPed& ped) {\n    return (eModelID)(ped.m_nModelIndex);\n}", "auto GetCharModel(CPed& ped) {\n    return (eModelID)(ped.m_nModelIndex & 0xFF);\n}", 1637),
        (CH, "auto GetCharCoordinates(CPed& ped) {\n    return ped.IsInVehicle()\n        ? ped.m_pVehicle->GetPosition()\n        : ped.GetPosition();\n}", "auto GetCharCoordinates(CPed& ped) {\n    return ped.GetPosition();\n}", 160),
    ],
    2: [
        (CH, "auto IsCharStopped(CPed& ped) {\n    return CTheScripts::IsPedStopped(&ped);\n}", "auto IsCharStopped(CPed& ped) {\n    return !CTheScripts::IsPedStopped(&ped);\n}", 672),
        (CH, "auto IsCharOnScreen(CPed& ped) {\n    return ped.GetIsOnScreen();\n}", "auto IsCharOnScreen(CPed& ped) {\n    return !ped.GetIsOnScreen();\n}", 715),
        (CH, "    return CTheZones::FindZone(GetCharCoordinates(ped), zoneName, ZONE_TYPE_NAVI);\n", "    return CTheZones::FindZone(ped.GetPosition(), zoneName, ZONE_TYPE_NAVI);\n", 340),
        (CH, "    return static_cast<int32>(ped.m_fHealth);\n", "    return static_cast<int32>(ped.m_fHealth + 0.5f);\n", 550),
        (CH, "    return static_cast<int32>(ped.m_fArmour);\n", "    return static_cast<int32>(ped.m_fArmour + 0.5f);\n", 1245),
    ],
}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def apply(r):
    GOOD.mkdir(parents=True, exist_ok=True)
    manifest = {}
    for f, old, new, cmd in ROUNDS[r]:
        p = ROOT / f
        data = p.read_bytes().decode('utf-8')
        crlf = '\r\n' in data
        norm = data.replace('\r\n', '\n')
        if norm.count(old) != 1:
            raise SystemExit(f'mutacion no aplicable ({cmd}): {norm.count(old)} coincidencias')
        bak = GOOD / (f.replace('/', '__'))
        if not bak.exists():
            bak.write_bytes(p.read_bytes())
        manifest[f] = sha(bak)
        out = norm.replace(old, new)
        # se aplican todas las mutaciones del mismo archivo sobre el resultado anterior
        p.write_bytes((out.replace('\n', '\r\n') if crlf else out).encode('utf-8'))
    (GOOD / 'manifest.json').write_text(json.dumps(manifest))
    print('mutaciones aplicadas:', [m[3] for m in ROUNDS[r]])


def restore():
    mf = GOOD / 'manifest.json'
    if not mf.exists():
        print('nada que restaurar')
        return
    for f, h in json.loads(mf.read_text()).items():
        bak = GOOD / f.replace('/', '__')
        (ROOT / f).write_bytes(bak.read_bytes())
        assert sha(ROOT / f) == h, f
        bak.unlink()
    mf.unlink()
    print('fuentes restauradas')


if __name__ == '__main__':
    c = sys.argv[1]
    if c == 'apply':
        apply(int(sys.argv[2]))
    elif c == 'restore':
        restore()
    else:
        print(json.dumps({k: [m[3] for m in v] for k, v in ROUNDS.items()}))
