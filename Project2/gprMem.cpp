/*******
	COMP 4300
	Cameron Mathis
	Project 2
	10/20/20
	General Purpose Register Machine Memory Simulation
********/

#include <cstdlib>
#include <cstdint>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <cstring>

#define TEXT_LENGTH  50
#define	DATA_LENGTH  50
#define STACK_LENGTH  50

using namespace std;

/*******
	Data Structures
********/
typedef uint32_t mem_addr;
typedef uint32_t instruction;

mem_addr text_top = 0x00001000;
mem_addr data_top = 0x00002000;
mem_addr stack_top = 0x00003000;

// Kernal data starts at mem_addr 0, omitted because we don't use it in this simulation
instruction text_segment[TEXT_LENGTH];
mem_addr data_segment[DATA_LENGTH];
mem_addr stack_segment[STACK_LENGTH];

class Memory {
	public:
		Memory();
		bool load_code(mem_addr memory_address_in);			
    	mem_addr * read(mem_addr memory_address_in);					
    	bool load_string(mem_addr m_add, char string_to_be_stored[]);	
    	string read_string(mem_addr memory_address);					
    	mem_addr read_byte(mem_addr memory_address_in, int byte);		
	private:
		int decode_address_bin(mem_addr memory_address_in);				
		int decode_address_index(mem_addr memory_address_in);	
		// Internal counter for text_segment		
		int text_next_open_memory_location;								
		int length_of_string(mem_addr memory_address_in, int max_length);
		mem_addr mem_byte(instruction data_in,int byte_number);			
		mem_addr mem_byte_string(instruction data_in,int byte_number);  
};

/*******
	Class Definition 
********/

// Initialize memory
Memory::Memory() {
	text_next_open_memory_location = -1;
	int hexidecimal1;
	int hexidecimal2;
	int hexidecimal3;
	string textLine;
	string dataAddress = "0000000000";
	char dataArray[41];
	memset(dataArray,'\0',41);
	int i = 0;
	ifstream gpr_file_code ("palindrome.s");
	if (gpr_file_code.is_open()) {
		while ( getline (gpr_file_code,textLine)) {
			if (textLine == "") {continue;}
			if (textLine == ".data") {i = 1; continue;}
			if (textLine == ".text") {continue;}	
			if (i == 0)	{	// Text
				hexidecimal1 = std::stoi(textLine.c_str(),0, 16);
				load_code(hexidecimal1);
			}
			if (i == 1) {	// Data
				for (int c = 0; c < 10; c++) {dataAddress[c] = textLine[c];}
				for (int c = 0;c <40; c++) {
					if (dataArray[c] == '\0'){dataArray[c] = '\0';}
					dataArray[c] = textLine[c+11];
				}
				hexidecimal2 = std::stoi(dataAddress.c_str(),0, 16);
				load_string(hexidecimal2, dataArray);
			}	
		}
	} else {
		cout << "Error: Unable to open file."; 
	}
	gpr_file_code.close();
}

// Loads from .text section
bool Memory::load_code(mem_addr memory_address_in) {
	text_next_open_memory_location++;
	// Checks memory length										
	if (text_next_open_memory_location < TEXT_LENGTH) {	
		// Stores instruction
		text_segment[text_next_open_memory_location] = memory_address_in;	
		return true;
	} else {		
		// No More memory open
		cout << "Error: Please expand space for Text Memory." << endl;
		return false;														
	}
}

// Reads based on given memory address
// Used in the load_next_instruction() method of the simulator
mem_addr * Memory::read(mem_addr memory_address_in) {	
	mem_addr memory_copy_bin = memory_address_in, memory_copy_index = memory_address_in;
	switch(decode_address_bin(memory_copy_bin)) {
		case 1: {
			int memory_index = (int) decode_address_index(memory_copy_index);	
			// Checks text memory length								
			if (memory_index < TEXT_LENGTH)	{									
				return &text_segment[memory_index];
			}
		} break;
		case 2: {
			int memory_index = (int) decode_address_index(memory_copy_index);
			// Checks data memory length
			if (memory_index < DATA_LENGTH)	{										
				return &data_segment[memory_index];									
			}
		} break;
		case 3: {
			int memory_index = (int) decode_address_index(memory_copy_index);
			// Checks stack memory length
			if (memory_index < STACK_LENGTH) {									
				return &stack_segment[memory_index];									
			}
		} break;
		default: {
			// Not in current memory space
			cout << "Error: Memory read is not within current memory." << endl;
			return &stack_top;														
		} break;
	}
	cout << "Error: Memory read went wrong." << endl;
	return &stack_top;
}

// Write given string to memory
// Used in the system call portion of the simulator
bool Memory::load_string(mem_addr m_add, char string_to_be_stored[]) {
	switch(decode_address_bin(m_add)) {
		case 1: { // TEXT  
			cout << "Error: There was an error loading your string into TEXT segment of memory." << endl;
			return false;
		}
		case 2: { // DATA
			int data_index = decode_address_index(m_add);
			if (data_index < DATA_LENGTH) {
				memset(&data_segment[data_index], 0, strlen(string_to_be_stored)+5);
				memcpy(&data_segment[data_index], string_to_be_stored,  strlen(string_to_be_stored)+1);
				return true;									
			}
			return false;
		}
		case 3: { // STACK
			int data_index = decode_address_index(m_add);
			if (data_index < STACK_LENGTH) {
				memset(&data_segment[data_index], 0, strlen(string_to_be_stored)+5);
				memcpy(&stack_segment[data_index], string_to_be_stored,  strlen(string_to_be_stored)+1);
				return true;
			}
			return false;
		}
		default : {
			cout << "Error: There was an error loading your string into memory, Bin not selected" << endl;
			return false;
		}
	}
	cout << "Error: There was an error loading your string into memory" << endl;
	return false;
}

// Read a string from memory
// Built for reading strings across memory segments
// Used in the system call portion of the simulator
string Memory::read_string(mem_addr memory_address) {	
	switch(decode_address_bin(memory_address)) {
		case 1: { // TEXT  
			cout << "Error: There was an error loading your string into TEXT segment of memory." << endl;
			return "Error";
		}
		case 2: { // DATA
			int data_index = decode_address_index(memory_address);
			char *data_out;
			data_out = (char*) malloc( length_of_string(memory_address, 2000));
			if (data_index < DATA_LENGTH) {
				memcpy(data_out, &data_segment[data_index],  length_of_string(memory_address, 2000));
				return string(data_out);									
			}
			return "Error";
		}
		case 3: { // STACK
			int data_index = decode_address_index(memory_address);
			char *data_out;
			data_out = (char*) malloc( length_of_string(memory_address, 2000));
			if (data_index < STACK_LENGTH) {
				memcpy(data_out, &stack_segment[data_index],  length_of_string(memory_address, 2000));
				return string(data_out);									
			}
			return "Error";
		}
		default :{
			cout << "Error: There was an error loading your string into memory, Bin not selected" << endl;
			return "Error";
		}
	}
	cout << "Error: There was an error loading your string into memory" << endl;
	return "Error";
}

// Reads byte at given address
// Used in theload byte portion of the simulator
mem_addr Memory::read_byte(mem_addr memory_address_in, int byte) {
	mem_addr memory_copy_bin = memory_address_in, memory_copy_index = memory_address_in;
	int memory_index = (int) decode_address_index(memory_copy_index);
	memory_index = (int) floor(memory_index/4.0);	
	mem_addr memory_value=0;
	switch(decode_address_bin(memory_copy_bin)) {
		case 1: {
			// Checks text memory length			
			if (memory_index < TEXT_LENGTH) {
				memory_value= text_segment[memory_index];
			}
		} break;
		case 2: {
			// Checks data memory length
			if (memory_index < DATA_LENGTH)	{
				memory_value= data_segment[memory_index];									
			}
		} break;
		case 3: {
			// Checks stack memory length
			if (memory_index < STACK_LENGTH) {
				memory_value= stack_segment[memory_index];									
			}
		} break;
		default: {
			// Not in current memory space
			cout << "Error: Memory read is not within current memory." << endl;
			memory_value= stack_top;													
		} break;
	}
	return mem_byte_string(memory_value, byte+1);
}

// Helps decode address into bin
// Wipes out everything but the bin bits
int Memory::decode_address_bin(mem_addr memory_address_in) {		
	// Shifts all bits to the left 	16													
	memory_address_in = memory_address_in << 16;
	// Shifts all bits to the right 28
	memory_address_in = memory_address_in >> 28;
	return memory_address_in;
	// 0 = kernal, 1 = text, 2 = data, 3 = stack, and (-1) = error
}

// Helps decode address into array index
// Removes the (potential) op code and bin
int Memory::decode_address_index(mem_addr memory_address_in) {
	// Shifts all bits to the left 20														
	memory_address_in = memory_address_in << 20;
	// Shifts all bits to the right 20
	memory_address_in = memory_address_in >> 20;
	return memory_address_in;
}

// Helper to find the end of string in memory
int Memory::length_of_string(mem_addr memory_address_in, int max_length) {
	switch(decode_address_bin(memory_address_in)) {
		case 1: { // TEXT 
			cout << "Error: There was an error finding the length of a string in TEXT segment of memory." << endl;
			return 0;
		}
		case 2: { // DATA
			int data_index = decode_address_index(memory_address_in);
			// Checks data memory length
			if (data_index < DATA_LENGTH) {
				bool end_not_found = true;
				int length =0;
				mem_addr current_byte=0;
				while(end_not_found && length < max_length) {
					current_byte = mem_byte( data_segment[data_index], 1+(length %4));
					if(0 == current_byte) {
						end_not_found = false;
					}
					length++;
				}
				return length--;
			}
			return 0;
		}
		case 3: { // STACK
			int data_index = decode_address_index(memory_address_in);
			// Checks data memory length
			if (data_index < STACK_LENGTH) {
				bool end_not_found = true;
				int length =0;
				mem_addr current_byte =0;
				while(end_not_found && length < max_length) {
					current_byte = mem_byte(stack_segment[data_index], 1+(length %4));
					if(0 == current_byte) {
						end_not_found = false;
					}
					length++;
				}
				return length--;
				
			}
			return 0;
		}
		default: {
			cout << "Error: There was an error finding the length of your string, Bin not selected" << endl;
			return 0;
		}
	}
	cout << "Error: There was an error finding the length of your string in memory" << endl;
	return 0;
}

// Returns a byte inside the instruction
mem_addr Memory::mem_byte(instruction data_in, int byte_number) {																					
	if (byte_number < 5 && byte_number > 0) {
		byte_number --;
		instruction data;
		data = data_in;
		data = data << 8*byte_number;
		data = data >> 24;
		return data;
	}
	cout << "Error: Memory byte read" << endl;
	return 0;
} 

// Returns a byte for a string
mem_addr Memory::mem_byte_string(instruction data_in, int byte_number) {																					
	if (byte_number < 5 && byte_number > 0) {
		byte_number --;
		switch(byte_number)	{ // Had to be done because of big Indian to little Indian flip
			case 0: {
				byte_number =3;
				break;
			}
			case 1: {
				byte_number =2;
				break;
			}
			case 2: {
				byte_number =1;
				break;
			}
			case 3: {
				byte_number =0;
				break;
			}
			default: {
				break;
			}
		}
		instruction data;
		data = data_in;
		data = data << 8*byte_number;
		data = data >> 24;
		return data;
	}
	cout << "Error: Memory STRING byte read" << endl;
	return 0;
} 