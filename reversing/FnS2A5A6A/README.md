# FnS2A5A6A: lotes S2A, S5A y S6A del plan de sustitucion (21/11/2026)

Plan: docs/ExeIndependiente/plan-sustitucion.md, raices.csv, lotes-sustitucion.csv. Worktree `wv-17`, rama `s2a5a6a` sobre origin/main (f16f4d4b82). Sin push.
Regla de la sesion: reconstruir y enganchar, compilar; sin bancos en juego. Todas las filas nuevas de docs/hooks.csv quedan con validated=0.
`exedis.py` (esta carpeta) desensambla el EXE con capstone: `python exedis.py <dir hex> <bytes>`.

Verificacion: NO se emulo con Unicorn ni se ejecuto ningun banco (coste alto, 54 funciones). La unica comprobacion es la lectura del desensamblado y la compilacion.
Comparaciones de float reproducidas del salto real (ordenadas, NaN segun fnstsw+test ah); pila x87 con Veh83D* (24 bits, exponente ampliado).

## S5A (RtQuat, RtBMP, RtDict, RtAnim)
| Funcion | Direccion | Estado |
|---|---|---|
| RtAnimAnimationFreeListCreateParams | 0x7CCC80 | reconstruida, hook |
| RtAnimEngineConstructor / Destructor (plugin 0x1B7; RtAnimInitialize ya pasa las propias) | 0x7CCCD0 / 0x7CCD10 | idem |
| RtAnimGetInterpolatorInfo | 0x7CCDE0 | idem |
| RtDictSchemaCreateDict, AddDict, RemoveDict, _rtDictSchemaInitDict, RtDictAddEntry, RtDictStreamGetSize | 0x7CED90, 7CEE50, 7CEE80, 7CEF00, 7CEFB0, 7CF1F0 | idem |
| RtQuatQueryRotate, RtQuatModulus | 0x7EBA80, 0x7EBD10 | idem (x87; fpatan con TrigAtan2Cpp, 1/r sin redondear) |
| RtBMPImageWrite (+ auxiliar RtBMPImageWriteRow 0x7CEC40, tambien enganchada) | 0x7CE990 | idem |

## S6A (RtAnim)
RtAnimAnimationWrite 7CD160, StreamRead 7CD220, StreamWrite 7CD410, StreamGetSize 7CD4D0, SetKeyFrameCallBacks 7CD660, SetAnimLoopCallBack 7CD6F0, SetAnimCallBack 7CD710, InterpolatorCopy 7CD730,
SubAnimTime 7CD760 (x87), SetCurrentTime 7CDAB0, AnimationMakeDelta 7CDB00, Blend 7CDBF0, AddTogether 7CDC50, CreateSubInterpolator 7CDCB0, BlendSubInterpolator 7CDCF0, AddSubInterpolator 7CDEF0.
Todas reconstruidas y enganchadas (14 de 14). Particularidades conservadas: RtAnimAnimationWrite devuelve TRUE aunque la escritura falle; MakeDelta busca sin limite; el bucle de SubAnimTime es `while (t < 0)`.

## S2A (juego)
| Funcion | Direccion | Estado |
|---|---|---|
| CGame::D3DRestoreCallback (Init2 la instala) | 0x5BA120 | hecha |
| CQuaternion::Scale | 0x4CF9B0 | hecha (Veh83CMulStore) |
| CGarages::StopCarFromBlowingUp | 0x448890 | hecha (`!(salud > 300)`, no `<=`) |
| FindClosestPtOnLine | 0x4A4880 | hecha |
| FxInterpInfoFloat_c::GetVal(attrib,time,delta) | 0x4A85C0 | hecha (integral por trapecios; lee times[-1] y keys[-1] como el EXE) |
| CCamera::ProcessShake() | 0x51A6F0 | hecha |
| CSprite2d::SetVerticesForSniper | 0x727FD0 | hecha |
| CPed::SortPeds | 0x5E17E0 | hecha |
| CLines::RenderLineWithClipping (+ CLines::InjectHooks nuevo) | 0x6FF4F0 | hecha |
| callbacks de RpClumpGetBoundingSphere | 0x734970, 0x734ED0 | hechas; el codigo propio las usa |
| ClumpCollisionStreamRead (SEH solo del `new`) | 0x41B1D0 | hecha, incluye la rama por defecto del EXE |
| TextBufferAddInt, AddTyped (+ AppendTagged 0x5089B0), BugstarFlushReport | 0x508B80, 0x508D30, 0x509190 | hechas; 0x509190 usa una vtabla de E/S inferida (semantica del objeto sin confirmar) y llama a 0x508EB0 por direccion |
| EnumDevicesCallback / diPadSetRanges | 0x747020, 0x746D80 | ya reconstruidas; ahora diPadEnumerate las usa y se enganchan tambien con SDL3 |
| CPad::UpdatePads | 0x541DD0 | ya reconstruida y enganchada con HookInstall (InjectHooksMain); sin cambios |

### No hechas
- 0x6DD6F0 CVehicle::DoSunGlare (2158 bytes x87, camino caliente de render, sin forma de validar): pendiente.
- 0x543100 ApplyFrictionTurnForce, 0x544100 ApplySpringDampeningOld, 0x546ED0 ApplyScriptCollision: x87 largos, sin llamadores (funciones sin uso); pendientes.
- 0x7469A0 diMouseInit: ya reconstruida pero bloqueada (el hook da ACCESS DENIED); sin cambios.
- 0x4A2BB0 (jmp 4A9F40, destructor de FxPrim_c) y 0x4CCE60 (callback de raster): excluidas del plan.
