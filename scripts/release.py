#!/usr/bin/env python3

import argparse
import os
import subprocess
import sys
import typing
from pathlib import Path

FileName = typing.Union[str, bytes, os.PathLike]


def shell(cmd: str, check: bool = True) -> str:
    result = subprocess.run(cmd, shell=True, stdout=subprocess.PIPE, check=check)
    return result.stdout.decode("utf-8").strip()


def patch_cmake_lists(filename: FileName, version: str):
    with open(filename, "r") as file:
        filedata = file.readlines()
    with open(filename, "w") as file:
        for l in filedata:
            if l.startswith("set(CALFENSTER_VERSION"):
                l = f'set(CALFENSTER_VERSION "{version}")\n'
            file.write(l)


def split_version(v: str) -> typing.Tuple[int, int, int]:
    if v.startswith("v"):
        v = v[1:]
    major, minor, patch, *_ = v.split(".")
    return int(major), int(minor), int(patch)


def update_semantic_version(
    version: typing.Tuple[int, int, int], release: str
) -> typing.Tuple[int, int, int]:
    semantic_inc = {"major": 0, "minor": 0, "patch": 0}
    semantic_inc[release] = 1
    return tuple(v + inc for v, inc in zip(version, semantic_inc.values()))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-r",
        "--release",
        default="minor",
        help="Type of Release.",
        type=str,
        choices=["major", "minor", "patch"],
    )
    args = parser.parse_args()

    current_tag = shell("git describe --abbrev=0 --tags")
    current_semantic_version = split_version(current_tag)
    release_semantic_version = update_semantic_version(
        current_semantic_version, args.release
    )
    release_tag = f"v{".".join(str(v) for v in release_semantic_version)}"

    # check if current commit has already a tag
    current_commit = shell("git rev-parse HEAD")
    has_tag = (
        len(shell(f"git describe --contains {current_commit} 2>/dev/null", False)) > 0
    )
    if has_tag:
        print("Current HEAD has a tag, skip release")
        sys.exit(1)

    # Update Debian package
    print("Run dch")
    dch_status = subprocess.call(f'dch -v {release_tag[1:]}-1 ""', shell=True)
    if dch_status != 0:
        print("dch failed")
        sys.exit(1)

    # Update CMakeLists
    print("Patch CMakeLists")
    script_dir = Path(__file__).resolve().parent.resolve()
    cmake_filename = script_dir / "../CMakeLists.txt"
    patch_cmake_lists(cmake_filename, release_tag[1:])

    # commit release changes
    print("Commit release")
    git_status = subprocess.call(
        f'git commit -a -m "Release {release_tag}"', shell=True
    )
    if git_status != 0:
        print("commit failed")
        sys.exit(1)

    print(f"Apply tag {release_tag}")
    shell(f"git tag {release_tag}")
    print('Run: "git push --tags" to finalize the release')


if __name__ == "__main__":
    main()
