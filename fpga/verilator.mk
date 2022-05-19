sim: $(SIM) $(SIM_SRC)
	verilator $(VFLAGS) -Wno-UNOPTFLAT -sv -cc --Mdir .verilator $(SIM_SRC) --top $(SIM_TOP) --trace --exe --build $< -o ../$(notdir $@)

SIMCLEAN=rm -rf sim .verilator trace.vcd
