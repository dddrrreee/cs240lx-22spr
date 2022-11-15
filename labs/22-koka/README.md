# Koka
Hi everyone. This is yet another language lab, this time on (Koka)[https://koka-lang.github.io/koka/doc/index.html], a functional language. Koka is a research language that, unlike the other languages we covered, is unlikely to break into the mainstream anytime soon. But it has a few ideas that I think will eventually break into the mainstream. Much like how Cyclone's ideas for memory management eventually made their way into and inspired rust, I think that Koka's ideas will eventually make it into a mainstream language at some point down the line. In particular, its reference counting technique and algebraic effects make pure functional programming relatively performant and easier to use. If you are interested in its reference counting, I recommend (this paper)[https://www.microsoft.com/en-us/research/uploads/prod/2020/11/perceus-tr-v1.pdf]. This lab will focus on the effect system of Koka.

As a sidenote, Koka uses a [novel reference counting](https://dl.acm.org/doi/pdf/10.1145/3453483.3454032) mechanism to claw back a lot of the performance that is lost by only having immutable data structures. We could have a whole lab on this too.

# Algebraic Effects
Algebraic Effects are, in my opinion, Koka's biggest contribution. They serve two main roles. They are a very versatile control flow mechanism and *effectively* eliminate the need for language level support for coroutines, exceptions, state, cooperative threading, and so on. They essentially allow you to bounce up and down the stack. Also, they can be tracked precisely by the type system, allowing pure functional code to have side effects without cumbersome monad mumbo jumbo. (Here)[https://goto.ucsd.edu/~nvazou/koka/padl16.pdf] is a paper that formally shows the relationship between effects and monads. Here is how you would implement exceptions using effects:

``` js
effect ctl raise(msg: string) : int

fun divide(a: int, b: int) : raise int
  if b == 0 then raise("div by zero") else a / b

fun main() : console ()
  with handler
    ctl raise(msg)
      println(msg)
  println(show(divide(3, 0)));
  // prints "div by zero"
```

Much like exceptions, when you perform an effect, the program traverses up the stack until it hits a *handler* and continues execution from there. 

Effects, however, are more powerful than just exceptions. A big part of their power comes from the fact that you can resume from them, allowing you to bounce up and down the stack. Here is an example of a resumable exception

```js
effect ctl raise(msg: string) : int

fun divide(a: int, b: int) : raise int
  if b == 0 then raise("div by zero") else a / b

fun main() : console ()
  with handler
    ctl raise(msg)
      println(msg)
      resume(42)
  println(show(divide(3, 0)));
// Prints 
// "div by zero"
// 42
```

Empirically, the majority of effect handlers we make will be of this form with a bunch of code and then a `resume` call at the end.

Types are, for the most part, optional and can be inferred. You do need them on effect declarations and sometimes on parameters, but the compiler will tell you when if you forget. Note that the type of the function does not just include the return type; it also includes the name of the effects that it performs. This allows us to track all effects in the type system. This makes it very easy to tell if a function is pure and gives us much finer granularity in being able to track side effects. Not only can we determine purity, but for nonpure functions, we can tell exactly what effects are performed. Furthermore, effects are extremely flexible because they can be redefined at any part of the program and are dynamically bound. For example, if GPIO was controlled with effects, you could redefine the effect handler to log reads and writes at certain points so that, for example, you don't log gpio reads and writes when you do UART operations but you do for all other GPIO operation, much like one of the early labs in 140e. Likewise, if we wrap all our system calls in algebraic effects, we can easily track which ones are used.

A handler can `resume` multiple times. You can do this to, for example, `resume` with multiple values. For example, if we wanted to print every possible schedule of restaurants for our 340lx dinner, we could do so like this:
```js
effect ctl getword() : string

fun print-schedules(lst : list<string>, n: int)
  with handler
    ctl getword()
      lst.foreach fn(elem: string) {
        resume(elem)
      };
  var result := ""
  repeat(n) {
    result := result ++ " " ++ getword()
  }
  println(result)

      
fun main()
  print-schedules(["Zareens", "Dominos", "Oren's"], 5)
```

This will print every possible combination of the 3 restaurants over 5 days.



# Part 0: Install koka
[here](https://koka-lang.github.io/koka/doc/index.html#install) is the link to install koka. I skipped alot of syntax stuff because I think the documentation does it better so take a quick skim through [here](https://koka-lang.github.io/koka/doc/book.html).

# Part 1: Assert
This is very similar to exceptions. Go to `prog/assert.kk` and implement an `assert` effect that takes in a `bool`. 

# Part 2: Coins

## 2a: yield
Here is a toy problem that I think is quite well suited to demonstrate some of the features of algebraic effects. Some of you might remember a similar problem from the intro CS sequence.

The premise is this. Given a list of numbers representing coin values `l` and a target `t`, we want to figure out the number of coins it will take to reach `t`. For the purpose of this exercise, we can assume that 1 will always be in `l` and thus there will always be at least one combination of coins that reaches `t`.

For example, if `l = [1, 3, 5]` and `t = 4`, two possible results would be `4`(1+1+1=1) and `2`(1+3).

One thing to notice is that there are multiple possible values. Other languages have constructs like generators that allow you yield values and then later resume within the function. But Koka doesn't. Instead, we are going to have to make a `yield` effect. For this part, solve this problem anyway you want(in part b we are going to solve it using effects and loops) and make it `yield` every result. You should yield the same value multiple times if there are multiple ways to get to that value. You can implement this any way you want but for the next part you are going to have to do it with algebraic effects. 

## 2b: Only Effects
This part is the same as part a, except now you can only use algebraic effects(no backtracking) to enumerate all the possible cases.

## 2c: Find the least coins required to hit the target
Now change the yield handler so that, rather than printing everything that gets yielded, saves the smallest coin count and returns it. Add your `assert` effect handler so that the test cases work.

# Part 3: Cooperative threading(optional)
Make a yield effect that takes no arguments and returns nothing and fork effect that takes in a function and returns nothing and implement a simple cooperative scheduler. The key is that `resume` itself can be treated as a first class function and as a result can be stored in a list, queue, etc.

# Part 4: Blink(optional)

One very practical way to contribute to programming languages is to port them to more platforms. Koka currently has no support for the pi or arm. But adding it shouldn't be too difficult because it just generates c code and needs no runtime. Use `koka --target=c` to get the c files, link the necessary files, and then compile and upload to the pi. Then get a simple blink working. Because koka generates c files, it isn't too hard to emit plain c code - [here](https://github.com/koka-lang/koka/blob/master/lib/std/text/regex.kk) is an example from the standard library of this.

# Part 3: Cooperative scheduling(optional)
Make a yield effect that takes no arguments and returns nothing and fork effect that takes in a function and returns nothing and implement a simple cooperative scheduler. The key is that `resume` itself can be treated as a first class function and as a result can be stored in a list, queue, etc.