# Main application:
#   py scripts/stm32_updater.py --port COM5 --app-id 1 --image build/app/stm32_bootloader_image.bin
# Bootloader:
#   py scripts/stm32_updater.py --port COM5 --app-id 0 --image build/boot/boot_stm32_bootloader_image.bin

import argparse
from pathlib import Path
import struct
import sys
import threading

import serial


DEFAULT_IMAGE = Path(__file__).resolve().parent.parent / "build" / "app" / "stm32_bootloader_image.bin"
BAUD_RATE = 115200
HEADER_SIZE = 32
READY_TIMEOUT_SECONDS = 10.0


def parse_args():
    parser = argparse.ArgumentParser(description="Send an STM32 update image over UART")
    parser.add_argument("--port", default="COM5", help="COM port name or number (default: COM5)")
    parser.add_argument("--app-id", type=int, choices=(0, 1), default=1, help="0 = bootloader, 1 = main app (default: 1)")
    parser.add_argument("--image", type=Path, default=DEFAULT_IMAGE, help="update image path")
    return parser.parse_args()


def print_uart_logs(uart, stop_event, ready_event):
    received = bytearray()

    while not stop_event.is_set():
        data = uart.read(uart.in_waiting or 1)

        if data:
            sys.stdout.write(data.decode(errors="replace"))
            sys.stdout.flush()

            received.extend(data)
            if b"READY" in received:
                ready_event.set()

            if len(received) > 64:
                del received[:-64]


args = parse_args()
com_port = args.port if args.port.upper().startswith("COM") else f"COM{args.port}"
data = bytearray(args.image.read_bytes())

if len(data) < HEADER_SIZE:
    raise ValueError("Image is smaller than its 32-byte header")

struct.pack_into("<I", data, 4, args.app_id)

stop_event = threading.Event()
ready_event = threading.Event()

print(f"Sending {len(data)} bytes from {args.image} through {com_port} (app ID: {args.app_id})...")

with serial.Serial(com_port, BAUD_RATE, timeout=0.1) as uart:
    log_thread = threading.Thread(
        target=print_uart_logs,
        args=(uart, stop_event, ready_event),
        daemon=True,
    )
    log_thread.start()

    try:
        uart.write(data[:HEADER_SIZE])
        uart.flush()

        if not ready_event.wait(READY_TIMEOUT_SECONDS):
            raise TimeoutError("Bootloader did not send READY")

        uart.write(data[HEADER_SIZE:])
        uart.flush()

        print("\nFinished sending. Listening for logs; press Ctrl+C to stop.")

        while True:
            stop_event.wait(1)
    except KeyboardInterrupt:
        pass
    finally:
        stop_event.set()
        log_thread.join()

print("Stopped")
