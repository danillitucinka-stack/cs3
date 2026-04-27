import os
import urllib.request
import zipfile
import shutil

def setup_local():
    print("Starting final CS3 build attempt...")

    # 1. Create folder for libraries
    ext_path = "external/hllib"
    if not os.path.exists(ext_path):
        os.makedirs(ext_path)

    # 2. Direct download of HLLib ZIP archive (this link is definitely live)
    # We take the code directly from a reliable repository
    url = "https://github.com/nemysis/hllib/archive/refs/heads/master.zip"
    zip_name = "hllib.zip"

    try:
        print("Downloading HLLib via direct link...")
        urllib.request.urlretrieve(url, zip_name)

        with zipfile.ZipFile(zip_name, 'r') as zip_ref:
            zip_ref.extractall("external")

        # Rename folder for CMake
        downloaded_folder = "external/hllib-master"
        if os.path.exists(downloaded_folder):
            if os.path.exists(ext_path):
                shutil.rmtree(ext_path)
            os.rename(downloaded_folder, ext_path)

        os.remove(zip_name)
        print("HLLib successfully extracted!")

    except Exception as e:
        print(f"Even this doesn't work: {e}")
        return

    print("\nNow run CMake through your script or VS Code.")
    print("It will see the files in external/hllib and everything will work!")

if __name__ == "__main__":
    setup_local()
    