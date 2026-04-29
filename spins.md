# Photonic Device Optimization on Khipu

Shape optimization for photonic devices using Maxwell FDTD simulations on the Khipu HPC cluster.

**Supported devices:** `bend` · `wdm` · `crossing` · `splitter`

---

## Requirements

- Python **3.10.2** (`/opt/ohpc/pub/libs/gnu12/python3/3.10.2/bin/python3`)
- CUDA 11.4 (GPU jobs)
- Modules: `gnu12/12.4.0`, `mpich/3.4.3-ofi`, `python3/3.10.2`

---

## Setup

### 1. Clone the repository

```bash
git clone --recurse-submodules <repo-url>
cd photonic-devices
```

### 2. Install dependencies

```bash
pip install -r requirements.txt
pip install -r maxwell-b/requirements.txt
```

**`requirements.txt`** (photonics):

```
appdirs==1.4.4
attrs==21.2.0
certifi==2021.5.30
charset-normalizer==2.0.6
cma==3.1.0
contours==0.0.2
cycler==0.10.0
dill==0.3.4
flatdict==4.0.1
fonttools==4.27.1
future==0.18.2
gdspy==1.6.9
h5py==3.6.0
idna==3.2
Jinja2==3.1.1
jsonschema==4.0.0a6
kiwisolver==1.3.2
Mako==1.2.0
MarkupSafe==2.1.1
matplotlib==3.5.0b1
mpi4py>=3.1.4
mypy-extensions==0.4.3
numpy==1.22.3
packaging==21.0
pandas==1.3.3
Pillow==8.3.2
platformdirs==2.5.1
pyparsing==3.0.0rc1
pyrsistent==0.18.0
pyswarms==1.3.0
python-dateutil==2.8.2
pytools==2022.1.3
pytz==2021.1
PyYAML>=6.0
requests==2.26.0
schematics==2.1.1
scipy==1.7.3
seaborn==0.11.2
setuptools-scm==6.3.2
Shapely==1.8a3
six==1.16.0
tomli==1.2.1
tqdm==4.62.3
typing-extensions==4.2.0
typing-inspect==0.7.1
urllib3==1.26.7
```

**`maxwell-b/requirements.txt`** (Maxwell):

```
appdirs==1.4.4
h5py==3.6.0
Jinja2==3.1.1
Mako==1.2.0
MarkupSafe==2.1.1
mpi4py>=3.1.4
numpy==1.22.3
platformdirs==2.5.1
pytools==2022.1.3
scipy==1.8.0
typing-extensions==4.2.0
```

### 3. Install additional packages

```bash
/opt/ohpc/pub/libs/gnu12/python3/3.10.2/bin/python3 -m pip install --user pycuda
/opt/ohpc/pub/libs/gnu12/python3/3.10.2/bin/python3 -m pip install --user progressbar2
/opt/ohpc/pub/libs/gnu12/python3/3.10.2/bin/python3 -m pip install --user nlopt
/opt/ohpc/pub/libs/gnu12/python3/3.10.2/bin/python3 -m pip install --user "numpy<2"
```

### 4. Fix port mismatch

Edit `maxwell-b/launch_webserver_khipu.sh` — change the webserver port from `9041` to `6000`:

```bash
python maxwell-server/webserver.py 6000 &> webserver.log
```

---

## Configuration

Open the optimize script for your device (e.g. `optimize-bend.py`) and set:

```python
ALGORITHM = 'L-BFGS-B'   # L-BFGS-B | GA | PSO | CMA-ES | MMA
STAGE     = 'cont'        # cont | disc | fab
SIM_3D    = True
SEED      = 256
```

---

## Running

### Step 1 — Start Maxwell

Submit both Maxwell jobs and confirm they are running before launching the optimizer.

```bash
sbatch maxwell-b/launch_simserver_khipu.sh
sbatch maxwell-b/launch_webserver_khipu.sh

# Wait until both show as RUNNING
squeue -u $USER

# Check for errors
tail maxwell-b/simserver.log
tail maxwell-b/webserver.log
```

> **Maxwell must stay running throughout all 3 stages.**

### Step 2 — Run the 3 stages in order

Each stage must complete before starting the next. Edit `STAGE` in the optimize script between submissions.

```bash
# Stage 1 — continuous
# optimize-bend.py: STAGE = 'cont'
sbatch launch_bend_khipu.sh && squeue -u $USER

# Stage 2 — discrete  (after Stage 1 finishes)
# optimize-bend.py: STAGE = 'disc'
sbatch launch_bend_khipu.sh && squeue -u $USER

# Stage 3 — fabrication  (after Stage 2 finishes)
# optimize-bend.py: STAGE = 'fab'
sbatch launch_bend_khipu.sh && squeue -u $USER
```

### Step 3 — Post-process

```bash
python postprocess-bend.py
```

---

## Output

Results are saved to:

```
output/<device>/<ALGORITHM>/<STAGE>/<SEED>/
```

> If an output folder already exists the script will skip that stage. Delete the folder to rerun it.

---

## Other Devices

Replace `bend` with `wdm`, `crossing`, or `splitter` throughout:

| Device | Optimize script | Launch script | Post-process |
|--------|----------------|---------------|-------------|
| Bend | `optimize-bend.py` | `launch_bend_khipu.sh` | `postprocess-bend.py` |
| WDM | `optimize-wdm.py` | `launch_wdm_khipu.sh` | `postprocess-wdm.py` |
| Crossing | `optimize-crossing.py` | `launch_crossing_khipu.sh` | `postprocess-crossing.py` |
| Splitter | `optimize-splitter.py` | `launch_splitter_khipu.sh` | `postprocess-splitter.py` |

---

## SLURM Scripts Reference

<details>
<summary><strong>Simserver</strong> — GPU FDTD solver (1 GPU, 2 cores, 8 GB)</summary>

```bash
#!/bin/sh
#SBATCH -J maxwell-sim
#SBATCH --partition=gpu
#SBATCH --gpus=1
#SBATCH --nodelist=g002
#SBATCH -c 2
#SBATCH --mem=8GB

module purge > /dev/null 2>&1
module load gnu12/12.4.0
module load cuda/11.4
module load mpich/3.4.3-ofi
module load python3/3.10.2

export PATH=/usr/local/cuda-11.4/targets/x86_64-linux/lib:$PATH
export LD_LIBRARY_PATH=/usr/local/cuda-11.4/lib64:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/opt/ohpc/pub/libs/gnu12/python3/3.10.2/lib:$LD_LIBRARY_PATH
export OMP_NUM_THREADS=2
export MAXWELL_SERVER_FILES=/tmp/maxwell-server-files

python3 maxwell-server/simserver.py 1 &> simserver.log

module unload mpich/3.4.3-ofi
module unload cuda/11.4
module unload gnu12/12.4.0
```

</details>

<details>
<summary><strong>Webserver</strong> — HTTP relay on port 6000 (2 cores, 8 GB)</summary>

```bash
#!/bin/sh
#SBATCH -J maxwell-web
#SBATCH --partition=gpu
#SBATCH --nodelist=g002
#SBATCH -c 2
#SBATCH --mem=8GB

module purge > /dev/null 2>&1
module load gnu12/12.4.0
module load cuda/11.4
module load mpich/3.4.3-ofi
module load python3/3.10.2

export PATH=/usr/local/cuda-11.4/targets/x86_64-linux/lib:$PATH
export LD_LIBRARY_PATH=/usr/local/cuda-11.4/lib64:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/opt/ohpc/pub/libs/gnu12/python3/3.10.2/lib:$LD_LIBRARY_PATH
export OMP_NUM_THREADS=2
export MAXWELL_SERVER_FILES=/tmp/maxwell-server-files

python3 maxwell-server/webserver.py 6000 &> webserver.log

module unload mpich/3.4.3-ofi
module unload cuda/11.4
module unload gnu12/12.4.0
```

</details>

<details>
<summary><strong>Photonic optimizer</strong> — CPU only (1 core, 16 GB) — example: bend</summary>

```bash
#!/bin/sh
#SBATCH -J bend-spins
#SBATCH --partition=gpu
#SBATCH --nodelist=g002
#SBATCH -c 1
#SBATCH --mem=16GB

module purge > /dev/null 2>&1
module load gnu12/12.4.0
module load mpich/3.4.3-ofi
module load python3/3.10.2

export LD_LIBRARY_PATH=/opt/ohpc/pub/libs/gnu12/python3/3.10.2/lib:$LD_LIBRARY_PATH
export OMP_NUM_THREADS=1
export MAXWELL_SERVER=localhost:6000

/opt/ohpc/pub/libs/gnu12/python3/3.10.2/bin/python3 optimize-bend.py

module unload mpich/3.4.3-ofi
module unload gnu12/12.4.0
```

</details>