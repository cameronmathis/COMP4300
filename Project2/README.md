COMP 4300
=====================
Cameron Mathis - clm0081 </br>
Project 2 - Attempt 2 </br>
11/03/20


Overview: 
-------------

I extended my accumulator-based machine into a General Purpose Register (GPR) machine that runs different instructions in different numbers of cycles (i.e. a multi-cycle machine). The code this machine runs can be found in gprCode.txt.

The written code evaluates if an entered word is a palindrome. For this program spaces are seen as an ending character. For example:

> Please enter a word: nascar </br>
> "The string is not a palindrome."

or

> Please enter a word: racecar </br>
> "The string is a palindrome."

To run the machine, look below!


Simple MIPS Machine
-------------

#### Compile MIPS Machine ####

In order to compile the GPR, navigate to the project folder and run the command:
	
	$ ./gpr.sh compile

In order to run the GPR, run the command: 

	$ ./gpr.sh run

You should see this:

>Please enter a word:

Type any word and hit enter.

Once you are done simulating the GPR run:
	
	$ ./gpr.sh clean

This will clean up the project folder.


Notes
-------------

#### "palindrome.s" File ####
************************************

This file has two main sections; ".text" and ".data".

>".text"

This is where the binary for the code is.

>".data"

This is where the beginning information is provided for the computation.


#### Thoughts and Issues ####
************************************

> I ran into a problem loading data to memory when simply using my same functions as project1, so a few changes had to be made. 
