import angr
import sys
import claripy

class FakeCrc32(angr.SimProcedure):
    def run(self, argc, argv):
        return 0xb9454816

PROG = "./crackme"

project = angr.Project(PROG)
#project.hook_symbol("crc32", FakeCrc32())

flag_chars = [claripy.BVS("flag_%d" % i, 8) for i in range(28)]
flag = claripy.Concat(*flag_chars + [claripy.BVV(b"\n")])

st = project.factory.entry_state(args=[PROG, flag])

for k, c in zip(flag_chars, "ictf"):
    st.solver.add(k == ord(c))


def is_successful(state):
    output = state.posix.dumps(sys.stdout.fileno())
    return b"you win!" in output


sim = project.factory.simgr(st)
#sim.explore(find=is_successful)
sim.explore(find=0x4013b9)

for sol in sim.found:
    print(sol.solver.eval(flag, cast_to=bytes))
