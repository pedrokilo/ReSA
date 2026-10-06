"""Compila la sonda temporal del banco de CPed grupo A (armas e inventario del peaton, 31 funciones):
AddWeaponModel, ClearWeapon(s), DoWeHaveWeaponAvailable, GiveDelayedWeapon, GiveWeapon, GiveWeaponAtStartOfFight,
GiveWeaponWhenJoiningGang, GrantAmmo, RemoveWeaponAnims, Remove/ReplaceWeapon*, RequestDelayedWeapon, SetAmmo,
SetCurrentWeapon, Get/SetWeaponSkill, GetWeaponSlot, gun flash y entidad sujeta por el peaton."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = [f'source/game_sa/Entity/Ped/Ped.{e}' for e in ('cpp', 'h')]
SOURCES = [s for s in SOURCES if (Path(__file__).resolve().parents[2]/s).exists()]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'cpeda', 'CPedALive', SOURCES)))
