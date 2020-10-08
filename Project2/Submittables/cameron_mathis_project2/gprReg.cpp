/*******
	COMP 4300
	Cameron Mathis
	Project 2
	10/20/20
	General Purpose Register Machine Registers Simulation
********/

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

#define REGISTER_LENGTH  32

using namespace std;

/*******
	Data Structures
********/
typedef uint32_t mem_addr;

mem_addr registers[REGISTER_LENGTH];

class Register_Bank {
	public:
		Register_Bank();
    	bool write(mem_addr memory_address_in, mem_addr data);			
    	mem_addr read(mem_addr memory_address_in);												
};

/*******
	Class Definition 
********/

// Initialize memory
Register_Bank::Register_Bank() {
	//empty on purpose
}

// Writes to a register
// Used in the add, subtract, and load portions of the simulator
bool Register_Bank::write(mem_addr memory_address_in, mem_addr data) {
	if (memory_address_in > REGISTER_LENGTH) {
		cout << "Error: Register write is out of bounds." << endl;
		return false;
	} else {
		registers[memory_address_in] = data;
		return true;
	}
	cout << "Error: Register write went wrong." << endl;
	return false;
}

// Reads based on given memory address
// Used in the add, subtract, branch, and system call portions of the simulator
mem_addr Register_Bank::read(mem_addr memory_address_in ) {
	if (memory_address_in > REGISTER_LENGTH) {
		cout << "Error: Register read is out of bounds." << endl;
		return false;
	} else {
		return registers[memory_address_in];
	}
	cout << "Error: Register read went wrong." << endl;
	return 0;
}