from chess.gui import Gui
from chess.engine import Engine
import os
import argparse


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-fp",
        "--filepath",
        required=True,
        help="filepath to chess engine",
    )
    args = parser.parse_args()

    with Engine(os.path.abspath(args.filepath)) as engine:
        gui = Gui(engine)
        gui.run()
