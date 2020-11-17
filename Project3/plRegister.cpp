/*******
	COMP 4300
	Cameron Mathis
	Project 3
	11/29/20
	Register Simulation -- exact same as project 2
********/

#include <iostream>

#define REGISTER_LENGTH  32

using namespace std;

typedef uint32_t registerAddress;

registerAddress registers[REGISTER_LENGTH];

class RegisterBank {
public:
	RegisterBank();
    bool writeToRegister(registerAddress registerAddressIndex, registerAddress data);			
    registerAddress readFromRegister(registerAddress registerAddressIndex);	
	void print_register();
};

RegisterBank::RegisterBank() {}

/* Writes to registers -- exact same as project 2 */
bool RegisterBank::writeToRegister(registerAddress registerAddressIndex, registerAddress data) {
	if (registerAddressIndex <= REGISTER_LENGTH) {
		registers[registerAddressIndex] = data;
		return true;
	}
	cout << "Error with register write." << endl;
	return false;
}


/* Reads from registers -- exact same as project 2 */
registerAddress RegisterBank::readFromRegister(registerAddress registerAddressIndex ) {
	if (registerAddressIndex <= REGISTER_LENGTH) {
		return registers[registerAddressIndex];
	}
	cout << "Error with register read." << endl;
	return 0;
}

void RegisterBank::print_register() {
	int memory_index = 0;
	cout <<	"==== REGISTERS ======================" << endl;
	while (memory_index < REGISTER_LENGTH) {
		if (registers[memory_index] != 0) {
			cout << memory_index << ":  " << std::dec << registers[memory_index] << endl;
		}
		memory_index++;
	}
	cout <<	"==========================" << endl;
}
