/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   header.h
 *
 *
 * Created on February 28, 2019, 8:47 PM
 */

#ifndef HEADER_H
#define HEADER_H

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/param.h>
#include <stdbool.h>
// #include <conio.h>

#include <ctype.h>
#define MEM_SIZE 1024*1024   // reserve 1 M bytes to play.
#define MIN_DUMP_SIZE  256   // minimum size of the dump size is set to 256 bytes.
#define DUMP_LINE 16    // the size of the memory to be dumped on each row.

#define N_REG 34
#define MAX_SIZE 2*1024  // set storage of data and code as 2k instructions/byte each.
#define MAX_LABEL 256

#define REG_LO 32
#define REG_HI 33

/*#define STACK_SEGMENT_BASE (256*1024 - 4) //0x7fffffff
#define DATA_SEGMENT_BASE 8192 //0x10000000
#define TEXT_SEGMENT_BASE 0 //0x00400000
*/
#define STACK_SEGMENT_BASE (MAX_SIZE - 4) //the stack is on the bottom of data section.
#define DATA_SEGMENT_BASE 0 //Data section array starts from 0
#define TEXT_SEGMENT_BASE 0     // Code section array starts from 0; nothing before it.

#define DEBUG_CODE   1     // set a flag to print debug information.

#define Endian LITTLE_ENDIAN

#define WORD_SIZE 4
#define DATASECTION 0x2000
#define CODESECTION 0x0000
/**
 * How an instruction is stored in the storage
 */
typedef struct MIPS_Instruction
{
    // This is the structure used to store the instructions.
       //  char *instruction;      ///< instruction name
        char instruction[8];  // use the instruction field.
        int rd;                                 ///< register number for rd
        int rs;                                 ///< register number for rs
        int rt;                                 ///< register number for rt
        int shamt;                              // for shift only
        int function;                           // for function.

        union{
            int immediate;                  ///< immediate value or address given directly.
            int address;                    ///< address that is translated from a label
        }; // immediate and address should be treated the same in the parser.

        unsigned int machineCode;  // returned machineCode
}MIPS_Instruction;
extern MIPS_Instruction Instruction_storage[MAX_SIZE];
extern int regFile[N_REG];
extern char Data_storage[MAX_SIZE];
extern unsigned int totalDataByte; // the number of bytes used in Data section
#define BUFMAX 128

 enum segment {DATA=1, TEXT=2};

 /**
  * Label data representation. The corresponding addresses for labels in source code are stored.
  */
 typedef struct labelType
 {
         // char *label;
         char label[20];    ///< label name; less than 20 bytes for now. Can be changed to malloc function.
         int addr;               ///< the associated address
         char type;
 }labelType;

 /**
  * Label table. Used only internally by parser.
  */
extern labelType labelTab[MAX_LABEL];
void parse_MIPS (FILE *fp);
void setup_memory();
void loadCodeToMem();
char *init_memory();
void free_memory(char *base_address);
unsigned char rand_generator();

void write_byte(char *base_address, const int offset, const unsigned char byte_data);
void write_dword(const char *base_address, const int offset, const unsigned int dword_data);
unsigned char read_byte(const char *base_address, int offset);
unsigned int read_dword(const char *base_address, int offset);
void memory_dump(const char *base_address, const int offset, unsigned int dumpsize);
void CPU(unsigned char *mem);   // This function needs to be finished in lab 10. 
#endif /* HEADER_H */

