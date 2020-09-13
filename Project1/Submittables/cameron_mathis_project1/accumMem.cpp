/*
COMP 4300
Cameron Mathis
Project 1
09/18/20
Accumulator Machine Simulation
*/

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

#define	DATA_LENGTH  50
#define TEXT_LENGTH  100
#define STACK_LENGTH  50

using namespace std;

typedef unsigned int uint32;
typedef uint32 mem_addr;
typedef uint32 instruction;

mem_addr text_top = 0x00100000;
mem_addr data_top = 0x00200000;
mem_addr stack_top = 0x00300000;

// Kernal data starts at mem_addr 0, ommited because it isn't used in this simulation
instruction text_segment[TEXT_LENGTH];
mem_addr data_segment[DATA_LENGTH];
mem_addr stack_segment[STACK_LENGTH];


class Memory
{
public:
	Memory();
	bool load_code(mem_addr memory_address_in);						
	bool load_data(mem_addr memory_address_in, mem_addr data);		
    bool write(mem_addr memory_address_in, mem_addr data);			
    mem_addr * read(mem_addr memory_address_in);					
private:
	int decode_address_bin(mem_addr memory_address_in);				
	int decode_address_index(mem_addr memory_address_in);			
	// Internal counter for text_segment
	int text_next_open_memory_location;								
};

Memory::Memory()  													// Initialize memory
{
	text_next_open_memory_location = -1;
	int hexidecimal;
	int hexidecimal1;
	int hexidecimal2;
	string line;
	string line1 = "0000000000";
	string line2 = "0";
	int i = 0;
	ifstream accum_file_code ("accumCode.txt");
	if (accum_file_code.is_open())
	{
		while ( getline (accum_file_code,line))
		{
			if (line == ""){continue;}
			if (line == ".data"){i = 1; continue;}
			if (line == ".text"){i = 0; continue;}	
			if (i == 0) 												// Text
			{
				hexidecimal = std::stoi(line.c_str(),0, 16);
				load_code(hexidecimal);
			}
			if (i == 1) 												// Data
			{
				for (int c = 0; c < 10; c++)
				{
					line1[c] = line[c];
					line2[0] = line[11];
				}
				hexidecimal1 = std::stoi(line1.c_str(),0, 16);
				hexidecimal2 = atoi(line2.c_str());
				load_data(hexidecimal1,hexidecimal2);
			}	
		}
	}
	else{
		cout << "Error: Unable to open file."; 
	}
	accum_file_code.close();
}

// Loads from .text section
bool Memory::load_code(mem_addr memory_address_in)
{
	text_next_open_memory_location++;					
	// Checks the memory length					
	if (text_next_open_memory_location < TEXT_LENGTH)						
	{ // Stores the instruction
		text_segment[text_next_open_memory_location] = memory_address_in;	
		return true;
	}
	else
	{	// No More memory open
		cout << "Error: Please expand space for Text Memory." << endl;
		return false;														
	}
}

// Loads from the .data section
bool Memory::load_data(mem_addr memory_address_in, mem_addr data)
{
	mem_addr memory_copy_index = memory_address_in;
	
	int memory_index = (int) decode_address_index(memory_copy_index);
	// Checks the memory length
	if (text_next_open_memory_location < DATA_LENGTH)						
	{	// Stores the data
		data_segment[memory_index] = data;									
		return true;
	}
	else
	{	// No More memory open
		cout << "Error: Please expand space for Data Memory." << endl;
		return false;														
	}
}

// Writes to stack section. Used by STORE
bool Memory::write(mem_addr memory_address_in, mem_addr data)
{
	mem_addr memory_copy_bin = memory_address_in, memory_copy_index = memory_address_in;
	switch(decode_address_bin(memory_copy_bin))
	{
	case 1:
			cout << "Error: You do not have the correct user privileges to write to text segment." << endl;
			return false;
		break;
	case 2:
			cout << "Error: You do not have the correct user privileges to write to data segment." << endl;
			return false;
		break;
	case 3:
		{
			int memory_index = (int) decode_address_index(memory_copy_index);
			// Checks the memory length
			if (text_next_open_memory_location < STACK_LENGTH)						
			{	// Store data in the stack
				stack_segment[memory_index] = data;									
				return true;
			}
			else
			{	// No more stack open
				cout << "Error: Please expand space for Stack Memory" << endl;
				return false;														
			}
		}
		break;
	default:	// Not in current memory
			cout << "Error: You cannot write to that memory area." << endl;
			return false;															
		break;
	}
	cout << "Error: Memory write went wrong." << endl;
	return false;
}

// Reads based on given memory address. Used by LOAD, ADD, and MULT.
mem_addr * Memory::read(mem_addr memory_address_in )
{	
	mem_addr memory_copy_bin = memory_address_in, memory_copy_index = memory_address_in;
	switch(decode_address_bin(memory_copy_bin))
	{
	case 1:
		{
			int memory_index = (int) decode_address_index(memory_copy_index);	
			// Checks the text memory length								
			if (memory_index < TEXT_LENGTH)											
			{
				return &text_segment[memory_index];
			}
		}
		break;
	case 2:
		{
			int memory_index = (int) decode_address_index(memory_copy_index);
			// Checks the data memory length
			if (text_next_open_memory_location < DATA_LENGTH)						
			{
				return &data_segment[memory_index];									
			}
		}
		break;
	case 3:
		{
			int memory_index = (int) decode_address_index(memory_copy_index);
			// Checks the stack memory length
			if (text_next_open_memory_location < STACK_LENGTH)						
			{
				return &stack_segment[memory_index];									
			}
		}
		break;
	default:	// Not in current memory space
			cout << "Error: Memory read is not within current memory." << endl;
			return &stack_top;														
		break;
	}
	cout << "Error: Memory read went wrong." << endl;
	return &stack_top;
}

// Helps decode address into bin
int Memory::decode_address_bin(mem_addr memory_address_in)
{	// Wipes out everything but the bin bits
	memory_address_in = memory_address_in << 7;
	memory_address_in = memory_address_in >> 27;
	return memory_address_in;
	//(-1) -- false
	// 0--kernal
	// 1--text
	// 2--data
	// 3--stack
}

// Helps decode address into array index
int Memory::decode_address_index(mem_addr memory_address_in)
{	// Removes the (potential) op code and bin
	memory_address_in = memory_address_in << 15;
	memory_address_in = memory_address_in >> 15;
	return memory_address_in;
}