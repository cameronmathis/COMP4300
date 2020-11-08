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

// Press any key to continue
void PressAnyKey(void);

struct if_id {
	instruction *instruct;
};

struct id_ex {
	instruction op;
	instruction inst;
	int8_t immediate;
	memoryAddress first_reg;
	memoryAddress second_reg;
	memoryAddress third_reg;
	instruction op_A;
	instruction op_B;
};

struct ex_mem {
	instruction op;
	instruction inst;
	int8_t immediate;
	memoryAddress first_reg;
	memoryAddress second_reg;
	memoryAddress third_reg;
	instruction op_A;
	instruction op_B;
	instruction alu_results;
};

struct mem_wb {
	instruction op;
	instruction inst;
	int8_t immediate;
	memoryAddress first_reg;
	memoryAddress second_reg;
	memoryAddress third_reg;
	instruction op_A;
	instruction op_B;
	instruction alu_results;
	instruction mem_read_results;
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
		int instructionsExecuted = 0;
		int cyclesSpentInExecution = 0;
		int totalNumberOfNoOperations = 0;
		bool isUserMode = true;		
		void IF();
		void ID();
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

void PressAnyKey(void) {
	cout << "Press any key to continue" << endl;
	cin.ignore().get(); // Pause command line
}


int main() {
	Sim *sim = new Sim();
	sim->run();
	return 0;
}

Sim::Sim()
{
	programCounter = textTop;
	memory = new Memory();
	registers = new RegisterBank();
	instructionsExecuted = 0;
	cyclesSpentInExecution = 0;
	totalNumberOfNoOperations = 0;
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

void Sim::run()
{
	while(isUserMode) {
		if_id_old = if_id_new;
		IF();
		id_ex_old = id_ex_new;
		ID();
		ex_mem_old = ex_mem_new;
		EX();
		mem_wb_old = mem_wb_new;
		MEM();
		WB();
		cyclesSpentInExecution++;
	}
}

/*
Used Paramaters:
Effected buffers:
*/
void Sim::IF()
{
	if_id_new.instruct = memory -> readFromMemory(programCounter);
	programCounter++;
}
/*
Used Paramaters:
Effected buffers:
*/
void Sim::ID()
{

	if(if_id_old.instruct != 0 ) //nop
	{
		//cout << std::hex << *(if_id_old.instruction) << endl;
		currentInstruction = if_id_old.instruct;
		id_ex_new.op = getCurrentOperationCode();
		id_ex_new.inst = *currentInstruction;
	}
	else
	{
		id_ex_new.op = 0;
		id_ex_new.inst = 0;
	}
	switch(id_ex_new.op)
	{
		case 0:
		{
			//Nop
			id_ex_new.immediate = 0 ;
			id_ex_new.first_reg = 0 ;
			id_ex_new.second_reg = 0 ;
			id_ex_new.third_reg = 0 ;
			id_ex_new.op_A = 0 ;
			id_ex_new.op_B = 0 ;
			break;
		}
		case 1: //ADDI ADD IMMEDIATE
		{
			id_ex_new.immediate = getSignedImmediate(rightBits());
			id_ex_new.second_reg = centerBits();
			id_ex_new.first_reg = leftBits();
			id_ex_new.op_A = registers->readFromRegister(centerBits());
			break;
		}
		case 2: //B BRANCH
		{
			int8_t label_offset =0;
			label_offset = getSignedImmediate(rightBits());
			programCounter--;
			programCounter += label_offset;
			break;
		}
		case 3: //BEQZ BRACH IF EQUAL TO ZERO
		{
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
		case 4: //BGE BRANCH IF GREATER OR EQUAL $t0,$t1,target,  $t0 >= $t1
		{
			instruction first_regist_value = registers->readFromRegister(leftBits());
			instruction second_regist_value = registers->readFromRegister(centerBits());
			if(leftBits() == ex_mem_old.first_reg)
			{
				first_regist_value = ex_mem_old.alu_results;
			}
			if(leftBits() == mem_wb_old.first_reg)
			{
				first_regist_value = mem_wb_old.alu_results;
			}
			if(centerBits() == ex_mem_old.first_reg)
			{
				second_regist_value = ex_mem_old.alu_results;
			}
			if(centerBits() == ex_mem_new.first_reg)
			{
				second_regist_value = ex_mem_new.alu_results;
			}
			if(centerBits() == mem_wb_old.first_reg)
			{
				second_regist_value = mem_wb_old.alu_results;
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
		case 5: //BNE BRANCH IF NOT EQUAL  $t0,$t1,target, $t0 <> $t1
		{
			instruction first_regist_value = registers->readFromRegister(leftBits());
			instruction second_regist_value = registers->readFromRegister(centerBits());
			if(centerBits() == ex_mem_new.first_reg)
			{
				second_regist_value = memory->readByte(ex_mem_new.alu_results, ex_mem_new.alu_results%4);
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
			id_ex_new.first_reg = leftBits();
			id_ex_new.op_A = immediateValue();
			break;
		}
		case 7: //LB LOAD BYTE
		{
			id_ex_new.first_reg = leftBits();
			id_ex_new.second_reg = centerBits();
		 	id_ex_new.immediate = getSignedImmediate(rightBits());
			id_ex_new.op_A = registers->readFromRegister(centerBits());//number of bytes
			break;
		}
		case 8: //LI LOAD IMMEDIATE
		{
			id_ex_new.first_reg = leftBits();
			id_ex_new.second_reg = centerBits();
			id_ex_new.op_A = centerBits();
			break;
		}
		case 9: //SUBI SUBTRACT IMMEDIATE
		{
			id_ex_new.immediate = getSignedImmediate(rightBits());
			id_ex_new.op_A = registers->readFromRegister(centerBits());
			id_ex_new.second_reg = centerBits();
			id_ex_old.first_reg = leftBits();
			break;
		}
		case 10: //SYSCALL
		{
			id_ex_new.second_reg = 3;
			id_ex_new.op_B = registers->readFromRegister(3);
			switch(id_ex_new.op_B)
			{
				case 1:
				{
					id_ex_new.first_reg = 1;
					id_ex_new.op_A = registers->readFromRegister(1);
					break;
				}
				case 4:	//Print String
				{
					id_ex_new.first_reg = 1;
					id_ex_new.op_A = registers->readFromRegister(1);
					break;
				}
				case 8:	//Read String In
				{
					id_ex_new.first_reg = 1;
					id_ex_new.op_A = registers->readFromRegister(1);
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

/*
Used Paramaters:
Effected buffers:
*/
void Sim::EX()
{
	//run the instruciton
	ex_mem_new.op = id_ex_old.op;
	ex_mem_new.inst = id_ex_old.inst;
	switch(id_ex_old.op)
	{
		case 0:
		{
			//Nop
			ex_mem_new.immediate = 0 ;
			ex_mem_new.first_reg = 0 ;
			ex_mem_new.second_reg = 0 ;
			ex_mem_new.third_reg = 0 ;
			ex_mem_new.op_A = 0 ;
			ex_mem_new.op_B = 0 ;
			ex_mem_new.alu_results = 0;
			break;
		}
		case 1: //ADDI ADD IMMEDIATE
		{
			ex_mem_new.immediate = id_ex_old.immediate;
			ex_mem_new.second_reg = id_ex_old.second_reg;
			ex_mem_new.first_reg = id_ex_old.first_reg;
			ex_mem_new.op_A = id_ex_old.op_A;
			if(id_ex_old.second_reg == ex_mem_old.first_reg)
			{
				id_ex_old.op_A = ex_mem_old.alu_results;
			}
			if(id_ex_old.second_reg == mem_wb_old.first_reg)
			{
				id_ex_old.op_A = mem_wb_old.alu_results;
			}
			ex_mem_new.alu_results = id_ex_old.immediate + id_ex_old.op_A;
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
			ex_mem_new.first_reg = id_ex_old.first_reg;
			ex_mem_new.op_A = id_ex_old.op_A;
			if(id_ex_old.second_reg == ex_mem_old.first_reg)
			{
				id_ex_old.op_A = ex_mem_old.alu_results;
			}
			if(id_ex_old.second_reg == mem_wb_old.first_reg)
			{
				id_ex_old.op_A = mem_wb_old.alu_results;
			}
			ex_mem_new.alu_results = id_ex_old.op_A;
			break;
		}
		case 7: //LB LOAD BYTE
		{
			ex_mem_new.first_reg = id_ex_old.first_reg;
			ex_mem_new.second_reg = id_ex_old.second_reg;
			ex_mem_new.immediate = id_ex_old.immediate;
			ex_mem_new.op_A = id_ex_old.op_A;			//number of bytes
			if(id_ex_old.second_reg == ex_mem_old.first_reg)
			{
				id_ex_old.op_A = ex_mem_old.alu_results;
			}
			if(id_ex_old.second_reg == mem_wb_old.first_reg)
			{
				id_ex_old.op_A = mem_wb_old.alu_results;
			}
			ex_mem_new.alu_results = id_ex_old.op_A + id_ex_old.immediate;
			break;
		}
		case 8: //LI LOAD IMMEDIATE
		{
			ex_mem_new.first_reg = id_ex_old.first_reg;
			ex_mem_new.second_reg = id_ex_old.second_reg;
			ex_mem_new.op_A = id_ex_old.op_A;
			if(id_ex_old.second_reg == ex_mem_old.first_reg)
			{
				id_ex_old.op_A = ex_mem_old.alu_results;
			}
			if(id_ex_old.second_reg == mem_wb_old.first_reg)
			{
				id_ex_old.op_A = mem_wb_old.alu_results;
			}
			ex_mem_new.alu_results = id_ex_old.op_A;
			break;
		}
		case 9: //SUBI SUBTRACT IMMEDIATE
		{
			ex_mem_new.immediate = id_ex_old.immediate;
			ex_mem_new.op_A = id_ex_old.op_A;
			ex_mem_new.second_reg = id_ex_old.second_reg;
			ex_mem_new.first_reg = id_ex_old.first_reg;
			if(id_ex_old.second_reg == ex_mem_old.first_reg)
			{
				id_ex_old.op_A = ex_mem_old.alu_results;
			}
			if(id_ex_old.second_reg == mem_wb_old.first_reg)
			{
				id_ex_old.op_A = mem_wb_old.alu_results;
			}
			ex_mem_new.alu_results =  id_ex_old.op_A + (- id_ex_old.immediate);
			break;
		}
		case 10: //SYSCALL
		{
			instruction regist_value = id_ex_old.op_B;
			if(id_ex_old.second_reg == ex_mem_old.first_reg)
			{
				regist_value = ex_mem_old.alu_results;
			}
			if(id_ex_old.second_reg == mem_wb_old.first_reg)
			{
				regist_value = mem_wb_old.alu_results;
			}
			ex_mem_new.second_reg = id_ex_old.second_reg;
			ex_mem_new.op_B = regist_value;
			switch(regist_value)
			{
				case 1:
				{
					ex_mem_new.first_reg = id_ex_old.first_reg;
					ex_mem_new.op_A = id_ex_old.op_A;
					break;
				}
				case 4:	//Print String
				{
					id_ex_old.first_reg =1;
					instruction regist_value = id_ex_old.op_A;
					if(id_ex_old.first_reg == ex_mem_old.first_reg)
					{
						regist_value = ex_mem_old.alu_results;
					}
					if(id_ex_old.first_reg == mem_wb_old.first_reg)
					{
						regist_value = mem_wb_old.alu_results;
					}
					if(id_ex_old.first_reg == mem_wb_new.first_reg)
					{
						regist_value = mem_wb_new.alu_results;
					}
					ex_mem_new.first_reg = id_ex_old.first_reg;
					ex_mem_new.op_A = regist_value;
					break;
				}
				case 8:	//Read String In
				{
					id_ex_old.first_reg = 1;
					instruction regist_value = id_ex_old.op_A;
					if(id_ex_old.first_reg == ex_mem_old.first_reg)
					{
						regist_value = ex_mem_old.alu_results;
					}
					if(id_ex_old.first_reg == mem_wb_old.first_reg)
					{
						regist_value = mem_wb_old.alu_results;
					}
					if(id_ex_old.first_reg == mem_wb_new.first_reg)
					{
						regist_value = mem_wb_new.alu_results;
					}
					ex_mem_new.first_reg = id_ex_old.first_reg;
					ex_mem_new.op_A = regist_value;
					break;
				}
				case 10:// End Program
				{
					break;
				}
				default:
				{
					cout << "Error: There was an error with the Execute of SYSCALL." << endl;
					cout << "PC: " << std::hex << programCounter << endl;
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
			cout << "PC: " << std::hex << programCounter << endl;
			cout << "Current Istruction: " <<std::hex << *currentInstruction << endl;
			isUserMode = false;
			break;
	}
}

/*
Used Paramaters:
Effected registers:
*/
void Sim::MEM()
{
	//store or load from Memory
	mem_wb_new.op = ex_mem_old.op;
	mem_wb_new.inst = ex_mem_old.inst;
	switch(ex_mem_old.op)
	{
		case 0:
		{
			//Nop
			mem_wb_new.immediate = 0 ;
			mem_wb_new.first_reg = 0 ;
			mem_wb_new.second_reg = 0 ;
			mem_wb_new.third_reg = 0 ;
			mem_wb_new.op_A = 0 ;
			mem_wb_new.op_B = 0 ;
			mem_wb_new.alu_results = 0;
			mem_wb_new.mem_read_results =0;
			break;
		}
		case 1: //ADDI ADD IMMEDIATE
		{
			mem_wb_new.immediate = ex_mem_old.immediate;
			mem_wb_new.second_reg = ex_mem_old.second_reg;
			mem_wb_new.first_reg = ex_mem_old.first_reg;
			mem_wb_new.op_A = ex_mem_old.op_A;
			mem_wb_new.alu_results = ex_mem_old.alu_results;
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
			mem_wb_new.first_reg = ex_mem_old.first_reg;
			mem_wb_new.op_A = ex_mem_old.op_A;
			mem_wb_new.alu_results = ex_mem_old.op_A;
			break;
		}
		case 7: //LB LOAD BYTE
		{
			mem_wb_new.first_reg = ex_mem_old.first_reg;
			mem_wb_new.second_reg = ex_mem_old.second_reg;
			mem_wb_new.immediate = ex_mem_old.immediate;
			mem_wb_new.op_A = ex_mem_old.op_A;//number of bytes
			mem_wb_new.alu_results = ex_mem_old.alu_results;
			mem_wb_new.mem_read_results = memory->readByte(ex_mem_old.alu_results, ex_mem_old.alu_results%4);
			break;
		}
		case 8: //LI LOAD IMMEDIATE
		{
			mem_wb_new.first_reg = ex_mem_old.first_reg;
			mem_wb_new.second_reg = ex_mem_old.second_reg;
			mem_wb_new.op_A = ex_mem_old.op_A;
			mem_wb_new.alu_results = ex_mem_old.alu_results;
			break;
		}
		case 9: //SUBI SUBTRACT IMMEDIATE
		{
			mem_wb_new.immediate = ex_mem_old.immediate;
			mem_wb_new.op_A = ex_mem_old.op_A;
			mem_wb_new.second_reg = ex_mem_old.second_reg;
			mem_wb_new.first_reg = ex_mem_old.first_reg;
			mem_wb_new.alu_results = ex_mem_old.alu_results;
			break;
		}
		case 10: //SYSCALL
		{
			mem_wb_new.second_reg = ex_mem_old.second_reg;
			mem_wb_new.op_B = ex_mem_old.op_B;
			switch(ex_mem_old.op_B)
			{
				case 1:
				{
					if(ex_mem_old.first_reg == 1)
						{
							cout << "Printed Integer: 1001" << endl;
						}
					break;
				}
				case 4:	//Print String
				{
					mem_wb_new.first_reg = ex_mem_old.first_reg;
					mem_wb_new.op_A = ex_mem_old.op_A;
					cout << memory->readStringFromMemory(ex_mem_old.op_A) << endl;
					break;
				}
				case 8:	//Read String In
				{
					mem_wb_new.first_reg = ex_mem_old.first_reg;
					mem_wb_new.op_A = ex_mem_old.op_A;

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
					memory->loadData(ex_mem_old.op_A,palin);
					break;
				}
				case 10:// End Program
				{
					break;
				}
				default:
				{
					cout << "Error: There was an error with the Memory of SYSCALL." << endl;
					cout << "PC: " << std::hex << programCounter << endl;
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
			cout << "PC: " << std::hex << programCounter << endl;
			cout << "Current Istruction: " <<std::hex << *currentInstruction << endl;
			isUserMode = false;
			break;
	}
}

/*
Used Paramaters:
Effected buffers:
*/
void Sim::WB()
{
	//store in registers
	switch(mem_wb_old.op)
	{
		case 0:
		{
			totalNumberOfNoOperations++;
			//Nop
			break;
		}
		case 1: //ADDI ADD IMMEDIATE
		{
			bool success = registers->writeToRegister(mem_wb_old.first_reg, mem_wb_old.alu_results);
			if (false == success)
			{
				cout << "Error: Adding value (1) to register: "<< std::dec << mem_wb_old.first_reg << endl;
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
			bool success = registers->writeToRegister(mem_wb_old.first_reg,mem_wb_old.op_A);
			if (false == success)
			{
				cout << "Error: Loading Address (6) to register: "<< std::dec << mem_wb_old.first_reg << endl;
			}
			instructionsExecuted++;
			break;
		}
		case 7: //LB LOAD BYTE
		{
			bool success = registers->writeToRegister(mem_wb_old.first_reg, mem_wb_old.mem_read_results );
			if (false == success)
			{
				cout << "Error: Loading Byte (7) into register: "<< std::dec << mem_wb_old.first_reg << endl;
			}
			instructionsExecuted++;
			break;
		}
		case 8: //LI LOAD IMMEDIATE
		{
			bool success = registers->writeToRegister(mem_wb_old.first_reg, mem_wb_old.second_reg);
			if (false == success)
			{
				cout << "Error: Loading Immediate value (8) to register: "<< std::dec << mem_wb_old.first_reg << endl;
			}
			instructionsExecuted++;
			break;
		}
		case 9: //SUBI SUBTRACT IMMEDIATE
		{
			bool success = registers->writeToRegister(mem_wb_old.first_reg, mem_wb_old.alu_results);
			if (false == success)
			{
				cout << "Error: Adding value (9) to register: "<< std::dec << mem_wb_old.first_reg << endl;
			}
			instructionsExecuted++;
			break;
		}
		case 10: //SYSCALL
		{
			switch(mem_wb_old.op_B)
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
					cout << endl;
					cout << "Number of Instructions Executed (IC): " << std::dec<< instructionsExecuted << endl;
					cout << "Number of Cycles Spent in Execution (C): " <<std::dec<<  cyclesSpentInExecution << endl;
					cout << "Number of NOPs: " << std::dec << totalNumberOfNoOperations << endl;
					cout << "Goodbye." << endl;
					isUserMode = false;
					break;
				}
				default:
				{
					cout << "Error: There was an error with the Write Buffer of SYSCALL." << endl;
					cout << "PC: " << std::hex << programCounter << endl;
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
			cout << "PC: " << std::hex << programCounter << endl;
			cout << "Current Istruction: " <<std::hex << currentInstruction << endl;
			isUserMode = false;
			break;
	}
}

/* Loads the next instruction and increments the program counter -- exact same as project 2 */
void Sim::loadNextInstruction() {															
	currentInstruction = memory -> readFromMemory(programCounter);
	programCounter++;
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
