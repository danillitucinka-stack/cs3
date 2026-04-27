import os
import subprocess
import shutil

# Шляхи
EXTERNAL_DIR = "external"
HLLIB_DIR = os.path.join(EXTERNAL_DIR, "hllib")
BUILD_DIR = "build"

def run_command(command, cwd=None):
    result = subprocess.run(command, shell=True, cwd=cwd)
    if result.returncode != 0:
        print(f"❌ Помилка при виконанні: {command}")
        exit(1)

print("🚀 Починаємо локальну збірку CS3...")

# 1. Створюємо папку для бібліотек
if not os.path.exists(EXTERNAL_DIR):
    os.makedirs(EXTERNAL_DIR)

# 2. Качаємо HLLib (якщо ще немає)
if not os.path.exists(HLLIB_DIR):
    print("📦 Завантажуємо HLLib...")
    run_command(f"git clone https://github.com/syndetic-networks/hllib.git {HLLIB_DIR}")
else:
    print("✅ HLLib вже завантажена.")

# 3. Створюємо папку для збірки
if os.path.exists(BUILD_DIR):
    shutil.rmtree(BUILD_DIR)
os.makedirs(BUILD_DIR)

# 4. Запускаємо CMake (конфігурація)
print("⚙️ Конфігурація проекту через CMake...")
run_command(f"cmake -B {BUILD_DIR} -S .", cwd=".")

# 5. Компіляція
print("🔨 Компіляція проекту...")
run_command(f"cmake --build {BUILD_DIR} --config Release", cwd=".")

print("🎉 Готово! Шукай файл у папку build/Release/")