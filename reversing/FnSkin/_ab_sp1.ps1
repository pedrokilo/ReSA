$ErrorActionPreference='Continue'
$exe='D:\Original\wv-sp1\reversing\FnFase8\salida\gta_sa_standalone.exe'
$sa="--escenarios pie,bici481,moto463,coche411 --minutos 8"
$env:GTASA_SILENTPATCH_SELFTEST='1'
$env:GTASA_SILENTPATCH='0'
& pwsh -NoProfile -File D:\Original\wv-sp1\reversing\FnSkin\ejecutar_skin.ps1 -Modo std -Etiqueta sp1_antes -Limite 700 -Vigilancia -Exe $exe -SondaArgs $sa
$env:GTASA_SILENTPATCH='1'
& pwsh -NoProfile -File D:\Original\wv-sp1\reversing\FnSkin\ejecutar_skin.ps1 -Modo std -Etiqueta sp1_despues -Limite 700 -Vigilancia -Exe $exe -SondaArgs $sa
'ab terminado' | Set-Content D:\Original\wv-sp1\reversing\FnSkin\_ab_fin.txt
