import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

def read_frame(tipo, basename, frame_num):
    filename = f"../2d/{tipo}/{basename}.{frame_num}"
    try:
        with open(filename, 'rb') as f:
            nx = int(np.frombuffer(f.read(4), dtype=np.float32)[0])
            ny = int(np.frombuffer(f.read(4), dtype=np.float32)[0])
            data = np.frombuffer(f.read(nx * ny * 4), dtype=np.float32)
            data = data.reshape(ny, nx)
        return data, nx, ny
    except FileNotFoundError:
        return None

def animate(tipo, basename):
    frames = []
    i = 0
    while True:
        result = read_frame(tipo, basename, i)
        if result is None:
            break
        frames.append(result[0])
        i += 1

    if not frames:
        print(f"No files found with base name '{basename}'.")
        return

    print(f"Loaded {len(frames)} frames.")
    vmax = max(np.abs(f).max() for f in frames)

    fig, ax = plt.subplots()
    im = ax.imshow(frames[0], origin='lower', cmap='RdBu_r', aspect='auto',
                   vmin=-vmax, vmax=vmax, animated=True)
    plt.colorbar(im, ax=ax, label='Ez' if tipo == "tmz" else "Hz")
    ax.set_xlabel('x'); ax.set_ylabel('y')
    title = ax.set_title(f'{basename}.0')

    def update(i):
        im.set_array(frames[i])
        title.set_text(f'{basename}.{i}  (step {i * 10 + 10})')
        return [im, title]

    ani = animation.FuncAnimation(fig, update, frames=len(frames),
                                  interval=100, blit=False)
    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    tipo = sys.argv[1]
    basename = sys.argv[2]
    animate(tipo, basename)