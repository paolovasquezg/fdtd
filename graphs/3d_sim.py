import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

PLANE_CONFIG = {("y", "z"): dict(normal="x", xlabel="y", ylabel="z"),
                ("x", "z"): dict(normal="y", xlabel="x", ylabel="z"),
                ("x", "y"): dict(normal="z", xlabel="x", ylabel="y")}

def parse_plane(arg):
    arg = arg.lower().replace(",", "").replace(" ", "")
    if len(arg) != 2 or not all(c in "xyz" for c in arg):
        return None
    key = tuple(sorted(arg))
    return key if key in PLANE_CONFIG else None

def read_frame(normal, tipo, basename, frame_num):
    filename = f"../3d/{tipo}/{basename}-{normal}.{frame_num}"
    try:
        with open(filename, "rb") as f:
            dim1 = int(np.frombuffer(f.read(4), dtype=np.float32)[0])
            dim2 = int(np.frombuffer(f.read(4), dtype=np.float32)[0])
            data = np.frombuffer(f.read(dim1 * dim2 * 4), dtype=np.float32)
            data = data.reshape(dim1, dim2)
        return data, dim1, dim2
    except FileNotFoundError:
        return None

def log_scale(f, ref=0.3, decades=3):
    with np.errstate(divide='ignore', invalid='ignore'):
        scaled = np.log10(np.abs(f) / ref)
    return np.clip(scaled, -decades, 0)

def animate(plane_key, tipo, basename):
    cfg    = PLANE_CONFIG[plane_key]
    normal = cfg["normal"]
    xlabel = cfg["xlabel"]
    ylabel = cfg["ylabel"]

    frames = []
    i = 0
    while True:
        result = read_frame(normal, tipo, basename, i)
        if result is None:
            break
        frames.append(result[0])
        i += 1

    if not frames:
        print(f"No se encontraron archivos '../3d/{tipo}/{basename}-{normal}.0', ...")
        return

    print(f"Plano: {xlabel.upper()}-{ylabel.upper()}  (corte constante-{normal.upper()})")
    print(f"Cargados {len(frames)} frames.")

    fig, ax = plt.subplots()
    im = ax.imshow(
        log_scale(frames[0]),
        origin="lower",
        cmap="jet",
        aspect="auto",
        vmin=-3,
        vmax=0,
        animated=True,
    )
    plt.colorbar(im, ax=ax, label="log10(|Ex| / 0.3)  [décadas]")
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    title = ax.set_title(
        f"{basename}-{normal}.0  "
        f"(plano {xlabel.upper()}-{ylabel.upper()}, corte en {normal.upper()})"
    )

    def update(i):
        im.set_array(log_scale(frames[i]))
        title.set_text(
            f"{basename}-{normal}.{i}  "
            f"(plano {xlabel.upper()}-{ylabel.upper()}, corte en {normal.upper()})"
        )
        return [im, title]

    ani = animation.FuncAnimation(fig, update, frames=len(frames), interval=100, blit=False)
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Uso: python3 3d_sim.py <plano> <tipo> <basename>")
        sys.exit(1)

    plane_key = parse_plane(sys.argv[1])
    if plane_key is None:
        print(f"Error: plano '{sys.argv[1]}' no válido.")
        print("  Valores aceptados: yz, xz, xy  (o con coma: y,z  x,z  x,y)")
        sys.exit(1)

    tipo     = sys.argv[2]
    basename = sys.argv[3]
    animate(plane_key, tipo, basename)

# python3 3d_sim.py <plano> <tipo> <basename>
# yz  (o y,z) -> corte constante-x  (ejes: y horiz, z vert)  -> archivo: snap-x.N
# xz  (o x,z) -> corte constante-y  (ejes: x horiz, z vert)  -> archivo: snap-y.N
# xy  (o x,y) -> corte constante-z  (ejes: x horiz, y vert)  -> archivo: snap-z.N
# python3 3d_sim.py yz basic snap