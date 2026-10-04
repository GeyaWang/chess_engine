import subprocess
import queue
import threading


class Engine:
    def __init__(self, filepath: str, verbose: bool = True):
        self.is_verbose = verbose

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
            raise ChildProcessError(f"Invalid filepath {filepath}")

        self._stderr_queue = queue.Queue()
        threading.Thread(target=self._listen_stderr, daemon=True).start()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.terminate()

    def _listen_stderr(self):
        for line in self._process.stderr:
            self._stderr_queue.put(line.rstrip("\n"))

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

        if self.is_verbose:
            print(f"Sent: {msg}")

    def listen(self) -> str:
        if self._is_terminated():
            raise ChildProcessError("Cannot listen to engine. Engine process is terminated")
        msg = self._process.stdout.readline().rstrip("\n")

        if self.is_verbose:
            print(f"Received: {msg}")

        return msg

    def listen_stderr(self) -> list[str]:
        lines = []
        while True:
            try:
                lines.append(self._stderr_queue.get_nowait())
            except queue.Empty:
                return lines
