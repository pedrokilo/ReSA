$gamePath = 'D:\JUEGOS\GTA San Andreas - CVehicle Test'
Start-Process -FilePath "$gamePath\gta_sa.exe" -WorkingDirectory $gamePath -ArgumentList '"--historical-hooks=D:\JUEGOS\GTA San Andreas - CVehicle Test\historical-hooks.txt"'
