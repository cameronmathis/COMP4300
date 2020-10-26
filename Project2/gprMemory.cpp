/*******
	COMP 4300
	Cameron Mathis
	Project 2
	10/20/20
	Memory Simulation
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
typedef uint32_t memoryAddress;
typedef uint32_t instruction;

memoryAddress text_top = 0x00001000;
memoryAddress data_top = 0x00002000;
memoryAddress stack_top = 0x00003000;

// Kernal data starts at memoryAddress 0, omitted because we don't use it in this simulation
instruction textSegment[TEXT_LENGTH];
memoryAddress dataSegment[DATA_LENGTH];
memoryAddress stackSegment[STACK_LENGTH];

class Memory {
	public:
		Memory();
		bool loadCode(memoryAddress memoryAddressIn);					
    	bool loadString(memoryAddress memoryAddressIn, char stringToBeStored[]);				
    	memoryAddress * read(memoryAddress memoryAddressIn);
    	string read_string(memoryAddress memoryAddress);					
    	memoryAddress read_byte(memoryAddress memoryAddressIn, int byte);		
	private:
		int decode_address_bin(memoryAddress memoryAddressIn);				
		int decode_address_index(memoryAddress memoryAddressIn);	
		// Internal counter for textSegment		
		int textNextOpenMemoryLocation;								
		int length_of_string(memoryAddress memoryAddressIn, int max_length);
		memoryAddress mem_byte(instruction data_in,int byte_number);			
		memoryAddress mem_byte_string(instruction data_in,int byte_number);  
};

/*******
	Class Definition 
********/

// Initialize memory
Memory::Memory() {
	textNextOpenMemoryLocation = -1;
	int hexidecimalOne;
	int hexidecimalTwo;
	int hexidecimalThree;
	string lineOne;
	string lineTwo = "0000000000";
	char dataArray[41];
	memset(dataArray, '\0', sizeof(dataArray)/sizeof(dataArray[0]));
	int i = 0;
	ifstream gprFileCode ("palindrome.s");
	if (gprFileCode.is_open()) {
		while ( getline (gprFileCode,lineOne)) {
			if (lineOne == "") {
				continue;
			}
			if (lineOne == ".text") {
				continue;
			}
			if (lineOne == ".data") {
				i = 1; 
				continue;
			}	
			// Text
			if (i == 0)	{	// Store lineOne as hexidecimal
				sscanf(lineOne.data(),"%x", &hexidecimalOne);
				loadCode(hexidecimalOne);
			}
			// Data
			if (i == 1) {	
				for (int ch = 0; ch < 10; ch++) {
					lineTwo[ch] = lineOne[ch];
				}
				// Store lineTwo as hexidecimal
				sscanf(lineTwo.data(), "%x", &hexidecimalTwo);
				// Store lineThree as hexidecimal
				hexidecimalTwo = atoi(lineTwo.c_str());
				for (int ch = 0; ch < 40; ch++) {
					dataArray[ch] = lineOne[ch+11];
				}
				loadString(hexidecimalTwo, dataArray);
			}	
		}
	} else {
		cout << "Error: Unable to open file."; 
	}
	gprFileCode.close();
}

/* Loads from .text section -- exact same as project 1 */
bool Memory::loadCode(memoryAddress memoryAddressIn) {
	textNextOpenMemoryLocation++;
	// Checks memory length										
	if (textNextOpenMemoryLocation < TEXT_LENGTH) {	// Stores instruction
		textSegment[textNextOpenMemoryLocation] = memoryAddressIn;	
		return true;
	} else { // No More memory open
		cout << "Error: Please expand space for Text Memory." << endl;
		return false;														
	}
}

/* Write the given string to memory */
bool Memory::loadString(memoryAddress memoryAddressIn, char stringToBeStored[]) {
	switch(decode_address_bin(memoryAddressIn)) {
		case 1: { // TEXT  
			cout << "Error: There was an error loading your string into TEXT segment of memory." << endl;
			return false;
		}
		case 2: { // DATA
			int dataIndex = decode_address_index(memoryAddressIn);
			// Checks the memory length
			if (dataIndex < DATA_LENGTH) {
				memset(&dataSegment[dataIndex], 0, strlen(stringToBeStored)+5);
				memcpy(&dataSegment[dataIndex], stringToBeStored,  strlen(stringToBeStored)+1);
				return true;									
			}
			return false;
		}
		case 3: { // STACK
			int dataIndex = decode_address_index(memoryAddressIn);
			if (dataIndex < STACK_LENGTH) {
				memset(&dataSegment[dataIndex], 0, strlen(stringToBeStored)+5);
				memcpy(&stackSegment[dataIndex], stringToBeStored,  strlen(stringToBeStored)+1);
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

/* Read the memory at the given memory address */
memoryAddress * Memory::read(memoryAddress memoryAddressIn) {	
	memoryAddress memory_copy_bin = memoryAddressIn, memory_copy_index = memoryAddressIn;
	switch(decode_address_bin(memory_copy_bin)) {
		case 1: {
			int memory_index = (int) decode_address_index(memory_copy_index);	
			// Checks text memory length								
			if (memory_index < TEXT_LENGTH)	{									
				return &textSegment[memory_index];
			}
		} break;
		case 2: {
			int memory_index = (int) decode_address_index(memory_copy_index);
			// Checks data memory length
			if (memory_index < DATA_LENGTH)	{										
				return &dataSegment[memory_index];									
			}
		} break;
		case 3: {
			int memory_index = (int) decode_address_index(memory_copy_index);
			// Checks stack memory length
			if (memory_index < STACK_LENGTH) {									
				return &stackSegment[memory_index];									
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

// Read a string from memory
// Built for reading strings across memory segments
// Used in the system call portion of the simulator
string Memory::read_string(memoryAddress memoryAddress) {	
	switch(decode_address_bin(memoryAddress)) {
		case 1: { // TEXT  
			cout << "Error: There was an error loading your string into TEXT segment of memory." << endl;
			return "Error";
		}
		case 2: { // DATA
			int dataIndex = decode_address_index(memoryAddress);
			char *data_out;
			data_out = (char*) malloc( length_of_string(memoryAddress, 2000));
			if (dataIndex < DATA_LENGTH) {
				memcpy(data_out, &dataSegment[dataIndex],  length_of_string(memoryAddress, 2000));
				return string(data_out);									
			}
			return "Error";
		}
		case 3: { // STACK
			int dataIndex = decode_address_index(memoryAddress);
			char *data_out;
			data_out = (char*) malloc( length_of_string(memoryAddress, 2000));
			if (dataIndex < STACK_LENGTH) {
				memcpy(data_out, &stackSegment[dataIndex],  length_of_string(memoryAddress, 2000));
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
memoryAddress Memory::read_byte(memoryAddress memoryAddressIn, int byte) {
	memoryAddress memory_copy_bin = memoryAddressIn, memory_copy_index = memoryAddressIn;
	int memory_index = (int) decode_address_index(memory_copy_index);
	memory_index = (int) floor(memory_index/4.0);	
	memoryAddress memory_value=0;
	switch(decode_address_bin(memory_copy_bin)) {
		case 1: {
			// Checks text memory length			
			if (memory_index < TEXT_LENGTH) {
				memory_value= textSegment[memory_index];
			}
		} break;
		case 2: {
			// Checks data memory length
			if (memory_index < DATA_LENGTH)	{
				memory_value= dataSegment[memory_index];									
			}
		} break;
		case 3: {
			// Checks stack memory length
			if (memory_index < STACK_LENGTH) {
				memory_value= stackSegment[memory_index];									
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
int Memory::decode_address_bin(memoryAddress memoryAddressIn) {		
	// Shifts all bits to the left 	16													
	memoryAddressIn = memoryAddressIn << 16;
	// Shifts all bits to the right 28
	memoryAddressIn = memoryAddressIn >> 28;
	return memoryAddressIn;
	// 0 = kernal, 1 = text, 2 = data, 3 = stack, and (-1) = error
}

// Helps decode address into array index
// Removes the (potential) op code and bin
int Memory::decode_address_index(memoryAddress memoryAddressIn) {
	// Shifts all bits to the left 20														
	memoryAddressIn = memoryAddressIn << 20;
	// Shifts all bits to the right 20
	memoryAddressIn = memoryAddressIn >> 20;
	return memoryAddressIn;
}

// Helper to find the end of string in memory
int Memory::length_of_string(memoryAddress memoryAddressIn, int max_length) {
	switch(decode_address_bin(memoryAddressIn)) {
		case 1: { // TEXT 
			cout << "Error: There was an error finding the length of a string in TEXT segment of memory." << endl;
			return 0;
		}
		case 2: { // DATA
			int dataIndex = decode_address_index(memoryAddressIn);
			// Checks data memory length
			if (dataIndex < DATA_LENGTH) {
				bool end_not_found = true;
				int length =0;
				memoryAddress current_byte=0;
				while(end_not_found && length < max_length) {
					current_byte = mem_byte( dataSegment[dataIndex], 1+(length %4));
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
			int dataIndex = decode_address_index(memoryAddressIn);
			// Checks data memory length
			if (dataIndex < STACK_LENGTH) {
				bool end_not_found = true;
				int length =0;
				memoryAddress current_byte =0;
				while(end_not_found && length < max_length) {
					current_byte = mem_byte(stackSegment[dataIndex], 1+(length %4));
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
memoryAddress Memory::mem_byte(instruction data_in, int byte_number) {																					
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
memoryAddress Memory::mem_byte_string(instruction data_in, int byte_number) {																					
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