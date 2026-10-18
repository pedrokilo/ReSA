import pathlib
p = pathlib.Path('contrib/audit_register_residue.py')
raw = p.read_bytes().decode('utf-8')
nl = '\r\n' if '\r\n' in raw else '\n'
t = raw.replace('\r\n', '\n')


def rep(o, n):
    global t
    assert o in t, o[:70]
    t = t.replace(o, n, 1)


rep("'WaterLevel', 'Radar']", "'WaterLevel', 'Radar', 'Stats', 'Vehicle', 'TaskSimpleFight', 'DecisionMakerTypes', 'Garage']")
rep("EVERY_PATH_OBJECTS = {'TheZones', 'Ped', 'MemoryMgr', 'PathFind', 'AEVehicleAudioEntity', 'WaterLevel'}",
    "EVERY_PATH_OBJECTS = {'TheZones', 'Ped', 'MemoryMgr', 'PathFind', 'AEVehicleAudioEntity', 'WaterLevel', 'Vehicle', 'DecisionMakerTypes'}")
rep(r"PathFindResidueFloatCpp@@YIMIIMI@Z)\b'", r"PathFindResidueFloatCpp@@YIMIIMI@Z)\b'")
rep(r"re.compile(r'^call\s+\?(ResidueRegsCpp", r"re.compile(r'^call\s+(?:\?)?(ResidueRegsCpp")
rep(r"PathFindResidueFloatCpp@@YIMIIMI@Z)\b')", r"PathFindResidueFloatCpp@@YIMIIMI@Z|VehicleResidueBoolCpp@12)\b')")
rep("ENTRY_PARAM_OBJECTS = {'PedGroups', 'PathFind', 'AEVehicleAudioEntity'}",
    "ENTRY_PARAM_OBJECTS = {'PedGroups', 'PathFind', 'AEVehicleAudioEntity', 'Stats', 'Vehicle', 'TaskSimpleFight', 'DecisionMakerTypes', 'Garage'}")
rep("'WaterLevel': 1}", "'WaterLevel': 1, 'Stats': 30, 'Vehicle': 2, 'TaskSimpleFight': 6, 'DecisionMakerTypes': 7, 'Garage': 2}")
p.write_bytes(t.replace('\n', nl).encode('utf-8'))
print('audit ok')
