import pathlib

p = pathlib.Path('contrib/audit_register_residue.py')
raw = p.read_bytes().decode('utf-8')
nl = '\r\n' if '\r\n' in raw else '\n'
t = raw.replace('\r\n', '\n')


def rep(o, n):
    global t
    assert o in t, o[:60]
    t = t.replace(o, n, 1)


rep("'PedGroups', 'Ped']", "'PedGroups', 'Ped', 'MemoryMgr', 'Pad', 'PathFind', 'AEVehicleAudioEntity', 'WaterLevel', 'Radar']")
rep("EVERY_PATH_OBJECTS = {'TheZones', 'Ped'}", "EVERY_PATH_OBJECTS = {'TheZones', 'Ped', 'MemoryMgr', 'PathFind', 'AEVehicleAudioEntity', 'WaterLevel'}")
rep(r"PlayerPedResidueFloatCpp@@YIMIIM@Z)\b'", r"PlayerPedResidueFloatCpp@@YIMIIM@Z|PathFindResidueFloatCpp@@YIMIIMI@Z)\b'")
rep("ENTRY_PARAM_OBJECTS = {'PedGroups'}", "ENTRY_PARAM_OBJECTS = {'PedGroups', 'PathFind', 'AEVehicleAudioEntity'}")
rep("MIN_RESIDUE_CALLS = {'PedGroups': 3, 'Ped': 1}", "MIN_RESIDUE_CALLS = {'PedGroups': 3, 'Ped': 1, 'PathFind': 8, 'AEVehicleAudioEntity': 4, 'WaterLevel': 1}")
p.write_bytes(t.replace('\n', nl).encode())
print('audit ok')
