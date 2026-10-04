from typing import Optional
from dataclasses import dataclass
import pygame
import pathlib
import chess
import sys
from settings import SQUARE_HEIGHT, SQUARE_WIDTH, FPS, DARK_CLR, LIGHT_CLR, MOVE_CLR, PLAYER_COLOUR, CHECKMATE_CLR
from engine import Engine


BASE_DIR = pathlib.Path(__file__).resolve().parent.parent
ASSETS_DIR = BASE_DIR / "assets"


@dataclass
class HeldPiece:
    rank: int
    file: int
    piece: chess.Piece


class Gui:
    pygame.init()
    screen = pygame.display.set_mode((SQUARE_WIDTH * 8, SQUARE_HEIGHT * 8))
    pygame.display.set_caption("Chess")

    @staticmethod
    def load_piece_img(file: str):
        return pygame.transform.smoothscale(pygame.image.load(str(ASSETS_DIR / file)).convert_alpha(), (SQUARE_WIDTH, SQUARE_HEIGHT))

    ASSET_DICT = {
        (chess.WHITE, chess.PAWN): load_piece_img("wp.png"),
        (chess.WHITE, chess.KNIGHT): load_piece_img("wn.png"),
        (chess.WHITE, chess.BISHOP): load_piece_img("wb.png"),
        (chess.WHITE, chess.ROOK): load_piece_img("wr.png"),
        (chess.WHITE, chess.QUEEN): load_piece_img("wq.png"),
        (chess.WHITE, chess.KING): load_piece_img("wk.png"),
        (chess.BLACK, chess.PAWN): load_piece_img("bp.png"),
        (chess.BLACK, chess.KNIGHT): load_piece_img("bn.png"),
        (chess.BLACK, chess.BISHOP): load_piece_img("bb.png"),
        (chess.BLACK, chess.ROOK): load_piece_img("br.png"),
        (chess.BLACK, chess.QUEEN): load_piece_img("bq.png"),
        (chess.BLACK, chess.KING): load_piece_img("bk.png"),
    }

    DARK_SQUARE = pygame.Surface((SQUARE_WIDTH, SQUARE_HEIGHT), pygame.SRCALPHA)
    LIGHT_SQUARE = pygame.Surface((SQUARE_WIDTH, SQUARE_HEIGHT), pygame.SRCALPHA)
    MOVE_SQUARE = pygame.Surface((SQUARE_WIDTH, SQUARE_HEIGHT), pygame.SRCALPHA)
    CHECKMATE_SQUARE = pygame.Surface((SQUARE_WIDTH, SQUARE_HEIGHT), pygame.SRCALPHA)

    DARK_SQUARE.fill(DARK_CLR)
    LIGHT_SQUARE.fill(LIGHT_CLR)
    MOVE_SQUARE.fill(MOVE_CLR)
    CHECKMATE_SQUARE.fill(CHECKMATE_CLR)

    def __init__(self, fen: Optional[str]=None):
        self._board = chess.Board()
        if fen is not None:
            self._board.set_fen(fen)
        self._clock = pygame.time.Clock()

        self.running = False
        self.held_piece = None
        self.prev_move = None

        assert PLAYER_COLOUR == "WHITE" or PLAYER_COLOUR == "BLACK"
        self._is_player_white = PLAYER_COLOUR == "WHITE"

    @staticmethod
    def _get_real_coord(x: int, y: int) -> tuple[int, int]:
        return SQUARE_WIDTH * x, SQUARE_HEIGHT * y

    @staticmethod
    def _get_rel_coord(x: int, y: int) -> tuple[int, int]:
        return x // SQUARE_WIDTH, y // SQUARE_HEIGHT

    @staticmethod
    def _get_square(rank: int, file: int):
        return file + rank * 8

    @staticmethod
    def _get_rank_file(square: chess.Square):
        return chess.square_rank(square), chess.square_file(square)

    def _get_coord_from_square(self, square: chess.Square):
        rank, file = self._get_rank_file(square)
        if self._is_player_white:
            return file, 7 - rank
        else:
            return file, rank

    def _get_mouse_square(self):
        x, y = self._get_rel_coord(*pygame.mouse.get_pos())
        if self._is_player_white:
            return chess.square(x, 7 - y)
        return chess.square(x, y)

    def _draw_square(self, x: int, y: int, surf: pygame.Surface) -> None:
        self.screen.blit(surf, self._get_real_coord(x, y))

    def _draw_board(self) -> None:
        for x in range(8):
            for y in range(8):
                surf = self.LIGHT_SQUARE if (x + y + self._is_player_white) % 2 else self.DARK_SQUARE
                self._draw_square(x, y, surf)

        # Move squares
        if self.prev_move is not None:
            for square in (self.prev_move.from_square, self.prev_move.to_square):
                x, y = self._get_coord_from_square(square)
                self._draw_square(x, y, self.MOVE_SQUARE)

        # Checkmate squares
        outcome = self._board.outcome()
        if outcome is not None and outcome.winner is not None:
            king_square = self._board.king(chess.WHITE) if outcome.winner == chess.BLACK else self._board.king(chess.BLACK)
            if king_square is not None:
                x, y = self._get_coord_from_square(king_square)
                self._draw_square(x, y, self.CHECKMATE_SQUARE)

    def _draw_pieces(self) -> None:
        for square in chess.SQUARES:
            if self.held_piece is not None and square == self._get_square(self.held_piece.rank, self.held_piece.file):
                continue

            piece = self._board.piece_at(square)
            if piece is None:
                continue

            img = self.ASSET_DICT[(piece.color, piece.piece_type)]
            self.screen.blit(img, self._get_real_coord(*self._get_coord_from_square(square)))

    def _draw_held_piece(self) -> None:
        if self.held_piece is None:
            return

        img = self.ASSET_DICT[(self.held_piece.piece.color, self.held_piece.piece.piece_type)]
        mouse_x, mouse_y = pygame.mouse.get_pos()
        self.screen.blit(img, (mouse_x - SQUARE_WIDTH // 2, mouse_y - SQUARE_HEIGHT // 2))

    def _get_move(self, from_square: chess.Square, to_square: chess.Square) -> Optional[chess.Move]:
        if from_square == to_square:
            return None

        from_alg = chess.square_name(from_square)
        to_alg = chess.square_name(to_square)

        # If promotion, promote to queen
        rank = chess.square_rank(to_square)
        if (rank == 0 or rank == 7) and self._board.piece_at(from_square).piece_type == chess.PAWN:
            return chess.Move.from_uci(from_alg + to_alg + "q")

        return chess.Move.from_uci(from_alg + to_alg)

    def _try_play_move(self, move: chess.Move) -> bool:
        if move in self._board.legal_moves:
            self._board.push(move)
            return True
        return False

    def _on_mouse_up(self) -> None:
        if self.held_piece is None:
            return

        move = self._get_move(
            chess.square(self.held_piece.file, self.held_piece.rank),
            self._get_mouse_square()
        )
        self._try_play_move(move)
        self.prev_move = move

        self.held_piece = None

    def _on_mouse_down(self) -> None:
        square = self._get_mouse_square()
        piece = self._board.piece_at(square)
        if piece is None:
            return

        rank, file = self._get_rank_file(square)
        self.held_piece = HeldPiece(rank, file, piece)

    @staticmethod
    def _exit():
        pygame.quit()

    def _main(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.running = False
            elif event.type == pygame.MOUSEBUTTONUP:
                self._on_mouse_up()
            elif event.type == pygame.MOUSEBUTTONDOWN:
                self._on_mouse_down()

            # Draw board
        self._draw_board()
        self._draw_pieces()
        self._draw_held_piece()

        pygame.display.flip()
        self._clock.tick(FPS)

    def run(self) -> None:
        self.running = True

        while self.running:
            self._main()

        self._exit()


class EngineGui(Gui):
    def _init_uci(self):
        self._engine.write("uci")
        while True:
            msg_rec = self._engine.listen()
            cmd = msg_rec.split(' ')[0]
            if cmd == "uciok":
                break

    def _set_engine_fen(self, fen: str):
        self._engine.write(f"position fen {fen}")

    def __init__(self, engine: Engine, fen: Optional[str]=None):
        super().__init__(fen)
        self._engine = engine
        self._is_waiting_for_engine = False

        if fen is not None:
            self._set_engine_fen(fen)

        self._init_uci()
        if not self._is_player_white:
            self._request_engine_move()
            self._is_waiting_for_engine = True

    def _undo_prev_move(self):
        self._board.pop()

    def _update_engine_pos(self, move: chess.Move) -> None:
        self._engine.write(f"position moves {move.uci()}")

    def _request_engine_move(self):
        self._engine.write("go")

    def _get_engine_move(self) -> Optional[chess.Move]:
        self._engine.write("go")
        msg_rec = self._engine.listen()

        msg_lines = msg_rec.split(' ')
        cmd = msg_lines[0]
        if cmd == "error":
            return None
        elif cmd == "bestmove":
            try:
                uci_move = msg_lines[1]
                engine_move = chess.Move.from_uci(uci_move)
                return engine_move
            except chess.InvalidMoveError:
                print("Invalid uci move received from engine")
                return None
        else:
            print("Unknown command received from engine")
            return None

    def _play_engine_move(self, move: chess.Move) -> bool:
        is_engine_move_valid = self._try_play_move(move)
        if not is_engine_move_valid:
            print(f"Invalid move from engine: {move}")
            return False

        self.prev_move = move
        self._update_engine_pos(move)
        return True

    def _play_player_move(self, move: chess.Move) -> None:
        if not self._is_waiting_for_engine and not self._board.is_game_over():
            is_played = self._try_play_move(move)
            if not is_played:
                print(f"Invalid move attempted: {move}")
                return

            self._update_engine_pos(move)

            if not self._board.is_game_over():
                self._request_engine_move()
                self._is_waiting_for_engine = True

    def _on_mouse_up(self) -> None:
        if self.held_piece is None:
            return

        move = self._get_move(
            chess.square(self.held_piece.file, self.held_piece.rank),
            self._get_mouse_square()
        )
        self.held_piece = None
        self._play_player_move(move)

    def _exit(self):
        super()._exit()
        self._engine.write("quit")

    def _handle_engine_msg(self, msg: str) -> bool:
        msg_lines = msg.split(' ')
        cmd = msg_lines[0]

        if cmd == "bestmove":
            try:
                uci_move = msg_lines[1]
                engine_move = chess.Move.from_uci(uci_move)
                return self._play_engine_move(engine_move)

            except chess.InvalidMoveError:
                print("Invalid uci move received from engine")
                return False
        elif cmd == "error":
            return False
        else:
            print("Unknown command received from engine")
            return False

    def _main(self):
        super()._main()

        if self._is_waiting_for_engine:
            msg = self._engine.async_listen()
            if msg is not None:
                self._is_waiting_for_engine = False
                is_valid = self._handle_engine_msg(msg)
                if not is_valid:
                    self._undo_prev_move()

        while True:
            msg = self._engine.async_listen_stderr()
            if msg is None:
                break
            print(f"[ENGINE ERROR] {msg}", file=sys.stderr)
