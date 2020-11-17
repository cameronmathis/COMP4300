COMP 4300
=====================
Cameron Mathis - clm0081
Project 3
11/29/20

Overview:
-------------

In this lab I extended my migrated my multi-cycle machine to a pipeline machine.

Pipe Line Machine
-------------

In order to compile the pipeline machine, navigate to the project folder and run the command:
	
	$ ./pipeline.sh compile

In order to run the pipeline machine, run the command: 

	$ ./pipeline.sh run

You should see this:

>Please enter a word:

Type any word and hit enter.

Once you are done simulating the pipeline machine run:
	
	$ ./pipeline.sh clean

This will clean up the project folder.

Results:
-------------

lab3a:
> Instructions Executed (IC): 68 <br/>
> Cycles Spent in Execution (C): 263 <br/>
> No Operations (NOPs): 196 <br/>

lab3b:
> Please enter a word: racecar <br/>
> "The string is a palindrome." <br/>
> Instructions Executed (IC): 65 <br/>
> Cycles Spent in Execution (C): 124 <br/>
> No Operations (NOPs): 60 <br/>

lab3c:
> Instructions Executed (IC): 12 <br/>
> Cycles Spent in Execution (C): 16 <br/>
> No Operations (NOPs): 5 <br/>

Notes
-------------

#### "lab3a.s, lab3b.s, and lab3c.s" Files ####

This file has two main sections; ".text" and ".data".

>".text"

This is where the binary for the code is.

>".data"

This is where the beginning information is provided for the computation.


#### Thoughts and Issues ####
************************************

A couple of things:

Firstly, I ran into the same issue I had in Project 1 when reading from a file, even thought I copied my memory file exactly from Project 2 (where it worked). I eventually figured it out.

Secondly, I quickly learned that it would be extremely hard to implement and test only one part of the pipeline. As a result I had to have some code for each stage written before I could run my simulation.

One of the main issues we had was forwarding logic. Our Machine works fine without any data hazards and a LOT of NOPs. Adding in the forwarding logic
took much more time than expected and is still not 100%. The programs we had to run for this project works, but that is about it.
