# COFF y cobertura adicional offline

spec-extra3-exact-symbols.json contiene tres aliases de helpers y ocho símbolos exactos obtenidos del objeto antiguo: CalcBlendRatio, GetClimbOffset3D y seis consumidores Climb. spec-extra3-with-climb-lambdas.json añade siete métodos/constructores lambda definidos de ProcessPed: quince consumidores COFF en total. Es recomendable exigir también esta segunda comparación, porque equivalencia del cuerpo exterior no prueba un lambda interno cambiado. Símbolos seleccionados directamente con compare_coff.parse, incluida corrección BSS; no se inventaron nombres ni prefijos equivalentes.

Se ejecutó la comparación contra los objetos nuevos: los quince cuerpos pasaron en caller-proof-admitted.json. Se conservan hashes y todos los campos del resultado. Ningún alias fuera de los tres helpers se autoriza aquí.

Auditores nuevos preparados en CppTrigFollowPointRoute/audit/audit_run.py (1/512) y CppTrigClimb/audit/audit_run.py (6/4608). Gate heredado switches/clean, payload ret/trace completo, cobertura exacta casos y pares, restauración/hashsource/ASI/EXE/log/partidas/config. --require-registered comprueba begin registered_hook verdadero y counts/collaborators reales; --archive archiva gz determinista. No FP108/EFLAGS completo inventado.

Cobertura focal actualizada mediante inferencia de flujo, no contadores hardware: auditor Climb exige al menos un caso GetCameraStickModifier con evento process_vertical_line en la traza igual original/candidate. La única llamada fuente correspondiente ocurre tras GetClimbOffset3D en rama JUMP_B/FallAfterVault<0; ese helper ejecuta SinMul y CosMul incondicionalmente. Se archivan IDs de casos que lo demuestran. No se infiere cobertura a partir de un mero cambio geométrico.

Auditor Follow exige al menos un resultado CalcBlendRatio con bits distintos de BF800000 (-1.f). Ese es el único retorno temprano previo a FCOS en la fuente inspeccionada; otro resultado implica haber calculado cosine aunque el clamp final devuelva uno. Se archivan IDs y la naturaleza de la inferencia. No equivale a cobertura universal de todas las ramas.

UTF-8 estricto comprobado, no producción/build/GTA/Git.
