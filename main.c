 /*
 ============================================================================
 Name        : main.c
 Author      : Austin Tian
 Revised by  :
 Version     :
 Copyright   : Copyright 2023
 Description : main code in C
 ============================================================================
 */
#include "header.h"

MIPS_Instruction Instruction_storage[MAX_SIZE];
int regFile[N_REG];
char Data_storage[MAX_SIZE];
unsigned int totalDataByte;
labelType labelTab[MAX_LABEL];
int main (int argc, char *argv[]) {        
        // Don't touch the commented code until told so in lab 7        
        
	    char *mem = NULL;  //  memory space pointer.
        FILE *fp = NULL;
        int stopChar;
	    // Step 1: Parse the ASM code.
		if (argc < 2) {
            puts("\nIncorrect number of arguments.");
            puts("Usage: ProgramName.exe MIPSCode.asm \n");
            stopChar = getchar();
            return EXIT_FAILURE;  // if the file cannot be found, then return.
        }
        if ((fp = fopen(argv[1], "r"))== NULL) {  // read the asm file.
                printf("Input file could not be opened.");
                stopChar = getchar();
                return EXIT_FAILURE;  // if the file cannot be read, then return.
        }
        parse_MIPS(fp);  // call this function to parse the code into
                        // Data_storage, Instruction_Storage and labelTab 
		
        // Step 2: Setup the memory (lab 6)
          // setup_memory(); // Important! -- REMOVE THIS LINE FOR LAB 7 and later.
          // include your memory.c file into the project from lab 6 and later on so you can use memory related functions;
          /* The following code is for code after lab 7.*/
          // IMPORTANT: When you initialize the memory, please fill in 0, not the random number
          // to all the memory locations.
        mem = init_memory();  // start the memory to hold the code.
        stopChar = getchar();
        // Step 3: Load the code into memory (lab 7)
        puts("----Lab 7 Code Starts to Parse the ASM Code----");
        loadCodeToMem(mem);
        stopChar = getchar();
		// puts("----Lab 8 Code Starts to Fetch and Decode the Code ----");
        // Step 4: CPU Fetch the machinecode (lab 8)
        // Step 5: CPU Decode the machine code (lab 8)
		
		// puts("----Lab 9 Code Starts to Execute the Code ----");
        // Step 6: Execute the code until the code is finished(lab 9)                  
        CPU(mem);  // this is needed in lab 8 and later 
          // stopChar = getchar();
        free_memory(mem);  // free the memory, this is for lab 7 and later 
}
