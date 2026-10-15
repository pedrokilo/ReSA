"""Sonda de los emisores de particulas de pisada de CPed::DoFootLanded (EmitFootRainParticles 0x5E3630, EmitFootDustParticles 0x5E37C0).

Las estaticas de Ped.cpp no son visibles desde Game.cpp: antes de compilar se anaden a Ped.cpp tres envoltorios exportados
(PedFootEmittersProbeRain/Dust/CtorEntry) y se restaura SIEMPRE el archivo original (aunque falle la compilacion), como se hace con Game.cpp.
La direccion del constructor de FxPrtMult_c (que no admite &) se obtiene con /alternatename sobre su simbolo decorado.
"""
import json
import sys
import time
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from runtime_runner import digest
from probe_build import build, ROOT

HERE = Path(__file__).resolve().parent
PED = ROOT / 'source/game_sa/Entity/Ped/Ped.cpp'
PED_REL = 'source/game_sa/Entity/Ped/Ped.cpp'
BEGIN = '// BEGIN CppPedFootEmitters1 PROBE (temporal; lo anade y lo quita build_probe.py)'
END = '// END CppPedFootEmitters1 PROBE'
WRAPPERS = '\n'.join([
    '',
    BEGIN,
    'extern "C" char ProbeFxPrtMultCtorEntry;',
    '#pragma comment(linker, "/alternatename:_ProbeFxPrtMultCtorEntry=??0FxPrtMult_c@@QAE@MMMMMMM@Z")',
    '__declspec(noinline) void PedFootEmittersProbeRain(CPed* ped, CVector* position) { EmitFootRainParticles(ped, position); }',
    '__declspec(noinline) void PedFootEmittersProbeDust(CPed* ped, CVector* position) { EmitFootDustParticles(ped, position); }',
    '__declspec(noinline) uintptr_t PedFootEmittersProbeCtorEntry() { return reinterpret_cast<uintptr_t>(&ProbeFxPrtMultCtorEntry); }',
    END,
    '',
])
SOURCES = [
    'source/game_sa/Entity/Ped/Ped.h',
    'source/game_sa/Fx/FxPrtMult.cpp', 'source/game_sa/Fx/FxPrtMult.h',
    'source/game_sa/General.cpp', 'source/game_sa/General.h',
    'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
]


def restore_ped(saved: bytes, original_hash: str, backup: Path):
    deadline = time.monotonic() + 10
    while True:
        try:
            PED.write_bytes(saved)
            break
        except OSError:
            if time.monotonic() >= deadline:
                raise
            time.sleep(.2)
    if digest(PED) != original_hash:
        raise RuntimeError('Ped.cpp no quedo restaurado; copia en ' + str(backup))
    backup.unlink()


def main():
    artifacts = HERE / 'artifacts'
    artifacts.mkdir(exist_ok=True)
    saved = PED.read_bytes()
    if BEGIN.encode() in saved:
        raise RuntimeError('Ped.cpp ya contiene los envoltorios de una sonda anterior; restaurarlo antes de repetir.')
    original_hash = digest(PED)
    backup = artifacts / 'ped-cpp-before-probe.bin'
    backup.write_bytes(saved)
    written = None
    try:
        text = saved.decode('utf-8')
        if not text.endswith('\n'):
            text += '\n'
        written = (text + WRAPPERS).encode('utf-8')
        PED.write_bytes(written)
        hashes = build(HERE, 'cpppedfootemitters1', 'CppPedFootEmitters1Live', SOURCES)
        if PED.read_bytes() != written:
            raise RuntimeError('Ped.cpp cambio durante la compilacion; repetir la sonda.')
    finally:
        restore_ped(saved, original_hash, backup)
    manifest_path = artifacts / 'build-manifest.json'
    manifest = json.loads(manifest_path.read_text())
    # Ped.cpp se registra con el hash del archivo ORIGINAL (la sonda compilo ese contenido mas los envoltorios temporales).
    manifest['sources'][PED_REL] = original_hash
    manifest['probe_wrappers'] = {'file': PED_REL, 'restored': True}
    manifest_path.write_text(json.dumps(manifest, indent=2))
    return hashes


if __name__ == '__main__':
    print(json.dumps(main()))
