/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
// Please finish the following functions for lab 8.
// Lab 8 will perform the following functions:
//   1. Fetch the code stored in memory
//   2. Decode the code and prepare for the execution of the code.
//   3. Setup the execution function for CPU.

// Lab 9 will perform the following functions:
//   4. Execute the code stored in the memory and print the results. 
#include "header.h"
#include "lab8header.h"
extern char *regNameTab[N_REG];
extern unsigned int PCRegister; // PC register, always pointing to the next instruction.  

void CPU(unsigned char *mem){
    unsigned int machineCode = 0;
    unsigned char opcode = 0;
    PCRegister = CODESECTION;  // at the beginning, PCRegister is the starting point,
                       // it should be a global integer defined in header.h
    do{
      printf("\nPC:%x\n", PCRegister);
      machineCode = CPU_fetchCode(mem, PCRegister);
      if (machineCode == 0)  // quit the program when machineCode is 0, that is the end of the code.
          break;  // break the infinite loop. 
      PCRegister += 4;                                                     // update the program counter
      opcode = CPU_Decode(machineCode);
      printf("Decoded Opcode is: %02X. \n", opcode);

      // Lab 9: Finish the execution of the code.
      // Only finish this part when the CPU_Decode is done.
      // CPU_Execution(opcode, machineCode, mem);
    }while (1);  // This is an infinite while loop
                 // When you fetch a machineCode of 00000000, the loop breaks.
    printRegisterFiles();     // After the code execution, print all the register contents on screen.
    printDataMemoryDump(mem); // After the code execution, print the memory dump of the data section.
}

// Lab 8 - Step 1. Finish the CPU_fectchCode function to
//         read the code section from memory and
//         get the 32-bit machine code from the memory.
unsigned int CPU_fetchCode(char *mem, int codeOffset){
    return read_dword(mem, codeOffset);
}

// Lab 8 - Step 2. Finish the CPU_Decode function to
//         decode the instruction and return the
//         opcode/function of the instruction.
//         Hints: Need to consider how to find the opcode/function from different types of instructions:
//                i.e., I-, J- and R-type instructions. 
//                The return value should be a 6-bit bianry code. 
unsigned char CPU_Decode(unsigned int machineCode){
    unsigned char opcode;
    opcode = (machineCode >> 26) & 0x3F;
    if (opcode == 0x00){
        return (unsigned char)(machineCode & 0x3F);
    }
return opcode;

}
// Lab 9: Finish the function CPU_Execution to run all the instructions.
void CPU_Execution(unsigned char opcode, unsigned int machineCode, char *mem){
    unsigned char rt = 0;
    switch (opcode)  // execute different functions when opcode is set differently. 
    {
		// This is an example how lab will be executed. Please follow this example and finish exections of the code.
		// Hint: you need to implement the following instructions here: 
		//       la, add, lb, bge, lw, sw, addi, j
        case 0b101111:   //"la" instruction. 
            // assign the address rt = immediate address stored in machineCode;
            // first find the rt index in the register array. 
            rt = (machineCode & 0x001F0000) >> 16; 
            // assign the address stored in immediate field to regFile[rt];
            regFile[rt] = machineCode & 0x0000FFFF;  // get the last 16 bit as address. 
            // update PCregister ???? Pay special attention to branch instructions. 
            PCRegister += 4;
            if (DEBUG_CODE){   // print the hints to the user in DEBUG_MODE
                printf("Code Executed: %08X\n", machineCode);
                printf("****** PC Register is %08X ******\n", PCRegister);                
            }            
            break;
        case 0b100000://"lb" instruction. 
		    //....
			break;
		
        // continue to all the other cases used in the program.
        // case ......:    
            
            
        // Should never go to default part when complete. Otherwise, that is a mistake. 
        default:
            printf("Wrong instruction! You need to fix this instruction %02X %08X\n", opcode,  machineCode);
            system("PAUSE");
            exit(3);  // exit the program if running here.     
            break;
    }
}
// Lab 8 - Step 3. Print all the 32 registers in regFile and names saved in
//         regNameTab. For example, it should print
//         $zero = 0x00000000
//         $at  = ... ... etc.
void printRegisterFiles(){
    int i;

    puts("\n---- Register File Dump ----");
    for (i = 0; i < N_REG; i++) {
        if (regNameTab[i] != NULL) {
            printf("%-6s = 0x%08X\n", regNameTab[i], regFile[i]);
        } else {
            printf("$r%-3d = 0x%08X\n", i, regFile[i]);
        }
    }
}

 // Lab 8 - Step 4. Call function memory_dump and pass the proper parameters to dump the first 256
//          bytes from Data section.
void printDataMemoryDump(unsigned char *mem){
    puts("\nData Memory Dump");
    memory_dump(mem, DATASECTION, 256);
}
