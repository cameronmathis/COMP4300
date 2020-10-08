COMP 4300
=====================
Cameron Mathis - clm0081 </br>
Project 2 </br>
10/20/20 </br>


Overview: 
-------------

I upgraded out single register accumulator to a functioning MIPS machine. Not all instructions of the MIPS ISA are implemented in this machine, but 12 are. The code this machine runs can be found in gprCode.txt.

The written code evaluates if an entered word is a palindrome. For this program spaces are seen as an ending character. For example:

> Please enter a word: nascar </br>
> The string is not a palindrome.

or

> Please enter a word: racecar </br>
> The string is a palindrome.

To run the machine, look below!


Simple MIPS Machine
-------------

#### Compile MIPS Machine ####

Open up the terminal navigate to this project and run:
	
	$ ./gpr.sh compile

You should see this:

>g++ -c gprMem.cpp -o gprMemory.out </br>
>g++ -c gprReg.cpp -o gprRegister.out </br>
>g++ gprSim.cpp -o gprSimulator.out 

Run: 

	$ ./gpr.sh run

You should see this:

>Please enter a word:

Enter any word and hit enter.

Once you are done run:
	
	$ ./gpr.sh clean

This will clean up the folder.


Notes
-------------

#### "palindrome.s" File ####
************************************

If you take a look in these, they will have two main sections; ".text" and ".data".

>".text"

This is where the binary for the code is.

>".data"

This is where the beginning information is provided for the computation.


#### Thoughts and Issues ####
************************************

###### MIPS in general ######

> I ran into a bug where the 32bit integer notation actually flipped the way the characters were stored. Some special functions had to be built to retrieve strings. 