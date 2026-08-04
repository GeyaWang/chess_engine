from enum import Enum, auto
from chess.coord import Coord


class PieceColour(Enum):
    WHITE = auto()
    BLACK = auto()
    NONE = auto()


class PieceType(Enum):
    KING = auto()
    QUEEN = auto()
    ROOK = auto()
    BISHOP = auto()
    KNIGHT = auto()
    PAWN = auto()
    EMPTY = auto()


class Piece:
    def __init__(self, clr: PieceColour, piece_type: PieceType, pos: tuple[str, int] | tuple[int, int]):
        self.colour = clr
        self.type = piece_type
        self._pos = Coord(*pos)
        self.is_dead = False

    def get_pos(self):
        return self._pos.x, self._pos.y

    def set_pos(self, pos: tuple[str, int] | tuple[int, int]):
        self._pos = Coord(*pos)

    def __repr__(self):
        return f"Piece(clr={self.colour}, type={self.type}, pos={self._pos})"
