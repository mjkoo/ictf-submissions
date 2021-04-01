import itertools
import os
import subprocess
import zlib

CC = os.getenv("CC", "musl-gcc")

subprocess.check_call([CC, "-Os", "-c", "decrypt.c"])
subprocess.check_call(["objcopy", "-Obinary", "--only-section=.text", "decrypt.o", "decrypt.bin"])

with open("decrypt.bin", "rb") as f:
    pt = f.read()

    # Align to multiple of 4
    target_len = len(pt) + 4 & ~0x3
    pt = pt.ljust(target_len, b"\x90")
    print(hex(zlib.crc32(pt)))

    # Decode step 2
    ct = bytes([p ^ k for p, k in zip(pt, itertools.cycle(b"ictf"[::-1]))])

    # Decode step 1
    ct = bytes([p ^ 0x42 for p in ct])


with open("decrypt.enc", "wb") as f:
    f.write(ct)

subprocess.check_call(["objcopy", "-Bi386", "-Ibinary", "-Oelf64-x86-64", "decrypt.enc", "decrypt_enc.o"])
