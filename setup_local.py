import os
import urllib.request
import zipfile
import shutil

def setup_local():
    print("🚀 Починаємо фінальну спробу збірки CS3...")
    
    # 1. Створюємо папку для бібліотек
    ext_path = "external/hllib"
    if not os.path.exists(ext_path):
        os.makedirs(ext_path)

    # 2. Пряме завантаження ZIP-архіву HLLib (це посилання точно живе)
    # Ми беремо код прямо з надійного сховища
    url = "https://github.com/nemysis/hllib/archive/refs/heads/master.zip"
    zip_name = "hllib.zip"
    
    try:
        print("📦 Завантажуємо HLLib через пряме посилання...")
        urllib.request.urlretrieve(url, zip_name)
        
        with zipfile.ZipFile(zip_name, 'r') as zip_ref:
            zip_ref.extractall("external")
        
        # Перейменовуємо папку для CMake
        downloaded_folder = "external/hllib-master"
        if os.path.exists(downloaded_folder):
            if os.path.exists(ext_path):
                shutil.rmtree(ext_path)
            os.rename(downloaded_folder, ext_path)
            
        os.remove(zip_name)
        print("✅ HLLib успішно розпакована!")
        
    except Exception as e:
        print(f"❌ Навіть так не виходить: {e}")
        return

    print("\n🔨 Тепер запускай CMake через свій скрипт або VS Code.")
    print("Він побачить файли в external/hllib і все заведеться!")

if __name__ == "__main__":
    setup_local()
    