/*******
	COMP 4300
	Cameron Mathis
	Project 2
	10/20/20
	General Purpose Register Machine Simulation
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
		int instructionOperation();						
		mem_addr first_register();					
		mem_addr second_register();					
		mem_addr third_register();					
		mem_addr immediate_value();					
		void loadNextInstruction();		
		// Program counter		
		mem_addr pc;								
		// Pointer to the current instruction
		instruction *current_instruction;			
		// Memory object
		Memory *mem;								
		// CPU internal registers
		Register_Bank *registers;					
		int8_t signed_immediate(mem_addr m_addr);  
};

int main() {
	Sim *sim = new Sim();
	sim->run();
	return 0;
}

// Starts state of simulator and initializes memory
Sim::Sim() {
	pc = text_top;
	mem = new Memory();
	registers = new Register_Bank();
}

// This is the simulation
void Sim::run() {
	bool more_instructions = true;
	int total_instructions_executed = 0;
	int totalCyclesSpent = 0;
	while(more_instructions) {
		loadNextInstruction();
		switch(instructionOperation()) {
			case 1: { // ADDI: ADD IMMEDIATE 
				int8_t immediate = signed_immediate(third_register());
				uint32_t register_value = registers->read(second_register());
				bool success = registers->write(first_register(), immediate + register_value);
				if (false == success) {
					cout << "Error: Adding value to register: "<< std::dec << second_register() << endl;
				}
				total_instructions_executed += 1;
				totalCyclesSpent += 6;
				break;
			}
			case 2: { // B: BRANCH
				int8_t label_offset =0;
				label_offset = signed_immediate(third_register());
				pc += label_offset;
				
				total_instructions_executed += 1;
				totalCyclesSpent += 4;
				break;
			}
			case 3: { // BEQZ: BRACH IF EQUAL TO ZERO
				int8_t label_offset =0;
				if (registers->read(first_register()) == 0) {
					label_offset = signed_immediate(third_register());
					pc += label_offset;
				}
				total_instructions_executed += 1;
				totalCyclesSpent += 5;
				break;
			}
			case 4: { // BGE: BRANCH IF GREATER OR EQUAL
				int8_t label_offset =0;
				if ( registers->read(first_register())  >=  registers->read(second_register())) {
					label_offset = signed_immediate(third_register());
					pc += label_offset;
				}
				total_instructions_executed += 1;
				totalCyclesSpent += 5;
				break;
			}
			case 5: { // BNE: BRANCH IF NOT EQUAL
				int8_t label_offset =0;
				if ( registers->read(first_register())  !=  registers->read(second_register())) {
					label_offset = signed_immediate(third_register());
					pc += label_offset;
				}
				total_instructions_executed += 1;
				totalCyclesSpent += 5;
				break;
			}
			case 6: { // LA: LOAD ADDRESS
				bool success = registers->write(first_register(),immediate_value());
				if (false == success) {
					cout << "Error: Loading Address to register: "<< std::dec << first_register() << endl;
				}
				total_instructions_executed += 1;
				totalCyclesSpent += 5;
				break;
			}
			case 7: { // LB: LOAD BYTE
				// number of bytes
				mem_addr address_value = registers->read(second_register()); 
				int8_t immediate = signed_immediate(third_register());
				address_value += immediate;
				bool success = registers->write(first_register(),mem->read_byte(address_value, address_value%4) );
				if (false == success) {
					cout << "Error: Loading Byte into register: "<< std::dec << first_register() << endl;
				}
				total_instructions_executed += 1;
				totalCyclesSpent += 6;
				break;
			}
			case 8: { // LI: LOAD IMMEDIATE
				bool success = registers->write(first_register(), second_register());
				if (false == success) {
					cout << "Error: Loading Immediate value to register: "<< std::dec << first_register() << endl;
				}
				total_instructions_executed += 1;
				totalCyclesSpent += 3;
				break;
			}
			case 9: { // SUBI: SUBTRACT IMMEDIATE
				int8_t immediate = signed_immediate(third_register());
				uint32_t register_value = registers->read(second_register());
				bool success = registers->write(first_register(), register_value - immediate);
				if (false == success) {
					cout << "Error: Adding value to register: "<< std::dec << second_register() << endl;
				}
				total_instructions_executed += 1;
				totalCyclesSpent += 6;
				break;
			}
			case 10: { // SYSCALL
				total_instructions_executed += 1;
				totalCyclesSpent += 8;
				switch(registers->read(3)) {
					case 4:	{ // Print String
						cout << mem->read_string(registers->read(1)) << endl;
						break;
					}
					case 8:	{ // Read String In
						char palin[1024];
						string incoming_palin;
						int length=1024;
						// Clear memory
					    for (int i=0; i<1024; i++) {
					            palin[i]=0;
					    }
						cout << "Please enter a word: ";
						getline(cin, incoming_palin);
						incoming_palin.copy(palin,1024,0);
						int len=strlen(palin);
						palin[len] = '\0';
						mem->load_string(registers->read(1),palin);
						break;
					}
					case 10: { // End Program
						more_instructions = false;
						cout << endl;
						cout << "Number of Instructions Executed (IC): " << std::dec<< total_instructions_executed << endl;
						cout << "Number of Cycles Spent in Execution (C): " <<std::dec<<  totalCyclesSpent << endl;
						printf("Speed-up: %3.2F \n",(8.0*total_instructions_executed) / totalCyclesSpent );
						break;
					}
					default: {
						cout << "Error: There was an error with the execution of SYSCALL." << endl;
						cout << "PC: " << std::hex << pc << endl;
						cout << "Current Istruction: " <<std::hex << current_instruction << endl;
						more_instructions = false;
						break;
					}
				}
				break;
			}
			case 11: { // LOAD
				cout << "Error: LOAD Instruction not implemented." << endl;
				break;
			}
			case 12: { // STORE
				cout << "Error: STORE Instruction not implemented." << endl;
				break;
			}
			default: {
				more_instructions = false;
				break;
			}
		}
	}
}

/* This removes the memory address from instruction, bits 32-24
 and returns the op code of internal current instruction */
int Sim::instructionOperation() {															
	instruction op_value;					
	op_value = *current_instruction;
	op_value = op_value >> 24;
	return op_value;
}

/* Left most register slot in instruction
 and eturns bits 24 - 16 */
mem_addr Sim::first_register() {
	instruction memory_address;
	memory_address = *current_instruction;
	memory_address = memory_address << 8;
	memory_address = memory_address >> 24;
	return memory_address;
}

// Center register slot in instruction
// Returns bits 16-8
mem_addr Sim::second_register() {
	instruction memory_address;
	memory_address = *current_instruction;
	memory_address = memory_address << 16;
	memory_address = memory_address >> 24;
	return memory_address;
}

// Right most register slot in instruction
// Returns bits 8-0
mem_addr Sim::third_register() {
	instruction memory_address;
	memory_address = *current_instruction;
	memory_address = memory_address << 24;
	memory_address = memory_address >> 24;
	return memory_address;
}

// Gives value of immediate slot
// Returns bits 16-0
mem_addr Sim::immediate_value() {
	instruction memory_address;
	memory_address = *current_instruction;
	memory_address = memory_address << 16;
	memory_address = memory_address >> 16;
	return memory_address;
}

// Returns a signed value
// Helper method for handling immediates and signing them correctly
int8_t Sim::signed_immediate(mem_addr m_addr) {
	mem_addr sign_bit = m_addr, value = m_addr;
	sign_bit = sign_bit >> 7;
	value = value <<26;
	value = value >>26;
	int return_value = 0;
	if (sign_bit == 1) {
		return_value = 0 -value;
		return return_value;
	} else {
		return value;
	}
	return 0;
}

// Takes all the steps to load next instruction
void Sim::loadNextInstruction() {															
	current_instruction = mem->read(pc);
	pc++;
}