module kernel.gpio;

version (raspi1ap) import gpio = kernel.board.raspi.gpio;

version (raspi3b) import gpio = kernel.board.raspi.gpio;

version (raspi4b) import gpio = kernel.board.raspi.gpio;

alias set_output = gpio.set_output;
alias set_input = gpio.set_input;
alias set_on = gpio.set_on;
alias set_off = gpio.set_off;
alias read = gpio.read;

void write(uint pin, bool v) {
    if (v)
        set_on(pin);
    else
        set_off(pin);
}
