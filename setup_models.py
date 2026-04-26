import os
import shutil
from pathlib import Path

def copy_cs16_assets():
    cs16_path = r"C:\Users\admin\Documents\GitHub\Counter-Strike 1.6 Original"
    dest_models = "resources/models"
    dest_sounds = "resources/sounds"

    if not os.path.exists(cs16_path):
        print("CS 1.6 path not found!")
        return

    os.makedirs(dest_models, exist_ok=True)
    os.makedirs(dest_sounds, exist_ok=True)

    # Find model, sound, map files
    model_extensions = ['.mdl', '.obj', '.iqm']
    sound_extensions = ['.wav']
    map_extensions = ['.bsp', '.obj']  # Maps

    models_copied = 0
    sounds_copied = 0
    maps_copied = 0
    dest_maps = "resources/maps"
    os.makedirs(dest_maps, exist_ok=True)

    for root, dirs, files in os.walk(cs16_path):
        for file in files:
            if any(file.lower().endswith(ext) for ext in model_extensions):
                src = os.path.join(root, file)
                dst = os.path.join(dest_models, file)
                shutil.copy2(src, dst)
                print(f"Copied model {src} to {dst}")
                models_copied += 1
            elif any(file.lower().endswith(ext) for ext in sound_extensions):
                src = os.path.join(root, file)
                dst = os.path.join(dest_sounds, file)
                shutil.copy2(src, dst)
                print(f"Copied sound {src} to {dst}")
                sounds_copied += 1
            elif any(file.lower().endswith(ext) for ext in map_extensions):
                src = os.path.join(root, file)
                dst = os.path.join(dest_maps, file)
                shutil.copy2(src, dst)
                print(f"Copied map {src} to {dst}")
                maps_copied += 1

    print(f"Copied {models_copied} model files, {sounds_copied} sound files, and {maps_copied} map files.")
    if models_copied > 0:
        print("Note: Convert .mdl to .obj using Noesis or MilkShape 3D for Raylib compatibility.")
    if sounds_copied > 0:
        print("Sounds copied. Raylib supports .wav.")
    if maps_copied > 0:
        print("Maps copied. Convert .bsp to .obj if needed.")

if __name__ == "__main__":
    copy_cs16_assets()