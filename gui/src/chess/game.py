from chess.board import Board
from chess.piece import Piece, PieceColour, PieceType


_BISHOP_DIRECTIONS = [(1, 1), (1, -1), (-1, 1), (-1, -1)]
_ROOK_DIRECTIONS = [(0, 1), (0, -1), (1, 0), (-1, 0)]
_KNIGHT_DIRECTIONS = [(1, -2), (1, 2), (2, 1), (2, -1), (-1, -2), (-1, 2), (-2, 1), (-2, -1)]
_KING_DIRECTIONS = [(0, 1), (1, 1), (1, 0), (1, -1), (0, -1), (-1, -1), (-1, 0), (-1, 1)]

SLIDING_DIRECTIONS = {
    PieceType.BISHOP: _BISHOP_DIRECTIONS,
    PieceType.ROOK: _ROOK_DIRECTIONS,
    PieceType.QUEEN: _BISHOP_DIRECTIONS + _ROOK_DIRECTIONS,
}

STEPPING_DIRECTIONS = {
    PieceType.KNIGHT: _KNIGHT_DIRECTIONS,
    PieceType.KING: _KING_DIRECTIONS,
}


class Game:
    def __init__(self):
        self._board = Board()
        self.turn = PieceColour.WHITE

        # Find kings
        self._white_king = None
        self._black_king = None
        for piece in self._board.pieces:
            if piece.get_type() == PieceType.KING:
                if piece.get_colour() == PieceColour.WHITE:
                    self._white_king = piece
                else:
                    self._black_king = piece
        assert self._white_king is not None and self._black_king is not None

        self._white_en_passent_target = None
        self._black_en_passent_target = None

        self._is_white_able_queenside_castle = True
        self._is_white_able_kingside_castle = True
        self._is_black_able_queenside_castle = True
        self._is_black_able_kingside_castle = True

        self.white_win = False
        self.black_win = False

        self.prev_move = (None, None)

    def _is_piece_at(self, pos: tuple[int, int], clr: PieceColour=None):
        if clr is None:
            return self._board.get(*pos).get_type() != PieceType.EMPTY
        return self._board.get(*pos).get_colour() == clr

    @staticmethod
    def _is_in_bounds(pos: tuple[int, int]):
        return 0 <= pos[0] <= 7 and 0 <= pos[1] <= 7

    def _stepping_attacks(self, piece: Piece) -> list[tuple[int, int]]:
        moves = []
        x, y = piece.get_pos()
        for (dx, dy) in STEPPING_DIRECTIONS[piece.get_type()]:
            p = (x + dx, y + dy)
            if self._is_in_bounds(p):
                moves.append(p)
        return moves

    def _stepping_moves(self, piece: Piece) -> list[tuple[int, int]]:
        moves = []
        x, y = piece.get_pos()
        for (dx, dy) in STEPPING_DIRECTIONS[piece.get_type()]:
            p = (x + dx, y + dy)
            if self._is_in_bounds(p) and not self._is_piece_at(p, piece.get_colour()):
                moves.append(p)
        return moves

    def _sliding_attacks(self, piece: Piece) -> list[tuple[int, int]]:
        moves = []
        x, y = piece.get_pos()
        for (dx, dy) in SLIDING_DIRECTIONS[piece.get_type()]:
            p = (x + dx, y + dy)
            while self._is_in_bounds(p):
                moves.append(p)
                if self._is_piece_at(p):
                    break
                p = (p[0] + dx, p[1] + dy)
        return moves

    def _sliding_moves(self, piece: Piece) -> list[tuple[int, int]]:
        moves = []
        x, y = piece.get_pos()
        for (dx, dy) in SLIDING_DIRECTIONS[piece.get_type()]:
            p = (x + dx, y + dy)
            while self._is_in_bounds(p) and not self._is_piece_at(p, piece.get_colour()):
                moves.append(p)
                if self._is_piece_at(p):
                    break
                p = (p[0] + dx, p[1] + dy)
        return moves

    def _pawn_attacks(self, piece: Piece) -> list[tuple[int, int]]:
        moves = []
        x, y = piece.get_pos()
        rel_dy = -1 if piece.get_colour() == PieceColour.WHITE else 1

        # Diagonal captures
        for p in [(x + 1, y + rel_dy), (x - 1, y + rel_dy)]:
            if self._is_in_bounds(p) and not self._is_piece_at(p, piece.get_colour()):
                moves.append(p)
        return moves

    def _pawn_moves(self, piece: Piece) -> list[tuple[int, int]]:
        moves = []
        x, y = piece.get_pos()
        rel_dy = -1 if piece.get_colour() == PieceColour.WHITE else 1

        # Step one square
        p = (x, y + rel_dy)
        if self._is_in_bounds(p) and not self._is_piece_at(p):
            moves.append(p)

            # Double jump
            p = (x, y + 2 * rel_dy)
            is_at_start = True if (piece.get_colour() == PieceColour.WHITE and y == 6) or (piece.get_colour() == PieceColour.BLACK and y == 1) else False
            if is_at_start and self._is_in_bounds(p) and not self._is_piece_at(p):
                moves.append(p)

        # Diagonal captures
        for p in [(x + 1, y + rel_dy), (x - 1, y + rel_dy)]:
            if self._is_in_bounds(p) and self._is_piece_at(p, ~piece.get_colour()):
                moves.append(p)
        return moves

    def _is_en_passent(self, piece: Piece, pos: tuple[int, int]) -> bool:
        rel_dy = -1 if piece.get_colour() == PieceColour.WHITE else 1
        x, y = piece.get_pos()
        en_passent_target = self._black_en_passent_target if piece.get_colour() == PieceColour.WHITE else self._white_en_passent_target
        if pos in [(x + 1, y + rel_dy), (x - 1, y + rel_dy)] and self._is_in_bounds(pos) and pos == en_passent_target:
            return True
        return False

    def _is_castle(self, piece: Piece, pos: tuple[int, int]) -> bool:
        if piece.get_type() != PieceType.KING:
            return False

        clr = piece.get_colour()

        is_able_kingside_castle = self._is_black_able_kingside_castle if clr == PieceColour.BLACK else self._is_white_able_kingside_castle
        is_able_queenside_castle = self._is_black_able_queenside_castle if clr == PieceColour.BLACK else self._is_white_able_queenside_castle

        y = 7 if clr == PieceColour.WHITE else 0
        if pos == (2, y) and is_able_queenside_castle:
            if not self._is_piece_at((1, y)) and not self._is_piece_at((2, y)) and not self._is_piece_at((3, y)) \
                    and not self._is_attacked(clr, (4, y)) and not self._is_attacked(clr, (3, y)) and not self._is_attacked(clr, (2, y)):
                return True
            return False
        elif pos == (6, y) and is_able_kingside_castle:
            if not self._is_piece_at((5, y)) and not self._is_piece_at((6, y)) \
                    and not self._is_attacked(clr, (4, y)) and not self._is_attacked(clr, (5, y)) and not self._is_attacked(clr, (6, y)):
                return True
            return False
        else:
            return False

    @staticmethod
    def _is_pawn_double(piece: Piece, pos: tuple[int, int]) -> bool:
        if piece.get_type() != PieceType.PAWN:
            return False

        x, y = piece.get_pos()
        rel_dy = -1 if piece.get_colour() == PieceColour.WHITE else 1
        is_at_start = True if (piece.get_colour() == PieceColour.WHITE and y == 6) or (piece.get_colour() == PieceColour.BLACK and y == 1) else False
        if is_at_start and pos == (x, y + 2 * rel_dy):
            return True

        return False

    def _is_attacks(self, piece: Piece, pos: tuple[int, int]) -> bool:
        moves = self._gen_attacks(piece)
        return pos in moves

    def _is_attacked(self, clr: PieceColour, pos: tuple[int, int]) -> bool:
        for p in self._board.pieces:
            if p.get_colour() == clr:
                continue
            if self._is_attacks(p, pos):
                return True
        return False

    def _gen_attacks(self, piece: Piece) -> list[tuple[int, int]]:
        piece_type = piece.get_type()
        if piece_type in STEPPING_DIRECTIONS:
            return self._stepping_attacks(piece)
        elif piece_type in SLIDING_DIRECTIONS:
            return self._sliding_attacks(piece)
        else:  # piece_type == PieceType.PAWN
            return self._pawn_attacks(piece)

    def _gen_moves(self, piece: Piece) -> list[tuple[int, int]]:
        piece_type = piece.get_type()
        if piece_type in STEPPING_DIRECTIONS:
            return self._stepping_moves(piece)
        elif piece_type in SLIDING_DIRECTIONS:
            return self._sliding_moves(piece)
        else:  # piece_type == PieceType.PAWN
            return self._pawn_moves(piece)

    def _is_checkmate(self, clr: PieceColour) -> bool:
        king = self._white_king if clr == PieceColour.WHITE else self._black_king

        king_moves = self._gen_moves(king)
        if not king_moves:
            return False
        for pos in self._gen_moves(king):
            if not self._is_attacked(clr, pos):
                return False

        return True

    def get(self, x: int, y: int) -> Piece:
        return self._board.get(x, y)

    def make_move(self, pos1: tuple[int, int], pos2: tuple[int, int]):
        self._board.make_move(pos1, pos2)
        self.turn = ~self.turn
        self.prev_move = (pos1, pos2)

    def try_move(self, pos1: tuple[int, int], pos2: tuple[int, int]) -> bool:
        if pos1 == pos2:
            return False

        if self.white_win or self.black_win:
            return False

        piece = self.get(*pos1)
        clr = piece.get_colour()
        typ = piece.get_type()
        piece2 = self.get(*pos2)

        if clr != self.turn:
            return False

        is_pawn_double_jump = self._is_pawn_double(piece, pos2)

        if self._is_castle(piece, pos2):
            self._board.make_move(pos1, pos2)
            y = pos2[1]
            if pos2[0] == 2:
                self._board.make_move((0, y), (3, y))
            else:
                self._board.make_move((7, y), (5, y))
        elif self._is_en_passent(piece, pos2):
            # Do en passent
            self._board.make_move(pos1, pos2)
            pawn_pos = (pos2[0], pos2[1]-1) if piece.get_colour() == PieceColour.BLACK else (pos2[0], pos2[1]+1)
            self._board.set_empty(pawn_pos)
        else:
            # Make move if in move list
            move_list = self._gen_moves(piece) if piece2.get_type() == PieceType.EMPTY else self._gen_attacks(piece)
            if pos2 not in move_list:
                return False
            self._board.make_move(pos1, pos2)

        # If king attacked, undo move
        king = self._white_king if clr == PieceColour.WHITE else self._black_king
        if self._is_attacked(clr, king.get_pos()):
            self._board.undo()
            return False

        ## Move is legal
        self.turn = ~self.turn
        self.prev_move = (pos1, pos2)

        # Set en passent target
        if is_pawn_double_jump:
            if clr == PieceColour.WHITE:
                self._white_en_passent_target = (pos1[0], pos1[1]-1)
            else:
                self._black_en_passent_target = (pos1[0], pos1[1]+1)
        else:
            if clr == PieceColour.WHITE:
                self._white_en_passent_target = None
            else:
                self._black_en_passent_target = None

        # Set castling permission
        if typ == PieceType.KING:
            if clr == PieceColour.WHITE:
                self._is_white_able_kingside_castle = False
                self._is_white_able_queenside_castle = False
            else:
                self._is_black_able_kingside_castle = False
                self._is_black_able_queenside_castle = False
        if typ == PieceType.ROOK:
            if clr == PieceColour.WHITE:
                if self._is_white_able_queenside_castle and pos1[0] == 0:
                    self._is_white_able_queenside_castle = False
                elif self._is_white_able_kingside_castle and pos1[0] == 7:
                    self._is_white_able_kingside_castle = False
            else:
                if self._is_black_able_queenside_castle and pos1[0] == 0:
                    self._is_black_able_queenside_castle = False
                elif self._is_black_able_kingside_castle and pos1[0] == 7:
                    self._is_black_able_kingside_castle = False

        # Check for checkmate
        if self._is_checkmate(~clr):
            if clr == PieceColour.WHITE:
                self.black_win = True
            else:
                self.white_win = True

        return True

    def is_game_over(self) -> bool:
        return self.white_win or self.black_win
