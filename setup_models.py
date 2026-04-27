import os
import shutil
import sys

def setup_resources(cs_path):
    # Папка назначения
    dest_models = "resources/models"
    dest_sounds = "resources/sounds"

    os.makedirs(dest_models, exist_ok=True)
    os.makedirs(dest_sounds, exist_ok=True)

    # Источники из CS 1.6
    src_models = os.path.join(cs_path, "cstrike", "models")
    src_sounds = os.path.join(cs_path, "cstrike", "sound")

    # Копируем модели (только .mdl, .obj если есть)
    if os.path.exists(src_models):
        for file in os.listdir(src_models):
            if file.endswith(('.mdl', '.obj')):
                shutil.copy2(os.path.join(src_models, file), dest_models)
                print(f"Скопировано: {file}")
    else:
        print("Папка моделей не найдена")

    # Копируем звуки (.wav)
    if os.path.exists(src_sounds):
        for root, dirs, files in os.walk(src_sounds):
            for file in files:
                if file.endswith('.wav'):
                    rel_path = os.path.relpath(root, src_sounds)
                    dest_dir = os.path.join(dest_sounds, rel_path)
                    os.makedirs(dest_dir, exist_ok=True)
                    shutil.copy2(os.path.join(root, file), dest_dir)
                    print(f"Скопировано: {file}")
    else:
        print("Папка звуков не найдена")

    print("Ресурсы скопированы! Теперь dotnet build соберёт проект с моделями и звуками.")

if __name__ == "__main__":
    if len(sys.argv) > 1:
        cs_path = sys.argv[1]
        setup_resources(cs_path)
    else:
        print(C:\Users\admin\Documents\GitHub\Counter-Strike 1.6 Original")