def coord_to_algebraic(coord: tuple[int, int]) -> str:
    return chr(coord[0] + ord('a')) + chr(7 - (coord[1]) + ord('1'))

def algebraic_to_coord(note: str) -> tuple[int, int]:
    return ord(note[0]) - ord('a'), 7 - (ord(note[1]) - ord('1'))
