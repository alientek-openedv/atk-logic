'''
Intel 8080.

In addition to the 8-bit data bus, this decoder requires the input signals
A0/DC (Command/Data Select), /WR (Write) and optionally /CS (Chip Select) to do its work. An explicit
clock signal is not required.
'''

from .pd import Decoder
