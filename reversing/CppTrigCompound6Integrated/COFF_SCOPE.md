# Objetos originales preservados antes de aplicar

Cuatro objetos copiados exclusivamente a CppOnlyMigration/artifacts/old-trig-compound6-coff; SHA256 individual y relocations directas en old-coff-evidence.json. spec-compound6-exact.json usa seis aliases exactos de símbolos antiguos a helpers nuevos y trece consumidores. Incluye todos los cuerpos con relocations hacia helpers, cierre transitivo de callers dentro de cada objeto y CreateNumberOfBirds como acompañante seleccionado. No se normaliza ningún otro destino/instrucción.

Descubrimiento de cobertura: BirdsCosMulAdd se usa en Update0x712330, no CreateNumberOfBirds0x711EF0. COFF Update relocations607SinMulAdd/654CosMulAdd, Render510SinMulAdd. Banco Birds necesita Update1024 además Render512 para acreditar CosMulAdd; CreateNumber1024 puede mantenerse como acompañante, no cobertura del helper. Misc6 ya incluye Update1024 y conserva corpus completo. Tabla fuente anterior de consumidores fue imprecisa.

Los metadatos .debug contienen referencias a helpers sin cuerpo functiontype: se conservan en objetos originales, no se cuentan como consumidores ejecutables. El parser actual BSSfix se usó tal cual. No compile/compare nuevos/GTA/Git ni edición de producción o bancos.
