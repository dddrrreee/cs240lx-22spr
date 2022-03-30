## CS240lx: advanced operating system lab, accelerated
### Engler + Akshay, Spr 2022.

tl;dr index:

  - [This is a lab class and here are the labs](./labs/README.md).
  - [Course website](https://github.com/dddrrreee/cs240lx-22spr).
  - [Course newsgroup](https://github.com/dddrrreee/cs240lx-22spr).
  - To get the repository: 

        git clone git@github.com:dddrrreee/cs240lx-22spr.git

    Put the path to the repo in your shell startup file:

    E.g. for `tcsh` I put the following in my `.tcshrc` file (and
    did `source ~/.tcshrc` after):
 
        setenv CS240LX_2022_PATH /home/engler/class/cs240lx-22spr/

    If these steps worked, you should be able to type `make` and have
    it work.

This is a implementation-heavy, lab-based class that will cover similar
topics as CS240 --- threads, virtual memory, file systems and distributed
systems --- but by writing code versus discussing papers.  After taking an
initial operating systems course you are often left in the odd situation
of having spent significant time getting the background knowledge needed
to do interesting actions, but not being left with enough time to actually
do them.  This course attacks this problem by assuming basic knowledge and
using it as a springboard to advance quickly through a set of powerful,
useful techniques that even advanced practioners can be ignorant of.

We will write custom code from scratch for the widely-used, ARM-based
raspberry pi; our code will run "bare-metal" without an operating system,
which means we can do interesting things without constantly fighting
with a lumbering OS that cannot get out of its own way.

By combining research insights (many unpublished) and our our lightweight
bare-metal code we will be in the unusual position of having foundational
abilities that most people have assumed are in-practice impossible.
As one example, we will implement fast, flexible exception handling
and then use this ability to build a variety of tools that find race
conditions, check code correctness, and memory  corruption.  By using
exceptions rather than binary rewriting we can build tools that find
similar errors as Purify / Valgrind, but can be implemented in merely
hundreds rather than hundreds of thousands of lines of code; our tools
will also be faster and more extensible.

Our initial plan was to do ten projects, one per week, where each project
covers two labs of (at a minimum) several hours each and a non-trivial
amount of outside work.

The course workload is significant, but I will aim to not waste your time.
CS140E is strongly encouraged as a prerequisite, but a sufficiently
talented and motivated implementor can make up for its lack.

I'm teaching this course because doing CS140E was was so much fun.
Each time I drove into lab, I would get an idea for one or two additional
labs I really wanted to do, and each time I wrote up an lab there would
be a long `NOTES.txt` of additional ideas.  After last year and this, I
have a big document of all the possible ideas.  This class will implement
the ones that seem the most interesting.

Also, we have Akshay for another quarter as a TA so it seems sensible
to exploit that happy fact!
