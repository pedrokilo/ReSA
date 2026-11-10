"""Tabla de las 21 clases del lote 147A (destructor con borrado + cuerpo del destructor) leida del desensamblado: (clase, fuente, entrada del destructor con borrado, vtable (ranura), cuerpo, hook del cuerpo)."""
G = 'source/game_sa/Tasks/'
T = G + 'TaskTypes/'
# (clase, fuente, SDD, vtable base, ranura, cuerpo, cuerpo con hook RH)
CLASSES = [
    ('CTaskComplexCarSlowBeDraggedOutAndStandUp', T + 'TaskComplexCarSlowBeDraggedOutAndStandUp.cpp', 0x64C930, 0x86EF80, 0, 0x648690, True),
    ('CTaskSimpleBikeJacked', T + 'TaskSimpleBikeJacked.cpp', 0x64C950, 0x86EFAC, 0, 0x648C40, True),
    ('CTaskComplexWanderFlee', T + 'TaskComplexWanderFlee.cpp', 0x65D0C0, 0x86F690, 0, 0x65B370, True),
    ('CTaskComplexFleePoint', T + 'TaskComplexFleePoint.cpp', 0x65D0E0, 0x86F6CC, 0, 0x65B410, True),
    ('CTaskComplexFollowPointRoute', T + 'TaskComplexFollowPointRoute.cpp', 0x671610, 0x870078, 0, 0x66E050, False),
    ('CTaskComplexFollowNodeRoute', T + 'TaskComplexFollowNodeRoute.cpp', 0x671730, 0x8700A8, 0, 0x66EB70, True),
    ('CTaskComplexHitByGunResponse', T + 'TaskComplexHitByGunResponse.cpp', 0x638160, 0x86E06C, 0, 0x631E30, True),
    ('CTaskComplexSunbathe', T + 'TaskComplexSunbathe.cpp', 0x638180, 0x86E0AC, 0, 0x632050, True),
    ('CTaskSimpleGangDriveBy', T + 'TaskSimpleGangDriveBy.cpp', 0x6275E0, 0x86D944, 0, 0x6218C0, False),
    ('CTaskComplexDestroyCar', T + 'TaskComplexDestroyCar.cpp', 0x628780, 0x86D968, 0, 0x621CB0, True),
    ('CTaskSimplePlayerOnFire', T + 'TaskSimplePlayerOnFire.cpp', 0x639310, 0x86E334, 0, 0x6335B0, True),
    ('CTaskSimpleIKManager', T + 'TaskSimpleIKManager.cpp', 0x639330, 0x86E358, 0, 0x633830, True),
    ('CTaskComplexGangJoinRespond', T + 'TaskComplexGangJoinRespond.cpp', 0x664A80, 0x86FB5C, 0, 0x661720, True),
    ('CTaskComplexProstituteSolicit', T + 'TaskComplexProstituteSolicit.cpp', 0x664AA0, 0x86FB88, 0, 0x661AF0, True),
    ('CTaskComplexWaitForDryWeather', T + 'TaskComplexWaitForDryWeather.cpp', 0x638900, 0x86E160, 0, 0x632830, True),
    ('CTaskComplexWaitForBus', T + 'TaskComplexWaitForBus.cpp', 0x638920, 0x86E18C, 0, 0x632930, True),
    ('CTaskComplexAttractorPartnerWait', T + 'TaskComplexAttractorPartnerWait.cpp', 0x638F00, 0x86E284, 0, 0x633290, True),
    ('CTaskComplexUseSequence', G + 'TaskComplexUseSequence.cpp', 0x639F90, 0x86E518, 0, 0x6396C0, False),
    ('CTaskComplexWanderCriminal', T + 'TaskComplexWanderCriminal.cpp', 0x48E700, 0x85A23C, 0, 0x48E720, True),
    ('CTaskComplexGoToAttractor', T + 'TaskComplexGoToAttractor.cpp', 0x670400, 0x86FF3C, 0, 0x66B6A0, True),
]
