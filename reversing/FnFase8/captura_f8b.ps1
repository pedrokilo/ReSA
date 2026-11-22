# Captura de pantalla completa a los N segundos de ver la ventana del juego (prueba visual de que dibuja). Uso: captura_f8b.ps1 -Salida x.png -Espera 15
param([string]$Salida, [int]$Espera = 15)
Add-Type -AssemblyName System.Windows.Forms, System.Drawing
$t0 = Get-Date
while (-not (Get-Process gta_sa_standalone -ErrorAction SilentlyContinue)) { if (((Get-Date) - $t0).TotalSeconds -gt 120) { return }; Start-Sleep 1 }
Start-Sleep $Espera
$b = [System.Windows.Forms.SystemInformation]::VirtualScreen
$bmp = New-Object System.Drawing.Bitmap $b.Width, $b.Height
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.CopyFromScreen($b.Left, $b.Top, 0, 0, $bmp.Size)
$bmp.Save($Salida, [System.Drawing.Imaging.ImageFormat]::Png)
$g.Dispose(); $bmp.Dispose()
