"""Documentary cut447 (+75 funciones: VehModelInfoFinal4); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
CFG={'complete': ['CVehicleModelInfo'],
 'groups_drop': 1,
 'intro': 'setenta y cinco de CVehicleModelInfo (VehModelInfoFinal4)',
 'kind': 'CPU en el juego completo con partida cargada, mundo falso estático y colaboradores doblados con traza (banco por pares, '
         'passed=true global)',
 'lim': {'CVehicleModelInfo::AddRemap': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C86F4): no bounds check. ',
         'CVehicleModelInfo::AddWheelUpgrade': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::AssignRemapTxd': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C937B/4C93BD): isdigit on the '
                                              'unsigned byte and strncpy. ',
         'CVehicleModelInfo::ChooseComponent': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C7FC2..4C8100): random from '
                                               '0x407180 = GetRandomIntInRange, signed m_nNumExtras (4C80E2). ',
         'CVehicleModelInfo::ChooseComponent-Global': 'mundo falso y stubs trazados sin crédito; fuente corregida: random from 0x407180 = '
                                                      'GetRandomIntInRange. ',
         'CVehicleModelInfo::ChooseSecondComponent': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C80B7, 4C81C1): random '
                                                     'from 0x407180, signed m_nNumExtras. ',
         'CVehicleModelInfo::ChooseVehicleColour': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C8570): movzx index of '
                                                   'the byte, array unchecked. ',
         'CVehicleModelInfo::ClearAtomicFlagCB': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::CollapseFramesCB': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::ConvertAnimFileIndex': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::CountCompsInRule': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C7F9B): sar ecx,4; negatives '
                                                'outside the fixture. ',
         'CVehicleModelInfo::CreateInstance': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C96E2/4C9750): no bounds '
                                              'check; class subfunctions doubled with trace. ',
         'CVehicleModelInfo::DeleteRwObject': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::DeleteVehicleColourTextures': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::DisableEnvMap': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::FindTextureCB': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C7560): unbounded strcpy. ',
         'CVehicleModelInfo::GetAnimFileIndex': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::GetCustomCarPlateText': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::GetListOfComponentsNotUsedByRules': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::GetMatFXEffectMaterialCB': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C8816/4C8823): two '
                                                        'calls to RpMatFXMaterialGetEffects. ',
         'CVehicleModelInfo::GetMaximumNumberOfPassengersFromNumberOfDoors': 'mundo falso y stubs trazados sin crédito; fuente corregida '
                                                                             '(4C89BF, 4C8A31): low byte of the handling id, unsigned '
                                                                             'm_nNumDoors on that path; -1 outside the fixture. ',
         'CVehicleModelInfo::GetModelType': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::GetNumDoors': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C73C0): returns the byte '
                                           'sign-extended (movsx). ',
         'CVehicleModelInfo::GetNumRemaps': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::GetNumWheelUpgrades': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::GetOkAndDamagedAtomicCB': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::GetOriginalCompPosition': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::GetWheelPosn': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C7D5A): rep movsd of 0x40 bytes '
                                            '(sizeof(RwMatrix)). ',
         'CVehicleModelInfo::GetWheelUpgrade': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::HasAlphaMaterialCB': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::HideAllComponentsAtomicCB': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::HideDamagedAtomicCB': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::Init': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::IsUpgradeAvailable': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::IsValidCompRule': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::LoadEnvironmentMaps': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C8781..4C8793): '
                                                   'FindTxdSlot before PushCurrentTxd. ',
         'CVehicleModelInfo::LoadVehicleColours': 'mundo falso y stubs trazados sin crédito; fuente corregida (5B68E0): no 10-character '
                                                  'limit, unsigned compare, no FIX_BUGS for "77.93,96", red/green/blue persist between '
                                                  'lines; lines without # outside the fixture. ',
         'CVehicleModelInfo::LoadVehicleUpgrades': 'mundo falso y stubs trazados sin crédito; fuente corregida (5B65BE): opens the file '
                                                   'before zeroing the counters, persistent iWheelSet, no VERIFY. ',
         'CVehicleModelInfo::MoveObjectsCB': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::PreprocessHierarchy': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C8E6F, 4C919A): low byte '
                                                   'of the handling id, default callback 0x7491C0, m_apExtras indexed with movsx. ',
         'CVehicleModelInfo::ReduceMaterialsInVehicle': 'mundo falso y stubs trazados sin crédito; fuente corregida '
                                                        '(4C8C02/4C8C54/4C8C59): CTimer calls dropped, signed numExtras. ',
         'CVehicleModelInfo::RemoveWindowAlphaCB': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::ResetEditableMaterials': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::SetAnimFile': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C767E): call 8229B6 is _stricmp '
                                           '("NULL" ignored) and strcpy instead of strcpy_s (4C76C0). ',
         'CVehicleModelInfo::SetAtomicFlagCB': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::SetAtomicRenderCallbacks': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::SetAtomicRendererCB': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::SetAtomicRendererCB_Boat': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C79B1): the material '
                                                        'walk precedes the name comparison. ',
         'CVehicleModelInfo::SetAtomicRendererCB_Heli': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::SetAtomicRendererCB_Plane': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::SetAtomicRendererCB_RealHeli': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::SetAtomicRendererCB_Train': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::SetCarCustomPlate': 'mundo falso y stubs trazados sin crédito; only the low byte of the dword passed to '
                                                 'SetupClump is traced. ',
         'CVehicleModelInfo::SetClump': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::SetCustomCarPlateText': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C898F): strncpy(..,8) '
                                                     '(NOTSA deviation removed). ',
         'CVehicleModelInfo::SetDirtTextures': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::SetEditableMaterials': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::SetEditableMaterialsCB-RpAtomic': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::SetEditableMaterialsCB-RpMaterial': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::SetEnvMapCoeff': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C9819): 64-bit truncation '
                                              '(_ftol2). ',
         'CVehicleModelInfo::SetEnvMapCoeffAtomicCB': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::SetEnvMapCoeffCB': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C88C7..4C88DC): x87 fild, '
                                                'fadd 2^32 if negative, fmul 0.001f, with __asm. ',
         'CVehicleModelInfo::SetEnvironmentMapAtomicCB': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::SetEnvironmentMapCB': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C8840): compares the full '
                                                   'dword with 0xFFFF and calls RpMatFXMaterialSetEffects(material,0) directly. ',
         'CVehicleModelInfo::SetRenderPipelines': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::SetRenderPipelinesCB': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::SetVehicleColour': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::SetVehicleComponentFlags': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C7C1B): low byte of '
                                                        'the handling id (movzx). ',
         'CVehicleModelInfo::SetupCommonData': 'mundo falso y stubs trazados sin crédito; fuente corregida (5B8F93): no '
                                               'assert(vehicleTxd); pool byte 0x11 and DLL 0xFD filler masked. ',
         'CVehicleModelInfo::SetupLightFlags': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::ShutdownEnvironmentMaps': 'mundo falso y stubs trazados sin crédito; fuente corregida (4C87D9): clears the '
                                                       'global byte 0xB4E6BC. ',
         'CVehicleModelInfo::ShutdownLightTexture': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::StopUsingCommonVehicleTexDicationary': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::StoreAtomicUsedMaterialsCB': 'mundo falso y stubs trazados sin crédito; ',
         'CVehicleModelInfo::UseCommonVehicleTexDicationary': 'mundo falso y stubs trazados sin crédito; '},
 'n': 75,
 'new_word': 'Mil ciento cincuenta y siete',
 'note': 'VehModelInfoFinal4 añade la aprobación de las 75 funciones de CVehicleModelInfo, 16384 pares con cero diferencias. La corrida '
         '20261010-005102-024078 tiene passed=true global y evento complete con state_restored=true y layout_ok=true, saves y '
         'configuraciones intactos, con el banco (hooks de la clase apagados) frente a la candidata dentro del juego completo con partida '
         'cargada. Límites: mundo falso estático (model infos con la vtable del EXE, CVehicleStructure en un pool falso 0xB4E680, TXD '
         'falso 0xC8800C, clumps, atomics, geometrías, materiales y texturas) y stubs trazados sin crédito para primitivas RW, '
         'CFileMgr/CFileLoader, CTxdStore, CModelInfo::GetModelInfo, azar, operator new/delete, CTimer, scratchpad, CClumpModelInfo base y '
         'subfunciones de la clase en CreateInstance/SetClump; enmascarados el byte alto de SetCarCustomPlate, el byte 0x11 y el relleno '
         '0xFD del pool de SetupCommonData; fuera del fixture CountCompsInRule con negativos, nibbles 6..14, '
         'GetMaximumNumberOfPassengersFromNumberOfDoors(-1), LoadVehicleColours sin # y más de 6 extras. Correcciones de fuente contra el '
         'EXE en VehicleModelInfo.cpp/.h (ver el README del banco). No quedan pendientes en CVehicleModelInfo. Aprobación histórica con '
         'dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/VehModelInfoFinal4/validation-summary.json).',
 'old_appr': 1082,
 'old_groups': 8,
 'old_pend': 628,
 'old_valid': 7818,
 'old_word': 'Mil ochenta y dos',
 'pairs': {'CVehicleModelInfo::AddRemap': 256,
           'CVehicleModelInfo::AddWheelUpgrade': 256,
           'CVehicleModelInfo::AssignRemapTxd': 256,
           'CVehicleModelInfo::ChooseComponent': 512,
           'CVehicleModelInfo::ChooseComponent-Global': 512,
           'CVehicleModelInfo::ChooseSecondComponent': 512,
           'CVehicleModelInfo::ChooseVehicleColour': 512,
           'CVehicleModelInfo::ClearAtomicFlagCB': 128,
           'CVehicleModelInfo::CollapseFramesCB': 256,
           'CVehicleModelInfo::ConvertAnimFileIndex': 128,
           'CVehicleModelInfo::CountCompsInRule': 256,
           'CVehicleModelInfo::CreateInstance': 512,
           'CVehicleModelInfo::DeleteRwObject': 256,
           'CVehicleModelInfo::DeleteVehicleColourTextures': 64,
           'CVehicleModelInfo::DisableEnvMap': 128,
           'CVehicleModelInfo::FindTextureCB': 512,
           'CVehicleModelInfo::GetAnimFileIndex': 64,
           'CVehicleModelInfo::GetCustomCarPlateText': 128,
           'CVehicleModelInfo::GetListOfComponentsNotUsedByRules': 512,
           'CVehicleModelInfo::GetMatFXEffectMaterialCB': 128,
           'CVehicleModelInfo::GetMaximumNumberOfPassengersFromNumberOfDoors': 512,
           'CVehicleModelInfo::GetModelType': 64,
           'CVehicleModelInfo::GetNumDoors': 256,
           'CVehicleModelInfo::GetNumRemaps': 128,
           'CVehicleModelInfo::GetNumWheelUpgrades': 128,
           'CVehicleModelInfo::GetOkAndDamagedAtomicCB': 128,
           'CVehicleModelInfo::GetOriginalCompPosition': 128,
           'CVehicleModelInfo::GetWheelPosn': 256,
           'CVehicleModelInfo::GetWheelUpgrade': 128,
           'CVehicleModelInfo::HasAlphaMaterialCB': 128,
           'CVehicleModelInfo::HideAllComponentsAtomicCB': 128,
           'CVehicleModelInfo::HideDamagedAtomicCB': 256,
           'CVehicleModelInfo::Init': 64,
           'CVehicleModelInfo::IsUpgradeAvailable': 256,
           'CVehicleModelInfo::IsValidCompRule': 256,
           'CVehicleModelInfo::LoadEnvironmentMaps': 128,
           'CVehicleModelInfo::LoadVehicleColours': 256,
           'CVehicleModelInfo::LoadVehicleUpgrades': 256,
           'CVehicleModelInfo::MoveObjectsCB': 64,
           'CVehicleModelInfo::PreprocessHierarchy': 512,
           'CVehicleModelInfo::ReduceMaterialsInVehicle': 256,
           'CVehicleModelInfo::RemoveWindowAlphaCB': 128,
           'CVehicleModelInfo::ResetEditableMaterials': 128,
           'CVehicleModelInfo::SetAnimFile': 256,
           'CVehicleModelInfo::SetAtomicFlagCB': 128,
           'CVehicleModelInfo::SetAtomicRenderCallbacks': 128,
           'CVehicleModelInfo::SetAtomicRendererCB': 256,
           'CVehicleModelInfo::SetAtomicRendererCB_Boat': 128,
           'CVehicleModelInfo::SetAtomicRendererCB_Heli': 128,
           'CVehicleModelInfo::SetAtomicRendererCB_Plane': 128,
           'CVehicleModelInfo::SetAtomicRendererCB_RealHeli': 128,
           'CVehicleModelInfo::SetAtomicRendererCB_Train': 128,
           'CVehicleModelInfo::SetCarCustomPlate': 128,
           'CVehicleModelInfo::SetClump': 256,
           'CVehicleModelInfo::SetCustomCarPlateText': 256,
           'CVehicleModelInfo::SetDirtTextures': 128,
           'CVehicleModelInfo::SetEditableMaterials': 256,
           'CVehicleModelInfo::SetEditableMaterialsCB-RpAtomic': 256,
           'CVehicleModelInfo::SetEditableMaterialsCB-RpMaterial': 512,
           'CVehicleModelInfo::SetEnvMapCoeff': 256,
           'CVehicleModelInfo::SetEnvMapCoeffAtomicCB': 64,
           'CVehicleModelInfo::SetEnvMapCoeffCB': 256,
           'CVehicleModelInfo::SetEnvironmentMapAtomicCB': 64,
           'CVehicleModelInfo::SetEnvironmentMapCB': 256,
           'CVehicleModelInfo::SetRenderPipelines': 128,
           'CVehicleModelInfo::SetRenderPipelinesCB': 64,
           'CVehicleModelInfo::SetVehicleColour': 128,
           'CVehicleModelInfo::SetVehicleComponentFlags': 512,
           'CVehicleModelInfo::SetupCommonData': 128,
           'CVehicleModelInfo::SetupLightFlags': 256,
           'CVehicleModelInfo::ShutdownEnvironmentMaps': 128,
           'CVehicleModelInfo::ShutdownLightTexture': 128,
           'CVehicleModelInfo::StopUsingCommonVehicleTexDicationary': 64,
           'CVehicleModelInfo::StoreAtomicUsedMaterialsCB': 256,
           'CVehicleModelInfo::UseCommonVehicleTexDicationary': 64},
 'pending_counts': {},
 'summary': 'reversing/VehModelInfoFinal4/validation-summary.json'}
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
VALID=sum(x['validated']=='1' for x in rows);PEND=sum(pending.values());GROUPS=len(pending)
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
OLD_APPR=CFG['old_appr'];NEW_APPR=OLD_APPR+CFG['n'];OLD_VALID=CFG['old_valid'];OLD_PEND=CFG['old_pend'];OLD_GROUPS=CFG['old_groups']
assert VALID==OLD_VALID+CFG['n'] and PEND==OLD_PEND-CFG['n'],(VALID,PEND)
assert 'distribuidas en **%d grupos**'%OLD_GROUPS in s
assert GROUPS==OLD_GROUPS-CFG['groups_drop'],GROUPS
for c in CFG['complete']:assert c not in pending,c
for c,k in CFG['pending_counts'].items():assert pending[c]==k,(c,pending[c])
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p_:x['sha256'] for p_,x in old['frozen_markdown_exceptions'].items()}
for p_,h in frozen.items():assert sha(R/p_)==h,p_
INTRO=', hora de Ciudad de México.**'
assert INTRO in s and s.index(INTRO)<4000
s=s.replace(INTRO,' y '+CFG['intro']+INTRO,1)
for a,b in ((f'{OLD_APPR} aprobaciones nuevas',f'{NEW_APPR} aprobaciones nuevas'),(f'{OLD_APPR} identidades nuevas',f'{NEW_APPR} identidades nuevas'),(f'{OLD_VALID} entradas',f'{VALID} entradas'),(f'{OLD_PEND} pendientes',f'{PEND} pendientes'),
 (f'distribuidas en **{OLD_GROUPS} grupos**',f'distribuidas en **{GROUPS} grupos**'),(CFG['old_word']+' aprobaciones nuevas',CFG['new_word']+' aprobaciones nuevas')):
 assert a in s,a;s=s.replace(a,b)
s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text(encoding='utf-8'));new=[x for x in reg['functions'] if x['evidence']['path']==CFG['summary']]
assert len(new)==CFG['n'] and {x['hook']['class']+'::'+x['hook']['fn_name']:x['evidence']['pairs'] for x in new}==CFG['pairs']
LIM=CFG['lim']
newrows=''
for x in new:
 name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
 newrows+=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | {CFG['kind']}; {LIM[name]}x87 PC24, traza de llamadas con argumentos. |\n"
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+newrows.rstrip('\n')+s[pos:]
marker='## Pendientes por clase'
note=CFG['note']+chr(10)+chr(10)
assert marker in s;s=s.replace(marker,note+marker,1);a=s.index('| Clase/grupo | Funciones sin validar |');s=s[:a]+'| Clase/grupo | Funciones sin validar |\n| --- | ---: |\n'+'\n'.join(f'| {c} | {n} |' for c,n in sorted(pending.items()))+'\n';p.write_text(s,encoding='utf8')
plan=json.loads((D/'documentation-plan-107.json').read_text());eligible=set(plan['eligible_current_markdown'])
tracked=set(subprocess.check_output(['git','ls-files','--cached'],cwd=R,text=True).splitlines())
for name_ in subprocess.check_output(['git','ls-files','--cached','--others','--exclude-standard'],cwd=R,text=True).splitlines():
 if name_.endswith('.md') and not any(part in ('artifacts','build','thirdparty','third-party','runs','preparation-archive','baseline-v1','archive','build-history') or 'archive' in part or part.startswith('baseline-before') for part in Path(name_).parts):
  if name_ in tracked or name_ in eligible:eligible.add(name_)
updated=[]
for name_ in sorted(eligible):
 if name_ in frozen or name_=='docs/CurrentStatus.md':continue
 p=R/name_
 if not p.exists():continue
 try:s=p.read_text(encoding='utf-8-sig')
 except UnicodeDecodeError:s=p.read_text(encoding='cp1252')
 s=re.sub(r'<!-- RESA-CURRENT-STATUS:BEGIN -->.*?<!-- RESA-CURRENT-STATUS:END -->\s*','',s,flags=re.S);link=posixpath.relpath('docs/CurrentStatus.md',posixpath.dirname(name_) or '.')
 prefix=f'<!-- RESA-CURRENT-STATUS:BEGIN -->\n> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; {VALID} con evidencia histórica, 0 con vigencia acreditada y {PEND} pendientes en {GROUPS} grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance]({link}). Las mediciones anteriores conservan su fecha y alcance histórico.\n<!-- RESA-CURRENT-STATUS:END -->\n\n';p.write_text(prefix+s,encoding='utf8');updated.append(name_)
for p_,h in frozen.items():assert sha(R/p_)==h,p_
snapshot={'historical':VALID,'pending':PEND,'pending_groups':GROUPS,'current':0,'new_approvals':NEW_APPR}
(HERE/'documentation-refresh-cut447.json').write_text(json.dumps({'date':'2026-10-10','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
