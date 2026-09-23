from pathlib import Path
root=Path(r"E:\Original\gta-reversed-master")
out=root/"reversing/CVehicle/runtime"
v=root/"source/game_sa/Entity/Vehicle/Vehicle.cpp"
module=root/"source/toolsmenu/DebugModules/VehicleInfoDebugModule.cpp"
header=root/"source/toolsmenu/DebugModules/VehicleInfoDebugModule.h"
for p in [v,module,header]:
    backup=out/(p.name+".pre-instrument")
    if backup.exists(): raise SystemExit("Already instrumented")
    backup.write_bytes(p.read_bytes())
text=v.read_text(encoding="utf-8")
text='#include "../../../../reversing/CVehicle/runtime/RuntimeProbe.hpp"\n'+text
text=text.replace("RwObject* RemoveObjectsCB(RwObject* object, void* data) {\n",
"""RwObject* RemoveObjectsCB(RwObject* object, void* data) {
    using namespace CVehicleRuntimeProbe;
    if (armed) ++counts.calls;
""",1)
start=text.index("RwObject* RemoveObjectsCB(RwObject* object, void* data)")
end=text.index("// 0x6D3420",start)
body=text[start:end]
body=body.replace("if (RwObjectGetType(object) != rpATOMIC) {","if (RwObjectGetType(object) != rpATOMIC) {\n        if (armed) ++counts.nonAtomic;")
body=body.replace("if (flags & eAtomicComponentFlag::ATOMIC_UPGRADE) {","if (flags & eAtomicComponentFlag::ATOMIC_UPGRADE) {\n        if (armed) ++counts.keptUpgrade;")
body=body.replace("    RpAtomicDestroy(atomic);","    RpAtomicDestroy(atomic);\n    if (armed) ++counts.removed;")
body=body.replace("        RwFrameDestroy(frame);","        RwFrameDestroy(frame);\n        if (armed) ++counts.destroyedFrame;")
body=body.replace("        modelInfo->RemoveRef();","        modelInfo->RemoveRef();\n        if (armed) ++counts.modelRef;")
text=text[:start]+body+text[end:]
v.write_text(text,encoding="utf-8",newline="\n")
text=module.read_text(encoding="utf-8").replace('#include "VehicleInfoDebugModule.h"','#include "VehicleInfoDebugModule.h"\n#include "../../../reversing/CVehicle/runtime/LiveComparison.hpp"')
text += "\nvoid VehicleInfoDebugModule::Update() { CVehicleRuntime::Update(); }\n"
module.write_text(text,encoding="utf-8",newline="\n")
text=header.read_text(encoding="utf-8").replace("public:\n","public:\n    void Update() override final;\n",1)
header.write_text(text,encoding="utf-8",newline="\n")
(out/".gitignore").write_text("*.before-runtime\n*.pre-instrument\n*.log\n*.jsonl\n",encoding="ascii")
print("Temporary runtime probe installed into source for test build.")

