"""Registro historico de las 63 funciones de CRadar del banco RadarFinal3 (corrida 20261009-203202-095801). Re-audita en streaming la corrida, conserva la evidencia en runs/ y anade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'bank': 'RadarFinal3',
 'run_id': '20261009-203202-095801',
 'probe_prefix': 'c1dce395',
 'tag': 'radarfinal3',
 'registry_before': 7550,
 'error': None,
 'passed': True,
 'bad': {},
 'expect': {'CRadar::AddBlipToLegendList': 256,
            'CRadar::CalculateBlipAlpha': 256,
            'CRadar::CalculateCachedSinCos': 512,
            'CRadar::ChangeBlipBrightness': 128,
            'CRadar::ChangeBlipColour': 128,
            'CRadar::ChangeBlipDisplay': 128,
            'CRadar::ChangeBlipScale': 128,
            'CRadar::ClearActualBlip-OG': 128,
            'CRadar::ClearBlip': 128,
            'CRadar::ClearBlipForEntity-OG': 128,
            'CRadar::ClipRadarTileCoords': 128,
            'CRadar::DisplayThisBlip': 512,
            'CRadar::Draw3dMarkers': 512,
            'CRadar::DrawAreaOnRadar': 512,
            'CRadar::DrawBlips': 512,
            'CRadar::DrawCoordBlip': 1024,
            'CRadar::DrawLegend': 512,
            'CRadar::DrawMap': 512,
            'CRadar::DrawRadarGangOverlay': 256,
            'CRadar::DrawRadarMap': 256,
            'CRadar::DrawRadarMask': 128,
            'CRadar::DrawRadarSection': 512,
            'CRadar::DrawRadarSectionMap': 256,
            'CRadar::DrawRadarSprite': 256,
            'CRadar::DrawRotatingRadarSprite': 256,
            'CRadar::DrawYouAreHereSprite': 256,
            'CRadar::GetActualBlipArrayIndex': 128,
            'CRadar::GetNewUniqueBlipIndex': 128,
            'CRadar::GetRadarTraceColour': 128,
            'CRadar::GetTextureCorners': 128,
            'CRadar::InitFrontEndMap': 128,
            'CRadar::Initialise': 64,
            'CRadar::IsPointInsideRadar': 256,
            'CRadar::LimitRadarPoint': 256,
            'CRadar::LimitToMap': 256,
            'CRadar::LineRadarBoxCollision': 512,
            'CRadar::Load': 64,
            'CRadar::LoadTextures': 64,
            'CRadar::RemoveMapSection': 128,
            'CRadar::RemoveRadarSections': 64,
            'CRadar::RequestMapSection': 128,
            'CRadar::Save': 64,
            'CRadar::SetBlipAlwaysDisplayInZoom': 128,
            'CRadar::SetBlipEntryExit': 128,
            'CRadar::SetBlipFade': 128,
            'CRadar::SetBlipFriendly': 128,
            'CRadar::SetBlipSprite': 128,
            'CRadar::SetCoordBlip': 256,
            'CRadar::SetCoordBlipAppearance': 128,
            'CRadar::SetEntityBlip': 256,
            'CRadar::SetMapCentreToPlayerCoords': 128,
            'CRadar::SetShortRangeCoordBlip': 256,
            'CRadar::SetupAirstripBlips': 512,
            'CRadar::SetupRadarRect': 64,
            'CRadar::ShowRadarMarker': 128,
            'CRadar::ShowRadarTrace': 256,
            'CRadar::ShowRadarTraceWithHeight': 512,
            'CRadar::Shutdown': 64,
            'CRadar::StreamRadarSections-vec': 256,
            'CRadar::StreamRadarSections-xy': 128,
            'CRadar::TransformRadarPointToRealWorldSpace': 128,
            'CRadar::TransformRadarPointToScreenSpace': 256,
            'CRadar::TransformRealWorldPointToRadarSpace': 256},
 'total': 15040,
 'new': ['CRadar::AddBlipToLegendList',
         'CRadar::CalculateBlipAlpha',
         'CRadar::CalculateCachedSinCos',
         'CRadar::ChangeBlipBrightness',
         'CRadar::ChangeBlipColour',
         'CRadar::ChangeBlipDisplay',
         'CRadar::ChangeBlipScale',
         'CRadar::ClearActualBlip-OG',
         'CRadar::ClearBlip',
         'CRadar::ClearBlipForEntity-OG',
         'CRadar::ClipRadarTileCoords',
         'CRadar::DisplayThisBlip',
         'CRadar::Draw3dMarkers',
         'CRadar::DrawAreaOnRadar',
         'CRadar::DrawBlips',
         'CRadar::DrawCoordBlip',
         'CRadar::DrawLegend',
         'CRadar::DrawMap',
         'CRadar::DrawRadarGangOverlay',
         'CRadar::DrawRadarMap',
         'CRadar::DrawRadarMask',
         'CRadar::DrawRadarSection',
         'CRadar::DrawRadarSectionMap',
         'CRadar::DrawRadarSprite',
         'CRadar::DrawRotatingRadarSprite',
         'CRadar::DrawYouAreHereSprite',
         'CRadar::GetActualBlipArrayIndex',
         'CRadar::GetNewUniqueBlipIndex',
         'CRadar::GetRadarTraceColour',
         'CRadar::GetTextureCorners',
         'CRadar::InitFrontEndMap',
         'CRadar::Initialise',
         'CRadar::IsPointInsideRadar',
         'CRadar::LimitRadarPoint',
         'CRadar::LimitToMap',
         'CRadar::LineRadarBoxCollision',
         'CRadar::Load',
         'CRadar::LoadTextures',
         'CRadar::RemoveMapSection',
         'CRadar::RemoveRadarSections',
         'CRadar::RequestMapSection',
         'CRadar::Save',
         'CRadar::SetBlipAlwaysDisplayInZoom',
         'CRadar::SetBlipEntryExit',
         'CRadar::SetBlipFade',
         'CRadar::SetBlipFriendly',
         'CRadar::SetBlipSprite',
         'CRadar::SetCoordBlip',
         'CRadar::SetCoordBlipAppearance',
         'CRadar::SetEntityBlip',
         'CRadar::SetMapCentreToPlayerCoords',
         'CRadar::SetShortRangeCoordBlip',
         'CRadar::SetupAirstripBlips',
         'CRadar::SetupRadarRect',
         'CRadar::ShowRadarMarker',
         'CRadar::ShowRadarTrace',
         'CRadar::ShowRadarTraceWithHeight',
         'CRadar::Shutdown',
         'CRadar::StreamRadarSections-vec',
         'CRadar::StreamRadarSections-xy',
         'CRadar::TransformRadarPointToRealWorldSpace',
         'CRadar::TransformRadarPointToScreenSpace',
         'CRadar::TransformRealWorldPointToRadarSpace'],
 'rejected': [],
 'addr': {'CRadar::AddBlipToLegendList': '0x005859F0',
          'CRadar::CalculateBlipAlpha': '0x00583420',
          'CRadar::CalculateCachedSinCos': '0x00583670',
          'CRadar::ChangeBlipBrightness': '0x00583C70',
          'CRadar::ChangeBlipColour': '0x00583AB0',
          'CRadar::ChangeBlipDisplay': '0x00583D20',
          'CRadar::ChangeBlipScale': '0x00583CC0',
          'CRadar::ClearActualBlip-OG': '0x00587C10',
          'CRadar::ClearBlip': '0x00587CE0',
          'CRadar::ClearBlipForEntity-OG': '0x00587C60',
          'CRadar::ClipRadarTileCoords': '0x00584B00',
          'CRadar::DisplayThisBlip': '0x00583B40',
          'CRadar::Draw3dMarkers': '0x00585BF0',
          'CRadar::DrawAreaOnRadar': '0x005853D0',
          'CRadar::DrawBlips': '0x00588050',
          'CRadar::DrawCoordBlip': '0x00586D60',
          'CRadar::DrawLegend': '0x005828A0',
          'CRadar::DrawMap': '0x00586B00',
          'CRadar::DrawRadarGangOverlay': '0x00586650',
          'CRadar::DrawRadarMap': '0x00586880',
          'CRadar::DrawRadarMask': '0x00585700',
          'CRadar::DrawRadarSection': '0x00586110',
          'CRadar::DrawRadarSectionMap': '0x00586520',
          'CRadar::DrawRadarSprite': '0x00585FF0',
          'CRadar::DrawRotatingRadarSprite': '0x00584850',
          'CRadar::DrawYouAreHereSprite': '0x00584960',
          'CRadar::GetActualBlipArrayIndex': '0x00582870',
          'CRadar::GetNewUniqueBlipIndex': '0x00582820',
          'CRadar::GetRadarTraceColour': '0x00584770',
          'CRadar::GetTextureCorners': '0x00584D90',
          'CRadar::InitFrontEndMap': '0x00585960',
          'CRadar::Initialise': '0x00587FB0',
          'CRadar::IsPointInsideRadar': '0x00584D40',
          'CRadar::LimitRadarPoint': '0x005832F0',
          'CRadar::LimitToMap': '0x00583350',
          'CRadar::LineRadarBoxCollision': '0x00584E00',
          'CRadar::Load': '0x005D53C0',
          'CRadar::LoadTextures': '0x005827D0',
          'CRadar::RemoveMapSection': '0x00584BB0',
          'CRadar::RemoveRadarSections': '0x00584BF0',
          'CRadar::RequestMapSection': '0x00584B50',
          'CRadar::Save': '0x005D5860',
          'CRadar::SetBlipAlwaysDisplayInZoom': '0x00583DB0',
          'CRadar::SetBlipEntryExit': '0x00583F00',
          'CRadar::SetBlipFade': '0x00583E00',
          'CRadar::SetBlipFriendly': '0x00583EB0',
          'CRadar::SetBlipSprite': '0x00583D70',
          'CRadar::SetCoordBlip': '0x00583820',
          'CRadar::SetCoordBlipAppearance': '0x00583E50',
          'CRadar::SetEntityBlip': '0x005839A0',
          'CRadar::SetMapCentreToPlayerCoords': '0x00585B20',
          'CRadar::SetShortRangeCoordBlip': '0x00583920',
          'CRadar::SetupAirstripBlips': '0x00587D20',
          'CRadar::SetupRadarRect': '0x00584A80',
          'CRadar::ShowRadarMarker': '0x00584480',
          'CRadar::ShowRadarTrace': '0x00583F40',
          'CRadar::ShowRadarTraceWithHeight': '0x00584070',
          'CRadar::Shutdown': '0x00585940',
          'CRadar::StreamRadarSections-vec': '0x005858D0',
          'CRadar::StreamRadarSections-xy': '0x00584C50',
          'CRadar::TransformRadarPointToRealWorldSpace': '0x005835A0',
          'CRadar::TransformRadarPointToScreenSpace': '0x00583480',
          'CRadar::TransformRealWorldPointToRadarSpace': '0x00583530'},
 'types': {'CRadar::AddBlipToLegendList': 'S',
           'CRadar::CalculateBlipAlpha': 'S',
           'CRadar::CalculateCachedSinCos': 'S',
           'CRadar::ChangeBlipBrightness': 'S',
           'CRadar::ChangeBlipColour': 'S',
           'CRadar::ChangeBlipDisplay': 'S',
           'CRadar::ChangeBlipScale': 'S',
           'CRadar::ClearActualBlip-OG': 'S',
           'CRadar::ClearBlip': 'S',
           'CRadar::ClearBlipForEntity-OG': 'S',
           'CRadar::ClipRadarTileCoords': 'S',
           'CRadar::DisplayThisBlip': 'S',
           'CRadar::Draw3dMarkers': 'S',
           'CRadar::DrawAreaOnRadar': 'S',
           'CRadar::DrawBlips': 'S',
           'CRadar::DrawCoordBlip': 'S',
           'CRadar::DrawLegend': 'S',
           'CRadar::DrawMap': 'S',
           'CRadar::DrawRadarGangOverlay': 'S',
           'CRadar::DrawRadarMap': 'S',
           'CRadar::DrawRadarMask': 'S',
           'CRadar::DrawRadarSection': 'S',
           'CRadar::DrawRadarSectionMap': 'S',
           'CRadar::DrawRadarSprite': 'S',
           'CRadar::DrawRotatingRadarSprite': 'S',
           'CRadar::DrawYouAreHereSprite': 'S',
           'CRadar::GetActualBlipArrayIndex': 'S',
           'CRadar::GetNewUniqueBlipIndex': 'S',
           'CRadar::GetRadarTraceColour': 'S',
           'CRadar::GetTextureCorners': 'S',
           'CRadar::InitFrontEndMap': 'S',
           'CRadar::Initialise': 'S',
           'CRadar::IsPointInsideRadar': 'S',
           'CRadar::LimitRadarPoint': 'S',
           'CRadar::LimitToMap': 'S',
           'CRadar::LineRadarBoxCollision': 'S',
           'CRadar::Load': 'S',
           'CRadar::LoadTextures': 'S',
           'CRadar::RemoveMapSection': 'S',
           'CRadar::RemoveRadarSections': 'S',
           'CRadar::RequestMapSection': 'S',
           'CRadar::Save': 'S',
           'CRadar::SetBlipAlwaysDisplayInZoom': 'S',
           'CRadar::SetBlipEntryExit': 'S',
           'CRadar::SetBlipFade': 'S',
           'CRadar::SetBlipFriendly': 'S',
           'CRadar::SetBlipSprite': 'S',
           'CRadar::SetCoordBlip': 'S',
           'CRadar::SetCoordBlipAppearance': 'S',
           'CRadar::SetEntityBlip': 'S',
           'CRadar::SetMapCentreToPlayerCoords': 'S',
           'CRadar::SetShortRangeCoordBlip': 'S',
           'CRadar::SetupAirstripBlips': 'S',
           'CRadar::SetupRadarRect': 'S',
           'CRadar::ShowRadarMarker': 'S',
           'CRadar::ShowRadarTrace': 'S',
           'CRadar::ShowRadarTraceWithHeight': 'S',
           'CRadar::Shutdown': 'S',
           'CRadar::StreamRadarSections-vec': 'S',
           'CRadar::StreamRadarSections-xy': 'S',
           'CRadar::TransformRadarPointToRealWorldSpace': 'S',
           'CRadar::TransformRadarPointToScreenSpace': 'S',
           'CRadar::TransformRealWorldPointToRadarSpace': 'S'},
 'pending_after': [],
 'allow_src_change': [],
 'allow_bank_change': [],
 'extra_unregistered': [],
 'provenance': 'Exact compiled RadarFinal3 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build manifest and '
               'equal to the working tree at registration; full 15040-row log reaudited offline in streaming and preserved gzipped with the original '
               'hash.',
 'common': 'Simple pair bench inside the full game with a loaded save (original with the function hook off vs candidate with the hook on), 15040 '
           'isolated CPU pairs for 63 functions of CRadar, ALL pairs equal, pair counts equal to the bank table, passed=true globally, natural '
           '"complete" event with state_restored, saves unchanged, configs restored. Each case rebuilds the same deterministic fixture (seed = case '
           'index) in both legs and compares the return value, the bytes of the fake objects, the touched global ranges, the x87 control word (24 '
           'bits) and the whole trace of doubled collaborators with raw arguments; foreign threads ignored. Traced doubles get NO credit. Only the '
           'own part of each function is validated. Source tested == current working tree at registration (class sources match the probe build '
           'manifest). Historical evidence approved with explicit limits; no current dependency accreditation. Padding and uninitialised bytes '
           'masked. Limits are declared per function in the function-specific scope and in the bank README.',
 'specific': {'CRadar::AddBlipToLegendList': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                             'Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, '
                                             'vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas '
                                             'enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::CalculateBlipAlpha': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                            'fuente corregida: constante 0x85F0A0. Limites del banco: motor RW falso (zBufferFar, estados, Im2D), '
                                            'pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin '
                                            'credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::CalculateCachedSinCos': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git '
                                               'diff). objetivo sin matriz con dobles de AllocateMatrix/UpdateMatrix; fuente corregida: fpatan/fsin '
                                               'sin redondear. Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, '
                                               'EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y '
                                               'alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::ChangeBlipBrightness': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                              'fuente corregida: 0x583CAE. Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools '
                                              'falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); '
                                              'colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::ChangeBlipColour': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                          'Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, '
                                          'peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; '
                                          'FindPlayerPed/Vehicle no se trazan.',
              'CRadar::ChangeBlipDisplay': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                           'Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, '
                                           'vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados '
                                           'a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::ChangeBlipScale': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                         'Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, '
                                         'peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; '
                                         'FindPlayerPed/Vehicle no se trazan.',
              'CRadar::ClearActualBlip-OG': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                            'Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, '
                                            'vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas '
                                            'enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::ClearBlip': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). Limites '
                                   'del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y '
                                   'objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; '
                                   'FindPlayerPed/Vehicle no se trazan.',
              'CRadar::ClearBlipForEntity-OG': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                               'diff). Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, '
                                               'EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y '
                                               'alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::ClipRadarTileCoords': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                             'Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, '
                                             'vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas '
                                             'enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::DisplayThisBlip': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                         'GetBlipName con sprites 0,1,2,4,56 o >63 fuera de dominio (NOTSA_UNREACHABLE en la DLL). Limites del '
                                         'banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y '
                                         'objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; '
                                         'FindPlayerPed/Vehicle no se trazan.',
              'CRadar::Draw3dMarkers': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                       'FindPlayerPed siempre valido (el EXE desreferencia [eax+2Fh] sin comprobar nulo, 0x585F37); fuente '
                                       'corregida: Nevada 0.6, orden de suma, enex. Limites del banco: motor RW falso (zBufferFar, estados, Im2D), '
                                       'pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); '
                                       'colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::DrawAreaOnRadar': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff). solo '
                                         'rectangulos ordenados porque ClipRadarPoly (0x585040) sobrepasa un arreglo local de 8 vertices con '
                                         'rectangulo invertido (0x5851F4, 0x585300) y el EXE no ordena (0x585431..0x5855B9); UV enmascaradas por '
                                         'memoria sin inicializar (0x585662..0x5856B5), solo se traza posicion y color; fuente corregida: rechazo '
                                         '0x5853E2. Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, '
                                         'vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados a '
                                         '8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::DrawBlips': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                   'FindPlayerPed siempre valido porque el EXE desreferencia [eax+2Fh] sin comprobar nulo (0x585CA7); fuente '
                                   'corregida: orden isSprite, jugador 0, pi, color 50/50/80, ancho=alto. Limites del banco: motor RW falso '
                                   '(zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados '
                                   'con traza (sin credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::DrawCoordBlip': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                       'fuente corregida: apariencia, alfa, marcador de depuracion, alturas. Limites del banco: motor RW falso '
                                       '(zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD '
                                       'doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se '
                                       'trazan.',
              'CRadar::DrawLegend': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff). fuente '
                                    'corregida: 0x582E62. Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, '
                                    'EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas '
                                    'enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::DrawMap': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                 'FindPlayerPed siempre valido (el EXE desreferencia [eax+2Fh] sin comprobar nulo, 0x583B50); fuente corregida: sin '
                                 'FindPlayerPed inicial, 16.67, coop. Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de '
                                 'TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas '
                                 'enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::DrawRadarGangOverlay': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                              'fuente corregida: constante 0x865034. Limites del banco: motor RW falso (zBufferFar, estados, Im2D), '
                                              'pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin '
                                              'credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::DrawRadarMap': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                      'vehiculos con matriz (0x586A76); fuente corregida: pi double, fpatan. Limites del banco: motor RW falso '
                                      '(zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados '
                                      'con traza (sin credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::DrawRadarMask': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). fuente '
                                       'corregida: cuadrado previo a zBufferFar y esquinas. Limites del banco: motor RW falso (zBufferFar, estados, '
                                       'Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin '
                                       'credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::DrawRadarSection': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff). las '
                                          'UV si se trazan; fuente corregida contra el EXE. Limites del banco: motor RW falso (zBufferFar, estados, '
                                          'Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin '
                                          'credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::DrawRadarSectionMap': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                             'Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, '
                                             'vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas '
                                             'enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::DrawRadarSprite': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                         'Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, '
                                         'peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; '
                                         'FindPlayerPed/Vehicle no se trazan.',
              'CRadar::DrawRotatingRadarSprite': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                 'diff). fuente corregida: constantes del EXE. Limites del banco: motor RW falso (zBufferFar, '
                                                 'estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados '
                                                 'con traza (sin credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se '
                                                 'trazan.',
              'CRadar::DrawYouAreHereSprite': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                              'fuente corregida: 500 ms (0x58497B) / 200 ms (0x58499E). Limites del banco: motor RW falso '
                                              '(zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD '
                                              'doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no '
                                              'se trazan.',
              'CRadar::GetActualBlipArrayIndex': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                 'diff). Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, '
                                                 'EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y '
                                                 'alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::GetNewUniqueBlipIndex': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                               'diff). Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, '
                                               'EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y '
                                               'alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::GetRadarTraceColour': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                             'Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, '
                                             'vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas '
                                             'enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::GetTextureCorners': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                           'Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, '
                                           'vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados '
                                           'a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::InitFrontEndMap': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                         'Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, '
                                         'peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; '
                                         'FindPlayerPed/Vehicle no se trazan.',
              'CRadar::Initialise': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff). fuente '
                                    'corregida: contador = 1. Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, '
                                    'EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas '
                                    'enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::IsPointInsideRadar': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                            'fuente corregida: comparacion inclusiva. Limites del banco: motor RW falso (zBufferFar, estados, Im2D), '
                                            'pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin '
                                            'credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::LimitRadarPoint': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                         'Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, '
                                         'peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; '
                                         'FindPlayerPed/Vehicle no se trazan.',
              'CRadar::LimitToMap': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). fuente '
                                    'corregida: ftol + movsx (0x583351). Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos '
                                    'de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas '
                                    'enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::LineRadarBoxCollision': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git '
                                               'diff). fuente corregida contra el EXE. Limites del banco: motor RW falso (zBufferFar, estados, '
                                               'Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza '
                                               '(sin credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::Load': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff). Limites del '
                              'banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo '
                              'y TXD doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::LoadTextures': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff). Limites '
                                      'del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y '
                                      'objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; '
                                      'FindPlayerPed/Vehicle no se trazan.',
              'CRadar::RemoveMapSection': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                          'fuente corregida: orden x,y y RemoveModel. Limites del banco: motor RW falso (zBufferFar, estados, Im2D), '
                                          'pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); '
                                          'colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::RemoveRadarSections': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                             'fuente corregida: orden x,y y RemoveModel. Limites del banco: motor RW falso (zBufferFar, estados, '
                                             'Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin '
                                             'credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::RequestMapSection': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                           'fuente corregida: orden x,y. Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos '
                                           'de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y '
                                           'alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::Save': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff). Limites del '
                              'banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo '
                              'y TXD doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::SetBlipAlwaysDisplayInZoom': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                    'diff). Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, '
                                                    'EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y '
                                                    'alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::SetBlipEntryExit': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                          'Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, '
                                          'peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; '
                                          'FindPlayerPed/Vehicle no se trazan.',
              'CRadar::SetBlipFade': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). Limites '
                                     'del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y '
                                     'objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; '
                                     'FindPlayerPed/Vehicle no se trazan.',
              'CRadar::SetBlipFriendly': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                         'Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, '
                                         'peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; '
                                         'FindPlayerPed/Vehicle no se trazan.',
              'CRadar::SetBlipSprite': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                       'Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, '
                                       'peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; '
                                       'FindPlayerPed/Vehicle no se trazan.',
              'CRadar::SetCoordBlip': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). fuente '
                                      'corregida: byte de banderas = 3 (0x5838D7). Limites del banco: motor RW falso (zBufferFar, estados, Im2D), '
                                      'pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); '
                                      'colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::SetCoordBlipAppearance': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                'diff). fuente corregida: tipo COORD, bits 6-7 del primer byte (0x583E8C). Limites del banco: motor '
                                                'RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y objetos; '
                                                'dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; '
                                                'FindPlayerPed/Vehicle no se trazan.',
              'CRadar::SetEntityBlip': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). fuente '
                                       'corregida: byte de banderas = 3 (0x583A60). Limites del banco: motor RW falso (zBufferFar, estados, Im2D), '
                                       'pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); '
                                       'colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::SetMapCentreToPlayerCoords': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                    'diff). Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, '
                                                    'EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y '
                                                    'alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::SetShortRangeCoordBlip': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                'diff). Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, '
                                                'EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y '
                                                'alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::SetupAirstripBlips': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                            'fuente corregida: contador de fotogramas y empates. Limites del banco: motor RW falso (zBufferFar, '
                                            'estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con '
                                            'traza (sin credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::SetupRadarRect': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                        'Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, '
                                        'peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; '
                                        'FindPlayerPed/Vehicle no se trazan.',
              'CRadar::ShowRadarMarker': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                         'Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, '
                                         'peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; '
                                         'FindPlayerPed/Vehicle no se trazan.',
              'CRadar::ShowRadarTrace': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                        'fuente corregida: 7 dwords, rectangulos +-1. Limites del banco: motor RW falso (zBufferFar, estados, Im2D), '
                                        'pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); '
                                        'colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::ShowRadarTraceWithHeight': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git '
                                                  'diff). Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, '
                                                  'EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); colores y '
                                                  'alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::Shutdown': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff). Limites del '
                                  'banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y objetos; '
                                  'dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no '
                                  'se trazan.',
              'CRadar::StreamRadarSections-vec': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                 'diff). fuente corregida: orden x,y y RemoveModel. Limites del banco: motor RW falso (zBufferFar, '
                                                 'estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados '
                                                 'con traza (sin credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se '
                                                 'trazan.',
              'CRadar::StreamRadarSections-xy': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                'diff). fuente corregida: orden x,y y RemoveModel. Limites del banco: motor RW falso (zBufferFar, '
                                                'estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados '
                                                'con traza (sin credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se '
                                                'trazan.',
              'CRadar::TransformRadarPointToRealWorldSpace': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed '
                                                             '(see git diff). fuente corregida: reciproco y orden x87. Limites del banco: motor RW '
                                                             'falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y '
                                                             'objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados '
                                                             'a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::TransformRadarPointToScreenSpace': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed '
                                                          '(see git diff). funcion naked con retorno por puntero oculto; fuente corregida: reciproco '
                                                          'y orden x87. Limites del banco: motor RW falso (zBufferFar, estados, Im2D), pools falsos '
                                                          'de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin '
                                                          'credito); colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan.',
              'CRadar::TransformRealWorldPointToRadarSpace': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed '
                                                             '(see git diff). fuente corregida: reciproco y orden x87. Limites del banco: motor RW '
                                                             'falso (zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y '
                                                             'objetos; dibujo y TXD doblados con traza (sin credito); colores y alturas enmascarados '
                                                             'a 8 bits; FindPlayerPed/Vehicle no se trazan.'},
 'source_changes': {'source/game_sa/Radar.cpp': 'Fuente de CRadar corregida contra el EXE antes de la corrida (correcciones por funcion en el README '
                                                'del banco y en el alcance de cada funcion; ver git diff)',
                    'source/game_sa/Radar.h': 'Fuente de CRadar corregida contra el EXE antes de la corrida (correcciones por funcion en el README '
                                              'del banco y en el alcance de cada funcion; ver git diff)'},
 'notes': 'RadarFinal3 registers 63 functions of CRadar (15040 pairs, 0 differences). Limits are declared per function; see scope and the bank '
          'README.',
 'readme_section': '## Aprobación (09/10/2026)\n'
                   '\n'
                   'Corrida `20261009-203202-095801`: passed=true, 15040 pares, 63 funciones registradas con 0 fallos, evento complete '
                   '(state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_radarfinal3.py` (evidencia en `runs/`, '
                   '`validation-summary.json`). Limites declarados en el alcance de cada funcion y en las secciones anteriores de este README.'}
BANK=CFG['bank'];RUN_ID=CFG['run_id']
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test')/('resultados-'+BANK.lower())/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
EXPECT=CFG['expect'];TOTAL=sum(EXPECT.values());NEW=CFG['new'];REJECTED=CFG['rejected'];PASSED=CFG['passed']
EXTRA=CFG.get('extra_unregistered',[])
assert TOTAL==CFG['total'] and set(NEW)|set(REJECTED)|set(EXTRA)==set(EXPECT) and not set(NEW)&set(REJECTED) and not set(EXTRA)&set(NEW)
assert summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['executed_probe_sha256'].startswith(CFG['probe_prefix']) and summary['original_exe_sha256'].startswith('72ae59e4')
log_sha=sha(SRC/'comparisons.jsonl')
if PASSED:
    assert summary['passed'] is True and summary['all_equal'] is True and summary['state_restored'] is True and summary['pairs']==TOTAL and summary['pairs_per_function']==EXPECT and log_sha==summary['log_sha256']
else:
    assert summary['passed'] is False and summary['error']==CFG['error'] and 'pairs' not in summary
events={};per={};bad={};fsum={};cases=set();complete=None;layout=None;err=None;begin=None
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event');events[ev]=events.get(ev,0)+1
        if ev=='comparison':
            f=row['function'];per[f]=per.get(f,0)+1;cases.add((f,row['case']))
            if row.get('equal') is not True or row.get('accepted') is not True:bad[f]=bad.get(f,0)+1
        elif ev=='function_summary':assert row['function'] not in fsum;fsum[row['function']]=row
        elif ev=='complete':complete=row
        elif ev=='error':err=row
        elif ev=='layout':layout=row
        elif ev=='begin':begin=row
        else:assert ev in('waiting_for_player','case_start'),ev
print('events',events)
term=complete if PASSED else err
if PASSED:
    assert err is None and complete is not None and complete['pairs']==TOTAL and complete['state_restored'] is True and complete.get('layout_ok',True) is True
else:
    assert complete is None and err is not None and set(err)<={'event','layout_ok','pairs','state_restored'} and err['pairs']==TOTAL and err['state_restored'] is True and err.get('layout_ok') is True
assert layout is not None and layout.get('ok') is True and events.get('layout')==1 and begin is not None and events.get('begin')==1 and begin.get('known') is True and isinstance(begin.get('sites'),list) and len(begin['sites'])>0
if begin is not None and isinstance(begin.get('sites'),list):assert all(x.get('ok') is True for x in begin['sites'])
assert events.get('comparison')==TOTAL and events.get('function_summary')==len(EXPECT) and len(cases)==TOTAL
assert events.get('case_start',TOTAL)==TOTAL and events.get('complete',0)+events.get('error',0)==1
assert per==EXPECT and set(fsum)==set(EXPECT)
for f,r in fsum.items():assert r['cases']==EXPECT[f] and r['failures']==bad.get(f,0),f
assert bad==CFG['bad'],bad
for f in NEW:assert f not in bad
PAIRS={f:per[f] for f in NEW}
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
ADDR={};TYPE={}
for name in NEW+REJECTED:
    cls,fn=name.split('::',1);m=[r for r in rows if r['class']==cls and r['fn_name']==fn]
    assert len(m)==1,(name,len(m));assert m[0]['validated']=='0',(name,'already validated');ADDR[name]=m[0]['address'];TYPE[name]=m[0]['type']
assert ADDR==CFG['addr'],ADDR
for name,t in CFG['types'].items():assert TYPE[name]==t,(name,TYPE[name])
CLS=sorted({n.split('::',1)[0] for n in NEW})
pend_after=[r['class']+'::'+r['fn_name'] for r in rows if r['class'] in CLS and r['validated']=='0' and (r['class']+'::'+r['fn_name']) not in NEW]
assert sorted(pend_after)==sorted(CFG['pending_after']),pend_after
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
CHANGED_SRC={};CHANGED_BANK={}
for p,h in manifest['sources'].items():
    if sha(ROOT/p)!=h:CHANGED_SRC[p]=dict(tested_sha256=h,current_sha256=sha(ROOT/p))
for n,h in manifest['instrumentation'].items():
    if sha(HERE/n)!=h:CHANGED_BANK[n]=dict(tested_sha256=h,current_sha256=sha(HERE/n))
assert set(CHANGED_SRC)<=set(CFG['allow_src_change']),CHANGED_SRC
assert set(CHANGED_BANK)<=set(CFG['allow_bank_change']),CHANGED_BANK
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
assert sha(HERE/('artifacts/gta_reversed-%s-probe.asi'%BANK.lower()))==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/('gta_reversed-%s-probe.pdb'%BANK.lower());assert sha(pdb)==manifest['binaries']['pdb']
assert not RUN.exists();RUN.mkdir(parents=True)
shutil.copy2(SRC/'summary.json',RUN/'summary.json');shutil.copy2(SRC/'RESULTADO.txt',RUN/'RESULTADO.txt')
raw=(SRC/'comparisons.jsonl').read_bytes();assert hashlib.sha256(raw).hexdigest()==log_sha
with open(RUN/'comparisons.jsonl.gz','wb') as out:
    with gzip.GzipFile(filename='',mode='wb',fileobj=out,compresslevel=9,mtime=0) as z:z.write(raw)
digest=hashlib.sha256()
with gzip.open(RUN/'comparisons.jsonl.gz','rb') as stream:
    for block in iter(lambda:stream.read(1<<20),b''):digest.update(block)
assert digest.hexdigest()==log_sha
assert sha(RUN/'summary.json')==sha(SRC/'summary.json') and sha(RUN/'RESULTADO.txt')==sha(SRC/'RESULTADO.txt')
storage=dict(uncompressed_sha256=log_sha,uncompressed_bytes=len(raw),compressed_path=rel(RUN/'comparisons.jsonl.gz'),compressed_sha256=sha(RUN/'comparisons.jsonl.gz'),compression='gzip',gzip_mtime=0,lossless_roundtrip_sha256=digest.hexdigest(),original_location=str(SRC/'comparisons.jsonl'))
COMMON=CFG['common'];SPECIFIC=CFG['specific'];SOURCE_CHANGES=CFG['source_changes']
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,registered_pairs=sum(PAIRS.values()),asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
rejected={f:dict(pairs=per[f],failures=bad[f],equal=per[f]-bad[f],reason='differences against the EXE in the run; not registered, no credit (another agent is diagnosing it)') for f in REJECTED}
reaudit=dict(comparison_events=TOTAL,complete_events=events.get('complete',0),terminal_error_events=events.get('error',0),terminal_event=term,layout_events=events.get('layout',0),begin_events=events.get('begin',0),begin_checked=True,waiting_for_player_events=events.get('waiting_for_player',0),case_start_events=events.get('case_start',0),function_summaries=len(EXPECT),functions_with_zero_failures=len(NEW),unequal_pairs=sum(bad.values()),unequal_by_function=bad,distinct_cases=len(cases),per_function=per)
out=dict(passed=PASSED,pairs=TOTAL,differences=sum(bad.values()),registered_pairs=sum(PAIRS.values()),registered_functions=len(NEW),not_registered_extra=EXTRA,functions=PAIRS,covered_functions={k:True for k in NEW},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=PASSED,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,reaudit=reaudit,source_corrections=SOURCE_CHANGES,notes=CFG['notes'])
if not PASSED:
    out.update(partial_per_function_approval=True,rejected_functions=rejected)
if CHANGED_SRC or CHANGED_BANK:
    out.update(source_changed_after_run=CHANGED_SRC,bank_changed_after_run=CHANGED_BANK)
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/('reversing/validation-registry.backup-before-%s.json'%CFG['tag'])
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
assert n==CFG['registry_before'],n
for name in NEW:
    cls,fn=name.split('::',1);hook={'class':cls,'fn_name':fn,'address':ADDR[name],'type':TYPE[name]}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=PAIRS[name],original_exe_sha256=summary['original_exe_sha256']),tested_sources=dict(manifest['sources']),
     source_provenance=CFG['provenance'],
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/%s/README.md'%BANK))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.%s.tmp'%CFG['tag']);temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before
for _try in range(30):
    try:os.replace(temp,registry);break
    except PermissionError:time.sleep(2)
else:raise SystemExit('registry locked')
# README: only append the approval section at the end (append mode, bank file otherwise untouched)
readme=HERE/'README.md';raw_r=readme.read_bytes();nl='\r\n' if b'\r\n' in raw_r else '\n'
section=CFG['readme_section'].replace('\n',nl)
assert '## Aprobación' not in raw_r.decode('utf-8')
with open(readme,'ab') as fh:fh.write((nl+section+nl).encode('utf-8'))
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),summary_sha256=sha(sp),changed_src=list(CHANGED_SRC),changed_bank=list(CHANGED_BANK)),indent=2))
