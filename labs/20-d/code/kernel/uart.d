module kernel.uart;

version (raspi1ap) import uart = kernel.board.raspi.uart;

version (raspi3b) import uart = kernel.board.raspi.uart;

version (raspi4b) import uart = kernel.board.raspi.uart;

alias init = uart.init;
alias rx = uart.rx;
alias tx = uart.tx;
alias tx_flush = uart.tx_flush;
alias rx_empty = uart.rx_empty;
