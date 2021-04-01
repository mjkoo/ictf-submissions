import itertools
import sys

KEY = "w35tg@t3"

infile = sys.argv[1]
outfile = sys.argv[2]

with open(infile, "rb") as f:
    ct = bytes(b ^ ord(k) for b, k in zip(f.read(), itertools.cycle(KEY)))

with open(outfile, "wb") as f:
    f.write(ct)
