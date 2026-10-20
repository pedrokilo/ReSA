"""Oraculo del banco AutoPreRender60: CAutomobile::PreRender (0x6AAB50).

El oraculo de la FUNCION es el propio EXE (hook RH apagado) ejecutado en el juego: el banco exige original == candidato fila a fila (memoria completa, traza de colaboradores, ABI, x87).
Este modulo es el oraculo de los FIXTURES y de la cobertura, independiente del C++ del banco:
  * regenera en Python (xorshift64, mismas constantes que Fixtures.hpp) los parametros de cada caso y exige que la fila `input` registrada sea exactamente esa;
  * calcula la cobertura (modelos, valores extremos, familia de frontera, llamadas a cada colaborador, bytes de traza) a partir de las filas `input` y de `observed.calls`.
No repite ninguna constante de arena (viven solo en Core.hpp); los tamanos de la memoria comparada se contrastan con `mem_size` (igual en ambos lados y constante en todas las filas).
"""
import json
import sys

M32 = 0xFFFFFFFF
M64 = 0xFFFFFFFFFFFFFFFF
CASES = 8192
NAMES = ['CAutomobile::PreRender']
MODELS = [400, 407, 416, 420, 424, 427, 432, 434, 438, 441, 443, 471, 477, 486, 490, 495, 524, 525, 530, 531, 532, 539, 544, 568, 596, 597, 598, 599, 601, 503, -2]
CALL_NAMES = ['', 'BasePreRender', 'DoHover', 'DoBurst', 'TowHitch', 'TowBar', 'ColModel', 'AddSingleWheel', 'CamNoRain', 'PlayerNoRain', 'WaterSplash', 'Exhaust',
              'DamagedParticles', 'Corona', 'AddLight', 'Appearance', 'VehicleLights', 'FindPlayerVehicle', 'LookingForward', 'StoreShadow', 'SetComponentRotation',
              'ProcessWheelRotation', 'UpdateWheelMatrix', 'SwingingDoor', 'ProcessPanel', 'DoorProcess', 'GetWheelPosn', 'GetFrameFromId', 'SetTransmissionRotation', 'BadVirtual']


class Rng:
    def __init__(self, seed):
        self.s = (seed * 0x9E3779B97F4A7C15 + 0x1234567) & M64
        for _ in range(6):
            self.next()

    def next(self):
        s = self.s
        s ^= (s << 13) & M64
        s ^= s >> 7
        s ^= (s << 17) & M64
        self.s = s
        return ((s >> 11) ^ (s >> 32)) & M32


def make_case(n):
    r = Rng(0xC0DE * (n + 1))
    lo = r.next()
    hi = r.next()
    seed = lo ^ (hi << 32)
    wild = int(n % 5 >= 3)
    model = MODELS[n % len(MODELS)]
    eax = r.next()
    edx = r.next()
    stack = r.next()
    lo = r.next()
    hi = r.next()
    fake = lo | (hi << 32)
    if fake == 0:
        fake = 1
    return {'n': n, 'seed': seed, 'wild': wild, 'model': model, 'eax': eax, 'edx': edx, 'stack': stack, 'fake_seed': fake, 'boundary': int(n % 11 == 0)}


# Cobertura minima (calibrada con la corrida en juego; ver README.md). Claves: modelo:<m>, wild, boundary, call:<Nombre>
COVERAGE_MIN = {'wild': 3000, 'boundary': 700, 'plain': 3000}
for m in MODELS:
    COVERAGE_MIN['model:%d' % m] = 240
# Casos con al menos una llamada a cada colaborador (corrida en juego del 19/10/2026: valores entre parentesis en README.md); minimos = ~50 % con margen
CALL_MIN = {'call:AddLight': 840, 'call:AddSingleWheel': 3710, 'call:Appearance': 3250, 'call:BasePreRender': 4090, 'call:CamNoRain': 4090, 'call:ColModel': 4090, 'call:Corona': 770, 'call:DamagedParticles': 4090, 'call:DoBurst': 2340, 'call:DoHover': 1040, 'call:DoorProcess': 2540, 'call:Exhaust': 700, 'call:FindPlayerVehicle': 4090, 'call:GetFrameFromId': 110, 'call:GetWheelPosn': 1270, 'call:LookingForward': 2050, 'call:PlayerNoRain': 3100, 'call:ProcessPanel': 3900, 'call:ProcessWheelRotation': 4010, 'call:SetComponentRotation': 1700, 'call:SetTransmissionRotation': 1170, 'call:StoreShadow': 3030, 'call:SwingingDoor': 3960, 'call:TowBar': 51, 'call:TowHitch': 65, 'call:UpdateWheelMatrix': 4090, 'call:VehicleLights': 1990, 'call:WaterSplash': 740}


def expected_input(case):
    return make_case(case)


def tally(input_row, observed, cov):
    cov['wild'] = cov.get('wild', 0) + input_row['wild']
    cov['plain'] = cov.get('plain', 0) + (1 - input_row['wild'])
    cov['boundary'] = cov.get('boundary', 0) + input_row['boundary']
    k = 'model:%d' % input_row['model']
    cov[k] = cov.get(k, 0) + 1
    for i, n in enumerate(observed['calls']):
        if n:
            k = 'call:' + CALL_NAMES[i]
            cov[k] = cov.get(k, 0) + 1
            k = 'calls_total:' + CALL_NAMES[i]
            cov[k] = cov.get(k, 0) + n


def selftest(path):
    cov = {}
    count = 0
    for line in open(path, encoding='utf-8'):
        r = json.loads(line)
        assert r['input'] == expected_input(r['case']), (r['case'], r['input'], expected_input(r['case']))
        cov['wild'] = cov.get('wild', 0) + r['input']['wild']
        cov['plain'] = cov.get('plain', 0) + 1 - r['input']['wild']
        cov['boundary'] = cov.get('boundary', 0) + r['input']['boundary']
        k = 'model:%d' % r['input']['model']
        cov[k] = cov.get(k, 0) + 1
        count += 1
    assert count == CASES, count
    short = {k: (cov.get(k, 0), m) for k, m in COVERAGE_MIN.items() if cov.get(k, 0) < m}
    print(dict(sorted(cov.items())))
    if short:
        print('COBERTURA INSUFICIENTE', short)
        sys.exit(1)
    print('selftest ok')


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--selftest':
        selftest(sys.argv[2])
        sys.exit(0)
    assert make_case(0)['n'] == 0 and make_case(11)['boundary'] == 1 and make_case(3)['wild'] == 1
    print('ok')
