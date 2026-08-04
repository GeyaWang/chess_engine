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
        self._board_data = []
        for i in range(8):
            row = []
            for j, p in enumerate(DEFAULT_BOARD[i]):
                clr, typ = PIECE_DICT[p]
                if typ == PieceType.EMPTY:
                    row.append(None)
                else:
                    row.append(Piece(clr, typ, (j, i)))
            self._board_data.append(row)

    def get(self, x: int, y: int):
        return self._board_data[y][x]

    def make_move(self, pos1: tuple[int, int], pos2: tuple[int, int]):
        if pos1 == pos2:
            return

        piece1 = self._board_data[pos1[1]][pos1[0]]
        piece2 = self._board_data[pos2[1]][pos2[0]]
        if piece1 is None:
            return

        if piece2 is not None:
            piece2.is_dead = True
        piece1.set_pos(pos2)
        self._board_data[pos2[1]][pos2[0]] = piece1
        self._board_data[pos1[1]][pos1[0]] = None

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
