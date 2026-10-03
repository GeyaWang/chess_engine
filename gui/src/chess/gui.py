import pygame
import os
import pathlib
from chess.settings import SQUARE_HEIGHT, SQUARE_WIDTH, FPS, DARK_CLR, LIGHT_CLR, DARK_MOVE_CLR, LIGHT_MOVE_CLR
from chess.piece import Piece, PieceType, PieceColour
from chess.game import Game
from chess.engine import Engine
from chess.utils import coord_to_algebraic, algebraic_to_coord


BASE_DIR = pathlib.Path(__file__).resolve().parent.parent.parent
ASSETS_DIR = BASE_DIR / "assets"

ASSET_DICT = {
    (PieceColour.WHITE, PieceType.PAWN): str(ASSETS_DIR / "wp.png"),
    (PieceColour.WHITE, PieceType.KNIGHT): str(ASSETS_DIR / "wn.png"),
    (PieceColour.WHITE, PieceType.BISHOP): str(ASSETS_DIR / "wb.png"),
    (PieceColour.WHITE, PieceType.ROOK): str(ASSETS_DIR / "wr.png"),
    (PieceColour.WHITE, PieceType.QUEEN): str(ASSETS_DIR / "wq.png"),
    (PieceColour.WHITE, PieceType.KING): str(ASSETS_DIR / "wk.png"),
    (PieceColour.BLACK, PieceType.PAWN): str(ASSETS_DIR / "bp.png"),
    (PieceColour.BLACK, PieceType.KNIGHT): str(ASSETS_DIR / "bn.png"),
    (PieceColour.BLACK, PieceType.BISHOP): str(ASSETS_DIR / "bb.png"),
    (PieceColour.BLACK, PieceType.ROOK): str(ASSETS_DIR / "br.png"),
    (PieceColour.BLACK, PieceType.QUEEN): str(ASSETS_DIR / "bq.png"),
    (PieceColour.BLACK, PieceType.KING): str(ASSETS_DIR / "bk.png"),
}


class PieceSprite(pygame.sprite.Sprite):
    def __init__(self, piece: Piece):
        pygame.sprite.Sprite.__init__(self)

        self.piece = piece
        self.image = pygame.transform.smoothscale(pygame.image.load(ASSET_DICT[(self.piece.get_colour(), self.piece.get_type())]).convert_alpha(), (SQUARE_WIDTH, SQUARE_HEIGHT))
        self.rect = self.image.get_rect()
        self.rect.x = SQUARE_WIDTH * self.piece.get_pos()[0]
        self.rect.y = SQUARE_HEIGHT * self.piece.get_pos()[1]

    def update(self) -> None:
        if self.piece.is_dead:
            self.kill()
            return
        self.rect.x = SQUARE_WIDTH * self.piece.get_pos()[0]
        self.rect.y = SQUARE_HEIGHT * self.piece.get_pos()[1]

    def get_pos(self) -> tuple[int, int]:
        return self.piece.get_pos()

    def draw(self, surface: pygame.Surface, pos: tuple[int, int]) -> None:
        surface.blit(self.image, pos)


class Gui:
    def __init__(self, engine: Engine):
        self._engine = engine
        self._game = Game()

        pygame.init()
        self.screen = pygame.display.set_mode((SQUARE_WIDTH * 8, SQUARE_HEIGHT * 8))
        pygame.display.set_caption("Chess")
        self._clock = pygame.time.Clock()
        self.running = False
        self.is_over = False

        self._hold_piece = None
        self._is_holding_piece = False

        self._piece_list = pygame.sprite.Group()
        for i in range(8):
            for j in range(8):
                piece = self._game.get(i, j)
                if piece.get_type() != PieceType.EMPTY:
                    self._piece_list.add(PieceSprite(piece))
        self._non_hold_piece_list = pygame.sprite.Group(self._piece_list)

    @staticmethod
    def _get_real_coord(x: int, y: int) -> tuple[int, int]:
        return SQUARE_WIDTH * x, SQUARE_HEIGHT * y

    def _draw_square(self, x: int, y: int, clr: tuple[int, int, int]) -> None:
        pygame.draw.rect(self.screen, clr, pygame.Rect(*self._get_real_coord(x, y), SQUARE_WIDTH, SQUARE_HEIGHT))

    def _draw_board(self) -> None:
        self.screen.fill(LIGHT_CLR)
        for x in range(8):
            for y in range(8):
                if (x + y) % 2:
                    self._draw_square(x, y, DARK_CLR)

        # Draw prev move
        for pos in self._game.prev_move:
            if pos is None:
                continue
            if (pos[0] + pos[1]) % 2:
                self._draw_square(*pos, DARK_MOVE_CLR)
            else:
                self._draw_square(*pos, LIGHT_MOVE_CLR)

    def _draw_pieces(self) -> None:
        self._non_hold_piece_list.draw(self.screen)
        if self._is_holding_piece:
            mouse_pos = pygame.mouse.get_pos()
            pos_x = mouse_pos[0] - SQUARE_WIDTH // 2
            pos_y = mouse_pos[1] - SQUARE_HEIGHT // 2
            self._hold_piece.draw(self.screen, (pos_x, pos_y))

    def _get_piece_on_mouse(self) -> PieceSprite|None:
        for piece in self._non_hold_piece_list:
            if piece.rect.collidepoint(pygame.mouse.get_pos()):
                return piece
        return None

    def _get_and_play_engine_move(self, pos1, pos2):
        # Send move to engine
        msg_send = f"move {coord_to_algebraic(pos1)} {coord_to_algebraic(pos2)}"
        self._engine.write(msg_send)
        print(f"Sent: {msg_send.strip()}")
        msg_rec = self._engine.listen()
        print(f"Received: {msg_rec.strip()}")

        engine_msg = msg_rec.split(' ')
        engine_pos1 = engine_msg[1]
        engine_pos2 = engine_msg[2]
        print(algebraic_to_coord(engine_pos1), algebraic_to_coord(engine_pos2))

        if self._game.try_move(algebraic_to_coord(engine_pos1), algebraic_to_coord(engine_pos2)):
            self._piece_list.update()
        else:
            print(f"Invalid move received by client: {engine_pos1} {engine_pos2}")
            self.is_over = True

        if self._game.is_game_over():
            print("Checkmate!")
            self.is_over = True

    def _on_mouse_up(self) -> None:
        if self._is_holding_piece and self._hold_piece is not None:
            pos1 = self._hold_piece.get_pos()
            mouse_pos = pygame.mouse.get_pos()
            pos2 = (mouse_pos[0] // SQUARE_WIDTH, mouse_pos[1] // SQUARE_HEIGHT)

            self._is_holding_piece = False
            self._non_hold_piece_list.add(self._hold_piece)
            self._hold_piece = None

            # Move piece
            if self._game.try_move(pos1, pos2):
                self._piece_list.update()

                if self._game.is_game_over():
                    print("Checkmate!")
                    self.is_over = True

                self._get_and_play_engine_move(pos1, pos2)

    def _on_mouse_down(self) -> None:
        if self.is_over:
            return

        if not self._is_holding_piece:
            piece = self._get_piece_on_mouse()
            if piece is None:
                return

            # Pick up piece
            self._is_holding_piece = True
            self._hold_piece = piece
            self._non_hold_piece_list.remove(piece)

    def run(self) -> None:
        self.running = True
        self._engine.write("gui")

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

            pygame.display.flip()
            self._clock.tick(FPS)

        pygame.quit()
