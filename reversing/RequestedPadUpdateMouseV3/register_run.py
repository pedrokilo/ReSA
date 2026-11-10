"""CASO-CONGELADOS: archiva la corrida APROBADA de V3 y escribe `validation-summary.json` (formato de registro por banco).

    python -B reversing/RequestedPadUpdateMouseV3/register_run.py <carpeta-de-evidencia> [carpeta de runs/ del banco]

Copia summary.json, RESULTADO.txt, el registro del arnes y comparisons.jsonl.gz a reversing/CppTwelveLevel6Final/runs/RequestedPadUpdateMouseV3-<corrida>/ (.xz si pasa de ~90 MB).
No escribe en reversing/validation-registry/: CPad::UpdateMouse ya esta registrada por UnlockInput y PadMouse1; esta corrida es evidencia adicional del banco (9216 pares, oraculo del modelo).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
BANK = 'RequestedPadUpdateMouseV3'


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def main(src):
    SRC = Path(src)
    RUN = ROOT / 'reversing/CppTwelveLevel6Final/runs' / (BANK + '-' + SRC.name)
    RUN.mkdir(exist_ok=True)
    for n in ('summary.json', 'RESULTADO.txt'):
        if not (RUN / n).exists():
            shutil.copyfile(SRC / n, RUN / n)
    harness = HERE / 'runs' / SRC.name / 'harness.jsonl'
    if harness.exists() and not (RUN / 'harness.jsonl').exists():
        shutil.copyfile(harness, RUN / 'harness.jsonl')
    if not (RUN / 'comparisons.jsonl.gz').exists() and not (RUN / 'comparisons.jsonl.xz').exists():
        with open(SRC / 'comparisons.jsonl', 'rb') as f, gzip.GzipFile(RUN / 'comparisons.jsonl.gz', 'wb', mtime=0) as g:
            shutil.copyfileobj(f, g)
        if (RUN / 'comparisons.jsonl.gz').stat().st_size > 90 * 1000 * 1000:
            import lzma
            with open(SRC / 'comparisons.jsonl', 'rb') as f, lzma.open(RUN / 'comparisons.jsonl.xz', 'wb', preset=6) as g:
                shutil.copyfileobj(f, g)
            (RUN / 'comparisons.jsonl.gz').unlink()
    s = json.loads((RUN / 'summary.json').read_text(encoding='utf-8'))
    assert s['passed'] is True and s['differences'] == 0 and s['model_mismatches'] == 0 and s['comparisons'] == 9216 and s['errors'] == [] and s['natural_exit'] is True
    manifest = json.loads((HERE / 'artifacts/manifest.json').read_text(encoding='utf-8'))
    tested = {k: v for k, v in manifest['tested_sources'].items()}
    compressed = RUN / 'comparisons.jsonl.gz' if (RUN / 'comparisons.jsonl.gz').exists() else RUN / 'comparisons.jsonl.xz'
    ev = dict(passed=True, pairs=int(s['comparisons']), differences=0, model_mismatches=0, functions={'CPad::UpdateMouse': int(s['comparisons'])},
              scope=('9216 casos aislados en CPU de CPad::UpdateMouse (EXE 0x0053F3C0): original (hook apagado) contra candidato (hook activo) con helpers sustituidos por falsos identicos en el EXE y en la DLL '
                     '(IsForegroundApp, IsVideoModeExclusive, diMouseInit, GetMouseState con sus dos ABI), CheckForInput nativo real en ambos lados, oraculo del modelo (model.py) y puertas estrictas de FP '
                     'y de restauracion de los globales y de los hooks. No cubre NaN/Inf/INT_MIN, COM/DirectInput real ni entrada real. Repetido el 2026-11-10 (CASO-CONGELADOS): ver README.md.'),
              dependency_review=None, run=RUN.relative_to(ROOT).as_posix(), summary_sha256=sha(RUN / 'summary.json'), result_txt_sha256=sha(RUN / 'RESULTADO.txt'),
              comparison_sha256=s['comparison_sha256'], harness_sha256=s['harness_sha256'],
              comparison_storage={'compressed_path': compressed.relative_to(ROOT).as_posix(), 'compressed_sha256': sha(compressed), 'uncompressed_sha256': s['comparison_sha256']},
              all_equal=True, state_restored=True, saves_unchanged=s['saves_unchanged'], configs_restored=s['configs_restored'],
              executed_probe_sha256=s['executed_probe_sha256'], original_exe_sha256=s['original_exe_sha256'], manifest=dict(tested_sources=tested))
    out = HERE / 'validation-summary.json'
    out.write_bytes((json.dumps(ev, ensure_ascii=False, indent=2) + '\n').encode('utf-8'))
    print('ok', RUN)


if __name__ == '__main__':
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
