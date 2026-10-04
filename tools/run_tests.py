"""Build and run all host tests at both fusion rates and gyro full scales."""
import argparse
import importlib.util
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parent.parent

def run(command):
    print("+", " ".join(map(str, command)), flush=True)
    subprocess.run(list(map(str, command)), cwd=ROOT, check=True)

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cc", default="gcc", help="GCC-compatible compiler executable")
    parser.add_argument("--sanitize", action="store_true", help="Enable undefined-behavior sanitizer")
    args = parser.parse_args()
    spec = importlib.util.spec_from_file_location("coefficients", ROOT / "tools/vqf_coefficients.py")
    coefficients = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(coefficients)
    assert (ROOT / "src/fixed_vqf_tuning.h").read_text() == coefficients.generate(), "Coefficient header is stale"
    out = ROOT / "build-host"
    out.mkdir(exist_ok=True)
    for hz in (1000, 2000):
        for fs in (0, 1):
            common = [args.cc, "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-UNDEBUG",
                      "-Iinclude", f"-DFIXED_VQF_SAMPLE_HZ={hz}U", f"-DLSM6DSV_GYRO_FS_2000DPS={fs}"]
            if args.sanitize:
                common += ["-fsanitize=undefined", "-fno-sanitize-recover=all"]
            for name in ("vqf_profiles", "imu_numeric", "vqf_motion"):
                exe = out / f"test_{name}_{hz}_{fs}.exe"
                run(common + [f"tests/test_{name}.c", "src/fixed_vqf.c", "-lm", "-o", exe])
                run([exe])
    print("All 12 host test configurations passed; coefficient header matches generator.")

if __name__ == "__main__":
    try:
        main()
    except subprocess.CalledProcessError as exc:
        sys.exit(exc.returncode)
