// computes dividend / divisor
// from: https://stackoverflow.com/questions/29908103/implementing-c-binary-division-in-o1
static inline unsigned divide(unsigned dividend, unsigned divisor) { 
    unsigned current = 1;
    unsigned answer = 0;
    if(divisor > dividend){
        return 0;
    }
    if(divisor == dividend){
        return 1;
    }
    unsigned k = 1 << (sizeof(unsigned) * 8 - 1);
    while(((divisor & k) == 0) && ((divisor << 1) <= dividend)) {
        divisor <<= 1;
        current <<= 1;
    }
    while(current!=0) {
        if(dividend >= divisor) {
            dividend -= divisor;
            answer |= current;
        }
        current >>= 1;
        divisor >>= 1;
    }    
    return answer;
}

// computes x (mod y)
// from: https://stackoverflow.com/questions/12486883/write-a-modulo-function-using-only-addition-subtraction 
static inline unsigned mod(unsigned x, unsigned y)
{
    unsigned modulus = x, divisor = y;

    while (divisor <= modulus)
        divisor <<= 1;

    while (modulus >= y) {
        while (divisor > modulus)
            divisor >>= 1;
        modulus -= divisor;
    }

    return modulus;
}

