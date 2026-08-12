from chess.coord import Coord
from chess.piece import Piece, PieceType


def main():
    p = Piece(PieceType.PAWN, 0, 0)
    print(p.piece_type)
    print(p.pos)


if __name__ == '__main__':
    main()
