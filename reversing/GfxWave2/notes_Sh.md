<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7496 con evidencia histórica, 0 con vigencia acreditada y 950 pendientes en 13 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# notes_Sh (CShadows) - GfxWave2

## Correcciones de fuente (source/game_sa/Shadows.cpp), evidencia asm
- RemoveOilInArea 0x7074F0: comparaciones ESTRICTAS (fcomp + test ah,41h/5) y NaN fuera; antes CRect::IsPointInside inclusivo.
- AffectColourWithLighting 0x707850: nibbles * 0x3D088889 (fmul [858F10]) en vez de /30; minimo `(A<B)?A:B` (fcom+test ah,5+jnp).
- StoreShadowForPedObject 0x707B40: `!(d<225)` (jp 707BA7, NaN sale); fuerza int16 completa (intensidad 16 bits, rgb 8 bits, antes uint8); z de la posicion sin sumar 0.0f.
- StoreRealTimeShadow 0x707CA0: (a) `!(d<225)` (antes `>`); (b) intensidad movsx byte * 0.01f (fmul [858C58]) * fuerza int16 (antes /100); cc entero completo; (c) FALTABA GetShadowRwTexture + DrawBorderAroundTexture(RGBA 0,0,0,255) (call 705A00); (d) elevacion = -deg(atan2(-y,-z)) (antes atan2(x,-z)), fpatan x87 con __asm; (e) faltaba CVector::Normalise (59C910) del vector "at" del frame; (f) argumentos de CalcPedShadowValues en orden (front, side, displacement) (estaban desplazados); z sin sumar -0.
- RenderExtraPlayerShadows 0x707FA0: lee m_nLightShadowStrength (0xB7C4EA, antes 0xB7C4E8); `!(dist<radio)`; direccion = L * (1/dist) (CVector::operator*= 0x40FEF0, antes division); tamano = max-min y mitades *0.5; posicion = (pos - dir*1.2) - centerOffset*forward (antes SUMABA el termino forward); up.z>0 (antes >=0); 5*fuerza/8 con signo (antes /8u).
- RenderStaticShadows 0x708300: bucle interno desde la sombra actual hasta el final (antes desde 0); alpha con intensidad int16 y (1-fog*0.5)*int; UV * 0.005f (0x858B4C, antes /200); bRendered una vez tras el recorrido de poligonos.
- CastPlayerShadowSectorList 0x70A470: up.z `!(>0.97)` (NaN sale); solape con comparaciones estrictas sobre los campos de memoria del CRect (antes OverlapsWith inclusivo); colision de model info (0xA9B0C8[idx]+0x14), no CEntity::GetColModel; comparaciones Z negadas (NaN sale).
- RenderStoredShadows 0x70A960: sin return anticipado (el EXE fija render states y buffer aunque no haya sombras); rectangulo SIMETRICO pos -/+ (|side|+|front|) (antes pos..pos+ext); indices de sector con floor(v*0.02+60) y recortes del EXE; bucles y exterior / x interior; agua: limites floor/ceil((pos -/+ ext)*0.5), X exterior / Y interior, `!(agua<z)`, alpha int16*0.6. NO verificado: aritmetica de UV de agua (u,v: el EXE multiplica por 1/magSq precalculado y suma (-z)*0.0), ver abajo.
- StoreCarLightShadow 0x70C500: vector = posicion - camara (antes camara - posicion); `!(d2<729)`; dot > -maxViewAngle (antes dot_inv > max); fundido (dist-18)*0.11111111 (antes invLerp con division); velocidad `>0.4` (antes >=) y orden de llamadas (velocidad primero, luego FindPlayerVehicle, otra vez en la rama de StoreShadowToBeRendered).
- StoreShadowForPole 0x70C750: intensidad = int32 de ((up.z-0.5)*fuerza)*2 y luego 2*int/3 (antes int16 intermedio); posicion = front + ((offsetY*fwd + offsetX*right) + pos) (antes (pos+front)+offset); id = entity + localId + 0x33 (lea, el original NO lee m_pLod).
- RenderIndicatorShadow 0x70CCB0: tamano `(frontX > -sideX) ? frontX : -sideX` (std::max devolvia frontX con NaN/igualdad).
- MATCH sin cambios: Init 0x706CD0, Shutdown 0x706ED0, TidyUpShadows 0x707770, StoreShadowToBeRendered-Texture 0x707390 y -Type 0x707930 (tabla 0x707B20), SetRenderModeForShadowType 0x707460 (tabla 0x7074CC), GunShotSetsOilOnFire 0x707550, PrintDebugPoly 0x7076B0, CalcPedShadowValues 0x7076C0, UpdateStaticShadows 0x707F40.
- CalculateShadowStrength (NOTSA) ahora recibe int32 con signo (movsx word) y trunca como _ftol2.

## Que se compara
19 funciones en `fns` (original vs candidato, objetos falsos). Arrays de CShadows, buffer de RenderBuffer, tablas CTimeCycle, camara, luces, timers: Globals().

## Fuera del banco
- RenderStoredShadows 0x70A960: recorre sectores reales del mundo (CWorld::ms_aSectors 0xB7D0B8), llama a CastShadowSectorList/CastRealTimeShadowSectorList/CastPlayerShadowSectorList y a CWaterLevel (0x6E8580/0x6E8550); necesita listas de nodos de pool reales (regla 17).
- CastPlayerShadowSectorList<CPtrListSingleLink<CPhysical*>> 0x70A470: las listas de la DLL exigen nodos vivos del pool real y entidades vivas del pool de entidades (BRIEF), y llama a CastShadowEntityXY (0x7086B0, hooks de otra clase).

## Supuestos a revisar al compilar
- `Stub.dll` de CEntity::GetColModel, CPed::GetBonePosition, CCamera::IsSphereVisible(2 args), Fx_c::GetFxQuality, CRealTimeShadow::*: se parchean DLL+cuerpo+EXE (si alguno esta inlined en la DLL, el banco no los interceptaria).
- GfxCommon.hpp: en la copia leida, `Push` termina en la linea `if (!Mine() ...) return;` (faltan el cuerpo `auto& r = recs[nrec++]...}` y `Recs()`); ShadowsPart.hpp usa Push/Recs de GeoFinal1.
- NOTSA no tocados (el banco fija g_fx quality=0 con stub): drawOnBuildings por calidad FX en StoreShadowForPedObject/StoreRealTimeShadow/StoreCarLightShadow, comprobacion dist==0 en RenderExtraPlayerShadows. Aparte: NumLights es uint32 en la DLL y int con signo en el EXE (0x708008 jle); fixtures 0..6.
- 0xC4B6B0 (MAX_DISTANCE_PED_SHADOWS_SQR) es una variable en .bss del EXE; la DLL usa constexpr 225.
- Incluye `Fx/Fx.h`, `3dMarkers.h`, `StencilShadows.h`, `TxdStore.h`: ajustar si faltan.
