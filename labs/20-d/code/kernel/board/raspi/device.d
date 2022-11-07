module kernel.board.raspi.device;

version (raspi1ap) enum base = 0x20000000;
version (raspi3b) enum base = 0x3f000000;
version (raspi4b) enum base = 0xfe000000;
