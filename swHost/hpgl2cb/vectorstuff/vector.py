# Simple vector
import math

class vector:
    def __init__(self, x , y):
        self.x = x
        self.y = y

    def mag(self):
        return math.sqrt(self.x**2 + self.y**2)

    def phi(self):
        return 180 / math.pi * math.atan2( self.y, self.x )

    # Overload "+" Operator for vector addition
    def __add__(self, other):
        x_ges = self.x + other.x
        y_ges = self.y + other.y
        return vector(x_ges, y_ges)

    def __radd__(self, other):
        return self.__add__(other)

    # Overoad "-" Operator for vector subtraction
    def __sub__(self, other):
        x_sub = self.x - other.x
        y_sub = self.y - other.y
        return vector(x_sub, y_sub)

    # == Operator
    def __eq__(self, other):
        return self.x == other.x and self.y == other.y

    # Vector to String
    def __str__(self):
        return f'({self.x}, {self.y})'

    def __repr__(self):
        return self.__str__()