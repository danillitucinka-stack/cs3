import os
import random

def create_expanded_project():
    # Create directories
    os.makedirs("src", exist_ok=True)
    os.makedirs("assets/models", exist_ok=True)
    os.makedirs("assets/textures", exist_ok=True)
    os.makedirs("assets/sounds", exist_ok=True)
    os.makedirs("assets/levels", exist_ok=True)
    os.makedirs("scripts", exist_ok=True)

    # Generate 50 files with content
    files = {}

    # Levels (10 files)
    for i in range(1, 11):
        files[f"assets/levels/level{i}.json"] = f'{{"name": "Level {i}", "enemies": {random.randint(5, 20)}, "map": "dust2"}}'

    # Models (10 files)
    for i in range(1, 11):
        files[f"assets/models/model{i}.obj"] = f"# OBJ Model {i}\nv 0 0 0\nv 1 0 0\nf 1 2 3"

    # Textures (10 files)
    for i in range(1, 11):
        files[f"assets/textures/texture{i}.png"] = "Dummy PNG data"

    # Sounds (10 files)
    for i in range(1, 11):
        files[f"assets/sounds/sound{i}.wav"] = "Dummy WAV data"

    # Enemy scripts (5 files)
    for i in range(1, 6):
        files[f"scripts/enemy{i}.py"] = f"import random\ndef behavior{i}():\n    return random.choice(['attack', 'patrol'])"

    # Weapon configs (5 files)
    for i in range(1, 6):
        files[f"assets/weapons/weapon{i}.json"] = f'{{"name": "Weapon {i}", "damage": {random.randint(10, 50)}, "rate": {random.uniform(0.1, 1.0)}}'

    # Effects (5 files)
    for i in range(1, 6):
        files[f"assets/effects/effect{i}.txt"] = f"Particle effect {i}: speed {random.uniform(1, 5)}, life {random.uniform(0.5, 2.0)}"

    # UI elements (5 files)
    for i in range(1, 6):
        files[f"assets/ui/ui{i}.json"] = f'{{"element": "UI {i}", "position": [{random.randint(0, 1280)}, {random.randint(0, 720)}]}}'

    # Create files
    for file_path, content in files.items():
        os.makedirs(os.path.dirname(file_path), exist_ok=True)
        with open(file_path, "w") as f:
            f.write(content)
        print(f"Created {file_path}")

    print("Expanded project with 50+ files created!")

if __name__ == "__main__":
    create_expanded_project()