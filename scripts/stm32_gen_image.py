from pathlib import Path
import struct
import zlib


INPUT_BIN = Path(__file__).resolve().parent.parent / "build" / "app" / "stm32_bootloader.bin"
OUTPUT_BIN = Path(__file__).resolve().parent.parent / "build" / "app" / "stm32_bootloader_image.bin"
APP_TYPE = 1  # APP_LOADER_APP_TYPE_MAIN
HEADER_MAGIC = 0xDEADBEEF


app = INPUT_BIN.read_bytes()
app_crc = zlib.crc32(app) & 0xFFFFFFFF
header = struct.pack("<IIII", HEADER_MAGIC, APP_TYPE, len(app), app_crc)

OUTPUT_BIN.write_bytes(header + app)

print(f"Input:  {INPUT_BIN} ({len(app)} bytes)")
print(f"CRC32:  0x{app_crc:08X}")
print(f"Output: {OUTPUT_BIN} ({len(header) + len(app)} bytes)")
