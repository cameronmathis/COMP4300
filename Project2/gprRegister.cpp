/*******
	COMP 4300
	Cameron Mathis
	Project 2 - Attempt 2
	11/03/20
	Register Simulation
********/

#include <iostream>

#define REGISTER_LENGTH  32

using namespace std;

typedef uint32_t memoryAddress;

memoryAddress registers[REGISTER_LENGTH];

class RegisterBank {
	public:
		RegisterBank();
    	bool writeToRegister(memoryAddress memoryAddressIndex, memoryAddress data);			
    	memoryAddress readFromRegister(memoryAddress memoryAddressIndex);												
};

RegisterBank::RegisterBank() {}

/* Writes to registers */
bool RegisterBank::writeToRegister(memoryAddress memoryAddressIndex, memoryAddress data) {
	if (memoryAddressIndex <= REGISTER_LENGTH) {
		registers[memoryAddressIndex] = data;
		return true;
	}
	cout << "Error with register write." << endl;
	return false;
}

/* Reads from registers */
memoryAddress RegisterBank::readFromRegister(memoryAddress memoryAddressIndex ) {
	if (memoryAddressIndex <= REGISTER_LENGTH) {
		return registers[memoryAddressIndex];
	}
	cout << "Error with register read." << endl;
	return 0;
}
