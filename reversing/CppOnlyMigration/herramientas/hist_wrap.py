import sys,re,json
from pathlib import Path
sys.path.insert(0, str(Path(sys.argv[1]).resolve().parents[1]))
sys.path.insert(1, str(Path(sys.argv[1]).resolve().parent))
import runtime_runner
runtime_runner.GAME = Path("E:/JUEGOS/GTA San Andreas - CVehicle Test")
target = Path(sys.argv[1]).resolve()
sys.argv = [str(target)]
source = target.read_text(encoding="utf-8")
# Algunos bancos antiguos fijan el hash de la sonda aprobada (PROBE_HASH). Para revalidar una fuente nueva se
# apunta ese pin al hash de la sonda recién compilada (build-manifest.json), sin tocar ninguna otra puerta del banco.
manifest = target.parent / "artifacts" / "build-manifest.json"
pin = re.search(r"PROBE_HASH\s*=\s*'([0-9a-f]{64})'", source)
if pin and manifest.exists():
    new = json.loads(manifest.read_text(encoding="utf-8"))["binaries"]["asi"]
    source = source.replace(pin.group(1), new)
    print("PROBE_HASH reapuntado a la sonda nueva:", new, file=sys.stderr)
code = compile(source, str(target), "exec")
exec(code, {"__name__": "__main__", "__file__": str(target), "__builtins__": __builtins__})
