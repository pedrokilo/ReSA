# Expande los marcadores @RAIZ@ / @RAIZ_FS@ de cl.rsp con la raiz del repo en uso (el .rsp versionado no depende de ninguna ruta de otro arbol).
# Uso: powershell -File expandir_rsp.ps1 <raiz del repo> <rsp de origen> <rsp de destino>
param([string]$Raiz, [string]$Origen, [string]$Destino)
$r = $Raiz.TrimEnd('\')
$t = [IO.File]::ReadAllText($Origen).Replace('@RAIZ_FS@', $r.Replace('\', '/')).Replace('@RAIZ@', $r)
[IO.File]::WriteAllText($Destino, $t, (New-Object Text.UTF8Encoding($false)))
