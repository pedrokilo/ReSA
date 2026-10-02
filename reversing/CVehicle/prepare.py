from pathlib import Path
import shutil
root = Path(__file__).resolve().parents[2]
source = root / "source/game_sa/Entity/Vehicle/Vehicle.cpp"
out = root / "reversing/CVehicle"
out.mkdir(parents=True, exist_ok=True)
backup = out / "Vehicle.cpp.before"
if backup.exists():
    raise SystemExit("Backup already exists; inspect before changing")
shutil.copy2(source, backup)
text = source.read_text(encoding="utf-8")
old = """RwObject* RemoveObjectsCB(RwObject* object, void* data) {
    return ((RwObject * (__cdecl*)(RwObject*, void*))0x6D33B0)(object, data);
}"""
candidate = (out / "candidate.txt").read_text(encoding="utf-8-sig").rstrip()
assert text.count(old) == 1
text = text.replace(old, candidate)
old_caller = """    auto frameOfUpgrade = CClumpModelInfo::GetFrameFromId(GetRpClump(), frameId);
    RwFrameForAllObjects(frameOfUpgrade, RemoveObjectsCB, &frameOfUpgrade);
    RwFrameForAllChildren(frameOfUpgrade, RemoveObjectsCB, &frameOfUpgrade);"""
new_caller = """    auto frameOfUpgrade = CClumpModelInfo::GetFrameFromId(GetRpClump(), frameId);
    int32 atomicFlags = 0;
    RwFrameForAllObjects(frameOfUpgrade, RemoveObjectsCB, &atomicFlags);
    RwFrameForAllChildren(frameOfUpgrade, RemoveObjectsCB, &atomicFlags);"""
assert text.count(old_caller) == 1
text = text.replace(old_caller, new_caller)
source.write_text(text, encoding="utf-8", newline="\n")
print("Candidate and caller correction written. Hook remains disabled pending validation.")

