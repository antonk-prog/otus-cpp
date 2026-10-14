#!/usr/bin/env python3
"""End-to-end tests for bulk_server.

Usage: test_server.py <path-to-bulk_server-binary>
"""

import os
import signal
import socket
import subprocess
import sys
import tempfile
import time


def free_port():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind(("127.0.0.1", 0))
    port = sock.getsockname()[1]
    sock.close()
    return port


def wait_for_port(port, proc, timeout=5.0):
    deadline = time.time() + timeout
    while time.time() < deadline:
        if proc.poll() is not None:
            raise RuntimeError("server exited before accepting connections")
        try:
            with socket.create_connection(("127.0.0.1", port), timeout=0.2):
                return
        except OSError:
            time.sleep(0.05)
    raise RuntimeError("server did not start listening in time")


def start_server(binary, block_size):
    port = free_port()
    workdir = tempfile.mkdtemp(prefix="bulk_test_")
    proc = subprocess.Popen(
        [binary, str(port), str(block_size)],
        cwd=workdir,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
    )
    wait_for_port(port, proc)
    return proc, port


def stop_server(proc):
    if proc.poll() is None:
        proc.send_signal(signal.SIGINT)
    try:
        out, _ = proc.communicate(timeout=5)
    except subprocess.TimeoutExpired:
        proc.kill()
        out, _ = proc.communicate()
    return out


def bulk_lines(output):
    return [line.strip() for line in output.splitlines() if line.startswith("bulk:")]


def test_static_batches_and_disconnect_flush(binary):
    proc, port = start_server(binary, 2)
    client = socket.create_connection(("127.0.0.1", port), timeout=5)
    client.sendall(b"one\ntwo\nthree\n")
    time.sleep(0.2)
    client.close()
    time.sleep(0.2)
    lines = bulk_lines(stop_server(proc))
    assert "bulk: one, two" in lines, lines
    assert "bulk: three" in lines, lines


def test_dynamic_block(binary):
    proc, port = start_server(binary, 10)
    client = socket.create_connection(("127.0.0.1", port), timeout=5)
    client.sendall(b"alpha\n{\nA\nB\n}\nomega\n")
    time.sleep(0.2)
    client.close()
    time.sleep(0.2)
    lines = bulk_lines(stop_server(proc))
    assert "bulk: alpha" in lines, lines
    assert "bulk: A, B" in lines, lines
    assert "bulk: omega" in lines, lines


def test_shutdown_flushes_pending_static_block(binary):
    proc, port = start_server(binary, 10)
    client = socket.create_connection(("127.0.0.1", port), timeout=5)
    client.sendall(b"persist\n")
    time.sleep(0.2)
    # Stop the server while the connection is still open: the incomplete
    # static block must still be emitted.
    lines = bulk_lines(stop_server(proc))
    client.close()
    assert "bulk: persist" in lines, lines


def test_rejects_invalid_block_size(binary):
    for bad in ("-5", "+5", "abc", "0", ""):
        result = subprocess.run(
            [binary, str(free_port()), bad],
            capture_output=True,
            text=True,
            timeout=10,
        )
        assert result.returncode != 0, f"accepted block size {bad!r}"


def test_rejects_invalid_port(binary):
    for bad in ("-1", "70000", "abc", "0"):
        result = subprocess.run(
            [binary, bad, "10"],
            capture_output=True,
            text=True,
            timeout=10,
        )
        assert result.returncode != 0, f"accepted port {bad!r}"


def main():
    if len(sys.argv) != 2:
        print("usage: test_server.py <bulk_server binary>", file=sys.stderr)
        return 2
    binary = os.path.abspath(sys.argv[1])
    tests = [
        test_static_batches_and_disconnect_flush,
        test_dynamic_block,
        test_shutdown_flushes_pending_static_block,
        test_rejects_invalid_block_size,
        test_rejects_invalid_port,
    ]
    failed = 0
    for test in tests:
        try:
            test(binary)
            print(f"PASS {test.__name__}")
        except Exception as error:  # noqa: BLE001
            failed += 1
            print(f"FAIL {test.__name__}: {error}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
