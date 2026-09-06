from chess.piece import Piece, PieceType, PieceColour


DEFAULT_BOARD = [
    ['BR', 'BN', 'BB', 'BQ', 'BK', 'BB', 'BN', 'BR'],
    ['BP', 'BP', 'BP', 'BP', 'BP', 'BP', 'BP', 'BP'],
    ['##', '##', '##', '##', '##', '##', '##', '##'],
    ['##', '##', '##', '##', '##', '##', '##', '##'],
    ['##', '##', '##', '##', '##', '##', '##', '##'],
    ['##', '##', '##', '##', '##', '##', '##', '##'],
    ['WP', 'WP', 'WP', 'WP', 'WP', 'WP', 'WP', 'WP'],
    ['WR', 'WN', 'WB', 'WQ', 'WK', 'WB', 'WN', 'WR'],
]

PIECE_DICT = {
    'BK': (PieceColour.BLACK, PieceType.KING),
    'BQ': (PieceColour.BLACK, PieceType.QUEEN),
    'BR': (PieceColour.BLACK, PieceType.ROOK),
    'BB': (PieceColour.BLACK, PieceType.BISHOP),
    'BN': (PieceColour.BLACK, PieceType.KNIGHT),
    'BP': (PieceColour.BLACK, PieceType.PAWN),
    'WK': (PieceColour.WHITE, PieceType.KING),
    'WQ': (PieceColour.WHITE, PieceType.QUEEN),
    'WR': (PieceColour.WHITE, PieceType.ROOK),
    'WB': (PieceColour.WHITE, PieceType.BISHOP),
    'WN': (PieceColour.WHITE, PieceType.KNIGHT),
    'WP': (PieceColour.WHITE, PieceType.PAWN),
    '##': (PieceColour.NONE, PieceType.EMPTY),
}


class Board:
    def __init__(self):
        self.pieces = []
        self._board_data = []
        for i in range(8):
            row = []
            for j, p in enumerate(DEFAULT_BOARD[i]):
                clr, typ = PIECE_DICT[p]
                if typ == PieceType.EMPTY:
                    row.append(None)
                else:
                    piece = Piece(clr, typ, (j, i))
                    row.append(piece)
                    self.pieces.append(piece)
            self._board_data.append(row)

        self._EMPTY = Piece(PieceColour.NONE, PieceType.EMPTY, (0, 0))

        self._prev_piece_pos1 = None
        self._prev_piece_pos2 = None

    def _set(self, piece: Piece|None, x: int, y: int):
        self._board_data[y][x] = piece

    def get(self, x: int, y: int) -> Piece:
        piece = self._board_data[y][x]
        return self._EMPTY if piece is None else piece

    def undo(self) -> None:
        if self._prev_piece_pos1 is None:
            return

        piece1, pos1 = self._prev_piece_pos1
        piece2, pos2 = self._prev_piece_pos2
        piece1.set_pos(pos1)

        if piece2 is not None:
            piece2.set_pos(pos2)
            piece2.is_dead = False
            if piece2 not in self.pieces:
                self.pieces.append(piece2)

        self._set(piece1, *pos1)
        self._set(piece2, *pos2)

        self._prev_piece_pos1 = None
        self._prev_piece_pos2 = None

    def kill_piece(self, piece: Piece):
        if piece not in self.pieces:
            return

        piece.kill()
        self.pieces.remove(piece)

    def make_move(self, pos1: tuple[int, int], pos2: tuple[int, int]) -> None:
        piece1 = self.get(*pos1)
        piece2 = self.get(*pos2)
        self.kill_piece(piece2)

        piece1.set_pos(pos2)
        self._set(piece1, *pos2)
        self._set(None, *pos1)

        self._prev_piece_pos1 = (piece1, pos1)
        self._prev_piece_pos2 = (piece2, pos2)

    def set_empty(self, pos):
        piece = self._board_data[pos[1]][pos[0]]
        self.kill_piece(piece)
        self._board_data[pos[1]][pos[0]] = None

    def __str__(self):
        reverse_dict = {
            (PieceColour.BLACK, PieceType.KING): "BK",
            (PieceColour.BLACK, PieceType.QUEEN): "BQ",
            (PieceColour.BLACK, PieceType.ROOK): "BR",
            (PieceColour.BLACK, PieceType.BISHOP): "BB",
            (PieceColour.BLACK, PieceType.KNIGHT): "BN",
            (PieceColour.BLACK, PieceType.PAWN): "BP",
            (PieceColour.WHITE, PieceType.KING): "WK",
            (PieceColour.WHITE, PieceType.QUEEN): "WQ",
            (PieceColour.WHITE, PieceType.ROOK): "WR",
            (PieceColour.WHITE, PieceType.BISHOP): "WB",
            (PieceColour.WHITE, PieceType.KNIGHT): "WN",
            (PieceColour.WHITE, PieceType.PAWN): "WP",
        }

        rows = []
        for row in self._board_data:
            rows.append(" ".join(
                "##" if piece is None
                else reverse_dict[(piece.colour, piece.type)]
                for piece in row
            ))
        return "\n".join(rows)
