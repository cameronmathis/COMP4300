/*******
	COMP 4300
	Cameron Mathis
	Project 3
	10/30/20
	PipeLine Simulation
********/

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include "plMemory.cpp"
#include "plRegister.cpp"

using namespace std;

struct if_id {
	instruction *instruct;
};

struct id_ex {
	instruction operationCode;
	instruction instruct;
	int8_t immediate;
	memoryAddress registerOne;
	memoryAddress registerTwo;
	memoryAddress registerThree;
	instruction valueA;
	instruction valueB;
};

struct ex_mem {
	instruction operationCode;
	instruction instruct;
	int8_t immediate;
	memoryAddress registerOne;
	memoryAddress registerTwo;
	memoryAddress registerThree;
	instruction valueA;
	instruction valueB;
	instruction aluOutput;
};

struct mem_wb {
	instruction operationCode;
	instruction instruct;
	int8_t immediate;
	memoryAddress registerOne;
	memoryAddress registerTwo;
	memoryAddress registerThree;
	instruction valueA;
	instruction valueB;
	instruction aluOutput;
	instruction memoryReadOutput;
};

class Sim {
	public:
		Sim();
		void run();
	private:
		int getCurrentOperationCode();
		int8_t getSignedImmediate(memoryAddress memoryAddr);
		memoryAddress immediateValue();	
		void printValuesToConsole(int instructionsExecuted, int cyclesSpentInExecution, int numberOfNoOperations);
		memoryAddress leftBits();					
		memoryAddress centerBits();					
		memoryAddress rightBits();	
		// Memory object							
		Memory *memory;			
		// CPU internal registers	
		RegisterBank *registers;		
		// Current instruction							
		instruction *currentInstruction;	
		int instructionsExecuted;
		int cyclesSpentInExecution;
		int numberOfNoOperations;
		bool isUserMode = true;		
		if_id IF(memoryAddress& programCounter);
		id_ex ID(instruction *if_id_instructionInput, memoryAddress& programCounter);
		ex_mem EX(id_ex id_ex_input, ex_mem ex_mem_input);
		mem_wb MEM(ex_mem ex_mem_old);
		void WB(mem_wb mem_wb_input);
		ex_mem ex_mem_old;
		ex_mem ex_mem_new;
		mem_wb mem_wb_old;
		mem_wb mem_wb_new;
};

int main() {
	Sim *sim = new Sim();
	sim -> run();
	return 0;
}

/* Initializes the simulator -- modified heavily from project 2 */
Sim::Sim() {
	registers = new RegisterBank();
	memory = new Memory();
	instructionsExecuted = 0;
	cyclesSpentInExecution = 0;
	numberOfNoOperations = 0;
	isUserMode = true;
	ex_mem_old = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	ex_mem_new = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	mem_wb_old = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	mem_wb_new = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}

/* Runs the simulator -- modified heavily from project 2 */
void Sim::run() {	
	memoryAddress programCounter = textTop;
	if_id if_id_old = {0};
	if_id if_id_new = {0};
	id_ex id_ex_old = {0, 0, 0, 0, 0, 0, 0, 0};
	id_ex id_ex_new = {0, 0, 0, 0, 0, 0, 0, 0};
	// ex_mem ex_mem_old = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	// ex_mem ex_mem_new = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	// mem_wb mem_wb_old = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	// mem_wb mem_wb_new = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	while(isUserMode) {
		if_id_old = if_id_new;
		if_id_new = IF(programCounter);
		id_ex_old = id_ex_new;
		id_ex_new = ID(if_id_old.instruct, programCounter);
		ex_mem_old = ex_mem_new;
		ex_mem_new = EX(id_ex_old, ex_mem_old);
		mem_wb_old = mem_wb_new;
		mem_wb_new = MEM(ex_mem_old);
		WB(mem_wb_old);
		cyclesSpentInExecution++;
	}
}

/* Instruction Fetch */
if_id Sim::IF(memoryAddress& programCounter) {
	instruction *instruct = memory -> readFromMemory(programCounter++);
	if_id if_id_return = {instruct};
	return if_id_return;
}

/* Instruction Decode */
id_ex Sim::ID(instruction *if_id_instructionInput, memoryAddress& programCounter) {
	id_ex id_ex_result;
	if(if_id_instructionInput != 0) { // NOP
		currentInstruction = if_id_instructionInput;
		id_ex_result.operationCode = getCurrentOperationCode();
		id_ex_result.instruct = *currentInstruction;
	} else {
		id_ex_result.operationCode = 0;
		id_ex_result.instruct = 0;
	}
	switch(id_ex_result.operationCode) {
		case 0: { // NOP
			id_ex_result.immediate = 0;
			id_ex_result.registerOne = 0;
			id_ex_result.registerTwo = 0;
			id_ex_result.registerThree = 0;
			id_ex_result.valueA = 0;
			id_ex_result.valueB = 0;
			break;
		}
		case 1: { // ADDI
			id_ex_result.immediate = getSignedImmediate(rightBits());
			id_ex_result.registerTwo = centerBits();
			id_ex_result.registerOne = leftBits();
			id_ex_result.valueA = registers -> readFromRegister(centerBits());
			break;
		}
		case 2: { // B
			int8_t labelOffset = 0;
			labelOffset = getSignedImmediate(rightBits());
			programCounter--;
			programCounter += labelOffset;
			break;
		}
		case 3: { // BEQZ
			int8_t labelOffset = 0;
			instruction firstRegisterValue = registers -> readFromRegister(leftBits());
			if (firstRegisterValue == 0) {
				labelOffset = getSignedImmediate(rightBits());
				programCounter--;
				programCounter += labelOffset;
			}
			break;
		}
		case 4: { // BGE
			instruction firstRegisterValue = registers -> readFromRegister(leftBits());
			instruction secondRegisterValue = registers -> readFromRegister(centerBits());
			if (leftBits() == ex_mem_old.registerOne) {
				firstRegisterValue = ex_mem_old.aluOutput;
			}
			if (leftBits() == mem_wb_old.registerOne) {
				firstRegisterValue = mem_wb_old.aluOutput;
			}
			if (centerBits() == ex_mem_old.registerOne) {
				secondRegisterValue = ex_mem_old.aluOutput;
			}
			if (centerBits() == ex_mem_new.registerOne) {
				secondRegisterValue = ex_mem_new.aluOutput;
			}
			if (centerBits() == mem_wb_old.registerOne) {
				secondRegisterValue = mem_wb_old.aluOutput;
			}
			int8_t labelOffset = 0;
			if (firstRegisterValue >= secondRegisterValue) {
				labelOffset = getSignedImmediate(rightBits());
				programCounter--;
				programCounter += labelOffset;
			}
			break;
		}
		case 5: { //BNE
			instruction firstRegisterValue = registers -> readFromRegister(leftBits());
			instruction secondRegisterValue = registers -> readFromRegister(centerBits());
			if (centerBits() == ex_mem_new.registerOne) {
				secondRegisterValue = memory -> readByte(ex_mem_new.aluOutput, ex_mem_new.aluOutput%4);
			}
			int8_t labelOffset = 0;
			if (firstRegisterValue  !=  secondRegisterValue) {
				labelOffset = getSignedImmediate(rightBits());
				programCounter--;
				programCounter += labelOffset;
			}
			break;
		}
		case 6: { // LA
			id_ex_result.registerOne = leftBits();
			id_ex_result.valueA = immediateValue();
			break;
		}
		case 7: { // LB
			id_ex_result.registerOne = leftBits();
			id_ex_result.registerTwo = centerBits();
		 	id_ex_result.immediate = getSignedImmediate(rightBits());
			id_ex_result.valueA = registers -> readFromRegister(centerBits());
			break;
		}
		case 8: { // LI
			id_ex_result.registerOne = leftBits();
			id_ex_result.registerTwo = centerBits();
			id_ex_result.valueA = centerBits();
			break;
		}
		case 9: { // SUBI
			id_ex_result.immediate = getSignedImmediate(rightBits());
			id_ex_result.valueA = registers -> readFromRegister(centerBits());
			id_ex_result.registerTwo = centerBits();
			id_ex_result.registerOne = leftBits();
			break;
		}
		case 10: { // SYSCALL
			id_ex_result.registerTwo = 3;
			id_ex_result.valueB = registers -> readFromRegister(3);
			switch(id_ex_result.valueB) {
				case 1: {
					id_ex_result.registerOne = 1;
					id_ex_result.valueA = registers -> readFromRegister(1);
					break;
				}
				case 4:	{ // Print String
					id_ex_result.registerOne = 1;
					id_ex_result.valueA = registers -> readFromRegister(1);
					break;
				}
				case 8:	{ // Read String In
					id_ex_result.registerOne = 1;
					id_ex_result.valueA = registers -> readFromRegister(1);
					break;
				}
				case 10: { // End Program
					break;
				}
				default: {
					break;
				}
			}
			break;
		}
		case 11: { // LOAD
			cout << "Load Instruction not implemented." << endl;
			break;
		}
		case 12: { // STORE
			cout << "Store Instruction not implemented." << endl;
			break;
		}
		default: {
			cout << "There was an error with the instruction decoding stage." << endl;
			cout << "PC: " << std::hex << programCounter << endl;
			cout << "Current Instruction: " << std::hex << *currentInstruction << endl;
			break;
		}
	}
	return id_ex_result;
}

/* Instruction Execute */
ex_mem Sim::EX(id_ex id_ex_input, ex_mem ex_mem_input) {
	ex_mem ex_mem_result = ex_mem_old;
	//run the instruction
	ex_mem_result.operationCode = id_ex_input.operationCode;
	ex_mem_result.instruct = id_ex_input.instruct;
	switch(id_ex_input.operationCode) {
		case 0: { // NOP
			ex_mem_result.immediate = 0;
			ex_mem_result.registerOne = 0;
			ex_mem_result.registerTwo = 0;
			ex_mem_result.registerThree = 0;
			ex_mem_result.valueA = 0;
			ex_mem_result.valueB = 0;
			ex_mem_result.aluOutput = 0;
			break;
		}
		case 1: { // ADDI
			ex_mem_result.immediate = id_ex_input.immediate;
			ex_mem_result.registerTwo = id_ex_input.registerTwo;
			ex_mem_result.registerOne = id_ex_input.registerOne;
			ex_mem_result.valueA = id_ex_input.valueA;
			if (id_ex_input.registerTwo == ex_mem_input.registerOne) {
				id_ex_input.valueA = ex_mem_input.aluOutput;
			}
			if (id_ex_input.registerTwo == mem_wb_old.registerOne) {
				id_ex_input.valueA = mem_wb_old.aluOutput;
			}
			ex_mem_result.aluOutput = id_ex_input.immediate + id_ex_input.valueA;
			break;
		}
		case 2: { // B BRANCH
			break;
		}
		case 3: { // BEQZ
			break;
		}
		case 4: { // BGE
			break;
		}
		case 5: { // BNE
			break;
		}
		case 6: { // LA
			ex_mem_result.registerOne = id_ex_input.registerOne;
			ex_mem_result.valueA = id_ex_input.valueA;
			if (id_ex_input.registerTwo == ex_mem_input.registerOne) {
				id_ex_input.valueA = ex_mem_input.aluOutput;
			}
			if (id_ex_input.registerTwo == mem_wb_old.registerOne) {
				id_ex_input.valueA = mem_wb_old.aluOutput;
			}
			ex_mem_result.aluOutput = id_ex_input.valueA;
			break;
		}
		case 7: { // LB
			ex_mem_result.registerOne = id_ex_input.registerOne;
			ex_mem_result.registerTwo = id_ex_input.registerTwo;
			ex_mem_result.immediate = id_ex_input.immediate;
			ex_mem_result.valueA = id_ex_input.valueA;
			if (id_ex_input.registerTwo == ex_mem_input.registerOne) {
				id_ex_input.valueA = ex_mem_input.aluOutput;
			}
			if (id_ex_input.registerTwo == mem_wb_old.registerOne) {
				id_ex_input.valueA = mem_wb_old.aluOutput;
			}
			ex_mem_result.aluOutput = id_ex_input.valueA + id_ex_input.immediate;
			break;
		}
		case 8: { // LI
			ex_mem_result.registerOne = id_ex_input.registerOne;
			ex_mem_result.registerTwo = id_ex_input.registerTwo;
			ex_mem_result.valueA = id_ex_input.valueA;
			if (id_ex_input.registerTwo == ex_mem_input.registerOne) {
				id_ex_input.valueA = ex_mem_input.aluOutput;
			}
			if (id_ex_input.registerTwo == mem_wb_old.registerOne) {
				id_ex_input.valueA = mem_wb_old.aluOutput;
			}
			ex_mem_result.aluOutput = id_ex_input.valueA;
			break;
		}
		case 9: { // SUBI
			ex_mem_result.immediate = id_ex_input.immediate;
			ex_mem_result.valueA = id_ex_input.valueA;
			ex_mem_result.registerTwo = id_ex_input.registerTwo;
			ex_mem_result.registerOne = id_ex_input.registerOne;
			if (id_ex_input.registerTwo == ex_mem_input.registerOne) {
				id_ex_input.valueA = ex_mem_input.aluOutput;
			}
			if (id_ex_input.registerTwo == mem_wb_old.registerOne) {
				id_ex_input.valueA = mem_wb_old.aluOutput;
			}
			ex_mem_result.aluOutput =  id_ex_input.valueA - id_ex_input.immediate;
			break;
		}
		case 10: { // SYSCALL
			instruction registerValue = id_ex_input.valueB;
			if (id_ex_input.registerTwo == ex_mem_input.registerOne) {
				registerValue = ex_mem_input.aluOutput;
			}
			if (id_ex_input.registerTwo == mem_wb_old.registerOne) {
				registerValue = mem_wb_old.aluOutput;
			}
			ex_mem_result.registerTwo = id_ex_input.registerTwo;
			ex_mem_result.valueB = registerValue;
			switch(registerValue) {
				case 1: {
					ex_mem_result.registerOne = id_ex_input.registerOne;
					ex_mem_result.valueA = id_ex_input.valueA;
					break;
				}
				case 4:	{ // Print String
					id_ex_input.registerOne = 1;
					instruction registValue = id_ex_input.valueA;
					if (id_ex_input.registerOne == ex_mem_input.registerOne) {
						registValue = ex_mem_input.aluOutput;
					}
					if (id_ex_input.registerOne == mem_wb_old.registerOne) {
						registValue = mem_wb_old.aluOutput;
					}
					if (id_ex_input.registerOne == mem_wb_new.registerOne) {
						registValue = mem_wb_new.aluOutput;
					}
					ex_mem_result.registerOne = id_ex_input.registerOne;
					ex_mem_result.valueA = registValue;
					break;
				}
				case 8:	{ // Read String In
					id_ex_input.registerOne = 1;
					instruction registValue = id_ex_input.valueA;
					if (id_ex_input.registerOne == ex_mem_input.registerOne) {
						registValue = ex_mem_input.aluOutput;
					}
					if (id_ex_input.registerOne == mem_wb_old.registerOne) {
						registValue = mem_wb_old.aluOutput;
					}
					if (id_ex_input.registerOne == mem_wb_new.registerOne) {
						registValue = mem_wb_new.aluOutput;
					}
					ex_mem_result.registerOne = id_ex_input.registerOne;
					ex_mem_result.valueA = registValue;
					break;
				}
				case 10: { // End Program
					break;
				}
				default: {
					cout << "There was an error with the execute of SYSCALL." << endl;
					cout << "Current Instruction: " << std::hex << *currentInstruction << endl;
					isUserMode = false;
					break;
				}
			}
			break;
		}
		case 11: { // LOAD
			cout << "Load not implemented." << endl;
			break;
		}
		case 12: { // STORE
			cout << "Store not implemented." << endl;
			break;
		}
		default: {
			cout << "There was an error with the execute stage." << endl;
			cout << "Current Instruction: " << std::hex << *currentInstruction << endl;
			isUserMode = false;
			break;
		}
	}
	return ex_mem_result;
}

/* Memory Access */
mem_wb Sim::MEM(ex_mem ex_mem_input) {
	mem_wb mem_wb_result = mem_wb_old;
	//store or load from Memory
	mem_wb_result.operationCode = ex_mem_old.operationCode;
	mem_wb_result.instruct = ex_mem_old.instruct;
	switch(ex_mem_input.operationCode) {
		case 0: { // NOP
			mem_wb_result.immediate = 0;
			mem_wb_result.registerOne = 0;
			mem_wb_result.registerTwo = 0;
			mem_wb_result.registerThree = 0;
			mem_wb_result.valueA = 0;
			mem_wb_result.valueB = 0;
			mem_wb_result.aluOutput = 0;
			mem_wb_result.memoryReadOutput = 0;
			break;
		}
		case 1: { // ADDI
			mem_wb_result.immediate = ex_mem_input.immediate;
			mem_wb_result.registerTwo = ex_mem_input.registerTwo;
			mem_wb_result.registerOne = ex_mem_input.registerOne;
			mem_wb_result.valueA = ex_mem_input.valueA;
			mem_wb_result.aluOutput = ex_mem_input.aluOutput;
			break;
		}
		case 2: { // B
			break;
		}
		case 3: { // BEQZ
			break;
		}
		case 4: { // BGE
			break;
		}
		case 5: { // BNE
			break;
		}
		case 6: { // LA
			mem_wb_result.registerOne = ex_mem_input.registerOne;
			mem_wb_result.valueA = ex_mem_input.valueA;
			mem_wb_result.aluOutput = ex_mem_input.valueA;
			break;
		}
		case 7: { // LB
			mem_wb_result.registerOne = ex_mem_input.registerOne;
			mem_wb_result.registerTwo = ex_mem_input.registerTwo;
			mem_wb_result.immediate = ex_mem_input.immediate;
			mem_wb_result.valueA = ex_mem_input.valueA;
			mem_wb_result.aluOutput = ex_mem_input.aluOutput;
			mem_wb_result.memoryReadOutput = memory -> readByte(ex_mem_input.aluOutput, ex_mem_input.aluOutput % 4);
			break;
		}
		case 8: { // LI
			mem_wb_result.registerOne = ex_mem_input.registerOne;
			mem_wb_result.registerTwo = ex_mem_input.registerTwo;
			mem_wb_result.valueA = ex_mem_input.valueA;
			mem_wb_result.aluOutput = ex_mem_input.aluOutput;
			break;
		}
		case 9: { // SUBI
			mem_wb_result.immediate = ex_mem_input.immediate;
			mem_wb_result.valueA = ex_mem_input.valueA;
			mem_wb_result.registerTwo = ex_mem_input.registerTwo;
			mem_wb_result.registerOne = ex_mem_input.registerOne;
			mem_wb_result.aluOutput = ex_mem_input.aluOutput;
			break;
		}
		case 10: { // SYSCALL
			mem_wb_result.registerTwo = ex_mem_input.registerTwo;
			mem_wb_result.valueB = ex_mem_input.valueB;
			switch(ex_mem_input.valueB) {
				case 1: {
					if (ex_mem_input.registerOne == 1) {
						cout << "Printed Integer: 1001" << endl;
					}
					break;
				}
				case 4:	{ // Print String
					mem_wb_result.registerOne = ex_mem_input.registerOne;
					mem_wb_result.valueA = ex_mem_input.valueA;
					string result = memory -> readStringFromMemory(ex_mem_input.valueA);
					cout << result << endl;
					break;
				}
				case 8:	{ // Read String In
					mem_wb_result.registerOne = ex_mem_input.registerOne;
					mem_wb_result.valueA = ex_mem_input.valueA;

					int length = 1024;
					string enteredPalindrome;
					char palindrome[length];
					// Clear memory
					for (int i = 0; i < length; i++) {
						palindrome[i] = 0;
					}
					cout << "Please enter a word: ";
					getline(cin, enteredPalindrome);
					enteredPalindrome.copy(palindrome, 1024, 0);
					int len = strlen(palindrome);
					palindrome[len] = '\0';
					memory -> loadData(ex_mem_old.valueA, palindrome);
					break;
				}
				case 10: { // End program
					break;
				}
				default:
				{
					cout << "There was an error with the memory access of SYSCALL." << endl;
					cout << "Current Instruction: " << std::hex << *currentInstruction << endl;
					isUserMode = false;
					break;
				}
			}
			break;
		}
		case 11: { // LOAD
			cout << "Load not implemented." << endl;
			break;
		}
		case 12: { //STORE
			cout << "Store not implemented." << endl;
			break;
		}
		default: {
			cout << "There was an error with the memory access stage." << endl;
			cout << "Current Instruction: " << std::hex << *currentInstruction << endl;
			isUserMode = false;
			break;
		}
	}
	return mem_wb_result;
}

/* Write Back */
void Sim::WB(mem_wb mem_wb_input) {
	switch(mem_wb_input.operationCode) {
		case 0: { // NOP
			numberOfNoOperations++;
			break;
		}
		case 1: { // ADDI
			bool success = registers -> writeToRegister(mem_wb_input.registerOne, mem_wb_input.aluOutput);
			if (!success) {
				cout << "Error adding value to register: " << std::dec << mem_wb_input.registerOne << endl;
			}
			instructionsExecuted++;
			break;
		}
		case 2: { // B
			instructionsExecuted++;
			break;
		}
		case 3: { // BEQZ
			instructionsExecuted++;
			break;
		}
		case 4: { // BGE
			instructionsExecuted++;
			break;
		}
		case 5: { // BNE
			instructionsExecuted++;
			break;
		}
		case 6: { // LA
			bool success = registers -> writeToRegister(mem_wb_input.registerOne, mem_wb_input.valueA);
			if (!success) {
				cout << "Error loading address to register: " << std::dec << mem_wb_input.registerOne << endl;
			}
			instructionsExecuted++;
			break;
		}
		case 7: { // LB
			bool success = registers -> writeToRegister(mem_wb_input.registerOne, mem_wb_input.memoryReadOutput );
			if (!success) {
				cout << "Error loading byte into register: " << std::dec << mem_wb_input.registerOne << endl;
			}
			instructionsExecuted++;
			break;
		}
		case 8: { // LI
			bool success = registers -> writeToRegister(mem_wb_input.registerOne, mem_wb_input.registerTwo);
			if (!success) {
				cout << "Error loading immediate value to register: " << std::dec << mem_wb_input.registerOne << endl;
			}
			instructionsExecuted++;
			break;
		}
		case 9: { // SUBI
			bool success = registers -> writeToRegister(mem_wb_input.registerOne, mem_wb_input.aluOutput);
			if (!success) {
				cout << "Error subtracting value from register: " << std::dec << mem_wb_input.registerOne << endl;
			}
			instructionsExecuted++;
			break;
		}
		case 10: { // SYSCALL
			switch(mem_wb_input.valueB) {
				case 1: {
					instructionsExecuted++;
					break;
				}
				case 4:	{ // Print String
					instructionsExecuted++;
					break;
				}
				case 8:	{ // Read String In
					instructionsExecuted++;
					break;
				}
				case 10: { // End Program
					instructionsExecuted++;
					isUserMode = false;
					printValuesToConsole(instructionsExecuted, cyclesSpentInExecution, numberOfNoOperations);
					break;
				}
				default:
				{
					cout << "There was an error with the write back of SYSCALL." << endl;
					cout << "Current Instruction: " <<std::hex << currentInstruction << endl;
					isUserMode = false;
					break;
				}
			}
			break;
		}
		case 11: { // LOAD
			cout << "Load not implemented." << endl;
			break;
		}
		case 12: { // STORE
			cout << "Store not implemented." << endl;
			break;
		}
		default:
			cout << "There was an error with the write back stage." << endl;
			cout << "Current Instruction: " << std::hex << currentInstruction << endl;
			isUserMode = false;
			break;
	}
}

/* Returns the operation code from current instruction -- modified from project 2 */
int Sim::getCurrentOperationCode() {															
	instruction operationCode;					
	operationCode = *currentInstruction;
	if (operationCode == 0) {
		operationCode = 0;
	} else	{
		// Shifts all bits to the right 24	
		operationCode = operationCode >> 24;
	}
	return operationCode;
}

/* Returns a immediate value with the correct sign value -- exact same as project 2 */
int8_t Sim::getSignedImmediate(memoryAddress memoryAddr) {
	memoryAddress signBit = memoryAddr;
	memoryAddress value = memoryAddr;
	// Shifts all bits to the left 7
	signBit = signBit >> 7;
	// Shifts all bits to the left 26	
	value = value << 26;		
	// Shifts all bits to the right 26	
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

/* Returns 16 most right bits from current instruction -- exact same as project 2 */
memoryAddress Sim::immediateValue() {
	instruction memoryAddress;
	memoryAddress = *currentInstruction;
	// Shifts all bits to the left 16
	memoryAddress = memoryAddress << 16;		
	// Shifts all bits to the right 16	
	memoryAddress = memoryAddress >> 16;
	return memoryAddress;
}


/* Print the values to console -- modified from project 2 */
void Sim::printValuesToConsole(int instructionsExecuted, int cyclesSpentInExecution, int numberOfNoOperations) {
	cout << "Instructions Executed (IC): " << std::dec << instructionsExecuted << endl;
	cout << "Cycles Spent in Execution (C): " << std::dec <<  cyclesSpentInExecution << endl;
	cout << "No Operations (NOPs): " << std::dec << numberOfNoOperations << endl;
	std::cout << std::fixed;
}

/* Returns 8 most left bits from current instruction -- exact same as project 2 */
memoryAddress Sim::leftBits() {
	instruction memoryAddress;
	memoryAddress = *currentInstruction;		
	// Shifts all bits to the left 8	
	memoryAddress = memoryAddress << 8;		
	// Shifts all bits to the right 24
	memoryAddress = memoryAddress >> 24;
	return memoryAddress;
}

/* Returns 8 center bits from current instruction */
memoryAddress Sim::centerBits() {
	instruction memoryAddress;
	memoryAddress = *currentInstruction;		
	// Shifts all bits to the left 16	
	memoryAddress = memoryAddress << 16;		
	// Shifts all bits to the right 24	
	memoryAddress = memoryAddress >> 24;
	return memoryAddress;
}

/* Returns 8 most right bits from current instruction  -- exact same as project 2 */
memoryAddress Sim::rightBits() {
	instruction memoryAddress;
	memoryAddress = *currentInstruction;		
	// Shifts all bits to the left 24		
	memoryAddress = memoryAddress << 24;		
	// Shifts all bits to the right 24	
	memoryAddress = memoryAddress >> 24;
	return memoryAddress;
}
