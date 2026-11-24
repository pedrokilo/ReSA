cd /d/Original/wv-nubes/reversing/FnComparar
A="--fase cielo --puntos 2 --horas 12,18 --climas 0,4,7 --pitchs 0.7,-0.05 --reps 2 --gap 1.0"
for e in o_G1:orig s_G1:std o_G2:orig s_G2:std; do pwsh -NoProfile -File ejecutar_comparar.ps1 -Modo ${e#*:} -Etiqueta ${e%:*} -Limite 1500 -Sonda comparar.py -SondaArgs "$A" 2>&1 | tail -1; done
echo CADENA_FIN
