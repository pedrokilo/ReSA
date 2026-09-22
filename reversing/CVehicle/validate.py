r"""Build/run differential tests from the current Vehicle.cpp and user's own Compact EXE.
Usage: python validate.py --exe "D:\Grand Theft Auto San Andreas\gta-sa.exe"
Optional --source tests an alternate Vehicle.cpp (for regression/mutation checks).
No full game image is executed and no game installation is modified.
"""
from pathlib import Path
import argparse, hashlib, json, struct, subprocess, os
p = argparse.ArgumentParser()
p.add_argument("--exe", required=True, type=Path)
p.add_argument("--source", type=Path)
p.add_argument("--optimized", action="store_true")
args = p.parse_args()
here = Path(__file__).resolve().parent
root = here.parent.parent
source = args.source or root / "source/game_sa/Entity/Vehicle/Vehicle.cpp"
build = here / "test-build"
build.mkdir(exist_ok=True)
binary = args.exe.read_bytes()
sha = hashlib.sha256(binary).hexdigest()
expected = "72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b"
if sha != expected:
    raise SystemExit("Unverified executable hash; refusing to execute extracted bytes")
pe = struct.unpack_from("<I", binary, 0x3c)[0]
n, = struct.unpack_from("<H", binary, pe + 6)
opt_size, = struct.unpack_from("<H", binary, pe + 20)
opt = pe + 24
image_base, = struct.unpack_from("<I", binary, opt + 28)
sections = opt + opt_size
def extract(va, size):
    rva = va - image_base
    for i in range(n):
        s = sections + 40*i
        _, virtual_address, raw_size, raw = struct.unpack_from("<IIII", binary, s + 8)
        if virtual_address <= rva and rva + size <= virtual_address + raw_size:
            return binary[raw + rva - virtual_address:raw + rva - virtual_address + size]
    raise ValueError(hex(va))
parts = []
for name, va, size in [("Object", 0x6D33B0, 0x62), ("Frame", 0x6D3420, 0x30), ("Caller", 0x6D39E0, 0x70)]:
    data = extract(va, size)
    parts.append(f"constexpr unsigned char original{name}[] = {{{','.join(hex(b) for b in data)}}};")
(build / "original_bytes.inc").write_text("\n".join(parts))
text = source.read_text(encoding="utf-8-sig")
def function(signature):
    start = text.index(signature)
    opening = text.index("{", start)
    level = 1
    end = opening + 1
    while level:
        level += (text[end] == "{") - (text[end] == "}")
        end += 1
    return text[start:end]
selected = "\n\n".join(function(s) for s in [
    "RwObject* RemoveObjectsCB(RwObject* object, void* data)",
    "RwFrame* RemoveObjectsCB(RwFrame* frame, void* data)",
    "void CVehicle::RemoveReplacementUpgrade(int32 frameId)"
])
(build / "candidate.inc").write_text(selected, encoding="utf-8")
vs = Path(r"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat")
harness = here / "differential.cpp"
batch = build / "build.cmd"
optimization = "/O2" if args.optimized else "/Od"
batch.write_text(f'@echo off\ncall "{vs}" >nul\nif errorlevel 1 exit /b 1\ncl /nologo /EHsc /std:c++latest /W4 /WX {optimization} /MT /I"{build}" "{harness}" /Fe:"{build / "differential.exe"}" /Fo:"{build / "differential.obj"}" /link /DYNAMICBASE:NO /BASE:0x10000000\n', encoding="utf-8")
os.environ["PATH"] = r"C:\Program Files (x86)\Microsoft Visual Studio\Installer" + os.pathsep + os.environ["PATH"]
built = subprocess.run(["cmd.exe", "/d", "/c", str(batch)], cwd=build, capture_output=True, text=True)
(build / "compile.log").write_text(built.stdout + built.stderr)
print(built.stdout, built.stderr)
if built.returncode:
    raise SystemExit(built.returncode)
result = subprocess.run([str(build / "differential.exe")], cwd=build, capture_output=True, text=True)
print(result.stdout, result.stderr)
report = dict(exe_sha256=sha, source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),
              source=str(source), returncode=result.returncode, output=result.stdout,
              limitations="Controlled engine-call doubles; no complete ASI compilation or live game validation.")
(build / "result.json").write_text(json.dumps(report, indent=2), encoding="utf-8")
raise SystemExit(0 if result.returncode == 0 else 1)

