import os
import argparse
from engine import Engine
from gui import Gui


def main():
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



if __name__ == "__main__":
    main()
