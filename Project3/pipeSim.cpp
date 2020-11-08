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
		void loadNextInstruction();
		int getCurrentOperationCode();
		int8_t getSignedImmediate(memoryAddress memoryAddr);
		memoryAddress immediateValue();	
		void printValuesToConsole(int instructionsExecuted, int cyclesSpentInExecution, int numberOfNoOperations);
		memoryAddress leftBits();					
		memoryAddress centerBits();					
		memoryAddress rightBits();		
		// Program counter			
		memoryAddress pc;	
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
		void ID(memoryAddress& programCounter);
		void EX();
		void MEM();
		void WB();
		if_id if_id_old;
		if_id if_id_new;
		id_ex id_ex_old;
		id_ex id_ex_new;
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
	pc = textTop;
	memory = new Memory();
	instructionsExecuted = 0;
	cyclesSpentInExecution = 0;
	numberOfNoOperations = 0;
	isUserMode = true;
	if_id_new.instruct = 0;
	if_id_old.instruct = 0;
	id_ex id_ex_new = {0, 0, 0, 0, 0, 0, 0, 0};
	id_ex id_ex_old = {0, 0, 0, 0, 0, 0, 0, 0};
	ex_mem ex_mem_old = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	ex_mem ex_mem_new = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	mem_wb mem_wb_new = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	mem_wb mem_wb_old = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}

/* Runs the simulator -- modified heavily from project 2 */
void Sim::run() {	
	// memoryAddress programCounter = textTop;
	// int instructionsExecuted = 0;
	// int cyclesSpentInExecution = 0;
	// int numberOfNoOperations = 0;
	while(isUserMode) {
		if_id_old = if_id_new;
		if_id_new = IF(pc);
		id_ex_old = id_ex_new;
		ID(pc);
		ex_mem_old = ex_mem_new;
		EX();
		mem_wb_old = mem_wb_new;
		MEM();
		WB();
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
void Sim::ID(memoryAddress& programCounter) {
	if(if_id_old.instruct != 0) { //nop
		currentInstruction = if_id_old.instruct;
		id_ex_new.operationCode = getCurrentOperationCode();
		id_ex_new.instruct = *currentInstruction;
	} else {
		id_ex_new.operationCode = 0;
		id_ex_new.instruct = 0;
	}
	switch(id_ex_new.operationCode) {
		case 0: {
			//Nop
			id_ex_new.immediate = 0 ;
			id_ex_new.registerOne = 0 ;
			id_ex_new.registerTwo = 0 ;
			id_ex_new.registerThree = 0 ;
			id_ex_new.valueA = 0 ;
			id_ex_new.valueB = 0 ;
			break;
		}
		case 1: { //ADDI ADD IMMEDIATE
			id_ex_new.immediate = getSignedImmediate(rightBits());
			id_ex_new.registerTwo = centerBits();
			id_ex_new.registerOne = leftBits();
			id_ex_new.valueA = registers->readFromRegister(centerBits());
			break;
		}
		case 2: { //B BRANCH
			int8_t label_offset =0;
			label_offset = getSignedImmediate(rightBits());
			programCounter--;
			programCounter += label_offset;
			break;
		}
		case 3: { //BEQZ BRACH IF EQUAL TO ZERO
			int8_t label_offset = 0;
			instruction first_regist_value = registers->readFromRegister(leftBits());
			if (first_regist_value  == 0)
			{
				label_offset = getSignedImmediate(rightBits());
				programCounter--;
				programCounter += label_offset;
			}
			break;
		}
		case 4: { //BGE BRANCH IF GREATER OR EQUAL $t0,$t1,target,  $t0 >= $t1
			instruction first_regist_value = registers->readFromRegister(leftBits());
			instruction second_regist_value = registers->readFromRegister(centerBits());
			if(leftBits() == ex_mem_old.registerOne)
			{
				first_regist_value = ex_mem_old.aluOutput;
			}
			if(leftBits() == mem_wb_old.registerOne)
			{
				first_regist_value = mem_wb_old.aluOutput;
			}
			if(centerBits() == ex_mem_old.registerOne)
			{
				second_regist_value = ex_mem_old.aluOutput;
			}
			if(centerBits() == ex_mem_new.registerOne)
			{
				second_regist_value = ex_mem_new.aluOutput;
			}
			if(centerBits() == mem_wb_old.registerOne)
			{
				second_regist_value = mem_wb_old.aluOutput;
			}
			int8_t label_offset = 0 ;
			if ( first_regist_value >= second_regist_value )
			{
				label_offset = getSignedImmediate(rightBits());
				programCounter--;
				programCounter += label_offset;
			}
			break;
		}
		case 5: { //BNE BRANCH IF NOT EQUAL  $t0,$t1,target, $t0 <> $t1
			instruction first_regist_value = registers->readFromRegister(leftBits());
			instruction second_regist_value = registers->readFromRegister(centerBits());
			if(centerBits() == ex_mem_new.registerOne)
			{
				second_regist_value = memory->readByte(ex_mem_new.aluOutput, ex_mem_new.aluOutput%4);
			}
			int8_t label_offset =0;
			if ( first_regist_value  !=  second_regist_value)
			{
				label_offset = getSignedImmediate(rightBits());
				programCounter--;
				programCounter += label_offset;
			}
			break;
		}
		case 6: //LA LOAD ADDRESS
		{
			id_ex_new.registerOne = leftBits();
			id_ex_new.valueA = immediateValue();
			break;
		}
		case 7: //LB LOAD BYTE
		{
			id_ex_new.registerOne = leftBits();
			id_ex_new.registerTwo = centerBits();
		 	id_ex_new.immediate = getSignedImmediate(rightBits());
			id_ex_new.valueA = registers->readFromRegister(centerBits());//number of bytes
			break;
		}
		case 8: //LI LOAD IMMEDIATE
		{
			id_ex_new.registerOne = leftBits();
			id_ex_new.registerTwo = centerBits();
			id_ex_new.valueA = centerBits();
			break;
		}
		case 9: //SUBI SUBTRACT IMMEDIATE
		{
			id_ex_new.immediate = getSignedImmediate(rightBits());
			id_ex_new.valueA = registers->readFromRegister(centerBits());
			id_ex_new.registerTwo = centerBits();
			id_ex_old.registerOne = leftBits();
			break;
		}
		case 10: //SYSCALL
		{
			id_ex_new.registerTwo = 3;
			id_ex_new.valueB = registers->readFromRegister(3);
			switch(id_ex_new.valueB)
			{
				case 1:
				{
					id_ex_new.registerOne = 1;
					id_ex_new.valueA = registers->readFromRegister(1);
					break;
				}
				case 4:	//Print String
				{
					id_ex_new.registerOne = 1;
					id_ex_new.valueA = registers->readFromRegister(1);
					break;
				}
				case 8:	//Read String In
				{
					id_ex_new.registerOne = 1;
					id_ex_new.valueA = registers->readFromRegister(1);
					break;
				}
				case 10:// End Program
				{
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case 11:	//LOAD
		{
			cout << "Error: LOAD Instruction not implemented." << endl;
			break;
		}
		case 12:	//STORE
		{
			cout << "Error: STORE Instruction not implemented." << endl;
			break;
		}
		default:
			cout << "Error: There was an error with the Decoding Stage." << endl;
			cout << "PC: " << std::hex << programCounter << endl;
			cout << "Current Istruction: " <<std::hex << *currentInstruction << endl;
			//isUserMode = false;
			break;
	}
}

/* Instruction Execute */
void Sim::EX()
{
	//run the instruciton
	ex_mem_new.operationCode = id_ex_old.operationCode;
	ex_mem_new.instruct = id_ex_old.instruct;
	switch(id_ex_old.operationCode)
	{
		case 0:
		{
			//Nop
			ex_mem_new.immediate = 0 ;
			ex_mem_new.registerOne = 0 ;
			ex_mem_new.registerTwo = 0 ;
			ex_mem_new.registerThree = 0 ;
			ex_mem_new.valueA = 0 ;
			ex_mem_new.valueB = 0 ;
			ex_mem_new.aluOutput = 0;
			break;
		}
		case 1: //ADDI ADD IMMEDIATE
		{
			ex_mem_new.immediate = id_ex_old.immediate;
			ex_mem_new.registerTwo = id_ex_old.registerTwo;
			ex_mem_new.registerOne = id_ex_old.registerOne;
			ex_mem_new.valueA = id_ex_old.valueA;
			if(id_ex_old.registerTwo == ex_mem_old.registerOne)
			{
				id_ex_old.valueA = ex_mem_old.aluOutput;
			}
			if(id_ex_old.registerTwo == mem_wb_old.registerOne)
			{
				id_ex_old.valueA = mem_wb_old.aluOutput;
			}
			ex_mem_new.aluOutput = id_ex_old.immediate + id_ex_old.valueA;
			break;
		}
		case 2: //B BRANCH
		{
			break;
		}
		case 3: //BEQZ BRACH IF EQUAL TO ZERO
		{
			break;
		}
		case 4: //BGE BRANCH IF GREATER OR EQUAL $t0,$t1,target,  $t0 >= $t1
		{
			break;
		}
		case 5: //BNE BRANCH IF NOT EQUAL  $t0,$t1,target, $t0 <> $t1
		{
			break;
		}
		case 6: //LA LOAD ADDRESS
		{
			ex_mem_new.registerOne = id_ex_old.registerOne;
			ex_mem_new.valueA = id_ex_old.valueA;
			if(id_ex_old.registerTwo == ex_mem_old.registerOne)
			{
				id_ex_old.valueA = ex_mem_old.aluOutput;
			}
			if(id_ex_old.registerTwo == mem_wb_old.registerOne)
			{
				id_ex_old.valueA = mem_wb_old.aluOutput;
			}
			ex_mem_new.aluOutput = id_ex_old.valueA;
			break;
		}
		case 7: //LB LOAD BYTE
		{
			ex_mem_new.registerOne = id_ex_old.registerOne;
			ex_mem_new.registerTwo = id_ex_old.registerTwo;
			ex_mem_new.immediate = id_ex_old.immediate;
			ex_mem_new.valueA = id_ex_old.valueA;			//number of bytes
			if(id_ex_old.registerTwo == ex_mem_old.registerOne)
			{
				id_ex_old.valueA = ex_mem_old.aluOutput;
			}
			if(id_ex_old.registerTwo == mem_wb_old.registerOne)
			{
				id_ex_old.valueA = mem_wb_old.aluOutput;
			}
			ex_mem_new.aluOutput = id_ex_old.valueA + id_ex_old.immediate;
			break;
		}
		case 8: //LI LOAD IMMEDIATE
		{
			ex_mem_new.registerOne = id_ex_old.registerOne;
			ex_mem_new.registerTwo = id_ex_old.registerTwo;
			ex_mem_new.valueA = id_ex_old.valueA;
			if(id_ex_old.registerTwo == ex_mem_old.registerOne)
			{
				id_ex_old.valueA = ex_mem_old.aluOutput;
			}
			if(id_ex_old.registerTwo == mem_wb_old.registerOne)
			{
				id_ex_old.valueA = mem_wb_old.aluOutput;
			}
			ex_mem_new.aluOutput = id_ex_old.valueA;
			break;
		}
		case 9: //SUBI SUBTRACT IMMEDIATE
		{
			ex_mem_new.immediate = id_ex_old.immediate;
			ex_mem_new.valueA = id_ex_old.valueA;
			ex_mem_new.registerTwo = id_ex_old.registerTwo;
			ex_mem_new.registerOne = id_ex_old.registerOne;
			if(id_ex_old.registerTwo == ex_mem_old.registerOne)
			{
				id_ex_old.valueA = ex_mem_old.aluOutput;
			}
			if(id_ex_old.registerTwo == mem_wb_old.registerOne)
			{
				id_ex_old.valueA = mem_wb_old.aluOutput;
			}
			ex_mem_new.aluOutput =  id_ex_old.valueA + (- id_ex_old.immediate);
			break;
		}
		case 10: //SYSCALL
		{
			instruction regist_value = id_ex_old.valueB;
			if(id_ex_old.registerTwo == ex_mem_old.registerOne)
			{
				regist_value = ex_mem_old.aluOutput;
			}
			if(id_ex_old.registerTwo == mem_wb_old.registerOne)
			{
				regist_value = mem_wb_old.aluOutput;
			}
			ex_mem_new.registerTwo = id_ex_old.registerTwo;
			ex_mem_new.valueB = regist_value;
			switch(regist_value)
			{
				case 1:
				{
					ex_mem_new.registerOne = id_ex_old.registerOne;
					ex_mem_new.valueA = id_ex_old.valueA;
					break;
				}
				case 4:	//Print String
				{
					id_ex_old.registerOne =1;
					instruction regist_value = id_ex_old.valueA;
					if(id_ex_old.registerOne == ex_mem_old.registerOne)
					{
						regist_value = ex_mem_old.aluOutput;
					}
					if(id_ex_old.registerOne == mem_wb_old.registerOne)
					{
						regist_value = mem_wb_old.aluOutput;
					}
					if(id_ex_old.registerOne == mem_wb_new.registerOne)
					{
						regist_value = mem_wb_new.aluOutput;
					}
					ex_mem_new.registerOne = id_ex_old.registerOne;
					ex_mem_new.valueA = regist_value;
					break;
				}
				case 8:	//Read String In
				{
					id_ex_old.registerOne = 1;
					instruction regist_value = id_ex_old.valueA;
					if(id_ex_old.registerOne == ex_mem_old.registerOne)
					{
						regist_value = ex_mem_old.aluOutput;
					}
					if(id_ex_old.registerOne == mem_wb_old.registerOne)
					{
						regist_value = mem_wb_old.aluOutput;
					}
					if(id_ex_old.registerOne == mem_wb_new.registerOne)
					{
						regist_value = mem_wb_new.aluOutput;
					}
					ex_mem_new.registerOne = id_ex_old.registerOne;
					ex_mem_new.valueA = regist_value;
					break;
				}
				case 10:// End Program
				{
					break;
				}
				default:
				{
					cout << "Error: There was an error with the Execute of SYSCALL." << endl;
					cout << "PC: " << std::hex << pc << endl;
					cout << "Current Istruction: " <<std::hex << *currentInstruction << endl;
					isUserMode = false;
					break;
				}
			}
			break;
		}
		case 11:	//LOAD
		{
			cout << "Error: LOAD Instruction not implemented." << endl;
			break;
		}
		case 12:	//STORE
		{
			cout << "Error: STORE Instruction not implemented." << endl;
			break;
		}
		default:
			cout << "Error: There was an error with the Execute Stage." << endl;
			cout << "PC: " << std::hex << pc << endl;
			cout << "Current Istruction: " <<std::hex << *currentInstruction << endl;
			isUserMode = false;
			break;
	}
}

/* Memory Access */
void Sim::MEM()
{
	//store or load from Memory
	mem_wb_new.operationCode = ex_mem_old.operationCode;
	mem_wb_new.instruct = ex_mem_old.instruct;
	switch(ex_mem_old.operationCode)
	{
		case 0:
		{
			//Nop
			mem_wb_new.immediate = 0 ;
			mem_wb_new.registerOne = 0 ;
			mem_wb_new.registerTwo = 0 ;
			mem_wb_new.registerThree = 0 ;
			mem_wb_new.valueA = 0 ;
			mem_wb_new.valueB = 0 ;
			mem_wb_new.aluOutput = 0;
			mem_wb_new.memoryReadOutput =0;
			break;
		}
		case 1: //ADDI ADD IMMEDIATE
		{
			mem_wb_new.immediate = ex_mem_old.immediate;
			mem_wb_new.registerTwo = ex_mem_old.registerTwo;
			mem_wb_new.registerOne = ex_mem_old.registerOne;
			mem_wb_new.valueA = ex_mem_old.valueA;
			mem_wb_new.aluOutput = ex_mem_old.aluOutput;
			break;
		}
		case 2: //B BRANCH
		{
			break;
		}
		case 3: //BEQZ BRACH IF EQUAL TO ZERO
		{
			break;
		}
		case 4: //BGE BRANCH IF GREATER OR EQUAL $t0,$t1,target,  $t0 >= $t1
		{
			break;
		}
		case 5: //BNE BRANCH IF NOT EQUAL  $t0,$t1,target, $t0 <> $t1
		{
			break;
		}
		case 6: //LA LOAD ADDRESS
		{
			mem_wb_new.registerOne = ex_mem_old.registerOne;
			mem_wb_new.valueA = ex_mem_old.valueA;
			mem_wb_new.aluOutput = ex_mem_old.valueA;
			break;
		}
		case 7: //LB LOAD BYTE
		{
			mem_wb_new.registerOne = ex_mem_old.registerOne;
			mem_wb_new.registerTwo = ex_mem_old.registerTwo;
			mem_wb_new.immediate = ex_mem_old.immediate;
			mem_wb_new.valueA = ex_mem_old.valueA;//number of bytes
			mem_wb_new.aluOutput = ex_mem_old.aluOutput;
			mem_wb_new.memoryReadOutput = memory->readByte(ex_mem_old.aluOutput, ex_mem_old.aluOutput%4);
			break;
		}
		case 8: //LI LOAD IMMEDIATE
		{
			mem_wb_new.registerOne = ex_mem_old.registerOne;
			mem_wb_new.registerTwo = ex_mem_old.registerTwo;
			mem_wb_new.valueA = ex_mem_old.valueA;
			mem_wb_new.aluOutput = ex_mem_old.aluOutput;
			break;
		}
		case 9: //SUBI SUBTRACT IMMEDIATE
		{
			mem_wb_new.immediate = ex_mem_old.immediate;
			mem_wb_new.valueA = ex_mem_old.valueA;
			mem_wb_new.registerTwo = ex_mem_old.registerTwo;
			mem_wb_new.registerOne = ex_mem_old.registerOne;
			mem_wb_new.aluOutput = ex_mem_old.aluOutput;
			break;
		}
		case 10: //SYSCALL
		{
			mem_wb_new.registerTwo = ex_mem_old.registerTwo;
			mem_wb_new.valueB = ex_mem_old.valueB;
			switch(ex_mem_old.valueB)
			{
				case 1:
				{
					if(ex_mem_old.registerOne == 1)
						{
							cout << "Printed Integer: 1001" << endl;
						}
					break;
				}
				case 4:	//Print String
				{
					mem_wb_new.registerOne = ex_mem_old.registerOne;
					mem_wb_new.valueA = ex_mem_old.valueA;
					cout << memory->readStringFromMemory(ex_mem_old.valueA) << endl;
					break;
				}
				case 8:	//Read String In
				{
					mem_wb_new.registerOne = ex_mem_old.registerOne;
					mem_wb_new.valueA = ex_mem_old.valueA;

					char palin[1024];
					string incoming_palin;

					int length=1024;
				    for (int i=0;i<1024;i++)		// clear memory
				    {
				            palin[i]=0;
				    }
					cout << "Please enter a word: ";
					getline(cin, incoming_palin);
					incoming_palin.copy(palin,1024,0);
					int len=strlen(palin);
					palin[len] = '\0';
					//cin >> *palin >> "\0";
					memory->loadData(ex_mem_old.valueA,palin);
					break;
				}
				case 10:// End Program
				{
					break;
				}
				default:
				{
					cout << "Error: There was an error with the Memory of SYSCALL." << endl;
					cout << "PC: " << std::hex << pc << endl;
					cout << "Current Istruction: " <<std::hex << *currentInstruction << endl;
					isUserMode = false;
					break;
				}
			}
			break;
		}
		case 11:	//LOAD
		{
			cout << "Error: LOAD Instruction not implemented." << endl;
			break;
		}
		case 12:	//STORE
		{
			cout << "Error: STORE Instruction not implemented." << endl;
			break;
		}
		default:
			cout << "Error: There was an error with the Memory Stage." << endl;
			cout << "PC: " << std::hex << pc << endl;
			cout << "Current Istruction: " <<std::hex << *currentInstruction << endl;
			isUserMode = false;
			break;
	}
}

/* Write Back */
void Sim::WB()
{
	//store in registers
	switch(mem_wb_old.operationCode)
	{
		case 0:
		{
			numberOfNoOperations++;
			//Nop
			break;
		}
		case 1: //ADDI ADD IMMEDIATE
		{
			bool success = registers->writeToRegister(mem_wb_old.registerOne, mem_wb_old.aluOutput);
			if (false == success)
			{
				cout << "Error: Adding value (1) to register: "<< std::dec << mem_wb_old.registerOne << endl;
			}

			instructionsExecuted++;
			break;
		}
		case 2: //B BRANCH
		{
			instructionsExecuted++;
			break;
		}
		case 3: //BEQZ BRACH IF EQUAL TO ZERO
		{
			instructionsExecuted++;
			break;
		}
		case 4: //BGE BRANCH IF GREATER OR EQUAL $t0,$t1,target,  $t0 >= $t1
		{
			instructionsExecuted++;
			break;
		}
		case 5: //BNE BRANCH IF NOT EQUAL  $t0,$t1,target, $t0 <> $t1
		{
			instructionsExecuted++;
			break;
		}
		case 6: //LA LOAD ADDRESS
		{
			bool success = registers->writeToRegister(mem_wb_old.registerOne,mem_wb_old.valueA);
			if (false == success)
			{
				cout << "Error: Loading Address (6) to register: "<< std::dec << mem_wb_old.registerOne << endl;
			}
			instructionsExecuted++;
			break;
		}
		case 7: //LB LOAD BYTE
		{
			bool success = registers->writeToRegister(mem_wb_old.registerOne, mem_wb_old.memoryReadOutput );
			if (false == success)
			{
				cout << "Error: Loading Byte (7) into register: "<< std::dec << mem_wb_old.registerOne << endl;
			}
			instructionsExecuted++;
			break;
		}
		case 8: //LI LOAD IMMEDIATE
		{
			bool success = registers->writeToRegister(mem_wb_old.registerOne, mem_wb_old.registerTwo);
			if (false == success)
			{
				cout << "Error: Loading Immediate value (8) to register: "<< std::dec << mem_wb_old.registerOne << endl;
			}
			instructionsExecuted++;
			break;
		}
		case 9: //SUBI SUBTRACT IMMEDIATE
		{
			bool success = registers->writeToRegister(mem_wb_old.registerOne, mem_wb_old.aluOutput);
			if (false == success)
			{
				cout << "Error: Adding value (9) to register: "<< std::dec << mem_wb_old.registerOne << endl;
			}
			instructionsExecuted++;
			break;
		}
		case 10: //SYSCALL
		{
			switch(mem_wb_old.valueB)
			{
				case 1:
				{
					instructionsExecuted++;
					break;
				}
				case 4:	//Print String
				{
					instructionsExecuted++;
					break;
				}
				case 8:	//Read String In
				{
					instructionsExecuted++;
					break;
				}
				case 10:// End Program
				{
					instructionsExecuted++;
					isUserMode = false;
					printValuesToConsole(instructionsExecuted, cyclesSpentInExecution, numberOfNoOperations);
					break;
				}
				default:
				{
					cout << "Error: There was an error with the Write Buffer of SYSCALL." << endl;
					cout << "PC: " << std::hex << pc << endl;
					cout << "Current Istruction: " <<std::hex << currentInstruction << endl;
					isUserMode = false;
					break;
				}
			}
			break;
		}
		case 11:	//LOAD
		{
			cout << "Error: LOAD Instruction not implemented." << endl;
			break;
		}
		case 12:	//STORE
		{
			cout << "Error: STORE Instruction not implemented." << endl;
			break;
		}
		default:
			cout << "Error: There was an error with the Write Buffer Stage." << endl;
			cout << "PC: " << std::hex << pc << endl;
			cout << "Current Istruction: " <<std::hex << currentInstruction << endl;
			isUserMode = false;
			break;
	}
}

/* Loads the next instruction and increments the program counter -- exact same as project 2 */
void Sim::loadNextInstruction() {															
	currentInstruction = memory -> readFromMemory(pc);
	pc++;
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
