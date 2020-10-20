#!/usr/bin/env python3

import struct
import binascii
import sys

def apply_patch(srec_file, binary_file):
    file_size = binary_file.seek(0, 2)
    for line in srec_file.readlines():
        if not line.startswith('S3'):
            continue
        line = line.strip()

        byte_count = int(line[2:4], 16)
        address = int(line[4:12], 16)
        data = binascii.unhexlify(line[12:-2])
        # ignore checksum

        offset = address - 0x02000000
        if offset < 0 or offset > file_size:
            raise ValueError("attempt to write outside cart flash")

        binary_file.seek(offset)
        binary_file.write(data)

if __name__ == "__main__":
    _, patchfile, infile, outfile, maxsize = sys.argv
    patch_fp = open(patchfile)

    with open(infile, 'rb') as in_fp:
        out_fp = open(outfile, 'w+b')
        out_fp.write(in_fp.read(int(maxsize, 0)))

    apply_patch(patch_fp, out_fp)
