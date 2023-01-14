effect ctl assert(v: bool) : ()
effect ctl yield(v: int) : ()

// part a/b
fun num-coins(coins: list<int>, target)
  match xs
    Cons(x.xx) -> if yield(x) then traverse(xx) else ()
    Nil -> ()

// part c
fun least-coins(coins, tgt)
  0

fun print-coin-counts(coins, tgt)
  with handler
    ctl yield(v)
      println(show(v))
      resume(())
  num-coins([1, 2, 3, 4, 5], 5)

fun main()
  print-coin-counts([1, 2, 3, 4, 5], 5)

  with handler
    ctl assert(v)
      if(v) then
        println("assert success")
        resume(())
      else
        println("assertion failure")

  assert(least-coins([1, 2, 3, 4, 5], 1) == 1)
  assert(least-coins([1, 2, 3, 4, 5], 6) == 2)

