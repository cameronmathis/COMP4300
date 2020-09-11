COMP 4300
=====================
Cameron Mathis <br/>
Project 1 <br/>
09/18/20

Overview: 
-------------

There are two simulators, one is a stack Machine and one is an Accumulator. <br/>
They both read in code from their respective "stackCode.txt" and "accumCode.txt" files.

Each simulator has two parts. The memory simulation and the processor/instruction simulation.

The simulators are trying to evaluate this equation: 

>A*X**2 + B*X + C
>> - x = 3
>> - A = 7
>> - B = 5
>> - C = 4
>
> Answer: 82

To run each machine, look below.



Stack Machine
-------------

In order to compile the stack machine, open up the terminal navigate to this project and run:
	
	$ ./stack.sh compile

You should see this:

>g++ -c stackMem.cpp -o stackMem.out <br/>
>g++ stackSim.cpp -o stackSimulator.out

Run: 

	$ ./stack.sh run

You should see this:

>82 was popped from the top of the sack. <br/>

Once you are done run:
	
	$ ./stack.sh clean

This will clean up the folder by removing the stackSimulator.out and stackMemory.out files.


Accumulator
-------------

In order to compile the stack machine, open up the terminal navigate to this project and run:
	
	$ ./accum.sh compile

You should see this:

>g++ -c accumMem.cpp -o accumMem.out <br/>
>g++ accumSim.cpp -o accumSimulator.out

Run:

	$ ./accum.sh run

You should see this:

>82 is in the internal register. <br/>

Once you are done run:

	$ ./accum.sh clean

This will clean up the folder by removing the accumSimulator.out and accumMemory.out files.

Notes
-------------

#### "stackCode.txt" and "accumCode.txt" Files ####

If you take a look in these, they will have two main sections; ".text" and ".data".

>".text"

This is where the binary for the code is.

>".data"

This is where the beginning information is provided for the computation.


#### Thoughts and Issues ####
************************************

###### Memory Address Length ######

> Dealing with the memory address length and then having an instruction length was a particularly hard/confusing subject. We ended up going with the instruction being 32 bits with an op code having 8 bits on the left hand side. This meant that our memory address is only 24 bits. Unfortunately defining a 40 bit instruction didn't seem feasible.


###### Instruction Encoding ######

> With the requirment of supporting 140 operations, we were forced to have an 8 bit op code. <br/>
> 2^7 = 128 (Not Enough)<br/>
> 2^8 = 265 <br/>
> At first we were concerned this was going to be the largest hurdle to overcome. It ended up being taken care of with a few well placed bit shifts. In total only about 15 lines of code took care of all the operations we needed to decode out encoding. 
