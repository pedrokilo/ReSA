<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# COFF y cobertura adicional offline

spec-extra3-exact-symbols.json contiene tres aliases de helpers y ocho símbolos exactos obtenidos del objeto antiguo: CalcBlendRatio, GetClimbOffset3D y seis consumidores Climb. spec-extra3-with-climb-lambdas.json añade siete métodos/constructores lambda definidos de ProcessPed: quince consumidores COFF en total. Es recomendable exigir también esta segunda comparación, porque equivalencia del cuerpo exterior no prueba un lambda interno cambiado. Símbolos seleccionados directamente con compare_coff.parse, incluida corrección BSS; no se inventaron nombres ni prefijos equivalentes.

Se ejecutó la comparación contra los objetos nuevos: los quince cuerpos pasaron en caller-proof-admitted.json. Se conservan hashes y todos los campos del resultado. Ningún alias fuera de los tres helpers se autoriza aquí.

Auditores nuevos preparados en CppTrigFollowPointRoute/audit/audit_run.py (1/512) y CppTrigClimb/audit/audit_run.py (6/4608). Gate heredado switches/clean, payload ret/trace completo, cobertura exacta casos y pares, restauración/hashsource/ASI/EXE/log/partidas/config. --require-registered comprueba begin registered_hook verdadero y counts/collaborators reales; --archive archiva gz determinista. No FP108/EFLAGS completo inventado.

Cobertura focal actualizada mediante inferencia de flujo, no contadores hardware: auditor Climb exige al menos un caso GetCameraStickModifier con evento process_vertical_line en la traza igual original/candidate. La única llamada fuente correspondiente ocurre tras GetClimbOffset3D en rama JUMP_B/FallAfterVault<0; ese helper ejecuta SinMul y CosMul incondicionalmente. Se archivan IDs de casos que lo demuestran. No se infiere cobertura a partir de un mero cambio geométrico.

Auditor Follow exige al menos un resultado CalcBlendRatio con bits distintos de BF800000 (-1.f). Ese es el único retorno temprano previo a FCOS en la fuente inspeccionada; otro resultado implica haber calculado cosine aunque el clamp final devuelva uno. Se archivan IDs y la naturaleza de la inferencia. No equivale a cobertura universal de todas las ramas.

UTF-8 estricto comprobado, no producción/build/GTA/Git.
