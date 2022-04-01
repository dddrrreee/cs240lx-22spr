## dynamic code gen on pi.

Given a vector of function pointers, generate code dynamically that:
  1. Will do hardcoded function calls to them.
  2. When that works, rewrite the binary to jump from one function to the
     next.
