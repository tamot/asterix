__author__ = 'dsalanti'

import asterix

# This is binary presentation of asterix packet of CAT048
#                  len  cat 048
#                 ---- ---------
asterix_packet = bytearray([0x30, 0x00, 0x30, 0xfd, 0xf7, 0x02, 0x19, 0xc9, 0x35, 0x6d, 0x4d, 0xa0, 0xc5, 0xaf, 0xf1, 0xe0,
                        0x02, 0x00, 0x05, 0x28, 0x3c, 0x66, 0x0c, 0x10, 0xc2, 0x36, 0xd4, 0x18, 0x20, 0x01, 0xc0, 0x78,
                        0x00, 0x31, 0xbc, 0x00, 0x00, 0x40, 0x0d, 0xeb, 0x07, 0xb9, 0x58, 0x2e, 0x41, 0x00, 0x20, 0xf5])

# parse and print packet
parsed = asterix.parse(asterix_packet)

# describe Asterix data
formatted = asterix.describe(parsed)
print(formatted)


