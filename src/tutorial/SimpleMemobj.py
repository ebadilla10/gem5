from m5.params import *
from m5.proxy import *
from MemObject import MemObject

class SimpleMemobj(MemObject):
    type = 'SimpleMemobj'
    cxx_header = "tutorial/simple_memobj.hh"

    inst_port = SlavePort("CPU side instrucctions port, receives requests")
    data_port = SlavePort("CPU side data port, receives requests")
    mem_side = MasterPort("Memory side port, sends requests")
