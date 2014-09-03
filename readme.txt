pinlog
                                                   (c) 2012 deroko of ARTeam
                                  
        Well sometimes I need to go fast through binary, and see execution flow.
In IDA, obviously, you get many execution paths, but for faster disassembling it
is sometimes required to know which code is executed, and which not in normal 
execution flow, without figuring out all loops, jmps in static disassembly.

        What I do, is to use pin to log how many times certain parts of code are
executed, and based on this count assign color in IDA, thus I don't even bother
with code which is left "white", eg. wasn't executed. This "project" - and I call
it "project" because this "project" cann't be even called project, dut to it's
simplicity - is supposed to speedup rce tasks . Only reason why I've decided to
release it, is because somebody, somewhere might find it useful for his/hers rce
task(s). Remember, sometimes useful tools are written with only a few lines of code.

So how does it work?

pinlog.exe will allocate shared memory of 50mb (w0000000t), and this memory will
be used as byte index into instruction position in exe or dll. Yes, it can trace
also dll loaded by process, if you don't care about execution of process itself.
However on Mac, all data is written to the file at the end of execution as I failed
several times to implement shared section, which seems to be overall probelm on mac
according to some mail list I've seen. Reason for using shared memory is to get 
trace log even if we kill process through TaskManager/kill, and also to have log
even when child process is executed.

example for exe trace:
pinlog.exe c:\victim\victim.exe victim.exe log.log
example for dll trace:
pinlog.exe c:\victim\victim.exe victim.dll log.log

log file will have count of instructions which are executed, thus I can identify
commonly executed loops, and execution path of a process. 

After trace is done, or killed by you (pinlog.exe waits for pin to finish), log
file will be written to disk. Now you may use loadlog.py script, or you can use
loadlog_plugin.py for IDA thus you don't have to load script everytime you need
to load log into IDA. If you don't like blue color, you may experiment with 
colors in loadlog.py/loadlog_plugin.py to see which ones you like the most 
(red/green/blue/yellow/etc...).

def build_color_list():
        clist = [];
        
        r = 0x00/255.0;
        g = 0x00/255.0;
        b = 0xff/255.0;


Tool as always comes with source code. Ah yes, almost forgot path to pin must be
in your PATH for Linux, and Windows. If you want to compile windows version, you
will need SDK from Microsoft, and WDK (for pinlog.exe). Makefile for pin assumes
that pin is located at your $(HOME)\Desktop\pin, same goes for Linux version,
however for Mac I use $(HOME)\pin

Current supported PIN version:
Windows : pin 58423 for vc9 for precompiled binaries, or any version if you
          recompile sources
Linux   : 58423 version, recompile needed
Mac     : 58423 version, recompile needed

                                            S verom u Boga, deroko of ARTeam