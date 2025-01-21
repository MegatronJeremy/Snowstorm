import os
import subprocess
import sys


def run_command(command, cwd=None):
    """
    Run a shell command in a specified directory and handle errors.
    """
    try:
        subprocess.run(command, cwd=cwd, check=True, shell=True)
    except subprocess.CalledProcessError as e:
        print(f"Error: Command '{command}' failed with exit code {e.returncode}")
        sys.exit(e.returncode)


def create_venv(venv_dir):
    """
    Create a Python virtual environment if it doesn't already exist.
    """
    if not os.path.exists(venv_dir):
        print(f"Creating virtual environment in {venv_dir}...")
        subprocess.run([sys.executable, "-m", "venv", venv_dir], check=True)
        print("Virtual environment created.")
    else:
        print(f"Virtual environment already exists in {venv_dir}.")


def ensure_pip_in_venv(venv_dir):
    """
    Ensure pip is installed and upgraded in the virtual environment.
    """
    python_executable = os.path.join(
        venv_dir, "Scripts", "python.exe" if os.name == "nt" else "bin/python"
    )
    try:
        print("Ensuring pip is installed and updated...")
        run_command(f"{python_executable} -m ensurepip --upgrade")
        run_command(f"{python_executable} -m pip install --upgrade pip")
        print("Pip is ready.")
    except Exception as e:
        print(f"Error ensuring pip is installed: {e}")
        sys.exit(1)


def ensure_conan_profile(conan_executable):
    """
    Ensure the default Conan profile exists and set C++ standard to 20.
    """
    conan_home = os.path.expanduser("~/.conan2")
    default_profile = os.path.join(conan_home, "profiles", "default")
    if not os.path.exists(default_profile):
        print("Default Conan profile not found. Detecting and creating it...")
        run_command(f'"{conan_executable}" profile detect')
        print("Default Conan profile created.")


def configure_cmake(build_dir, project_root):
    """
    Configure the CMake project using the Conan toolchain file.
    """
    cmake_command = (
        f'cmake -G "Visual Studio 17 2022" -A x64 '
        f'-DCMAKE_TOOLCHAIN_FILE="{build_dir}/generators/conan_toolchain.cmake" '
        f'-S "{project_root}" -B "{build_dir}"'
    )
    print("Configuring the CMake project...")
    run_command(cmake_command)


def main():
    # Paths
    project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    build_dir = os.path.join(project_root, "build")
    venv_dir = os.path.join(project_root, "venv")
    conan_executable = os.path.join(
        venv_dir, "Scripts", "conan.exe" if os.name == "nt" else "bin/conan"
    )

    # Step 1: Create the Python virtual environment
    create_venv(venv_dir)

    # Step 2: Ensure pip is installed in the virtual environment
    ensure_pip_in_venv(venv_dir)

    # Step 3: Install Conan in the virtual environment
    python_executable = os.path.join(
        venv_dir, "Scripts", "python.exe" if os.name == "nt" else "bin/python"
    )
    print("Installing Conan in the virtual environment...")
    run_command(f"{python_executable} -m pip install conan")

    # Step 4: Ensure the default Conan profile exists
    ensure_conan_profile(conan_executable)

    # Step 5: Run Conan install using the executable
    print("Running Conan install...")
    conan_install_command = (
        f'"{conan_executable}" install "{project_root}" '
        f'--build=missing '
        f'-s os=Windows -s arch=x86_64 -s build_type=Release '
        f'-s compiler=msvc -s compiler.version=193 -s compiler.runtime=dynamic '
        f'-s compiler.cppstd=20'
    )
    run_command(conan_install_command)

    # Step 6: Create the build directory if it doesn't exist
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)
        print(f"Created build directory at {build_dir}.")
    else:
        print(f"Build directory already exists at {build_dir}.")

    # Step 7: Configure the CMake project
    configure_cmake(build_dir, project_root)

    print("\nSetup completed successfully. You can now open the Visual Studio project file.")
    print(f"Path: {os.path.join(build_dir, 'Snowstorm.sln')}")

if __name__ == "__main__":
    main()
