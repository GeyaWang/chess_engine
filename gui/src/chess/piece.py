from enum import Enum, auto


class PieceColour(Enum):
    WHITE = auto()
    BLACK = auto()
    NONE = auto()

    def __invert__(self):
        if self is PieceColour.WHITE:
            return PieceColour.BLACK
        elif self is PieceColour.BLACK:
            return PieceColour.WHITE
        return PieceColour.NONE


class PieceType(Enum):
    KING = auto()
    QUEEN = auto()
    ROOK = auto()
    BISHOP = auto()
    KNIGHT = auto()
    PAWN = auto()
    EMPTY = auto()


class Piece:
    def __init__(self, clr: PieceColour, piece_type: PieceType, pos: tuple[int, int]):
        if not (0 <= pos[0] <= 7 and 0 <= pos[1] <= 7):
            raise ValueError("Invalid position given")

        self._colour = clr
        self._type = piece_type
        self._pos = pos
        self.is_dead = False

    def get_colour(self) -> PieceColour:
        return self._colour

    def get_type(self) -> PieceType:
        return self._type

    def get_pos(self) -> tuple[int, int]:
        return self._pos

    def set_pos(self, pos: tuple[int, int]) -> None:
        if not (0 <= pos[0] <= 7 and 0 <= pos[1] <= 7):
            raise ValueError("Invalid position given")
        self._pos = pos

    def kill(self):
        self.is_dead = True

    def __repr__(self):
        return f"Piece(clr={self._colour}, type={self._type}, pos={self._pos})"
