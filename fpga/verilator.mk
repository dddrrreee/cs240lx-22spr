sim: $(SIM) $(SRC)
	verilator -Wno-UNOPTFLAT -sv -cc --Mdir .verilator $(SRC) --top $(SIM_TOP) --trace --exe --build $< -o ../$(notdir $@)

SIMCLEAN=rm -rf sim .verilator trace.vcd
