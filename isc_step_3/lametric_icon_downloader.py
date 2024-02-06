from pathlib import Path

import requests
from PIL import Image
from tqdm import tqdm

path_dir_icons = Path("icons")
path_dir_icons.mkdir(exist_ok=True)


def get_average_colour_of_gif(gif: Image):
    img = gif.convert("RGBA")
    avg_r, avg_g, avg_b = 0, 0, 0
    w, h = img.size
    pixel_cnt = 0
    for y in range(h):
        for x in range(w):
            r, g, b, a = img.getpixel((x, y))
            if a <= 0:
                continue
            pixel_cnt += 1
            avg_r += r
            avg_g += g
            avg_b += b
    avg_r /= pixel_cnt
    avg_g /= pixel_cnt
    avg_b /= pixel_cnt
    return avg_r, avg_g, avg_b


def generate_icon_array_code(lametric_icon_id: int):
    path_icon = path_dir_icons / f"{lametric_icon_id}.gif"

    if not path_icon.exists():
        res = requests.get(f"https://developer.lametric.com/content/apps/icon_thumbs/{lametric_icon_id}.gif")
        if not res.ok:
            raise Exception("Cannot find icon!")
        with open(path_icon, "wb") as f:
            f.write(res.content)

    gif = Image.open(path_icon)

    avg_r, avg_g, avg_b = [round(o * .4) for o in get_average_colour_of_gif(gif)]
    print(avg_r, avg_g, avg_b)

    frames = []
    while True:
        print(f"Processing {path_icon.name}/{gif.tell()}...")
        img = gif.convert("RGBA")
        w, h = img.size
        frame_hex = []

        for y in range(h):
            for x in range(w):
                r, g, b, a = img.getpixel((x, y))

                if a <= 0:
                    # If original colour is transparent, use black
                    # r, g, b = 0, 0, 0

                    # If original colour is transparent, use average colour
                    r, g, b = avg_r, avg_g, avg_b

                # Compress and Invert RGB Order
                r, g, b = r // 16, g // 16, b // 16
                colour_val = (r << 0) + (g << 4) + (b << 8)

                # Append colour value to code
                frame_hex.append("0x" + hex(colour_val)[2:].upper().zfill(3))
        frames.append(frame_hex)
        try:
            gif.seek(gif.tell() + 1)
        except EOFError:
            break

    generated_code = ""
    generated_code += f"static const uint16_t PROGMEM icon_{lametric_icon_id}_bitmaps[] = {{ "
    for frame in frames:
        for pixel in frame:
            generated_code += pixel
            generated_code += ", "
    generated_code += f" }};"

    return [
        generated_code,
        f"static const IconSequence PROGMEM icon_{lametric_icon_id} = {{ (uint16_t *) icon_{lametric_icon_id}_bitmaps, {w}, {h}, {len(frames)} }};"
    ]


lametric_icon_ids = [
    1769,  # pacman1
    4738,  # mario
    5588,
    2369,
    7645,
    26536,
    26910
]
output_cpp_path = Path("Icons.h")
output_cpp_template = """
typedef struct {
    uint16_t *frames;
    uint16_t width;
    uint16_t height;
    uint16_t count;
} IconSequence;
"""
output_cpp_content = [
    *output_cpp_template.strip("\n").splitlines(),
    ""
]
for lametric_icon_id in tqdm(lametric_icon_ids):
    output_cpp_content.extend(generate_icon_array_code(lametric_icon_id))
    output_cpp_content.append("")

with open(output_cpp_path, "w") as f:
    f.write("\n".join(output_cpp_content) + "\n")
