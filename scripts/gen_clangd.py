#!/bin/python3

import os
import io
import shutil
import platform
import sys

from typing import Optional
from pathlib import Path

ROOT_DIR = Path(__file__).parent.parent

CLANGD_FILE_PATH = ROOT_DIR / ".clangd"

COMPILE_FLAGS_PATH = ROOT_DIR / "compile_flags.txt"

def find_vk_sdk_path_windows(hint: Optional[str] = None) -> Optional[str]:
    if platform.system() != "Windows":
        return None

    vk_sdk_path = os.getenv("VK_SDK_PATH")

    if vk_sdk_path is not None:
        return vk_sdk_path

    print("Error: cannot find vulkan sdk path")

    return None

def find_vk_include_path_linux(hint: Optional[str] = None) -> Optional[str]:
    if platform.system() != "Linux":
        return None

    usr_include = Path("/usr/include")

    vk_include = usr_include / "vulkan"

    if vk_include.exists():
        return str(vk_include)

    return None

if __name__ == "__main__":
    buf = io.StringIO()

    vk_sdk_hint = None

    if len(sys.argv) > 1:
        print(f"Using hint to find Vulkan sdk path: {sys.argv[1]}")
        vk_sdk_hint = sys.argv[1]

    cpp_std = 20

    buf.write("CompileFlags:\n")
    buf.write("    Add:\n")
    buf.write("    - \"-Iinclude\"\n")
    buf.write("    - \"-Itests/utils\"\n")
    buf.write("    - \"-DPROSERPINE_IMPLEMENTATION\"\n")
    buf.write("    - \"-DPROSERPINE_INCLUDE_VULKAN\"\n")
    buf.write(f"    - \"-std=c++{cpp_std}\"\n")

    if platform.system() == "Windows":
        vk_sdk_path = find_vk_sdk_path_windows(vk_sdk_hint)

        if vk_sdk_path is not None:
            vk_sdk_path = vk_sdk_path.replace('\\', '/')
            buf.write(f"    - \"-I{vk_sdk_path}/Include\"\n")
    elif platform.system() == "Linux":
        vk_include_path = find_vk_include_path_linux()

        if vk_include_path is not None:
            buf.write(f"    - \"-I{vk_include_path}\"\n")

    buf.write("\n")
    buf.write("Diagnostics:\n")
    buf.write("    Suppress: macro-redefined")

    with open(CLANGD_FILE_PATH, "w", encoding="utf-8") as file:
        buf.seek(0)
        shutil.copyfileobj(buf, file)

    # clangd has a bug that makes it not working if there are no
    # compile_flags.txt file next to a .clangd file (even if empty)
    with open(COMPILE_FLAGS_PATH, "w", encoding="utf-8") as file:
        pass
