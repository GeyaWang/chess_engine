import pygame
from chess.settings import SQUARE_HEIGHT, SQUARE_WIDTH, FPS, DARK_CLR, LIGHT_CLR
from chess.piece import Piece, PieceType, PieceColour
from chess.board import Board


ASSET_DICT = {
    (PieceColour.WHITE, PieceType.PAWN): "./assets/wp.png",
    (PieceColour.WHITE, PieceType.KNIGHT): "./assets/wn.png",
    (PieceColour.WHITE, PieceType.BISHOP): "./assets/wb.png",
    (PieceColour.WHITE, PieceType.ROOK): "./assets/wr.png",
    (PieceColour.WHITE, PieceType.QUEEN): "./assets/wq.png",
    (PieceColour.WHITE, PieceType.KING): "./assets/wk.png",
    (PieceColour.BLACK, PieceType.PAWN): "./assets/bp.png",
    (PieceColour.BLACK, PieceType.KNIGHT): "./assets/bn.png",
    (PieceColour.BLACK, PieceType.BISHOP): "./assets/bb.png",
    (PieceColour.BLACK, PieceType.ROOK): "./assets/br.png",
    (PieceColour.BLACK, PieceType.QUEEN): "./assets/bq.png",
    (PieceColour.BLACK, PieceType.KING): "./assets/bk.png",
}


class PieceSprite(pygame.sprite.Sprite):
    def __init__(self, piece: Piece):
        pygame.sprite.Sprite.__init__(self)

        self.piece = piece
        self.image = pygame.transform.smoothscale(pygame.image.load(ASSET_DICT[(self.piece.colour, self.piece.type)]).convert_alpha(), (SQUARE_WIDTH, SQUARE_HEIGHT))
        self.rect = self.image.get_rect()
        self.rect.x = SQUARE_WIDTH * self.piece.get_pos()[0]
        self.rect.y = SQUARE_HEIGHT * self.piece.get_pos()[1]

    def update(self):
        if self.piece.is_dead:
            self.kill()
            return
        self.rect.x = SQUARE_WIDTH * self.piece.get_pos()[0]
        self.rect.y = SQUARE_HEIGHT * self.piece.get_pos()[1]

    def get_pos(self):
        return self.piece.get_pos()

    def draw(self, surface: pygame.Surface, pos: tuple[int, int]):
        surface.blit(self.image, pos)


class Gui:
    def __init__(self):
        pygame.init()

        self.screen = pygame.display.set_mode((SQUARE_WIDTH * 8, SQUARE_HEIGHT * 8))
        pygame.display.set_caption("Chess")

        self._clock = pygame.time.Clock()
        self.running = False

        self._hold_piece = None
        self._is_holding_piece = False

        self._board = Board()

        self._piece_list = pygame.sprite.Group()
        for i in range(8):
            for j in range(8):
                piece = self._board.get(i, j)
                if piece is not None:
                    self._piece_list.add(PieceSprite(piece))
        self._non_hold_piece_list = pygame.sprite.Group(self._piece_list)

    @staticmethod
    def _get_real_coord(x: int, y: int) -> tuple[int, int]:
        return SQUARE_WIDTH * x, SQUARE_HEIGHT * y

    def _draw_square(self, x: int, y: int, clr: tuple[int, int, int]):
        pygame.draw.rect(self.screen, clr, pygame.Rect(*self._get_real_coord(x, y), SQUARE_WIDTH, SQUARE_HEIGHT))

    def _draw_board(self):
        self.screen.fill(DARK_CLR)
        for x in range(8):
            for y in range(8):
                if not (x + y) % 2:
                    self._draw_square(x, y, LIGHT_CLR)

    def _draw_pieces(self):
        self._non_hold_piece_list.draw(self.screen)
        if self._is_holding_piece:
            mouse_pos = pygame.mouse.get_pos()
            pos_x = mouse_pos[0] - SQUARE_WIDTH // 2
            pos_y = mouse_pos[1] - SQUARE_HEIGHT // 2
            self._hold_piece.draw(self.screen, (pos_x, pos_y))

    def _get_piece_on_mouse(self):
        for piece in self._non_hold_piece_list:
            if piece.rect.collidepoint(pygame.mouse.get_pos()):
                return piece
        return None

    def _on_mouse_up(self):
        if self._is_holding_piece and self._hold_piece is not None:
            # Move piece
            mouse_pos = pygame.mouse.get_pos()
            self._board.make_move(self._hold_piece.get_pos(), (mouse_pos[0] // SQUARE_WIDTH, mouse_pos[1] // SQUARE_HEIGHT))
            self._piece_list.update()

            # Reset
            self._is_holding_piece = False
            self._non_hold_piece_list.add(self._hold_piece)
            self._hold_piece = None

    def _on_mouse_down(self):
        if not self._is_holding_piece:
            piece = self._get_piece_on_mouse()
            if piece is None:
                return

            # Pick up piece
            self._is_holding_piece = True
            self._hold_piece = piece
            self._non_hold_piece_list.remove(piece)

    def run(self):
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

            pygame.display.flip()
            self._clock.tick(FPS)

        pygame.quit()
