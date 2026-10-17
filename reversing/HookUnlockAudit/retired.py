"""Hooks del inventario congelado (catalog-locks-before.csv, 421 filas) que ya no existen en el arbol.

Cada baja se justifica contra source/. El banco sigue exigiendo una cuenta EXACTA (FROZEN_TOTAL - len(RETIRED) revisiones
distintas), que ninguna baja aparezca revisada ni como hook en ejecucion, y ninguna fila de mas.
"""
FROZEN_TOTAL = 421
RETIRED = {
    ('CShadows', 'ShadowRenderTriangleCB'):
        'Shadows.cpp ya no engancha 0x709CF0: usa EAX/EBX/EDI como entrada (usercall) y su unico llamador (0x70A437, en '
        'CastShadowEntityXYZ) ya esta reemplazado; la funcion queda como C++ de llamada directa (ver source/game_sa/Shadows.cpp, comentario junto a InjectHooks).',
}
EXPECTED_REVIEWS = FROZEN_TOTAL - len(RETIRED)
