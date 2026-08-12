from chess.board import Board
from enum import Enum, auto
from chess.piece import Piece, PieceType, PieceColour


def switch_turn(turn: PieceColour):
    if turn == PieceColour.WHITE:
        return PieceColour.BLACK
    return PieceColour.WHITE


class Game:
    def __init__(self):
        self._board = Board()
        self._turn = PieceColour.WHITE

    def get(self, x: int, y: int) -> Piece:
        return self._board.get(x, y)

    def _is_piece_at(self, clr: PieceColour, pos: tuple[int, int]) -> bool:
        return self.get(*pos).get_colour() == clr

    @staticmethod
    def _move_relative(piece: Piece, delta_x: int, delta_y: int) -> tuple[int, int]:
        pos = piece.get_pos()
        if piece.get_colour() == PieceColour.WHITE:
            return pos[0] + delta_x, pos[1] - delta_y
        else:
            return pos[0] + delta_x, pos[1] + delta_y

    def _get_possible_moves_list(self, piece: Piece) -> list[tuple[int, int]]:
        moves = []
        pos = piece.get_pos()

        directional_pos = pos if piece.get_colour() == PieceColour.BLACK else (pos[0], 7 - pos[1])
        opponent_clr = PieceColour.WHITE if piece.get_colour() == PieceColour.BLACK else PieceColour.BLACK
        ally_clr = PieceColour.WHITE if piece.get_colour() == PieceColour.WHITE else PieceColour.BLACK


        def rel_pos(dx: int, dy: int) -> tuple[int, int]:
            if piece.get_colour() == PieceColour.WHITE:
                return piece.get_pos()[0] + dx, piece.get_pos()[1] - dy
            else:
                return piece.get_pos()[0] + dx, piece.get_pos()[1] + dy

        def is_oob(p: tuple[int, int]):
            return not (0 <= p[0] <= 7 and 0 <= p[1] <= 7)

        def add_pos(p: tuple[int, int]):
            if not is_oob(p):
                moves.append(p)


        match piece.get_type():
            case PieceType.PAWN:
                # Forward 1
                if not self._is_piece_at(opponent_clr, rel_pos(0, 1)):
                    add_pos(rel_pos(0, 1))
                # Forward 2
                if directional_pos[1] == 1 and not self._is_piece_at(opponent_clr, rel_pos(0, 1)):
                    add_pos(rel_pos(0, 2))
                # Take diagonal
                if not is_oob(rel_pos(1, 1)) and self._is_piece_at(opponent_clr, rel_pos(1, 1)):
                    add_pos(rel_pos(1, 1))
                if not is_oob(rel_pos(-1, 1)) and self._is_piece_at(opponent_clr, rel_pos(-1, 1)):
                    add_pos(rel_pos(-1, 1))
            case PieceType.KNIGHT:
                add_pos(rel_pos(1, -2))
                add_pos(rel_pos(1, 2))
                add_pos(rel_pos(2, 1))
                add_pos(rel_pos(2, -1))
                add_pos(rel_pos(-1, -2))
                add_pos(rel_pos(-1, 2))
                add_pos(rel_pos(-2, 1))
                add_pos(rel_pos(-2, -1))
            case PieceType.BISHOP:
                for i in range(1, 8, 1):
                    x = rel_pos(i, i)
                    if is_oob(x) : break
                    if self._is_piece_at(ally_clr, x): break
                    add_pos(x)
                    if self._is_piece_at(opponent_clr, x): break
                for i in range(1, 8, 1):
                    x = rel_pos(-i, i)
                    if is_oob(x) : break
                    if self._is_piece_at(ally_clr, x): break
                    add_pos(x)
                    if self._is_piece_at(opponent_clr, x): break
                for i in range(1, 8, 1):
                    x = rel_pos(i, -i)
                    if is_oob(x) : break
                    if self._is_piece_at(ally_clr, x): break
                    add_pos(x)
                    if self._is_piece_at(opponent_clr, x): break
                for i in range(1, 8, 1):
                    x = rel_pos(-i, -i)
                    if is_oob(x) : break
                    if self._is_piece_at(ally_clr, x): break
                    add_pos(x)
                    if self._is_piece_at(opponent_clr, x): break
            case PieceType.ROOK:
                for i in range(1, 8, 1):
                    x = rel_pos(i, 0)
                    if is_oob(x) : break
                    if self._is_piece_at(ally_clr, x): break
                    add_pos(x)
                    if self._is_piece_at(opponent_clr, x): break
                for i in range(1, 8, 1):
                    x = rel_pos(-i, 0)
                    if is_oob(x) : break
                    if self._is_piece_at(ally_clr, x): break
                    add_pos(x)
                    if self._is_piece_at(opponent_clr, x): break
                for i in range(1, 8, 1):
                    x = rel_pos(0, i)
                    if is_oob(x) : break
                    if self._is_piece_at(ally_clr, x): break
                    add_pos(x)
                    if self._is_piece_at(opponent_clr, x): break
                for i in range(1, 8, 1):
                    x = rel_pos(0, -i)
                    if is_oob(x) : break
                    if self._is_piece_at(ally_clr, x): break
                    add_pos(x)
                    if self._is_piece_at(opponent_clr, x): break
            case PieceType.QUEEN:
                for i in range(1, 8, 1):
                    x = rel_pos(i, i)
                    if is_oob(x) : break
                    if self._is_piece_at(ally_clr, x): break
                    add_pos(x)
                    if self._is_piece_at(opponent_clr, x): break
                for i in range(1, 8, 1):
                    x = rel_pos(-i, i)
                    if is_oob(x) : break
                    if self._is_piece_at(ally_clr, x): break
                    add_pos(x)
                    if self._is_piece_at(opponent_clr, x): break
                for i in range(1, 8, 1):
                    x = rel_pos(i, -i)
                    if is_oob(x) : break
                    if self._is_piece_at(ally_clr, x): break
                    add_pos(x)
                    if self._is_piece_at(opponent_clr, x): break
                for i in range(1, 8, 1):
                    x = rel_pos(-i, -i)
                    if is_oob(x) : break
                    if self._is_piece_at(ally_clr, x): break
                    add_pos(x)
                    if self._is_piece_at(opponent_clr, x): break
                for i in range(1, 8, 1):
                    x = rel_pos(i, 0)
                    if is_oob(x) : break
                    if self._is_piece_at(ally_clr, x): break
                    add_pos(x)
                    if self._is_piece_at(opponent_clr, x): break
                for i in range(1, 8, 1):
                    x = rel_pos(-i, 0)
                    if is_oob(x) : break
                    if self._is_piece_at(ally_clr, x): break
                    add_pos(x)
                    if self._is_piece_at(opponent_clr, x): break
                for i in range(1, 8, 1):
                    x = rel_pos(0, i)
                    if is_oob(x) : break
                    if self._is_piece_at(ally_clr, x): break
                    add_pos(x)
                    if self._is_piece_at(opponent_clr, x): break
                for i in range(1, 8, 1):
                    x = rel_pos(0, -i)
                    if is_oob(x) : break
                    if self._is_piece_at(ally_clr, x): break
                    add_pos(x)
                    if self._is_piece_at(opponent_clr, x): break
            case PieceType.KING:
                add_pos(rel_pos(0, 1))
                add_pos(rel_pos(1, 1))
                add_pos(rel_pos(1, 0))
                add_pos(rel_pos(1, -1))
                add_pos(rel_pos(0, -1))
                add_pos(rel_pos(-1, -1))
                add_pos(rel_pos(-1, 0))
                add_pos(rel_pos(-1, 1))

        return moves

    def try_move(self, pos1: tuple[int, int], pos2: tuple[int, int]):
        if pos1 == pos2:
            return

        piece1 = self.get(*pos1)
        piece2 = self.get(*pos2)

        # if piece1.get_colour() != self._turn:
        #     return
        if piece1.get_colour() == piece2.get_colour():
            return

        move_list = self._get_possible_moves_list(piece1)
        if pos2 not in move_list:
            return

        # Make move
        self._board.make_move(pos1, pos2)
        self._turn = switch_turn(self._turn)
