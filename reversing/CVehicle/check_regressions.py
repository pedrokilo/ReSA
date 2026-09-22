from pathlib import Path
import sys, subprocess, json
here = Path(__file__).resolve().parent
source = here.parent.parent / "source/game_sa/Entity/Vehicle/Vehicle.cpp"
text = source.read_text(encoding="utf-8")
results = []
for name, old, new in [
    ("missing_flag_output", "*static_cast<int32*>(data) = flags;", "(void)data;"),
    ("old_caller_pointer", "RemoveObjectsCB, &atomicFlags", "RemoveObjectsCB, &frameOfUpgrade"),
]:
    mutant = text.replace(old, new)
    if name == "old_caller_pointer":
        mutant = mutant.replace("    int32 atomicFlags = 0;\n", "")
    assert mutant != text
    path = here / "test-build" / (name + ".cpp")
    path.write_text(mutant, encoding="utf-8")
    result = subprocess.run([sys.executable, str(here / "validate.py"), "--exe", sys.argv[1], "--source", str(path)], capture_output=True, text=True)
    report = json.loads((here / "test-build/result.json").read_text())
    assert report["source"] == str(path), result.stdout + result.stderr
    assert result.returncode != 0, f"Mutation unexpectedly passed: {name}"
    results.append({"mutation": name, "detected": True, "native_returncode": report["returncode"], "output": report["output"]})
(here / "test-build/regressions.json").write_text(json.dumps(results, indent=2))
print(json.dumps(results, indent=2))
# Finish with the real source and optimized code, leaving result.json as a passing production-source run.
subprocess.run([sys.executable, str(here / "validate.py"), "--exe", sys.argv[1], "--optimized"], check=True)

