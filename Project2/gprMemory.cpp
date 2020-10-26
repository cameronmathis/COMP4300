/*******
	COMP 4300
	Cameron Mathis
	Project 2 - Attempt 2
	11/03/20
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

typedef uint32_t memoryAddress;
typedef uint32_t instruction;

memoryAddress textTop = 0x00001000;
memoryAddress dataTop = 0x00002000;
memoryAddress stackTop = 0x00003000;

// Kernal data starts at memoryAddress 0, omitted because we don't use it in this simulation
instruction textSegment[TEXT_LENGTH];
memoryAddress dataSegment[DATA_LENGTH];
memoryAddress stackSegment[STACK_LENGTH];

class Memory {
	public:
		Memory();
		bool loadCode(memoryAddress memoryAddressIn);					
    	bool loadData(memoryAddress memoryAddressIn, char stringToBeStored[]);				
    	memoryAddress * readFromMemory(memoryAddress memoryAddressIn);
    	string read_string(memoryAddress memoryAddress);					
    	memoryAddress read_byte(memoryAddress memoryAddressIn, int byte);		
	private:
		int decodeAddressBin(memoryAddress memoryAddressIn);				
		int decodeAddressIndex(memoryAddress memoryAddressIn);	
		// Internal counter for textSegment		
		int textNextOpenMemoryLocation;								
		int length_of_string(memoryAddress memoryAddressIn, int max_length);
		memoryAddress mem_byte(instruction data_in, int byte_number);			
		memoryAddress mem_byte_string(instruction data_in, int byte_number);  
};

/*******
	Class Definition 
********/

/* Initialize memory -- modified from project 1 */
Memory::Memory() {
	textNextOpenMemoryLocation = -1;
	int hexidecimalOne;
	int hexidecimalTwo;
	int hexidecimalThree;
	string lineOne;
	string lineTwo = "0000000000";
	char dataArray[DATA_LENGTH];
	memset(dataArray, '\0', sizeof(dataArray) / sizeof(dataArray[0]));
	int i = 0;
	ifstream gprFileCode ("palindrome.s");
	if (gprFileCode.is_open()) {
		while ( getline(gprFileCode, lineOne)) {
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
				hexidecimalTwo = std::stoi(lineTwo.c_str(), 0, 16);
				for (int ch = 0; ch < DATA_LENGTH - 1; ch++) {
					dataArray[ch] = lineOne[ch+11];
				}
				loadData(hexidecimalTwo, dataArray);
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

/* Write the given data string to memory -- modified from project 1 */
bool Memory::loadData(memoryAddress memoryAddressIn, char stringToBeStored[]) {
	if (decodeAddressBin(memoryAddressIn) == 2) {
		int dataIndex = decodeAddressIndex(memoryAddressIn);
		// Checks the memory length
		if (dataIndex < DATA_LENGTH) {
			// Fill data segment with 0s
			memset(&dataSegment[dataIndex], 0, strlen(stringToBeStored)+5);
			// Store string in data segment
			memcpy(&dataSegment[dataIndex], stringToBeStored,  strlen(stringToBeStored)+1);
			return true;									
		}
		return false;
	}
	cout << "There was an error loading your data into memory." << endl;
	return false;
}

/* Read the memory at the given memory address -- exact same as project 1 */
memoryAddress * Memory::readFromMemory(memoryAddress memoryAddressIn) {	
	memoryAddress memoryCopyBin = memoryAddressIn;
	memoryAddress memoryCopyIndex = memoryAddressIn;
	switch(decodeAddressBin(memoryCopyBin)) {
		case 1: {
			int memoryIndex = (int) decodeAddressIndex(memoryCopyIndex);	
			// Checks text memory length								
			if (memoryIndex < TEXT_LENGTH)	{									
				return &textSegment[memoryIndex];
			}
		} break;
		case 2: {
			int memoryIndex = (int) decodeAddressIndex(memoryCopyIndex);
			// Checks data memory length
			if (memoryIndex < DATA_LENGTH)	{										
				return &dataSegment[memoryIndex];									
			}
		} break;
		case 3: {
			int memoryIndex = (int) decodeAddressIndex(memoryCopyIndex);
			// Checks stack memory length
			if (memoryIndex < STACK_LENGTH) {									
				return &stackSegment[memoryIndex];									
			}
		} break;
		default: {
			// Not in current memory space
			cout << "Error: Memory read is not within current memory." << endl;
			return &stackTop;														
		} break;
	}
	cout << "Error: Memory read went wrong." << endl;
	return &stackTop;
}

/* Decodes address into bin -- modified from project 1 */
int Memory::decodeAddressBin(memoryAddress memoryAddressIn) {		
	// Shifts all bits to the left 	16													
	memoryAddressIn = memoryAddressIn << 16;
	// Shifts all bits to the right 28
	memoryAddressIn = memoryAddressIn >> 28;
	return memoryAddressIn;
	// 0 = kernal, 1 = text, 2 = data, 3 = stack, and (-1) = error
}

/* Decodes address into array index -- modified from project 1 */
int Memory::decodeAddressIndex(memoryAddress memoryAddressIn) {
	// Shifts all bits to the left 20														
	memoryAddressIn = memoryAddressIn << 20;
	// Shifts all bits to the right 20
	memoryAddressIn = memoryAddressIn >> 20;
	return memoryAddressIn;
}

// Read a string from memory
// Built for reading strings across memory segments
// Used in the system call portion of the simulator
string Memory::read_string(memoryAddress memoryAddress) {	
	switch(decodeAddressBin(memoryAddress)) {
		case 1: { // TEXT  
			cout << "Error: There was an error loading your string into TEXT segment of memory." << endl;
			return "Error";
		}
		case 2: { // DATA
			int dataIndex = decodeAddressIndex(memoryAddress);
			char *data_out;
			data_out = (char*) malloc( length_of_string(memoryAddress, 2000));
			if (dataIndex < DATA_LENGTH) {
				memcpy(data_out, &dataSegment[dataIndex],  length_of_string(memoryAddress, 2000));
				return string(data_out);									
			}
			return "Error";
		}
		case 3: { // STACK
			int dataIndex = decodeAddressIndex(memoryAddress);
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
	cout << "There was an error loading your string into memory" << endl;
	return "Error";
}

// Reads byte at given address
// Used in theload byte portion of the simulator
memoryAddress Memory::read_byte(memoryAddress memoryAddressIn, int byte) {
	memoryAddress memory_copy_bin = memoryAddressIn, memory_copy_index = memoryAddressIn;
	int memoryIndex = (int) decodeAddressIndex(memory_copy_index);
	memoryIndex = (int) floor(memoryIndex/4.0);	
	memoryAddress memory_value=0;
	switch(decodeAddressBin(memory_copy_bin)) {
		case 1: {
			// Checks text memory length			
			if (memoryIndex < TEXT_LENGTH) {
				memory_value= textSegment[memoryIndex];
			}
		} break;
		case 2: {
			// Checks data memory length
			if (memoryIndex < DATA_LENGTH)	{
				memory_value= dataSegment[memoryIndex];									
			}
		} break;
		case 3: {
			// Checks stack memory length
			if (memoryIndex < STACK_LENGTH) {
				memory_value= stackSegment[memoryIndex];									
			}
		} break;
		default: {
			// Not in current memory space
			cout << "Error: Memory read is not within current memory." << endl;
			memory_value= stackTop;													
		} break;
	}
	return mem_byte_string(memory_value, byte+1);
}

// Helper to find the end of string in memory
int Memory::length_of_string(memoryAddress memoryAddressIn, int max_length) {
	switch(decodeAddressBin(memoryAddressIn)) {
		case 1: { // TEXT 
			cout << "Error: There was an error finding the length of a string in TEXT segment of memory." << endl;
			return 0;
		}
		case 2: { // DATA
			int dataIndex = decodeAddressIndex(memoryAddressIn);
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
			int dataIndex = decodeAddressIndex(memoryAddressIn);
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
