import subprocess
import os


class Engine:
    def __init__(self, filepath: str):
        try:
            self._process = subprocess.Popen(
                [filepath],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                bufsize=1,
            )
        except PermissionError:
            raise ChildProcessError(f"Bad filepath {filepath}")

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.terminate()

    def _is_terminated(self) -> bool:
        return self._process.poll() is not None

    def terminate(self) -> None:
        if self._is_terminated():
            return

        # Attempt to exit gracefully
        self._process.terminate()
        try:
            self._process.wait(timeout=3)
        except subprocess.TimeoutExpired:
            self._process.kill()
            self._process.wait()

        # Close streams
        self._process.stdin.close()
        self._process.stdout.close()
        self._process.stderr.close()

    def write(self, msg: str) -> None:
        if self._is_terminated():
            raise ChildProcessError("Cannot write to engine. Engine process is terminated")
        self._process.stdin.write(msg + "\n")
        self._process.stdin.flush()

    def listen(self) -> str:
        if self._is_terminated():
            raise ChildProcessError("Cannot listen to engine. Engine process is terminated")
        msg = self._process.stdout.readline()
        return msg
