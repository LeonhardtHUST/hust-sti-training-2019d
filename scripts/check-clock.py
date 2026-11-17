"""Fail the PlatformIO build when CubeMX and board clocks diverge."""

import json
import os
import re

Import("env")


def _read_ioc_frequency(path):
    with open(path, "r", encoding="utf-8") as ioc_file:
        for line in ioc_file:
            if line.startswith("RCC.SYSCLKFreq_VALUE="):
                value = line.partition("=")[2].strip()
                return int(value)
    raise RuntimeError("RCC.SYSCLKFreq_VALUE is missing from {}".format(path))


def _read_board_frequency(path):
    with open(path, "r", encoding="utf-8") as board_file:
        board = json.load(board_file)
    value = board.get("build", {}).get("f_cpu")
    if value is None:
        raise RuntimeError("build.f_cpu is missing from {}".format(path))
    match = re.match(r"^([0-9]+)", str(value).strip())
    if match is None:
        raise RuntimeError("build.f_cpu is not an integer literal: {}".format(value))
    return int(match.group(1))


project_dir = env.subst("$PROJECT_DIR")
board_path = os.path.join(project_dir, "boards", "{}.json".format(env.subst("$BOARD")))
ioc_path = os.path.join(project_dir, "template-mcu-minimal-STM32G474VET6-XPulse.ioc")

try:
    ioc_frequency = _read_ioc_frequency(ioc_path)
    board_frequency = _read_board_frequency(board_path)
except (IOError, OSError, ValueError, RuntimeError) as error:
    raise RuntimeError("Clock consistency check failed: {}".format(error))

if ioc_frequency != board_frequency:
    raise RuntimeError(
        "Clock mismatch: .ioc RCC.SYSCLKFreq_VALUE={} Hz, board f_cpu={} Hz"
        .format(ioc_frequency, board_frequency)
    )

print(">>> Clock consistency check: {} Hz (.ioc) == {} Hz (board f_cpu)".format(
    ioc_frequency, board_frequency
))
