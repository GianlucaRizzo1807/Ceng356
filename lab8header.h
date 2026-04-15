/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   lab8header.h
 * Author: 
 *
 */

#ifndef LAB8HEADER_H
#define LAB8HEADER_H

unsigned int CPU_fetchCode(char *mem, int codeOffset);
unsigned char CPU_Decode(unsigned int machineCode);
void CPU_Execution(unsigned char opcode, unsigned int machineCode, char *mem);
void printRegisterFiles();
void printDataMemoryDump(unsigned char *mem);
unsigned int PCRegister = 0;  // PC register, always pointing to the next instruction.  

#endif /* LAB8HEADER_H */

