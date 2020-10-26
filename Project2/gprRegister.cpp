/*******
	COMP 4300
	Cameron Mathis
	Project 2 - Attempt 2
	11/03/20
	Register Simulation
********/

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

#define REGISTER_LENGTH  32

using namespace std;

typedef uint32_t memoryAddress;

memoryAddress registers[REGISTER_LENGTH];

class RegisterBank {
	public:
		RegisterBank();
    	bool writeToRegister(memoryAddress memoryAddressIn, memoryAddress data);			
    	memoryAddress readFromRegister(memoryAddress memoryAddressIn);												
};

RegisterBank::RegisterBank() {}

/* Writes to register */
bool RegisterBank::writeToRegister(memoryAddress memoryAddressIn, memoryAddress data) {
	if (memoryAddressIn > REGISTER_LENGTH) {
		cout << "Error: Register write is out of bounds." << endl;
		return false;
	} else {
		registers[memoryAddressIn] = data;
		return true;
	}
	cout << "Error: Register write went wrong." << endl;
	return false;
}

/* Reads from register */
memoryAddress RegisterBank::readFromRegister(memoryAddress memoryAddressIn ) {
	if (memoryAddressIn > REGISTER_LENGTH) {
		cout << "Error: Register read is out of bounds." << endl;
		return false;
	} else {
		return registers[memoryAddressIn];
	}
	cout << "Error: Register read went wrong." << endl;
	return 0;
}
