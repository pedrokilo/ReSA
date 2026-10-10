"""Corte documental 450 (+84 funciones: VisPluginsFinal4); conserva byte a byte el Markdown congelado y no toca documentation-frozen-exceptions.json."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
CFG={'complete': ['CVisibilityPlugins'],
 'groups_drop': 1,
 'intro': 'ochenta y cuatro de CVisibilityPlugins (VisPluginsFinal4)',
 'kind': 'CPU en el juego completo con partida cargada, mundo falso estático y colaboradores doblados con traza (banco por '
         'pares, passed=true global)',
 'lim': {'CVisibilityPlugins::AddWeaponPedForPC': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::AtomicCopyConstructor': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::AtomicDestructor': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::CalculateFadingAtomicAlpha': 'mundo falso estático y colaboradores doblados con traza sin crédito; '
                                                           'fuente corregida (0x732500-0x7325B8): selección < exacta, factor '
                                                           '0.06666667f, recorte solo si > 1.0, _ftol2 de 64 bits. ',
         'CVisibilityPlugins::ClearAtomicFlag': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::ClearClumpForAllAtomicsFlag': 'mundo falso estático y colaboradores doblados con traza sin '
                                                            'crédito; ',
         'CVisibilityPlugins::ClumpConstructor': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::ClumpCopyConstructor': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::ClumpDestructor': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::DefaultVisibilityCB': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::FrameConstructor': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::FrameCopyConstructor': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::FrameDestructor': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::FrustumSphereCB': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::GetAtomicId': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::GetClumpAlpha': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::GetClumpModelInfo': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::GetDistanceSquaredFromCamera-Frame': 'mundo falso estático y colaboradores doblados con traza sin '
                                                                   'crédito; fuente corregida (0x732CA7): suma (z²+y²)+x². ',
         'CVisibilityPlugins::GetDistanceSquaredFromCamera-RwV3d': 'mundo falso estático y colaboradores doblados con traza sin '
                                                                   'crédito; fuente corregida (0x732CDA): suma (z²+y²)+x². ',
         'CVisibilityPlugins::GetDotProductWithCameraVector': 'mundo falso estático y colaboradores doblados con traza sin '
                                                              'crédito; fuente corregida (0x7326E0-0x732776): productos '
                                                              'escalares en orden y*y + x*x + z*z. ',
         'CVisibilityPlugins::GetFrameHierarchyId': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::GetModelInfo': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::GetModelInfoIndex': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::GetUserValue': 'mundo falso estático y colaboradores doblados con traza sin crédito; el retorno es '
                                             'AX: se enmascara a 16 bits. ',
         'CVisibilityPlugins::InitAlphaAtomicList': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::InitAlphaEntityList': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::Initialise': 'mundo falso estático y colaboradores doblados con traza sin crédito; operator '
                                           'new/delete doblados con traza sobre una arena de 128 KB, sin crédito. ',
         'CVisibilityPlugins::InsertAtomicIntoReallyDrawLastList': 'mundo falso estático y colaboradores doblados con traza sin '
                                                                   'crédito; solo se compara AL del retorno bool. ',
         'CVisibilityPlugins::InsertEntityIntoReallyDrawLastList': 'mundo falso estático y colaboradores doblados con traza sin '
                                                                   'crédito; solo se compara AL del retorno bool. ',
         'CVisibilityPlugins::InsertEntityIntoSortedList': 'mundo falso estático y colaboradores doblados con traza sin crédito; '
                                                           'fuente corregida (0x734570-0x73458B): movsx del índice de la '
                                                           'entidad. solo se compara AL del retorno bool. ',
         'CVisibilityPlugins::InsertEntityIntoUnderwaterList': 'mundo falso estático y colaboradores doblados con traza sin '
                                                               'crédito; solo se compara AL del retorno bool. ',
         'CVisibilityPlugins::InsertObjectIntoSortedList': 'mundo falso estático y colaboradores doblados con traza sin crédito; '
                                                           'solo se compara AL del retorno bool. ',
         'CVisibilityPlugins::IsAtomicVisible': 'mundo falso estático y colaboradores doblados con traza sin crédito; fuente '
                                                'corregida (0x7329C4-0x7329D8): RwV3dTransformPoints transforma la esfera en el '
                                                'sitio. solo se compara AL del retorno bool. ',
         'CVisibilityPlugins::IsClumpVisible': 'mundo falso estático y colaboradores doblados con traza sin crédito; fuente '
                                               'corregida (0x732AE0): llama a la callback del plugin con el clump (la DLL '
                                               'devolvía el puntero como bool). solo se compara AL del retorno bool. ',
         'CVisibilityPlugins::PluginAttach': 'mundo falso estático y colaboradores doblados con traza sin crédito; los tres '
                                             'callbacks de plugin (constructor, copia y destructor) se ejecutan de verdad sobre '
                                             'objetos de prueba, de modo que AtomicConstructor (0x732150, sin hook) queda '
                                             'comparado; solo se compara AL del retorno bool. ',
         'CVisibilityPlugins::RenderAlphaAtomics': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::RenderAtomicWithAlpha': 'mundo falso estático y colaboradores doblados con traza sin crédito; '
                                                      'fuente corregida (0x7324B6-0x7324EF): comparación con signo contra el '
                                                      'entero completo y restauración de atrás hacia delante. ',
         'CVisibilityPlugins::RenderAtomicWithAlphaCB': 'mundo falso estático y colaboradores doblados con traza sin crédito; '
                                                        'fuente corregida (0x73266B): data apunta al entero con la alfa. ',
         'CVisibilityPlugins::RenderBoatAlphaAtomics': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::RenderEntity': 'mundo falso estático y colaboradores doblados con traza sin crédito; fuente '
                                             'corregida (0x732B50): movsx del índice de la entidad frente a movzx del global. ',
         'CVisibilityPlugins::RenderFadingAtomic': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::RenderFadingClump': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::RenderFadingClumpCB': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::RenderFadingEntities': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::RenderFadingUnderwaterEntities': 'mundo falso estático y colaboradores doblados con traza sin '
                                                               'crédito; ',
         'CVisibilityPlugins::RenderHeliRotorAlphaCB': 'mundo falso estático y colaboradores doblados con traza sin crédito; '
                                                       'fuente corregida (0x7340E8-0x734137): orden dist + 20*dot(pos-cámara, at '
                                                       'del clump). ',
         'CVisibilityPlugins::RenderHeliTailRotorAlphaCB': 'mundo falso estático y colaboradores doblados con traza sin crédito; '
                                                           'fuente corregida (0x7341C2-0x734202): ((-dot_right) - dot_up) + dist '
                                                           'con suma y*y + x*x + z*z. ',
         'CVisibilityPlugins::RenderObjNormalAtomic': 'mundo falso estático y colaboradores doblados con traza sin crédito; '
                                                      'fuente corregida (0x732420-0x73243A): producto escalar en orden y*y + x*x '
                                                      '+ z*z. ',
         'CVisibilityPlugins::RenderOrderedList': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::RenderPedCB': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::RenderPlayerCB': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::RenderReallyDrawLastObjects': 'mundo falso estático y colaboradores doblados con traza sin '
                                                            'crédito; no se compara el rastreo de Tracy. ',
         'CVisibilityPlugins::RenderTrainHiDetailAlphaCB': 'mundo falso estático y colaboradores doblados con traza sin '
                                                           'crédito; ',
         'CVisibilityPlugins::RenderTrainHiDetailCB': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB': 'mundo falso estático y colaboradores doblados con traza sin '
                                                             'crédito; ',
         'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_BigVehicle': 'mundo falso estático y colaboradores doblados con traza '
                                                                        'sin crédito; ',
         'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_Boat': 'mundo falso estático y colaboradores doblados con traza sin '
                                                                  'crédito; ',
         'CVisibilityPlugins::RenderVehicleHiDetailCB': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::RenderVehicleHiDetailCB_BigVehicle': 'mundo falso estático y colaboradores doblados con traza sin '
                                                                   'crédito; ',
         'CVisibilityPlugins::RenderVehicleHiDetailCB_Boat': 'mundo falso estático y colaboradores doblados con traza sin '
                                                             'crédito; ',
         'CVisibilityPlugins::RenderVehicleLoDetailCB_Boat': 'mundo falso estático y colaboradores doblados con traza sin '
                                                             'crédito; ',
         'CVisibilityPlugins::RenderVehicleReallyLowDetailCB': 'mundo falso estático y colaboradores doblados con traza sin '
                                                               'crédito; ',
         'CVisibilityPlugins::RenderVehicleReallyLowDetailCB_BigVehicle': 'mundo falso estático y colaboradores doblados con '
                                                                          'traza sin crédito; ',
         'CVisibilityPlugins::RenderWeaponCB': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::RenderWeaponPedsForPC': 'mundo falso estático y colaboradores doblados con traza sin crédito; no '
                                                      'se compara el rastreo de Tracy. ',
         'CVisibilityPlugins::RenderWheelAtomicCB': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::SetAlphaCB': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::SetAtomicFlag': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::SetAtomicId': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::SetAtomicRenderCallback': 'mundo falso estático y colaboradores doblados con traza sin crédito; '
                                                        'fuente corregida (0x7328A6-0x7328B8): guarda 0x7491C0 y no su '
                                                        'envoltorio. ',
         'CVisibilityPlugins::SetClumpAlpha': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::SetClumpForAllAtomicsFlag': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::SetClumpModelInfo': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::SetFrameHierarchyId': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::SetModelInfoIndex': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::SetRenderWareCamera': 'mundo falso estático y colaboradores doblados con traza sin crédito; fuente '
                                                    'corregida (0x7328C0-0x732988): v*v en float, sin powf. ',
         'CVisibilityPlugins::SetTextureCB': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::SetUserValue': 'mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CVisibilityPlugins::SetupVehicleVariables': 'mundo falso estático y colaboradores doblados con traza sin crédito; '
                                                      'fuente corregida (0x733190-0x7331D4): suma (z²+y²)+x², atan2 por fpatan '
                                                      'x87, sqrt en float, cámara menos posición del frame; variables globales '
                                                      'en 0xC88024/0xC88020. ',
         'CVisibilityPlugins::Shutdown': 'mundo falso estático y colaboradores doblados con traza sin crédito; operator '
                                         'new/delete doblados con traza sobre una arena de 128 KB, sin crédito. ',
         'CVisibilityPlugins::VehicleVisibilityCB': 'mundo falso estático y colaboradores doblados con traza sin crédito; fuente '
                                                    'corregida (0x73372B-0x73373C): condiciones !(a > b) y !(a < b) (con NaN el '
                                                    'EXE toma la otra rama). ',
         'CVisibilityPlugins::VehicleVisibilityCB_BigVehicle': 'mundo falso estático y colaboradores doblados con traza sin '
                                                               'crédito; ',
         'CVisibilityPlugins::clearClumpForAllAtomicsFlagCB': 'mundo falso estático y colaboradores doblados con traza sin '
                                                              'crédito; ',
         'CVisibilityPlugins::setClumpForAllAtomicsFlagCB': 'mundo falso estático y colaboradores doblados con traza sin '
                                                            'crédito; '},
 'n': 84,
 'new_word': 'Mil cuatrocientas catorce',
 'note': 'VisPluginsFinal4 añade la aprobación de las 84 funciones de CVisibilityPlugins, 18176 pares con cero diferencias. La '
         'corrida 20261010-130234-135622 tiene passed=true global y evento complete con state_restored=true y layout_ok=true, '
         'saves y configuraciones intactos, con el banco (hooks de la clase apagados) frente a la candidata dentro del juego '
         'completo con partida cargada. Límites: mundo falso estático (atomics, frames, clumps, geometrías, model infos, '
         'entidades, peds con arma, cámara RW); variables y listas de la clase, offsets de plugin y estados de render guardados '
         'y restaurados en cada ejecución; primitivas RenderWare, luces, CRenderer::RenderOneNonRoad, CPed/CWeaponInfo y '
         'operator new/delete doblados con traza sin crédito; fixtures sin sNaN. Se corrigió la fuente de CVisibilityPlugins '
         'contra el EXE en varias funciones (ver el README del banco). Son aprobaciones históricas con dependency_review=null; '
         'la vigencia sigue en 0 y CVisibilityPlugins queda sin pendientes.',
 'old_appr': 1330,
 'old_groups': 5,
 'old_pend': 380,
 'old_valid': 8066,
 'old_word': 'Mil trescientas treinta',
 'pairs': {'CVisibilityPlugins::AddWeaponPedForPC': 192,
           'CVisibilityPlugins::AtomicCopyConstructor': 96,
           'CVisibilityPlugins::AtomicDestructor': 96,
           'CVisibilityPlugins::CalculateFadingAtomicAlpha': 512,
           'CVisibilityPlugins::ClearAtomicFlag': 96,
           'CVisibilityPlugins::ClearClumpForAllAtomicsFlag': 96,
           'CVisibilityPlugins::ClumpConstructor': 96,
           'CVisibilityPlugins::ClumpCopyConstructor': 96,
           'CVisibilityPlugins::ClumpDestructor': 96,
           'CVisibilityPlugins::DefaultVisibilityCB': 96,
           'CVisibilityPlugins::FrameConstructor': 96,
           'CVisibilityPlugins::FrameCopyConstructor': 96,
           'CVisibilityPlugins::FrameDestructor': 96,
           'CVisibilityPlugins::FrustumSphereCB': 256,
           'CVisibilityPlugins::GetAtomicId': 96,
           'CVisibilityPlugins::GetClumpAlpha': 96,
           'CVisibilityPlugins::GetClumpModelInfo': 96,
           'CVisibilityPlugins::GetDistanceSquaredFromCamera-Frame': 256,
           'CVisibilityPlugins::GetDistanceSquaredFromCamera-RwV3d': 256,
           'CVisibilityPlugins::GetDotProductWithCameraVector': 512,
           'CVisibilityPlugins::GetFrameHierarchyId': 96,
           'CVisibilityPlugins::GetModelInfo': 96,
           'CVisibilityPlugins::GetModelInfoIndex': 96,
           'CVisibilityPlugins::GetUserValue': 96,
           'CVisibilityPlugins::InitAlphaAtomicList': 96,
           'CVisibilityPlugins::InitAlphaEntityList': 96,
           'CVisibilityPlugins::Initialise': 96,
           'CVisibilityPlugins::InsertAtomicIntoReallyDrawLastList': 256,
           'CVisibilityPlugins::InsertEntityIntoReallyDrawLastList': 256,
           'CVisibilityPlugins::InsertEntityIntoSortedList': 384,
           'CVisibilityPlugins::InsertEntityIntoUnderwaterList': 256,
           'CVisibilityPlugins::InsertObjectIntoSortedList': 256,
           'CVisibilityPlugins::IsAtomicVisible': 256,
           'CVisibilityPlugins::IsClumpVisible': 96,
           'CVisibilityPlugins::PluginAttach': 192,
           'CVisibilityPlugins::RenderAlphaAtomics': 256,
           'CVisibilityPlugins::RenderAtomicWithAlpha': 384,
           'CVisibilityPlugins::RenderAtomicWithAlphaCB': 256,
           'CVisibilityPlugins::RenderBoatAlphaAtomics': 256,
           'CVisibilityPlugins::RenderEntity': 512,
           'CVisibilityPlugins::RenderFadingAtomic': 256,
           'CVisibilityPlugins::RenderFadingClump': 256,
           'CVisibilityPlugins::RenderFadingClumpCB': 256,
           'CVisibilityPlugins::RenderFadingEntities': 256,
           'CVisibilityPlugins::RenderFadingUnderwaterEntities': 256,
           'CVisibilityPlugins::RenderHeliRotorAlphaCB': 256,
           'CVisibilityPlugins::RenderHeliTailRotorAlphaCB': 256,
           'CVisibilityPlugins::RenderObjNormalAtomic': 256,
           'CVisibilityPlugins::RenderOrderedList': 192,
           'CVisibilityPlugins::RenderPedCB': 256,
           'CVisibilityPlugins::RenderPlayerCB': 256,
           'CVisibilityPlugins::RenderReallyDrawLastObjects': 256,
           'CVisibilityPlugins::RenderTrainHiDetailAlphaCB': 384,
           'CVisibilityPlugins::RenderTrainHiDetailCB': 384,
           'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB': 512,
           'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_BigVehicle': 512,
           'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_Boat': 256,
           'CVisibilityPlugins::RenderVehicleHiDetailCB': 512,
           'CVisibilityPlugins::RenderVehicleHiDetailCB_BigVehicle': 384,
           'CVisibilityPlugins::RenderVehicleHiDetailCB_Boat': 256,
           'CVisibilityPlugins::RenderVehicleLoDetailCB_Boat': 256,
           'CVisibilityPlugins::RenderVehicleReallyLowDetailCB': 256,
           'CVisibilityPlugins::RenderVehicleReallyLowDetailCB_BigVehicle': 256,
           'CVisibilityPlugins::RenderWeaponCB': 256,
           'CVisibilityPlugins::RenderWeaponPedsForPC': 512,
           'CVisibilityPlugins::RenderWheelAtomicCB': 256,
           'CVisibilityPlugins::SetAlphaCB': 128,
           'CVisibilityPlugins::SetAtomicFlag': 96,
           'CVisibilityPlugins::SetAtomicId': 96,
           'CVisibilityPlugins::SetAtomicRenderCallback': 128,
           'CVisibilityPlugins::SetClumpAlpha': 96,
           'CVisibilityPlugins::SetClumpForAllAtomicsFlag': 96,
           'CVisibilityPlugins::SetClumpModelInfo': 192,
           'CVisibilityPlugins::SetFrameHierarchyId': 96,
           'CVisibilityPlugins::SetModelInfoIndex': 96,
           'CVisibilityPlugins::SetRenderWareCamera': 256,
           'CVisibilityPlugins::SetTextureCB': 96,
           'CVisibilityPlugins::SetUserValue': 96,
           'CVisibilityPlugins::SetupVehicleVariables': 256,
           'CVisibilityPlugins::Shutdown': 192,
           'CVisibilityPlugins::VehicleVisibilityCB': 256,
           'CVisibilityPlugins::VehicleVisibilityCB_BigVehicle': 192,
           'CVisibilityPlugins::clearClumpForAllAtomicsFlagCB': 96,
           'CVisibilityPlugins::setClumpForAllAtomicsFlagCB': 96},
 'pending_counts': {'CAEVehicleAudioEntity': 102, 'CAutomobile': 99, 'CStreaming': 94, 'Win': 1},
 'summary': 'reversing/VisPluginsFinal4/validation-summary.json'}
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
(HERE/'documentation-refresh-cut450.json').write_text(json.dumps({'date':'2026-10-10','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
