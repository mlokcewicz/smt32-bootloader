from pathlib import Path
import sys
import threading

import serial


COM_PORT = "COM5"
BAUD_RATE = 115200
BIN_FILE = Path(__file__).resolve().parent.parent / "build" / "app" / "stm32_bootloader_image.bin"
HEADER_SIZE = 32
READY_TIMEOUT_SECONDS = 10.0


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


data = BIN_FILE.read_bytes()
stop_event = threading.Event()
ready_event = threading.Event()

print(f"Sending {len(data)} bytes from {BIN_FILE} through {COM_PORT}...")

with serial.Serial(COM_PORT, BAUD_RATE, timeout=0.1) as uart:
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
