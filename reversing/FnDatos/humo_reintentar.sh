#!/bin/bash
# repite prueba_marcadores.py hasta obtener veredicto (el candado de juego lo toman otros agentes entre medias)
cd /d/Original/wv-8/reversing/FnBIS1
export AGUA_ASI='D:\Original\wv-datos3\reversing\FnDatos\asi\gta_reversed.asi'
for n in 1 2 3 4 5 6 7 8; do
  python prueba_marcadores.py cal_mal.args g5b_mar$n > /d/Original/wv-datos3/reversing/FnDatos/humo_marcadores.txt 2>&1
  if grep -q VEREDICTO /d/Original/wv-datos3/reversing/FnDatos/humo_marcadores.txt; then break; fi
  sleep 20
done
