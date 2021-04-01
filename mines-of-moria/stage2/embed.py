import base64
import itertools
import struct
import sys
import zlib
from pngparser import PngParser, ChunkTypes, ChunkData, Chunk

PNG_HEADER = b"\x89\x50\x4e\x47\x0d\x0a\x1a\x0a"
KEY = "g@nd@1f"

TO_SKIP = [
    ChunkTypes.IHDR,
    ChunkTypes.bKGD,
    ChunkTypes.pHYs,
    ChunkTypes.IEND,
]

infile = sys.argv[1]
hiddenfile = sys.argv[2]
outfile = sys.argv[3]

with open(outfile, "wb") as out:
    out.write(PNG_HEADER)

    with PngParser(infile) as png:
        chunks = png.get_all()
        for chunk in chunks:
            if chunk.type == ChunkTypes.IEND:
                break

            if chunk.type == ChunkTypes.tEXt and chunk.data.data.startswith(b"date"):
                continue

            out.write(chunk.to_bytes())

        # Write our compressed data
        with open(hiddenfile, "rb") as f:
            b = bytes(c ^ ord(k) for c, k in zip(f.read(), itertools.cycle(KEY)))
            data = ChunkData(b"d00rz_0f_dur1n\x00\x00" + zlib.compress(b))
            ztxt_chunk = Chunk(ChunkTypes.zTXt, data)
            out.write(ztxt_chunk.to_bytes())

        iend = png.get_by_type(ChunkTypes.IEND)
        out.write(iend[0].to_bytes())
