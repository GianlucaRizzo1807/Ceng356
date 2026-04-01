/**
 * @file mips_asm.h
 *
 * Definitions 
 *
 * @author Yooseong Kim, Aug., 2010
 */
#ifndef MIPS_ASM_H
#define MIPS_ASM_H
#include <string.h>

int n_Label = 0;
char *regNameTab[N_REG] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",\
                           "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", \
                           "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", \
                           "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra", \
                           "lo", "hi"};
char *strlwr (char *);  // has to have this prototype to remove the warnings. 
int findLabel(char *label);    // Find the offset address from the labels. 
int getRegNum(char *regName);
int addLabel(char *label, int segType, int offset);
int addData(char *token, char *buffer, int offset);
void addInst(char *inst, char *operands, int offset);
 
#endif