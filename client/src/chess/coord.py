class Coord:
    def __init__(self, x, y):
        if type(x) is str and type(y) is int:
            # Chess notation given
            self.x = ord(x.lower()) - 97
            self.y = 8 - y
        elif type(x) is int and type(y) is int:
            # Coordinates given
            self.x = x
            self.y = y
        else:
            raise TypeError("Arguments are neither in chess notion nor coordinates")

        if not (0 <= self.x <= 7 and 0 <= self.y <= 7):
            raise ValueError("Invalid coordinates given")

    def __str__(self):
        return f"Coord({self.x},{self.y})"
