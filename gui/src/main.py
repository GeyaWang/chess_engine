import os
import argparse
from engine import Engine
from gui import EngineGui, Gui


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-fp",
        "--filepath",
        required=False,
        help="filepath to chess engine",
    )
    args = parser.parse_args()

    if args.filepath is None:
        gui = Gui()
        gui.run()
    else:
        with Engine(os.path.abspath(args.filepath)) as engine:
            gui = EngineGui(engine)
            gui.run()


if __name__ == "__main__":
    main()
