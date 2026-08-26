from pathlib import Path
import struct
import zlib


INPUT_BIN = Path(__file__).resolve().parent.parent / "build" / "app" / "stm32_bootloader.bin"
OUTPUT_BIN = Path(__file__).resolve().parent.parent / "build" / "app" / "stm32_bootloader_image.bin"
APP_TYPE = 1  # APP_LOADER_APP_TYPE_MAIN
HEADER_MAGIC = 0xDEADBEEF
APP_VERSION = "v1.2.3"


app = INPUT_BIN.read_bytes()
app_crc = zlib.crc32(app) & 0xFFFFFFFF
app_version = APP_VERSION.encode("ascii")

if len(app_version) >= 16:
    raise ValueError("APP_VERSION must be shorter than 16 ASCII bytes")

header = struct.pack(
    "<IIII16s",
    HEADER_MAGIC,
    APP_TYPE,
    len(app),
    app_crc,
    app_version,
)

OUTPUT_BIN.write_bytes(header + app)

print(f"Input:  {INPUT_BIN} ({len(app)} bytes)")
print(f"Version: {APP_VERSION}")
print(f"CRC32:  0x{app_crc:08X}")
print(f"Output: {OUTPUT_BIN} ({len(header) + len(app)} bytes)")
