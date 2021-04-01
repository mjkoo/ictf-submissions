import random

for i in range(20):
    op = random.choice(["ADD", "SUB", "XOR", "NOT", "ROL", "ROR"])

    if op == "ADD":
        print(f"c += {random.randrange(1, 256)};")
    elif op == "SUB":
        print(f"c -= {random.randrange(1, 256)};")
    elif op == "XOR":
        print(f"c ^= {random.randrange(1, 256)};")
    elif op == "NOT":
        print("c = ~c;");
    elif op == "ROL":
        d = random.randrange(1, 8)
        print(f"c = (c << {d}) | (c >> ({8 - d}));")
    elif op == "ROR":
        d = random.randrange(1, 8)
        print(f"c = (c >> {d}) | (c << ({8 - d}));")
