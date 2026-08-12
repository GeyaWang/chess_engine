from chess.coord import Coord
from chess.utils import coord_to_algebraic, algebraic_to_coord


def main():
    print(algebraic_to_coord('e4'))
    print(coord_to_algebraic((3, 7)))
    c = Coord('h4')
    print(c)


if __name__ == '__main__':
    main()
