from chess.engine import Engine
import os


def main():
    engine = Engine(os.path.abspath("../../engine/main"))
    print("started engine subprocess")
    engine.write("test!!")
    print(f"RCV: {engine.listen()}")


if __name__ == '__main__':
    main()
