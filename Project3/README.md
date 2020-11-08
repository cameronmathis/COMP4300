COMP 4300
=====================
Cameron Mathis - clm0081
Project 3
11/29/20

Overview:
-------------

In this lab I extended my migrated my multi-cycle machine to a pipe line machine.

Pipe Line Machine
-------------

In order to compile the pipe line machine, navigate to the project folder and run the command:
	
	$ ./pipeline.sh compile

In order to run the pipe line machine, run the command: 

	$ ./pipeline.sh run

You should see this:

>Please enter a word:

Type any word and hit enter.

Once you are done simulating the pipe line machine run:
	
	$ ./pipeline.sh clean

This will clean up the project folder.

Results:
-------------

Please enter a word: hahahah <br/>
"The string is a palindrome." <br/>
Number of Instructions Executed (IC): 65 <br/>
Number of Cycles Spent in Execution (C): 124 <br/>
Number of NOPs: 60 <br/>

Notes
-------------

#### "lab3c.s" File ####

This file has two main sections; ".text" and ".data".

>".text"

This is where the binary for the code is.

>".data"

This is where the beginning information is provided for the computation.


#### Thoughts and Issues ####
************************************

A couple of things:

One, this migration wasn't all that hard, it was just extremely tedious. All of instruction logic had already been written, so it was
just the simple task of splitting it into the five different pipe line steps. This quickly turned out to be quite confusing.

One of the main issues we had was forwarding logic. Our Machine works fine without any data hazards and a LOT  of NOPs. Adding in the forwarding logic
took much more time than expected and is still not 100%. The programs we had to run for this project work, but that is about it.

The more we thought about being able to run just part of the pipe line, the more we realized we need all of it. We can't just run part of the pipe Line
and hope that the rest will be taken care of. There is no easy way to simply remove a segment of the pipe line.
