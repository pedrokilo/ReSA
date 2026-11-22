"""Filas de _initterm que el generador general (datos_exe_staticinit.py) no sabe construir sola y que se escriben a mano aqui.

Son las 56 filas que estaban `pendiente` en docs/DatosExe/inicializadores-estado.csv y que el puente temporal StandaloneInit.inc
ejecutaba con un emulador (fase 8). Cada fila lleva lo que hacia la rutina del EXE, decodificado con capstone de la rutina original y
comprobado contra la memoria del EXE vivo (DatosExe::VerifyStaticInitializers, ver docs/CodigoVivoExe/ejecucion-init-estatica.md):

  idx     indice C++ de la tabla (docs/DatosExe/inicializadores.csv)
  va      direccion del objeto (o primer elemento)
  size    expresion C++ con el tamano total en bytes que escribe/ocupa la rutina (para la verificacion)
  ctor    cuerpo C++ que reproduce el constructor (None: la rutina solo escribe ceros sobre .bss, que ya arranca en cero)
  dtor    cuerpo C++ de lo que hay que registrar en el atexit (None: el destructor del EXE es un `ret` o no tiene efecto observable)
  nota    por que (va al CSV de estado y al comentario del .cpp)
  incs    cabeceras (relativas a source/game_sa/DatosExe) que hacen falta
"""

N = {}


def fila(idx, va, size, ctor, dtor, nota, incs=()):
    N[str(idx)] = dict(idx=str(idx), va=va, size=size, ctor=ctor, dtor=dtor, nota=nota, incs=list(incs))


Z = 'ctor de elemento: solo escribe ceros sobre .bss (ya a cero)'

fila(112, 0x96A9B8, '13 * 28', None, None,
     'CWeapon x 13 (GameLogic::SavedWeapons): ctor 0x441E00 solo escribe ceros (+0x14, +0x15, +0x18); dtor 0x4411A0 es un `ret`')
fila(125, 0x96C048, '50 * 216', 'ConstructArrayOnly<CGarage, 0x96C048, 50>();', 'OnexitArrayNV<CGarage, 0x96C048, 50>();',
     'CGarage x 50 (CGarages::aGarages): ctor 0x4470E0 (vtable de CAEDoorAudioEntity en +0x50 y ceros) = el constructor por defecto de la clase; dtor 0x447110 = ~CGarage',
     ['../Garage.h'])
fila(195, 0xA913E8, '96 * 0x44', None, None,
     'tScriptText x 96 (IntroTextLines): ctor 0x463750 es `mov eax,ecx; ret` (el C++ propio SI inicializa miembros, el EXE no); dtor 0x463760 `ret`')
fila(196, 0xA92D68, '128 * 0x3C', None, None,
     'tScriptRectangle x 128 (IntroRectangles): ctor 0x463770 es `mov eax,ecx; ret`; dtor 0x463780 `ret`')
fila(345, 0xA9AFB8, '8 * 12', 'ConstructArrayOnly<CInformFriendsEvent, 0xA9AFB8, 8>();',
     'OnexitFn(+[] { for (size_t i = 8; i-- > 0;) { CInformFriendsEventBody::Destructor(&StaticRef<CInformFriendsEvent>(0xA9AFB8) + i, nullptr); } });',
     'CInformFriendsEvent x 8: ctor 0x4AC0D0 (ped 0, evento 0, instante -1) = los inicializadores de miembro de la estructura; dtor 0x4B2950 (CInformFriendsEventBody::Destructor) en orden inverso',
     ['../InformFriendsEventQueue.h'])
_stores = [
    (358, 0xAAE950, 'ms_atomicModelInfoStore', 0), (359, 0xB1BF58, 'ms_damageAtomicModelInfoStore', 1),
    (360, 0xB1C934, 'ms_lodAtomicModelInfoStore', 2), (361, 0xB1C960, 'ms_timeModelInfoStore', 3),
    (362, 0xB1E128, 'ms_lodTimeModelInfoStore', 4), (363, 0xB1E158, 'ms_weaponModelInfoStore', 5),
    (364, 0xB1E958, 'ms_clumpModelInfoStore', 6), (365, 0xB1F650, 'ms_vehicleModelInfoStore', 7),
    (366, 0xB478F8, 'ms_pedModelInfoStore', 8),
]
_store_ctor = {0: 0x4C5CB0, 1: 0x4C5D10, 2: 0x4C5D70, 3: 0x4C5DD0, 4: 0x4C5E30, 5: 0x4C5E90, 6: 0x4C5EF0, 7: 0x4C5F50, 8: 0x4C67D0}
_store_dtor = {0: 0x4C5840, 1: 0x4C5860, 2: 0x4C5880, 3: 0x4C58A0, 4: 0x4C58C0, 5: 0x4C58E0, 6: 0x4C5900, 7: 0x4C6370, 8: 0x4C6390}
for i, va, nombre, k in _stores:
    fila(i, va, f'sizeof(std::remove_reference_t<decltype(CModelInfo::{nombre})>)',
         f'ModelInfoStoreConstructStatic({k});', f'OnexitFn(+[] {{ ModelInfoStoreDestructStatic({k}); }});',
         f'CModelInfo::{nombre}: constructor 0x{_store_ctor[k]:X} (ModelInfoStoreConstructStatic: constructor de elemento del EXE sobre cada elemento y contador a 0); '
         f'destructor 0x{_store_dtor[k]:X} (ModelInfoStoreDestructStatic: destructor de elemento en orden inverso)',
         ['../Models/ModelInfo.h'])
fila(431, 0xB612D8, 'sizeof(CAEStreamTransformer)', 'StaticRef<CAEStreamTransformer>(0xB612D8).Initialise();', None,
     'CAEStreamTransformer: 0x4F1750 (Initialise: clave fija de 16 bytes); sin destructor', ['../Audio/Loaders/AEStreamTransformer.h'])
fila(468, 0xB61C38, '6 * 28', None, None,
     'CAEPedSpeechSlot x 6: ctor 0x4E3330 es `mov eax,ecx; ret` (el C++ propio SI inicializa miembros, el EXE no); dtor 0x4E3340 `ret`')
fila(559, 0xB6BB18, 'sizeof(CAETwinLoopSoundEntity)', 'StaticRef<CAETwinLoopSoundEntity>(0xB6BB18).DefaultConstructorExe();',
     'OnexitNV<CAETwinLoopSoundEntity, 0xB6BB18>();',
     'CAEWeatherAudioEntity::m_sRainSoundL: constructor por defecto 0x4F63B0 (DefaultConstructorExe); dtor 0x4F2B80 = ~CAETwinLoopSoundEntity',
     ['../Audio/Entities/AETwinLoopSoundEntity.h'])
fila(560, 0xB6BBC0, 'sizeof(CAETwinLoopSoundEntity)', 'StaticRef<CAETwinLoopSoundEntity>(0xB6BBC0).DefaultConstructorExe();',
     'OnexitNV<CAETwinLoopSoundEntity, 0xB6BBC0>();',
     'CAEWeatherAudioEntity::m_sRainSoundR: constructor por defecto 0x4F63B0 (DefaultConstructorExe); dtor 0x4F2B80 = ~CAETwinLoopSoundEntity',
     ['../Audio/Entities/AETwinLoopSoundEntity.h'])
fila(577, 0xB6FDA0, 'sizeof(CIdleCam)', 'StaticRef<CIdleCam>(0xB6FDA0).Init();', None,
     'gIdleCam: 0x50E6D0 (CIdleCam::Init); sin destructor', ['../IdleCam.h'])
fila(580, 0xB6FEC0, '0x9C', 'DWHeliChaseParamsConstructExe(&StaticRef<uint8>(0xB6FEC0));', None,
     'parametros de la camara de helicoptero DW (1 x 0x9C): ctor de elemento 0x5173F0 (salta a 0x50E180: valores por defecto); dtor 0x50E080 `ret`')
fila(665, 0xB73458, '2 * 0x134', None, None,
     'CPad x 2 (CPad::Pads): ctor 0x541D80 = CPad::Clear(true, true), que solo deja ceros; dtor 0x53ED60 `ret`')
fila(672, 0xB74240, 'sizeof(CMatrix)', 'StaticRef<CMatrix>(0xB74240).SetScale(1.0f);', 'OnexitNV<CMatrix, 0xB74240>();',
     'gDummyMatrix: ctor 0x59AED0 (CMatrix::SetScale(1.0f)); dtor 0x59ACD0 = ~CMatrix', ['../Core/Matrix.h'])
fila(727, 0xB7D0B8, '14400 * 8', None,
     'OnexitFn(+[] { for (size_t i = 14400; i-- > 0;) { SectorDestructorExe(&StaticRef<CSector>(0xB7D0B8) + i, nullptr); } });',
     'CSector x 14400 (CWorld::ms_aSectors): ctor de elemento 0x564040 solo escribe ceros; dtor 0x563130 (SectorDestructorExe: Flush de dummies y de edificios) en orden inverso',
     ['../Sector.h'])
fila(728, 0xB992B8, '256 * 12', None,
     'OnexitFn(+[] { for (size_t i = 256; i-- > 0;) { for (size_t j = 3; j-- > 0;) { details::CPtrListDoubleLink__FlushExe(reinterpret_cast<void*>(0xB992B8 + i * 12 + j * 4), nullptr); } } });',
     'CRepeatSector x 256 (CWorld::ms_aRepeatSectors): ctor de elemento 0x563190 (tres listas, ctor 0x563180 pone ceros); dtor 0x5631B0 (tres Flush 0x5630F0 -> 0x552470) en orden inverso',
     ['../Core/PtrNodeDoubleLink.h'])
fila(729, 0xB99EB8, '900 * 4', None,
     'OnexitFn(+[] { for (size_t i = 900; i-- > 0;) { details::CPtrListSingleLink__FlushExe(reinterpret_cast<void*>(0xB99EB8 + i * 4), nullptr); } });',
     'CPtrListSingleLink x 900 (CWorld::ms_aLodPtrLists): ctor de elemento 0x43E3F0 pone el puntero a cero; dtor 0x43E1A0 (salto al Flush 0x552400) en orden inverso',
     ['../Core/PtrNodeSingleLink.h'])
fila(730, 0xB9ACC8, '4',
     None, 'OnexitFn(+[] { details::CPtrListDoubleLink__FlushExe(reinterpret_cast<void*>(0xB9ACC8), nullptr); });',
     'CWorld::ms_listMovingEntityPtrs: solo destructor (Flush 0x552470); no hay constructor', ['../Core/PtrNodeDoubleLink.h'])
fila(731, 0xB9ACCC, '4',
     None, 'OnexitFn(+[] { details::CPtrListDoubleLink__FlushExe(reinterpret_cast<void*>(0xB9ACCC), nullptr); });',
     'CWorld::ms_listObjectsWithControlCode: solo destructor (Flush 0x552470); no hay constructor', ['../Core/PtrNodeDoubleLink.h'])
fila(782, 0xBA86D4, '6 * 4', None, None,
     'CRGBA x 6 (CRadar::ArrowBlipColour): ctor 0x463360 es `mov eax,ecx; ret`; dtor 0x43CE70 `ret`')
fila(887, 0xBC1290, 'sizeof(COctTreeBase)', 'OctTreeBaseConstructorExe(&StaticRef<COctTreeBase>(0xBC1290), nullptr);',
     'OnexitNV<COctTreeBase, 0xBC1290>();',
     'gOctTreeBase: ctor 0x5A7570 (OctTreeBaseConstructorExe); dtor 0x5A4140 = ~COctTreeBase', ['../Core/OctTreeBase.h'])
fila(888, 0xBC12C0, 'sizeof(CDirectory)', 'std::construct_at(&StaticRef<CDirectory>(0xBC12C0));', 'OnexitNV<CDirectory, 0xBC12C0>();',
     'ClothesBuilder::playerImg: ctor vacio 0x532290 = CDirectory() (no escribe m_bOwnsEntries); dtor 0x5322D0 = ~CDirectory', ['../Directory.h'])
fila(975, 0xC03A48, '256 * 0x54', None, None,
     'CPlantLocTri x 256: ctor 0x5DB2B0 es `mov eax,ecx; ret`; dtor 0x5DB2C0 `ret`')
fila(976, 0xC08E48, '40 * 0x14', None, None,
     'CPlantColEntEntry x 40: ctor 0x5DB290 es `mov eax,ecx; ret`; dtor 0x5DB2A0 `ret`')
fila(1003, 0xC091F0, '10 * 16', 'ConstructArrayOnly<CGangInfo, 0xC091F0, 10>();', None,
     'CGangInfo x 10 (CGangs::Gang): ctor 0x5DE520 (byte +0 = 0xFF, resto a cero) = los inicializadores de miembro de la clase; dtor 0x5DE540 `ret`', ['../GangInfo.h'])
for idx, va, cls, ctor_va, dtor_va in [
        (1048, 0xC09908, 'CPedGroupDefaultTaskAllocatorFollowAnyMeans', 0x5F6450, 0x5F6460),
        (1049, 0xC0990C, 'CPedGroupDefaultTaskAllocatorFollowLimited', 0x5F6480, 0x5F6490),
        (1050, 0xC09910, 'CPedGroupDefaultTaskAllocatorStandStill', 0x5F64B0, 0x5F64C0),
        (1051, 0xC09914, 'CPedGroupDefaultTaskAllocatorChat', 0x5F64E0, 0x5F64F0),
        (1052, 0xC09918, 'CPedGroupDefaultTaskAllocatorRandom', 0x5F6510, 0x5F6520),
        (1053, 0xC0991C, 'CPedGroupDefaultTaskAllocatorSitInLeaderCar', 0x5F6540, 0x5F6550)]:
    ctor = (f'PedGroupDefaultTaskAllocatorFollowAnyMeansConstructorExe(&StaticRef<{cls}>(0x{va:X}), nullptr);'
            if idx == 1048 else f'StaticRef<{cls}>(0x{va:X}).Constructor();')
    fila(idx, va, '4', ctor, None,
         f'{cls}: ctor 0x{ctor_va:X} (solo la vtable de la clase del EXE); dtor 0x{dtor_va:X} solo devuelve la vtable a la base (sin efecto observable)',
         ['../Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.h'])
fila(1163, 0xC178F0, '64 * 0x40', 'ConstructArrayOnly<CTaskComplexSequence, 0xC178F0, 64>();', 'OnexitArrayNV<CTaskComplexSequence, 0xC178F0, 64>();',
     'CTaskComplexSequence x 64 (ms_taskSequence): ctor de elemento 0x632BD0 = el constructor por defecto de la clase; dtor 0x6389F0 = ~CTaskComplexSequence',
     ['../Tasks/TaskComplexSequence.h'])
fila(1279, 0xC196E8, 'sizeof(CTaskComplexFollowLeaderInFormation::Offsets)', 'StaticRef<CTaskComplexFollowLeaderInFormation::Offsets>(0xC196E8).Constructor();', None,
     'CTaskComplexFollowLeaderInFormation::ms_offsets: 0x694850 (Offsets::Constructor: tablas de formacion y escala 1.0); sin destructor',
     ['../Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.h'])
fila(1298, 0xC1A2B8, '4',
     None, 'OnexitFn(+[] { details::CPtrListDoubleLink__FlushExe(reinterpret_cast<void*>(0xC1A2B8), nullptr); });',
     'CCover::m_ListOfProcessedBuildings: solo destructor (Flush 0x552470); no hay constructor', ['../Core/PtrNodeDoubleLink.h'])
fila(1458, 0xC3A200, '64 * 0x24', 'for (size_t i = 0; i < 64; ++i) { (&StaticRef<tUserList>(0xC3A200) + i)->Constructor(); }', None,
     'tUserList x 64 (CScripted2dEffects::ms_userLists): ctor de elemento 0x6FA6D0 (tUserList::Constructor: -1 en las 8 entradas y bUseList = 0); dtor 0x6F9E00 `ret`',
     ['../Scripts/Scripted2dEffects.h'])
fila(1459, 0xC3BB00, '64 * 0x94', 'for (size_t i = 0; i < 64; ++i) { (&StaticRef<CScriptedEffectPairs>(0xC3BB00) + i)->Constructor(); }', None,
     'CScriptedEffectPairs x 64 (CScripted2dEffects::ms_effectPairs): ctor de elemento 0x6FA790 (CScriptedEffectPairs::Constructor: cuatro pares y Reset); dtor 0x6F9E60 solo recorre destructores vacios',
     ['../Scripts/Scripted2dEffects.h'])
fila(1532, 0xC71BF8, '45 * 0x70', None, None,
     'CFallingGlassPane x 45 (CGlass::aGlassPanes): ctor de elemento 0x71A8B0 solo escribe ceros (+0x40, +0x44 de la CMatrix); dtor 0x71A8C0 = ~CMatrix sobre una matriz sin RenderWare asociado (sin efecto)')
fila(1565, 0xC7DD58, '32 * 0xA0', None, None,
     'C3dMarker x 32 (C3dMarkers::m_aMarkerArray): ctor de elemento 0x720F60 solo escribe ceros (el C++ propio SI inicializa miembros, el EXE no); dtor 0x720F70 = ~CMatrix sobre una matriz sin RenderWare asociado')
fila(1573, 0xC80740, '3 * 0x3CC', 'ConstructArrayOnly<CWaterCannon, 0xC80740, 3>();', 'OnexitArrayNV<CWaterCannon, 0xC80740, 3>();',
     'CWaterCannon x 3 (CWaterCannons::aCannons): ctor de elemento 0x728B10 (vtable de CAEWaterCannonAudioEntity en +0x32c y ceros) = constructor por defecto; dtor 0x728B30 = ~CWaterCannon',
     ['../WaterCannon.h'])
fila(1580, 0xC81360, 'sizeof(CAEWeatherAudioEntity)', 'WeatherAudioEntityConstructorBody(&StaticRef<CAEWeatherAudioEntity>(0xC81360), nullptr);',
     'OnexitNV<CAEWeatherAudioEntity, 0xC81360>();',
     'CWeather::m_WeatherAudioEntity: ctor 0x72A620 (WeatherAudioEntityConstructorBody); dtor 0x72A400 = ~CAEWeatherAudioEntity', ['../Audio/Entities/AEWeatherAudioEntity.h'])
for idx, va in ((1591, 0xC87B40), (1592, 0xC87B88), (1593, 0xC87BD0)):
    fila(idx, va, 'sizeof(CMemoryHeap)', None, None,
         f'g_Heaps[{(va - 0xC87B40) // 0x48}] (CMemoryHeap): ctor 0x72E860 solo pone m_FirstBlock a cero; sin destructor', ['../Memory/MemoryHeap.h'])
fila(1620, 0xC888D0, 'sizeof(CAEExplosionAudioEntity)', 'StaticRef<CAEExplosionAudioEntity>(0xC888D0).Constructor();', 'OnexitNV<CAEExplosionAudioEntity, 0xC888D0>();',
     'CExplosion::m_ExplosionAudioEntity: ctor 0x5074B0 (CAEExplosionAudioEntity::Constructor); dtor 0x506B90 = ~CAEExplosionAudioEntity', ['../Audio/Entities/AEExplosionAudioEntity.h'])
fila(1646, 0xC8A838, '2 * 0x2C', None, None,
     'CWeaponEffects x 2 (gCrossHair): ctor 0x742A90 es `mov eax,ecx; ret`; dtor 0x742AA0 `ret`')
fila(1653, 0xC8AAB8, '80 * 0x70', None, None,
     'CWeaponInfo x 80 (CWeaponInfo::aWeaponInfo): ctor 0x743C30 es `mov eax,ecx; ret`; dtor 0x743C40 `ret`')

# 518: fild word / fdivr dword / fstp dword (53 bits de mantisa, redondeo a float al guardar)
fila(518, 0xB6BA54, '4',
     'StaticRef<float>(0xB6BA54) = static_cast<float>(static_cast<double>(StaticRef<float>(0xB6BA4C)) / static_cast<double>(StaticRef<int16>(0x8CBC8C)));', None,
     'float en 0xB6BA54 = [0xB6BA4C] / (float)(int16)[0x8CBC8C] (movsx; fild; fdivr; fstp dword); depende de la constante x87 0xB6BA4C (tabla C++ 0x84CF80, ya escrita por RunX87StaticInitializers)')

# 1: lo hace ExeCrt::InitSecurityCookie en StandaloneStartup
ESTADO_CRT = {
    '1': 'CRT: ExeCrt::InitSecurityCookie (CrtStartup.cpp, 0x8339CA, la hace StandaloneStartup)',
}
