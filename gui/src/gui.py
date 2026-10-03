from typing import Optional
from dataclasses import dataclass
import pygame
import pathlib
import chess
from settings import SQUARE_HEIGHT, SQUARE_WIDTH, FPS, DARK_CLR, LIGHT_CLR, DARK_MOVE_CLR, LIGHT_MOVE_CLR, PLAYER_COLOUR
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


    def __init__(self):
        self._board = chess.Board()
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

    def _draw_square(self, x: int, y: int, clr: tuple[int]) -> None:
        pygame.draw.rect(self.screen, clr, pygame.Rect(*self._get_real_coord(x, y), SQUARE_WIDTH, SQUARE_HEIGHT))

    def _draw_board(self) -> None:
        for x in range(8):
            for y in range(8):
                clr = LIGHT_CLR if (x + y + self._is_player_white) % 2 else DARK_CLR
                self._draw_square(x, y, clr)

        if self.prev_move is not None:
            for square in (self.prev_move.from_square, self.prev_move.to_square):
                x, y = self._get_coord_from_square(square)
                clr = LIGHT_MOVE_CLR if (x + y + self._is_player_white) % 2 else DARK_MOVE_CLR
                self._draw_square(x, y, clr)

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
            self.prev_move = move
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

        self.held_piece = None

    def _on_mouse_down(self) -> None:
        square = self._get_mouse_square()
        piece = self._board.piece_at(square)
        if piece is None:
            return

        rank, file = self._get_rank_file(square)
        self.held_piece = HeldPiece(rank, file, piece)

    def run(self) -> None:
        self.running = True

        while self.running:
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

        pygame.quit()


class EngineGui(Gui):
    def __init__(self, engine: Engine):
        super().__init__()
        self._engine = engine

    def _get_engine_move(self, move: chess.Move) -> Optional[chess.Move]:
        uci_move = move.uci()
        msg_send = f"move {uci_move}"
        self._engine.write(msg_send)
        print(f"Sent: {msg_send.strip()}")
        msg_rec = self._engine.listen()
        print(f"Received: {msg_rec.strip()}")

        engine_msg = msg_rec.strip().split(' ')
        if engine_msg[0] != "move":
            return None

        return chess.Move.from_uci(engine_msg[1])

    def _on_mouse_up(self) -> None:
        if self.held_piece is None:
            return

        move = self._get_move(
            chess.square(self.held_piece.file, self.held_piece.rank),
            self._get_mouse_square()
        )
        self.held_piece = None

        is_played = self._try_play_move(move)
        if is_played:
            engine_move = self._get_engine_move(self.prev_move)
            if engine_move is not None:
                is_engine_move_valid = self._try_play_move(engine_move)
                if not is_engine_move_valid:
                    print(f"Invalid move from engine: {engine_move}")

    def run(self) -> None:
        self._engine.write("gui")
        super().run()
