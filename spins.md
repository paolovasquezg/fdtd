# Guía local — SPINS-B + Maxwell-B + photonic-devices

Setup probado en: **Windows + WSL2 (Ubuntu 22.04) + RTX 4080 + CUDA 12.5**.

Esta guía tiene dos partes:
- **Parte 1 — Instalación** (haces una vez)
- **Parte 2 — Cómo correr una simulación** (cada vez que quieras optimizar)

---

## Parte 1 — Instalación (una sola vez)

### 1.1 Sistema base

WSL2 con Ubuntu, driver NVIDIA para WSL instalado en Windows, y CUDA Toolkit 12.5 montado en `/usr/local/cuda`.

Verificar:
```bash
nvidia-smi      # debe mostrar la 4080
nvcc --version  # debe mostrar release 12.5
```

### 1.2 Variables de entorno CUDA (en `~/.bashrc`)

```bash
# CUDA setup
export CUDA_HOME=/usr/local/cuda
export LD_LIBRARY_PATH=/usr/lib/wsl/lib:$LD_LIBRARY_PATH
```

Verificar después de `source ~/.bashrc`:
```bash
echo $CUDA_HOME              # /usr/local/cuda
ls $CUDA_HOME/include/cuda.h # archivo existe
ls /usr/lib/wsl/lib/libcuda.so* # libcuda.so y symlinks
```

### 1.3 Paquetes de sistema (Ubuntu)

```bash
sudo apt update
sudo apt install -y \
  build-essential python3-dev pkg-config \
  libopenmpi-dev openmpi-bin \
  libgeos-dev libhdf5-dev \
  git tmux
```

### 1.4 Conda env

```bash
conda create -n photonics python=3.10 -y
conda activate photonics
pip install --upgrade pip wheel setuptools
```

### 1.5 Clonar el proyecto (con submodules)

```bash
mkdir -p ~/photonics-project && cd ~/photonics-project

# Para que git recuerde el token después de la primera vez
git config --global credential.helper store

# Clona con submodules (spins-b y maxwell-b vienen como submodules
# del fork del grupo TISparta — NO los repos públicos de Stanford)
git clone --recurse-submodules https://github.com/TISparta/photonic-devices.git

# Si por alguna razón los submodules salen vacíos:
# cd photonic-devices && git submodule update --init --recursive
```

> **Auth**: usuario `FabricioChavez` + Personal Access Token (con scope `repo`).
> Token en https://github.com/settings/tokens.
> Para pegar token en el prompt de password: click derecho en la terminal WSL.

### 1.6 Dependencias Python del proyecto

Dentro del env `photonics`:

```bash
cat > /tmp/requirements-local.txt << 'EOF'
appdirs
attrs
certifi
charset-normalizer
cma
contours[shapely]
cycler
dill
flatdict
fonttools
future
gdspy
h5py
idna
Jinja2
jsonschema
kiwisolver
Mako
MarkupSafe
matplotlib
mpi4py
mypy-extensions
numpy<2
packaging
pandas
Pillow
platformdirs
pyparsing
pyrsistent
pyswarms
python-dateutil
pytools
pytz
PyYAML
requests
schematics
scipy
seaborn
setuptools-scm
Shapely
six
tomli
tqdm
typing-extensions
typing-inspect
urllib3
nlopt
progressbar2
EOF

pip install -r /tmp/requirements-local.txt
```

> `numpy<2` es **crítico** — el código usa APIs viejas que no existen en NumPy 2.x.

### 1.7 pycuda

```bash
pip install pycuda
```

Esto compila desde fuente contra CUDA 12.5. Tarda 1-3 min.

### 1.8 spins-b en modo editable

```bash
cd ~/photonics-project/photonic-devices/spins-b
pip install -e .
```

### 1.9 Smoke test (verificación final)

```bash
cd ~/photonics-project/photonic-devices

python -c "
import sys, os
sys.path.append(os.path.join(os.getcwd(), 'spins-b'))
from spins import goos
import pycuda.driver as drv
from device.bend.Bend import Bend
from algorithm.optimize import maximize_objective
from util.transform import Projection, DensityFilter
drv.init()
print('GPUs:', drv.Device.count(), '-', drv.Device(0).name())
print('Todo OK')
"
```

Si imprime "Todo OK" + "GPUs: 1 - NVIDIA GeForce RTX 4080", la instalación está completa.

---

## Parte 2 — Cómo correr una simulación

Cada vez que quieras optimizar un dispositivo, sigues este flujo. Necesitas **3 terminales WSL** (o 3 paneles tmux).

### 2.1 Configurar el script

Edita `optimize-bend.py` (o el dispositivo que toque: `wdm`, `crossing`, `splitter`):

```python
ALGORITHM = 'L-BFGS-B'   # opciones: L-BFGS-B | GA | PSO | CMA-ES | MMA
STAGE     = 'cont'       # empezar SIEMPRE por 'cont'
SIM_3D    = True         # True para 3D (usa GPU/Maxwell), False para 2D (solo CPU)
SEED      = 256
```

Las constantes están como índices en listas, así que en el archivo realmente se ven así:
```python
ALGORITHM = ALGORITHM_LIST[3]   # L-BFGS-B
STAGE     = STAGE_LIST[0]        # cont
SIM_3D    = True
SEED      = int(SEED_LIST[1])    # 256
```

### 2.2 Borrar carpeta vieja (si ya corriste antes)

El script SE NIEGA a re-correr si la carpeta de output existe:

```bash
cd ~/photonics-project/photonic-devices
rm -rfv output/bend/L-BFGS-B/cont/256/   # ajusta device/algoritmo/stage/seed
```

### 2.3 Levantar Maxwell-B (Terminales 1 y 2)

> Solo necesario si `SIM_3D = True`. Si es 2D, salta a 2.4.

#### Terminal 1 — simserver (GPU)

```bash
conda activate photonics
cd ~/photonics-project/photonic-devices/maxwell-b

export MAXWELL_SERVER_FILES=/tmp/maxwell-server-files
mkdir -p $MAXWELL_SERVER_FILES
export OMP_NUM_THREADS=2

python maxwell-server/simserver.py 1
```

Esperar a que aparezca:
```
[INFO][simserver][main] Number of GPUs detected on system: 1
[INFO][simserver][main] Ready to accept simulations.
```

**No cerrar esta terminal mientras corras simulaciones.**

#### Terminal 2 — webserver

```bash
conda activate photonics
cd ~/photonics-project/photonic-devices/maxwell-b

export MAXWELL_SERVER_FILES=/tmp/maxwell-server-files

python maxwell-server/webserver.py 9041
```

Esperar:
```
Serving at ('0.0.0.0', 9041)
```

> El puerto `9041` es el default que usa spins-b internamente. No hace falta cambiar nada en el cliente.

### 2.4 Lanzar el optimizador (Terminal 3)

```bash
conda activate photonics
cd ~/photonics-project/photonic-devices

export OMP_NUM_THREADS=1
python optimize-bend.py
```

Output esperado:
```
Initial:  [0.0457... 0.5861... ...]
[INFO][optplan][run] Running action 0 (goos.action.optimizer.custom.0).
[INFO][optimize][func] Function evaluated 1: -0.0005
[INFO][optimize][grad] Gradient norm evaluated 1: 0.0053
[INFO][optimize][func] Function evaluated 2: -0.0005
...
```

Cada solve 3D tarda ~30-60s en una RTX 4080. Stage 'cont' completo: ~30 min - 2 h.

### 2.5 Correr los 3 stages en orden

Cuando 'cont' termine, edita `optimize-bend.py`:

```python
STAGE = STAGE_LIST[1]   # 'disc' (discretization)
```

Y vuelve a correr en T3:
```bash
python optimize-bend.py
```

Después:
```python
STAGE = STAGE_LIST[2]   # 'fab' (fabrication)
```
```bash
python optimize-bend.py
```

> Maxwell-b sigue vivo en T1 y T2 todo este tiempo. No tocar.

### 2.6 Postprocess

Cuando los 3 stages terminen:

```bash
python postprocess-bend.py
```

Genera plots y GDS en `output/` y `plots/`.

### 2.7 Apagar Maxwell

`Ctrl+C` en Terminal 1 y Terminal 2.

---

## Otros dispositivos

Mismo procedimiento, cambiando el nombre:

| Dispositivo | Optimize              | Postprocess              |
|-------------|-----------------------|--------------------------|
| bend        | `optimize-bend.py`    | `postprocess-bend.py`    |
| wdm         | `optimize-wdm.py`     | `postprocess-wdm.py`     |
| crossing    | `optimize-crossing.py`| `postprocess-crossing.py`|
| splitter    | `optimize-splitter.py`| `postprocess-splitter.py`|

Carpeta de output: `output/<device>/<ALGORITHM>/<STAGE>/<SEED>/`

---

## Comandos útiles durante la corrida

```bash
# Ver GPU trabajando (cualquier terminal aparte)
watch -dc -n 2 nvidia-smi

# Tail del log del optimizador
tail -f ~/photonics-project/photonic-devices/output/bend/L-BFGS-B/cont/256/spins.log

# Verificar que el webserver responde
curl http://localhost:9041/   # devuelve "N jobs pending (maxwell-server)"

# Matar algo en el puerto 9041 si quedó colgado
lsof -ti:9041 | xargs kill -9
```

---

## tmux (recomendado para no manejar 3 ventanas)

```bash
tmux new -s photonics
```

Dentro de tmux:
- `Ctrl+B` `"` → split horizontal
- `Ctrl+B` `%` → split vertical
- `Ctrl+B` `flechas` → moverte entre paneles
- `Ctrl+B` `D` → detach (procesos siguen vivos)
- `tmux attach -t photonics` → reconectar

Layout sugerido: split vertical (mitad izq / der), luego en mitad izq split horizontal → 3 paneles.

---

## Problemas comunes

**`Optimization already made`**
→ La carpeta `output/<device>/<algo>/<stage>/<seed>/` existe. Borrar con `rm -rf` y reintentar.

**`ConnectionRefused` en localhost:9041**
→ Maxwell-b no está corriendo. Verifica T1 y T2.

**Token de GitHub no funciona al hacer git pull**
→ Token expirado. Generar nuevo en https://github.com/settings/tokens, scope `repo`. Pegar con click derecho.

**`numpy.dtype size changed`**
→ Tienes numpy 2.x instalado. Forzar: `pip install "numpy<2" --force-reinstall`.

**El simserver dice "GPUs detected: 0"**
→ pycuda no ve la GPU desde dentro del env. Verifica con:
```bash
python -c "import pycuda.driver as drv; drv.init(); print(drv.Device.count())"
```
Si dice 0, revisa `LD_LIBRARY_PATH` y `CUDA_HOME` en el env donde lanzaste simserver.

**`Address already in use` en puerto 9041**
→ Mata el proceso colgado: `lsof -ti:9041 | xargs kill -9`

**ComplexWarning en scipy**
→ Inofensivo, ignorar.

**404 File not found en logs del webserver**
→ Polling normal del cliente preguntando si su simulación terminó. No es error.

---

## Estructura del proyecto

```
~/photonics-project/
└── photonic-devices/             ← repo principal (privado, TISparta)
    ├── .gitmodules
    ├── spins-b/                  ← submodule (commit b271f94)
    ├── maxwell-b/                ← submodule (commit bd9e8db)
    ├── device/                   ← definición de cada dispositivo
    │   ├── bend/
    │   ├── crossing/
    │   ├── splitter/
    │   └── wdm/
    ├── algorithm/                ← wrappers de algoritmos de optimización
    ├── util/                     ← transformaciones (filtros, projection)
    ├── optimize-{device}.py      ← scripts de optimización
    ├── postprocess-{device}.py   ← scripts de post-procesamiento
    ├── output/                   ← resultados (creado al correr)
    └── plots/                    ← plots finales
```

---

## Recordatorios rápidos

1. Activar env SIEMPRE: `conda activate photonics`
2. Las 3 terminales necesitan el env activado
3. En Maxwell-b: `OMP_NUM_THREADS=2`. En el optimizador: `OMP_NUM_THREADS=1`
4. Stage 'cont' debe terminar antes que 'disc'. 'disc' antes que 'fab'.
5. Borrar carpeta de output antes de re-correr el mismo stage.
6. Maxwell-b queda vivo durante los 3 stages — no lo cierres entre ellos.
