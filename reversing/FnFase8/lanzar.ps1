# IND1A: lanza un exe con linea de argumentos ya entrecomillada y devuelve el codigo de salida (mata solo su propio proceso al vencer EsperaS).
param([string]$Exe, [string]$Dir, [int]$EsperaS = 120)
$ArgLine = $env:FASE8_ARGS   # los argumentos viajan por entorno (pwsh interpreta "--x" como parametro)
$p = Start-Process -FilePath $Exe -ArgumentList $ArgLine -WorkingDirectory $Dir -PassThru
if (-not $p.WaitForExit($EsperaS * 1000)) { 'TIMEOUT externo: el proceso sigue vivo (lo mato porque es mio)'; $p.Kill() }
'exit=0x{0:X8}' -f $p.ExitCode
