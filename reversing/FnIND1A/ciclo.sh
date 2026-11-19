#!/bin/bash
# IND1A: un ciclo completo: reenlaza (compile_lock) + ejecuta (gta_lock). Uso: ciclo.sh <etiqueta> <limite_s> [args...]
AQUI="$(cd "$(dirname "$0")" && pwd)"
bash "$AQUI/construir.sh" | tail -1
bash "$AQUI/ejecutar.sh" "$@"
echo CICLO_TERMINADO
