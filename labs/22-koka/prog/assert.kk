effect ctl assert(value: bool) : ()

fun main() : console ()
  //----------------your code here--------------------
  // make sure indented and return nothing.
  with ctl assert(v)
    if(v) then
      println("assert success")
      resume(())
    else
      println("assertion failure")

  //----------------end your code--------------------
  assert(3 == 3)
  assert(3 == 2 + 1)
  assert(3 == 2)
  assert(4 + 2 == 5)
  // Should print
  // assertion success
  // assertion success
  // assertion failure
