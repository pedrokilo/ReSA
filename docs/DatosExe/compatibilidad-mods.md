# Compatibilidad con mods que parchean datos por dirección

**Decisión (21/11/2026):** las copias propias de los datos del EXE (por ejemplo `DatosExe::Rdata::k...` del lote DE01) se dejan así. Los mods y CLEO que escriben en la dirección original del EXE no verán esas copias. Este documento deja el plan para resolverlo más adelante si hace falta.

## Problema
Un mod que cambia una constante escribe en su dirección del EXE original (por ejemplo `0x863984`). Con la DLL sobre el EXE, antes el código leía esa misma dirección y el cambio surtía efecto. Ahora lee una copia propia en otro sitio y el mod no hace nada. En el EXE independiente no existe el `.rdata` original en esas direcciones, así que estos mods tampoco funcionarían por dirección fija.

## Plan (solo si se necesita)
1. **Inventario.** Marcar qué constantes y variables son parcheadas por mods conocidos (CLEO, scripts `.cs`, ASI de terceros). Fuente: lista de direcciones de `docs/DatosExe/objetos.csv` cruzada con las que usan los mods populares.
2. **Opción A, tabla de redirección en la fase DLL.** Mientras la DLL va sobre el EXE, un modo de compatibilidad (opción de `gta-reversed.ini`, apagado por defecto) vuelve a leer las constantes de la dirección original en vez de la copia. Costo: dependencia del EXE solo en ese modo.
3. **Opción B, sincronización.** En cada fotograma o al leer, copiar el valor de la dirección original a la copia propia solo para las constantes marcadas en el paso 1.
4. **Opción C, API propia para el EXE independiente.** Exponer las variables propias con nombre, para que los mods nuevos las modifiquen por esa API y no por dirección. Los mods antiguos por dirección no funcionarían, y habría que documentarlo.
5. **Prueba.** Un mod de ejemplo que cambie una constante conocida y verifique el efecto en cada opción.

Prioridad: baja hasta que se confirme que se usan mods.
