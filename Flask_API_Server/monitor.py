import os
import time
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
from PIL import Image
import numpy as np
from concurrent.futures import ThreadPoolExecutor, as_completed

# Define folders
IMAGE_FOLDER = './images'
H_FILES_FOLDER = './h_files'

# Define the 7-color palette mapping
color_palette = {
    (255, 255, 255): 0xFF,  # White
    (255, 255, 0): 0xFC,    # Yellow
    (255, 165, 0): 0xEC,    # Orange
    (255, 0, 0): 0xE0,      # Red
    (0, 128, 0): 0x35,      # Green
    (0, 0, 255): 0x2B,      # Blue
    (0, 0, 0): 0x00         # Black
}

def closest_palette_color(rgb):
    """Find the closest color in the palette."""
    min_dist = float('inf')
    closest_color = (255, 255, 255)  # Default to white
    for palette_rgb in color_palette:
        # Cast to int32 to prevent overflow during calculations
        dist = sum((int(rgb[i]) - int(palette_rgb[i])) ** 2 for i in range(3))
        if dist < min_dist:
            min_dist = dist
            closest_color = palette_rgb
    return closest_color

def apply_floyd_steinberg_dithering(image):
    """Apply Floyd-Steinberg dithering to the image."""
    pixels = np.array(image, dtype=np.int16)  # Use int16 to allow negative values during error distribution
    for y in range(image.height):
        for x in range(image.width):
            old_pixel = tuple(pixels[y, x][:3])
            new_pixel = closest_palette_color(old_pixel)
            pixels[y, x][:3] = new_pixel
            quant_error = np.array(old_pixel) - np.array(new_pixel)
            
            # Distribute the quantization error to neighboring pixels (convert to int16 before adding)
            if x + 1 < image.width:
                pixels[y, x + 1][:3] += (quant_error * 7 / 16).astype(np.int16)
            if x - 1 >= 0 and y + 1 < image.height:
                pixels[y + 1, x - 1][:3] += (quant_error * 3 / 16).astype(np.int16)
            if y + 1 < image.height:
                pixels[y + 1, x][:3] += (quant_error * 5 / 16).astype(np.int16)
            if x + 1 < image.width and y + 1 < image.height:
                pixels[y + 1, x + 1][:3] += (quant_error * 1 / 16).astype(np.int16)
    
    # Clip pixel values to be within 0-255 range after dithering
    pixels = np.clip(pixels, 0, 255)
    return Image.fromarray(pixels.astype(np.uint8))

def convert_image_to_header(input_image_path, output_path):
    # Open and resize the image
    image = Image.open(input_image_path)
    image = image.resize((600, 448))
    image = image.convert("RGB")  # Ensure it’s in RGB format

    # Apply dithering
    dithered_image = apply_floyd_steinberg_dithering(image)

    output_file = os.path.join(output_path, os.path.splitext(os.path.basename(input_image_path))[0] + ".h")

    # Create the data array
    data_array = []
    for y in range(448):
        for x in range(600):
            rgb = dithered_image.getpixel((x, y))
            color_code = color_palette.get(tuple(rgb), 0xFF)  # Default to white if color is not mapped
            data_array.append(color_code)

    # Write to header file
    with open(output_file, 'w') as f:
        for i in range(0, len(data_array), 16):
            line = ', '.join(f"0x{data_array[j]:02X}" for j in range(i, min(i + 16, len(data_array))))
            f.write(f"    {line},\n")

# Handler for folder changes
class ImageHandler(FileSystemEventHandler):
    def __init__(self):
        self.executor = ThreadPoolExecutor()
        
    def on_created(self, event):
        if event.is_directory:
            return
        if event.src_path.lower().endswith(('.png', '.jpg', '.jpeg')):
            print(f"New image detected: {event.src_path}")

            # Wait until file is fully copied by checking file size stability
            stable = False
            while not stable:
                initial_size = os.path.getsize(event.src_path)
                time.sleep(1)  # Wait a second
                new_size = os.path.getsize(event.src_path)
                if initial_size == new_size:
                    stable = True
            try:
                 # Submit the file conversion task to the executor
                self.executor.submit(convert_image_to_header, event.src_path, H_FILES_FOLDER)
                print(f"Converted {event.src_path} to .h file")
            except Exception as e:
                print(f"Failed to convert {event.src_path}: {e}")

# Monitor the image folder for changes
def monitor_folder():
    if not os.path.exists(H_FILES_FOLDER):
        os.makedirs(H_FILES_FOLDER)

    if not os.path.exists(IMAGE_FOLDER):
        os.makedirs(IMAGE_FOLDER)

    event_handler = ImageHandler()
    observer = Observer()
    observer.schedule(event_handler, path=IMAGE_FOLDER, recursive=False)
    observer.start()
    
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        observer.stop()
    observer.join()

# Run the folder monitoring
if __name__ == "__main__":
    monitor_folder()
