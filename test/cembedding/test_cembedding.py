from pathlib import Path
import subprocess
import sys


def test_embeddedlpy(tmp_path):
    source = Path(__file__).resolve().parent
    build = tmp_path / "build"
    subprocess.run(
        [
            "cmake", "-S", str(source), "-B", str(build),
            f"-DPython3_EXECUTABLE={sys.executable}",
            f"-DCMAKE_PREFIX_PATH={sys.prefix}",
        ],
        check=True,
    )
    subprocess.run(
        ["cmake", "--build", str(build), "--config", "Release", "--parallel", "4"],
        check=True,
    )
    result = subprocess.run(
        ["ctest", "--test-dir", str(build), "-C", "Release", "--verbose", "--no-tests=error"],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
    )
    assert result.returncode == 0, result.stdout
    assert (
        "'+(90);(3)I(6765)I(4181)I(2584)I(1597)I(987)I(610)I(377)"
        "I(233)I(144)I(89)I(55)I(34)I(21)I(13)I(8)I(5)I(3)I(2)I(1)I(1)'"
    ) in result.stdout
