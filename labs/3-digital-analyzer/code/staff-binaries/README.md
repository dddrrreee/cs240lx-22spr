Two binaries:
  - `scope.bin`: simple digital analyzer that monitors pin 21 and records
     high-low cycle counts.
  - `test-gen.bin`: simple test signal generator that flips pin 21 every
    6000 cycles.

How-to:

   1. Connect both pi's using a jumper wire from pin 21 to pin 21.
   2. Connect both pi's to your laptop.
   3. In one window, start `scope.bin` first by giving one of the
      TTY-USBs:

            # I'm on linux, so one is conncted to /dev/ttyUSB0
            % my-install /dev/ttyUSB0 scope.bin

      Or (probably better) use the `-first` or `-last` option:

            % my-install -first scope.bin

   4. In another window, start `test-gen.bin`

            # I'm on linux, so one is conncted to /dev/ttyUSB0
            % my-install /dev/ttyUSB1 test-gen.bin

      Or better:

            % my-install -last test-gen.bin



      And then rerun:

            # After it exits, rerun it.
            % my-install -last test-gen.bin
            % my-install -last test-gen.bin
            % my-install -last test-gen.bin
            % my-install -last test-gen.bin


In the `scope.bin` window, you should get something like:

        timeout! start=-1627927595, t=-927782333, minux=700145262
        0: val=1, time=6007, tot=6007: exp=6000 (err=7, toterr=7)
        1: val=0, time=5933, tot=11940: exp=12000 (err=60, toterr=67)
        2: val=1, time=6039, tot=17979: exp=18000 (err=21, toterr=88)
        3: val=0, time=5967, tot=23946: exp=24000 (err=54, toterr=142)
        4: val=1, time=5984, tot=29930: exp=30000 (err=70, toterr=212)
        5: val=0, time=6029, tot=35959: exp=36000 (err=41, toterr=253)
        6: val=1, time=5966, tot=41925: exp=42000 (err=75, toterr=328)
        7: val=0, time=6029, tot=47954: exp=48000 (err=46, toterr=374)
    
        timeout! start=1008181678, t=1708377206, minux=700195528
        0: val=1, time=5996, tot=5996: exp=6000 (err=4, toterr=4)
        1: val=0, time=5992, tot=11988: exp=12000 (err=12, toterr=16)
        2: val=1, time=5992, tot=17980: exp=18000 (err=20, toterr=36)
        3: val=0, time=5992, tot=23972: exp=24000 (err=28, toterr=64)
        4: val=1, time=5992, tot=29964: exp=30000 (err=36, toterr=100)
        5: val=0, time=5992, tot=35956: exp=36000 (err=44, toterr=144)
        6: val=1, time=5992, tot=41948: exp=42000 (err=52, toterr=196)
        7: val=0, time=5992, tot=47940: exp=48000 (err=60, toterr=256)

