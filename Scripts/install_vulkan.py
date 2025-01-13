import os
import sys
import subprocess
import urllib.request
import zipfile
import requests
from pathlib import Path

def get_vulkan_sdk_url():
    """Fetch the appropriate Vulkan SDK URL for the platform."""
    base_url = "https://sdk.lunarg.com/sdk/download"
    version = "1.3.261.1"  # Change to the desired Vulkan SDK version
    platform = "windows"   # Adjust if needed for cross-platform support

    return f"{base_url}/{version}/{platform}/vulkan-sdk.exe"

def download_vulkan_sdk(url, download_path):
    """Download the Vulkan SDK executable with proper headers."""
    print(f"Downloading Vulkan SDK from {url}...")
    headers = {
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36"
    }
    try:
        response = requests.get(url, headers=headers, stream=True)
        response.raise_for_status()  # Raise an error for bad responses (4xx or 5xx)

        with open(download_path, "wb") as file:
            for chunk in response.iter_content(chunk_size=8192):
                file.write(chunk)
        print(f"Vulkan SDK downloaded to {download_path}")
    except requests.exceptions.RequestException as e:
        print(f"Failed to download Vulkan SDK: {e}")
        sys.exit(1)

def install_vulkan_sdk(installer_path):
    """Run the Vulkan SDK installer silently."""
    print("Installing Vulkan SDK...")
    try:
        subprocess.run([installer_path, "/S"], check=True)
        print("Vulkan SDK installed successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Failed to install Vulkan SDK: {e}")
        sys.exit(1)

def validate_vulkan_sdk():
    """Validate that the Vulkan SDK is properly installed."""
    vulkan_sdk = os.getenv("VULKAN_SDK")
    if vulkan_sdk:
        print(f"Vulkan SDK detected at: {vulkan_sdk}")
    else:
        print("Error: Vulkan SDK environment variable is not set.")
        sys.exit(1)

def main():
    """Main function to automate Vulkan SDK setup."""
    sdk_installer = Path("vulkan-sdk-installer.exe")
    if not sdk_installer.exists():
        vulkan_sdk_url = get_vulkan_sdk_url()
        download_vulkan_sdk(vulkan_sdk_url, sdk_installer)

    install_vulkan_sdk(sdk_installer)
    validate_vulkan_sdk()

    # Clean up the installer
    if sdk_installer.exists():
        sdk_installer.unlink()

if __name__ == "__main__":
    main()
