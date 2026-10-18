"""Extrae, sin cambios salvo dos sustituciones documentadas, el texto de las nueve reconstrucciones del arbol de fuentes para el arnes sin juego.

Uso: python -B extract.py [raiz_de_source]   (por defecto ../../../source, es decir reversing/Dependencies25A/offline -> source/)
Escribe offline/inc/*.inc. Sustituciones: `::operator delete(` y `operator delete(` pasan a `ExeOperatorDelete(` (el operator delete global de la DLL es
`plugin::Call<0x8214BD, void*>`, MemoryMgr.cpp; el arnes no puede sustituir el operator delete global del proceso porque lo usan nlohmann::json y std::string).
"""
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else HERE.parents[2] / 'source'
OUT = HERE / 'inc'
OUT.mkdir(exist_ok=True)


def read(rel):
    return (ROOT / rel).read_text(encoding='utf-8').replace('\r\n', '\n')


def grab(rel, start_pattern, leading_pragmas=0):
    """Texto desde la linea que casa con start_pattern hasta la llave que cierra la primera llave abierta (inclusive)."""
    text = read(rel)
    m = re.search(start_pattern, text, re.M)
    if not m:
        raise SystemExit('no se encontro %s en %s' % (start_pattern, rel))
    i = text.index('{', m.start())
    depth = 0
    j = i
    while True:
        c = text[j]
        if c == '{':
            depth += 1
        elif c == '}':
            depth -= 1
            if depth == 0:
                break
        j += 1
    return text[m.start():j + 1] + '\n'


def write(name, body):
    body = body.replace('::operator delete(', 'ExeOperatorDelete(').replace('operator delete(', 'ExeOperatorDelete(')
    (OUT / name).write_text(body, encoding='utf-8', newline='\n')


# 0 CTask::Destructor: con los pragmas de optimizacion que lo rodean (hasta `#pragma runtime_checks("", restore)`)
task = read('game_sa/Tasks/Task.cpp')
a = task.index('#pragma runtime_checks("", off)\n#pragma optimize("gty", on)\nvoid CTask::Destructor()')
b = task.index('#pragma runtime_checks("", restore)', a) + len('#pragma runtime_checks("", restore)')
write('task.inc', task[a:b] + '\n')
# 1 CPool::GetAt / IsFreeSlotAtIndex / IsIndexInBounds / SlotState (dentro de la clase del arnes)
pool = 'game_sa/Core/Pool.h'
write('pool_members.inc', grab(pool, r'^    T\* GetAt\(size_t idx\)') + grab(pool, r'^    bool IsFreeSlotAtIndex\(size_t idx\) const') + grab(pool, r'^    (?:\[\[nodiscard\]\] )?bool IsIndexInBounds\(size_t idx\) const'))
# 2 CAutoPilot::RemoveOnePathNode
write('autopilot.inc', grab('game_sa/AutoPilot.cpp', r'^void CAutoPilot::RemoveOnePathNode\(\)'))
# 3 CPedGroups::RemoveAllFollowersFromGroup
write('pedgroups.inc', grab('game_sa/PedGroups.cpp', r'^void CPedGroups::RemoveAllFollowersFromGroup\(int32 groupId\)'))
# 4 CDecisionMakerTypes::FlushDecisionMakerEventResponse (+ las constantes y AsU32 del espacio anonimo del archivo)
dm = read('game_sa/DecisionMakers/DecisionMakerTypes.cpp')
consts = [re.search(r'^constexpr uint32\s+kEventIndices\s*=.*$', dm, re.M).group(0),
          re.search(r'^constexpr uint32\s+kDecisionStride\s*=.*$', dm, re.M).group(0),
          re.search(r'^inline uint32 AsU32\(const void\* p\).*$', dm, re.M).group(0)]
write('decision.inc', '\n'.join(consts) + '\n' + grab('game_sa/DecisionMakers/DecisionMakerTypes.cpp', r'^void CDecisionMakerTypes::FlushDecisionMakerEventResponse\('))
# 5 CGarages::CountCarsInHideoutGarage
write('garages.inc', grab('game_sa/Garages.cpp', r'^int32 CGarages::CountCarsInHideoutGarage\('))
# 6 FindReplacementUpgradeCB
write('upgrade.inc', grab('game_sa/Entity/Vehicle/Vehicle.cpp', r'^RwObject\* FindReplacementUpgradeCB\('))
# 7 GetAnimHierarchyFromFrameCallback
write('hierarchy.inc', grab('game_sa/RwHelper.cpp', r'^RwFrame\* GetAnimHierarchyFromFrameCallback\('))
# 8 CAEStreamingDecoder::ScalarDeletingDestructor y CAEDataStream::~CAEDataStream (colaborador real que la DLL llama directamente)
write('decoder.inc', grab('game_sa/Audio/Loaders/AEStreamingDecoder.cpp', r'^CAEStreamingDecoder\* CAEStreamingDecoder::ScalarDeletingDestructor\('))
write('datastream.inc', grab('game_sa/Audio/Loaders/AEDataStream.cpp', r'^CAEDataStream::~CAEDataStream\(\)'))
print('extraido de', ROOT)
