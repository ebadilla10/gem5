import m5
from m5.objects import *

system = System()
# System Clock
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1GHz'
system.clk_domain.voltage_domain = VoltageDomain()
# System memory
system.mem_mode = 'timing'
system.mem_ranges = [AddrRange('512MB')]
# CPU
system.cpu = TimingSimpleCPU()
# Mem bus
system.membus = SystemXBar()
# No cache yet,so Data and Inst cache ports are directly connected to membus
system.cpu.icache_port = system.membus.slave
system.cpu.dcache_port = system.membus.slave
# I/O Controller
system.cpu.createInterruptController()
system.cpu.interrupts[0].pio = system.membus.master
system.cpu.interrupts[0].int_master = system.membus.slave
system.cpu.interrupts[0].int_slave = system.membus.master

system.system_port = system.membus.slave
# Memory Controller
system.mem_ctrl = DDR3_1600_8x8()
system.mem_ctrl.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.master

# Hello World Process
process = Process()
process.cmd = ['tests/test-progs/hello/bin/x86/linux/hello']
system.cpu.workload = process
system.cpu.createThreads()

# Root object
root = Root(full_system = False, system = system)
m5.instantiate()
print("Beginning simulation!")
exit_event = m5.simulate()
print('Exiting @ tick {} because {}'.format(m5.curTick(),
    exit_event.getCause()))
