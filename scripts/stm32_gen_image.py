# Main application:
#   py scripts/stm32_gen_image.py --input build/app/stm32_bootloader.bin --app-id 1 --version v1.2.3
# Bootloader header for flashing with J-Link:
#   py scripts/stm32_gen_image.py --input build/boot/boot_stm32_bootloader.bin --app-id 0 --version v1.2.3 --header-only

import argparse
from pathlib import Path
import struct
import zlib


DEFAULT_INPUT = Path(__file__).resolve().parent.parent / "build" / "app" / "stm32_bootloader.bin"
HEADER_MAGIC = 0xDEADBEEF
DEFAULT_APP_VERSION = "v1.2.3"


def parse_args():
    parser = argparse.ArgumentParser(description="Add an STM32 bootloader header to a binary")
    parser.add_argument("--input", type=Path, default=DEFAULT_INPUT, help="input .bin path")
    parser.add_argument("--output", type=Path, help="output path")
    parser.add_argument("--app-id", type=int, choices=(0, 1), default=1, help="0 = bootloader, 1 = main app (default: 1)")
    parser.add_argument("--version", default=DEFAULT_APP_VERSION, help=f"application version (default: {DEFAULT_APP_VERSION})")
    parser.add_argument("--header-only", action="store_true", help="write only the header")
    return parser.parse_args()


args = parse_args()
app = args.input.read_bytes()
app_crc = zlib.crc32(app) & 0xFFFFFFFF
app_version = args.version.encode("ascii")

if len(app_version) >= 16:
    raise ValueError("APP_VERSION must be shorter than 16 ASCII bytes")

header = struct.pack(
    "<IIII16s",
    HEADER_MAGIC,
    args.app_id,
    len(app),
    app_crc,
    app_version,
)

suffix = "_header.bin" if args.header_only else "_image.bin"
output = args.output or args.input.with_name(args.input.stem + suffix)
output.write_bytes(header if args.header_only else header + app)

print(f"Input:   {args.input} ({len(app)} bytes)")
print(f"App ID:  {args.app_id}")
print(f"Version: {args.version}")
print(f"CRC32:   0x{app_crc:08X}")
print(f"Output:  {output} ({output.stat().st_size} bytes)")
