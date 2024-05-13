##
## This file is part of the libsigrokdecode project.
##
## Copyright (C) 2016 Rudolf Reuter <reuterru@arcor.de>
## Copyright (C) 2023 ALIENTEK(正点原子) <39035605@qq.com>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
##

'''
This protocol decoder can decode the GPIB (IEEE-488) protocol.

If the clock channel DAV is set, GPIB decoding is shown.

If the total sample number is given, the last byte is shown.

If option "state" is set "True", a GPIB "State Analysis" is printed
in the terminal.
'''

from .pd import Decoder
