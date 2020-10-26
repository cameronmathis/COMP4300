/*******
	COMP 4300
	Cameron Mathis
	Project 2 - Attempt 2
	11/03/20
	General Purpose Register Machine Simulator
********/

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include "gprMemory.cpp"
#include "gprRegister.cpp"

using namespace std;

class Sim {
	public:
		Sim();										
		void run();									
	private:				
		void loadNextInstruction();	
		int instructionOperation();							
		int8_t getSignedImmediate(memoryAddress memoryAddr);  			
		memoryAddress immediateValue();	
		void addi(int& instructionsExecuted, int& cyclesSpentInExecution);		
		void b(int& instructionsExecuted, int& cyclesSpentInExecution);		
		void beqz(int& instructionsExecuted, int& cyclesSpentInExecution);	
		void bge(int& instructionsExecuted, int& cyclesSpentInExecution);	
		void bne(int& instructionsExecuted, int& cyclesSpentInExecution);
		void la(int& instructionsExecuted, int& cyclesSpentInExecution);	
		void lb(int& instructionsExecuted, int& cyclesSpentInExecution);	
		void li(int& instructionsExecuted, int& cyclesSpentInExecution);	
		void subi(int& instructionsExecuted, int& cyclesSpentInExecution);	
		void syscall(bool& isUserMode, int& instructionsExecuted, int& cyclesSpentInExecution);	
		memoryAddress leftBits();					
		memoryAddress centerBits();					
		memoryAddress rightBits();		
		// Program counter		
		memoryAddress programCounter;				
		// Memory object
		Memory *memory;								
		// CPU internal registers
		RegisterBank *registers;							
		// Current instruction
		instruction *currentInstruction;
};

int main() {
	Sim *sim = new Sim();
	sim->run();
	return 0;
}

/* Initializes the simulator -- modified from project 1 */
Sim::Sim() {
	registers = new RegisterBank();
	programCounter = textTop;
	memory = new Memory();
}

/* Runs the simulator -- modified heavily from project 1 */
void Sim::run() {
	int instructionsExecuted = 0;
	int cyclesSpentInExecution = 0;
	bool isUserMode = true;
	while(isUserMode) {
		loadNextInstruction();
		switch(instructionOperation()) {
			// ADDI
			case 1: { 
				addi(instructionsExecuted, cyclesSpentInExecution);
				break;
			}
			// B
			case 2: { 
				b(instructionsExecuted, cyclesSpentInExecution);
				break;
			}
			// BEQZ
			case 3: { 
				beqz(instructionsExecuted, cyclesSpentInExecution);
				break;
			}
			// BGE
			case 4: { 
				bge(instructionsExecuted, cyclesSpentInExecution);
				break;
			}
			// BNE
			case 5: { 
				bne(instructionsExecuted, cyclesSpentInExecution);
				break;
			}
			// LA
			case 6: { 
				la(instructionsExecuted, cyclesSpentInExecution);
				break;
			}
			// LB
			case 7: { 
				lb(instructionsExecuted, cyclesSpentInExecution);
				break;
			}
			// LI
			case 8: { 
				li(instructionsExecuted, cyclesSpentInExecution);
				break;
			}
			// SUBI
			case 9: { 
				subi(instructionsExecuted, cyclesSpentInExecution);
				break;
			}
			// SYSCALL
			case 10: { 
				syscall(isUserMode, instructionsExecuted, cyclesSpentInExecution);
				break;
			}
			default: {
				isUserMode = false;
				break;
			}
		}
	}
}

/* Loads the next instruction and increments the program counter*/
void Sim::loadNextInstruction() {															
	currentInstruction = memory -> readFromMemory(programCounter);
	programCounter++;
}

/* Returns the operation code from current instruction -- modified from project 1 */
int Sim::instructionOperation() {															
	instruction operationCode;					
	operationCode = *currentInstruction;
	operationCode = operationCode >> 24;
	return operationCode;
}

/* Returns a immediate value with the correct sign value */
int8_t Sim::getSignedImmediate(memoryAddress memoryAddr) {
	memoryAddress signBit = memoryAddr;
	memoryAddress value = memoryAddr;
	signBit = signBit >> 7;
	value = value << 26;
	value = value >> 26;
	int result = 0;
	if (signBit == 1) {
		result = 0 - value;
		return result;
	} else {
		return value;
	}
	return 0;
}

/* Returns 16 most right bits from current instruction */
memoryAddress Sim::immediateValue() {
	instruction memoryAddress;
	memoryAddress = *currentInstruction;
	memoryAddress = memoryAddress << 16;
	memoryAddress = memoryAddress >> 16;
	return memoryAddress;
}

/* Add immediate. Detailed syntax: ADDI Rdest, Rsrc1, Imm */
void Sim::addi(int& instructionsExecuted, int& cyclesSpentInExecution) {
	int8_t immediate = getSignedImmediate(rightBits());
	uint32_t registerValue = registers -> readFromRegister(centerBits());
	bool success = registers -> writeToRegister(leftBits(), immediate + registerValue);
	if (false == success) {
		cout << "Error with ADDI: "<< std::dec << centerBits() << endl;
	}
	instructionsExecuted += 1;
	cyclesSpentInExecution += 6;
}

/* Branch. Detailed syntax: label */
void Sim::b(int& instructionsExecuted, int& cyclesSpentInExecution) {
	int8_t labelOffset =0;
	labelOffset = getSignedImmediate(rightBits());
    programCounter += labelOffset;
	instructionsExecuted += 1;
	cyclesSpentInExecution += 4;
}	

/* Branch if equal to zero. Detailed syntax: Rsrc1, label */
void Sim::beqz(int& instructionsExecuted, int& cyclesSpentInExecution) {
	int8_t labelOffset = 0;
	if (registers -> readFromRegister(leftBits()) == 0) {
		labelOffset = getSignedImmediate(rightBits());
		programCounter += labelOffset;
	}
	instructionsExecuted += 1;
	cyclesSpentInExecution += 5;
}

/* Branch if greater than or equal to. Detailed syntax: Rsrc1, Rsrc2, label */
void Sim::bge(int& instructionsExecuted, int& cyclesSpentInExecution) {
	int8_t labelOffset = 0;
	if (registers -> readFromRegister(leftBits())  >=  registers -> readFromRegister(centerBits())) {
		labelOffset = getSignedImmediate(rightBits());
		programCounter += labelOffset;
	}
	instructionsExecuted += 1;
	cyclesSpentInExecution += 5;
}

/* Branch if not equal to. Detailed syntax: Rsrc1, Rsrc2, label */
void Sim::bne(int& instructionsExecuted, int& cyclesSpentInExecution) {
	int8_t labelOffset = 0;
	if ( registers -> readFromRegister(leftBits())  !=  registers -> readFromRegister(centerBits())) {
		labelOffset = getSignedImmediate(rightBits());
		programCounter += labelOffset;
	}
	instructionsExecuted += 1;
	cyclesSpentInExecution += 5;
}

/* Load address. Detailed syntax: Rdest, label */
void Sim::la(int& instructionsExecuted, int& cyclesSpentInExecution) {
	bool success = registers -> writeToRegister(leftBits(), immediateValue());
	if (false == success) {
		cout << "Error with LA: "<< std::dec << leftBits() << endl;
	}
	instructionsExecuted += 1;
	cyclesSpentInExecution += 5;
}

/* Load byte. Detailed syntax: Rdest, offset(Rsrc1) */
void Sim::lb(int& instructionsExecuted, int& cyclesSpentInExecution) {
	memoryAddress addressValue = registers -> readFromRegister(centerBits()); 
	int8_t immediate = getSignedImmediate(rightBits());
	addressValue += immediate;
	bool success = registers ->writeToRegister(leftBits() ,memory -> read_byte(addressValue, addressValue%4) );
	if (false == success) {
		cout << "Error with LB: "<< std::dec << leftBits() << endl;
	}
	instructionsExecuted += 1;
	cyclesSpentInExecution += 6;
}

/* Load address. Detailed syntax: Rdest, Imm */
void Sim::li(int& instructionsExecuted, int& cyclesSpentInExecution) {
	bool success = registers -> writeToRegister(leftBits(), centerBits());
	if (false == success) {
		cout << "Error with LI: "<< std::dec << leftBits() << endl;
	}
	instructionsExecuted += 1;
	cyclesSpentInExecution += 3;
}

/* Subtract immediate. Detailed syntax: Rdest, Rsrc1, Imm */
void Sim::subi(int& instructionsExecuted, int& cyclesSpentInExecution) {
	int8_t immediate = getSignedImmediate(rightBits());
	uint32_t registerValue = registers -> readFromRegister(centerBits());
	bool success = registers -> writeToRegister(leftBits(), registerValue - immediate);
	if (false == success) {
		cout << "Error with SUBI: "<< std::dec << centerBits() << endl;
	}
	instructionsExecuted += 1;
	cyclesSpentInExecution += 6;
}

/* System call. Used to request a service from the kernel */
void Sim::syscall(bool& isUserMode, int& instructionsExecuted, int& cyclesSpentInExecution) {
	instructionsExecuted += 1;
	cyclesSpentInExecution += 8;
	switch(registers -> readFromRegister(3)) {
		case 4:	{ // Print String
			cout << memory -> read_string(registers -> readFromRegister(1)) << endl;
			break;
		}
		case 8:	{ // Read String In
			int length = 1024;
			string enteredPalindrome;
			char palindrome[length];
			// Clear memory
			for (int i = 0; i < length; i++) {
					palindrome[i] = 0;
			}
			cout << "Please enter a word: ";
			getline(cin, enteredPalindrome);
			enteredPalindrome.copy(palindrome, length, 0);
			int len = strlen(palindrome);
			palindrome[len] = '\0';
			memory -> loadData(registers -> readFromRegister(1), palindrome);
			break;
		}
		case 10: { // End Program
			isUserMode = false;
			cout << "Instructions Executed (IC): " << std::dec << instructionsExecuted << endl;
			cout << "Cycles Spent in Execution (C): " << std::dec <<  cyclesSpentInExecution << endl;
			printf("Speed-up ([8*IC]/C): %3.2F \n", (8.0*instructionsExecuted) / cyclesSpentInExecution );
			break;
		}
		default: {
			cout << "There was an error with SYSCALL." << endl;
			cout << "Program Counter: " << std::hex << programCounter << endl;
			cout << "Current Instruction: " << std::hex << currentInstruction << endl;
			isUserMode = false;
			break;
		}
	}
}	

/* Returns 8 most left bits from current instruction */
memoryAddress Sim::leftBits() {
	instruction memoryAddress;
	memoryAddress = *currentInstruction;
	memoryAddress = memoryAddress << 8;
	memoryAddress = memoryAddress >> 24;
	return memoryAddress;
}

/* Returns 8 center bits from current instruction */
memoryAddress Sim::centerBits() {
	instruction memoryAddress;
	memoryAddress = *currentInstruction;
	memoryAddress = memoryAddress << 16;
	memoryAddress = memoryAddress >> 24;
	return memoryAddress;
}

/* Returns 8 most right bits from current instruction */
memoryAddress Sim::rightBits() {
	instruction memoryAddress;
	memoryAddress = *currentInstruction;
	memoryAddress = memoryAddress << 24;
	memoryAddress = memoryAddress >> 24;
	return memoryAddress;
}
