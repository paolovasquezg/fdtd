# Khipu UTEC - Uso y Profilling

> **Nota:** Login Node para descargas, GPU Node para ejecución.

Transferir archivos:
```bash
scp <email>@khipu.utec.edu.pe:<path_remoto> <path_local>
```

Ver particiones disponibles:
```bash
sinfo -O "partition"
```

---

# 1. Entrar al Login Node

```bash
ssh paolo.vasquez@khipu.utec.edu.pe
```

Instalar Julia (última versión vía juliaup):
```bash
curl -fsSL https://install.julialang.org | sh
```

Hacer persistentes el PATH de juliaup y CUDA en `~/.bashrc`:
```bash
grep -qxF 'export PATH=~/.juliaup/bin:$PATH' ~/.bashrc || echo 'export PATH=~/.juliaup/bin:$PATH' >> ~/.bashrc
grep -qxF 'module load cuda/12.6' ~/.bashrc || echo 'module load cuda/12.6' >> ~/.bashrc
grep -qxF 'export JULIA_CUDA_USE_BINARYBUILDER=false' ~/.bashrc || echo 'export JULIA_CUDA_USE_BINARYBUILDER=false' >> ~/.bashrc
source ~/.bashrc
```

> **Nota:** No cargar `module load julia/...` — se usa la versión instalada por juliaup, que siempre es la más reciente.

Verificar versión activa:
```bash
which julia
julia --version
```

Instalar CUDA en Julia:
```julia
import Pkg
Pkg.add("CUDA")
using CUDA
```

---

# 2. Entrar al Nodo GPU

4 cores, 16 GB RAM:

```bash
srun --partition=debug-gpu --gres=gpu:1 -c 4 --mem=16GB --pty bash
```

---

# 3. Cargar los Módulos

Los módulos y exports ya están configurados vía `~/.bashrc`. Para verificar:

```bash
module list
which julia
julia --version
```

---

# 4. Abrir Julia

```bash
julia
```

```julia
using CUDA
```

> **Nota:** La primera vez tarda varios minutos en precompilar CUDA. Las siguientes ejecuciones son instantáneas.

---

# 5. Profiling

## a. Instalar JuNest

```bash
git clone https://github.com/fsquillace/junest.git ~/.local/share/junest
grep -qxF 'export PATH=~/.local/share/junest/bin:$PATH' ~/.bashrc || echo 'export PATH=~/.local/share/junest/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
```

Configurar el entorno:
```bash
junest setup
```

## b. Corregir el mirrorlist

```bash
echo 'Server = https://mirror.rackspace.com/archlinux/$repo/os/$arch' > ~/.junest/etc/pacman.d/mirrorlist
```

## c. Deshabilitar verificación de firmas PGP

```bash
junest -f -- bash -c "sed -i 's/^SigLevel.*/SigLevel = Never/' /etc/pacman.conf"
```

Verifica:
```bash
junest -f -- grep SigLevel /etc/pacman.conf
```

## d. Verificar versión del driver NVIDIA del host

```bash
nvidia-smi | grep "Driver Version"
```

## e. Descargar paquetes NVIDIA

Reemplaza `VERSION` con tu versión exacta (ej: `560.35.03`):

```bash
wget "https://archive.archlinux.org/packages/n/nvidia-dkms/nvidia-dkms-VERSION-1-x86_64.pkg.tar.zst"
wget "https://archive.archlinux.org/packages/n/nvidia-utils/nvidia-utils-VERSION-1-x86_64.pkg.tar.zst"
```

Ejemplo con versión `560.35.03`:
```bash
wget "https://archive.archlinux.org/packages/n/nvidia-dkms/nvidia-dkms-560.35.03-1-x86_64.pkg.tar.zst"
wget "https://archive.archlinux.org/packages/n/nvidia-utils/nvidia-utils-560.35.03-1-x86_64.pkg.tar.zst"
```

## f. Instalar paquetes NVIDIA

```bash
junest -f -- pacman -U nvidia-dkms-560.35.03-1-x86_64.pkg.tar.zst nvidia-utils-560.35.03-1-x86_64.pkg.tar.zst
```

> Los warnings de systemd y permisos son normales en JuNest. La instalación es exitosa si aparece `installing nvidia-dkms` e `installing nvidia-utils`.

## g. Agregar IgnorePkg para evitar actualizaciones automáticas

```bash
junest -f -- bash -c "sed -i 's/^#IgnorePkg.*/IgnorePkg = nvidia-dkms nvidia-utils/' /etc/pacman.conf"
```

Verifica:
```bash
junest -f -- grep IgnorePkg /etc/pacman.conf
```

## h. Verificar instalación

```bash
junest -f -- pacman -Q nvidia-dkms nvidia-utils
```

Salida esperada:
```
nvidia-dkms 560.35.03-1
nvidia-utils 560.35.03-1
```

## i. Instalar Julia y paquetes dentro de JuNest

```bash
junest -f -- pacman -Syu julia
junest -f -- bash -c "julia -e 'import Pkg; Pkg.add(\"CUDA\"); Pkg.add(\"BenchmarkTools\")'"
```

## j. Instalar Nsight Systems dentro de JuNest

```bash
junest -f -- bash -c "pacman -Sy && pacman -S --noconfirm nsight-systems"
```

Verificar instalación:
```bash
junest -f -- bash -c "nsys --version"
```

> **Nota:** `nsys` debe instalarse dentro de junest para poder instrumentar julia. Al ejecutar el profiling, `nsys` corre desde el host con `LD_PRELOAD=""` y julia corre dentro de junest.

---

> **Nota:** El mensaje `environment: line 1: /usr/bin/which: No such file or directory` es un warning normal de JuNest, no es un error.