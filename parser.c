 /**
  * @file parser.c
  *
  * parsing a MIPS assembly file
  *
  * Reference from the work of Yooseong Kim
  *
  * Revised by Austin Tian
  */


// Please don't change anything in this file.

#include "header.h"
#include "parser.h"

/**
 * Given the register name, return the corresponding register number. If register number is directly given, then return the number back.
 *
 * @param regName register name (ex. $t0, $s0, $sp, etc.)
 *
 * @return the register number
 */
int getRegNum(char *regName)
{
        int i;
        if (isdigit(regName[1])){
                return atoi(regName+1);
        }
        else{
                for (i = 0; i < N_REG; i++){
                        if (strcmp(regName, regNameTab[i]) == 0)
                        return i;  // return the register index.
                }
        }

        return -1;
}


 /**
  * add a new label into the label list
  *
  * @param label the name of the label
  * @param segType is it data segment or text segment?
  * @param offset an offset that represents the position within the segment
  *
  * @return returns 0 if no error. Otherwise, returns -1.
  */
 int addLabel(char *label, int segType, int offset)
 {
     int i  = 0;
         if (findLabel(label) >= 0)
         {
                 printf("duplicate label %s found\n", label);
                 return -1;
         }

         // labelTab[n_Label].label = strdup(label);
         if (strcmp(label, "exit") == 0)
             i =0;
         strcpy(labelTab[n_Label].label, label);
         if (segType == DATA){
             labelTab[n_Label].type = DATA;
             labelTab[n_Label++].addr = DATA_SEGMENT_BASE + offset;

         }
         else if (segType == TEXT){
                 labelTab[n_Label].type = TEXT;
                 labelTab[n_Label++].addr = TEXT_SEGMENT_BASE + offset;
         }
         else
                 return -1;
         return 0;
 }

 /**
  * find a label in the label list and return the associated address
  *
  * @param label the name of the label
  *
  * @return the address associated with the label
  */
 int findLabel(char *label)
 {
         int i;
         if (strcmp(label, "exit") == 0)
             i=0;
         for (i = 0; i < n_Label; i++)
         {
                 if (strcmp(labelTab[i].label, label) == 0)
                         return labelTab[i].addr;
         }

         return -1;
 }

 /**
  * modified atoi which supports hexadecimal numbers
  *
  * @param str string which is in either a decimal or a hexadecimal format
  * hexadecimal numbers start with '0x'
  *
  * @return the number represented by the string
  */
 int mod_atoi(char *str)
 {
         int val;

         if (strncmp(str, "0x", 2) == 0)
                 sscanf(str, "%x", &val);
         else
                 val = atoi(str);
         return val;
 }

 /**
  * add a variable declaration in data segment into the storage.
  * ex) .word 1,2,3
  *
  * @param token the type of the trailing list of data (ex. .word)
  * @param buffer the list that has data to initialize (ex. 1,2,3)
  * @param offset the address which this variable should be located at
  *
  * @return the size of the data (ex. 3x4 = 12 in this case)
  */
 int addData(char *token, char *buffer, int offset)
 {
         int i = 0, j = 0;

         char *delim = " ,\r\n";
         char *item;

         int addr = DATA_SEGMENT_BASE + offset;

         if (strcmp(token, ".ascii") == 0 ||
             strcmp(token, ".asciiz") == 0)  // if characters
         {
                 int iBegin = -1, iEnd = -1;
                 for (i = 0; i <strlen(buffer); i++){
                         if (buffer[i] == '\"'){
                                 if (iBegin < 0)
                                         iBegin = i;
                                 else
                                         iEnd = i;
                         }
                 }

                 for (i = iBegin+1; i < iEnd; i++){
                         // special characters.. only supports \t and \n
                         if (buffer[i] == '\\') {
                                 if (buffer[i+1] == 't')
                                         Data_storage[addr++] = '\t';
                                 else if (buffer[i+1] == 'n')
                                         Data_storage[addr++] = '\n';
                                 i++;
                         }
                         else
                                 Data_storage[addr++] = buffer[i];
                 }

                 if (strcmp(token, ".asciiz") == 0)
                         Data_storage[addr++] = '\0';
         }
         else if (strcmp(token, ".byte") == 0){
                 item = strtok(buffer, delim);
                 while (item)
                 {
                     if (item && (*item == '#'))  // remove the comments.
                         break;
                     Data_storage[addr++] = mod_atoi(item);
                     item = strtok(NULL, delim);
                 }
         }
         else if (strcmp(token, ".word") == 0){
                 item = strtok(buffer, delim);
                 while (item)
                 {
                         // in .word case, labels can be given to represent addresses.
                         //  labels that are already found (in the above) can only be supported.
                         int data;
                         if (item && (*item == '#'))  // remove the comments.
                            break;
                         if ((data = findLabel(item)) < 0)
                                 data = mod_atoi(item);
                         if (data == 0xAA00)
                              j = 0;
                         // BIG ENDIAN Format
                         if (Endian == BIG_ENDIAN){
                            Data_storage[addr++] = (data >> 24) & 0xFF;
                            Data_storage[addr++] = (data >> 16) & 0xFF;
                            Data_storage[addr++] = (data >> 8)  & 0xFF;
                            Data_storage[addr++] = (data)       & 0xFF;
                         }
                         else{ // little endian format
                            Data_storage[addr++] = (data)        & 0xFF;
                            Data_storage[addr++] = (data >> 8)   & 0xFF;
                            Data_storage[addr++] = (data >> 16)  & 0xFF;
                            Data_storage[addr++] = (data >> 24)  & 0xFF;
                         }
                         item = strtok(NULL, delim);
                 }
         }
         else if (strcmp(token, ".space") == 0){
                 item = strtok(buffer, delim);
                 int size = mod_atoi(item);
                 addr += size;
         }
         return addr - (DATA_SEGMENT_BASE + offset);
 }

 /**
  * add an instruction in text segment into the storage
  *
  * @param inst instruction (ex. add, sub, beq, etc.)
  * @param operands list of operands (ex. $t0, $t1, $t2)
  * @param offset the address which the instruction should be placed at
  */
 void addInst(char *inst, char *operands, int offset)
 {
         char *delim = " ,\t\r\n";
         char *op;

         int addr = TEXT_SEGMENT_BASE + offset/WORD_SIZE;
         // int sizeofInstruction = strlen(inst)+1;
         // Instruction_storage[addr].instruction = (char *)malloc(sizeofInstruction);
         // memset(Instruction_storage[addr].instruction, sizeofInstruction, 0);
         // strcpy (Instruction_storage[addr].instruction, inst);
         strcpy(Instruction_storage[addr].instruction, inst);  // copy the instruction nemonics.

         if (strcmp(inst, "add") == 0 || strcmp(inst, "sub") == 0 ||
                 strcmp(inst, "and") == 0 || strcmp(inst, "or") == 0 ||
                 strcmp(inst, "xor") == 0 || strcmp(inst, "slt") == 0)
         {
                 op = strtok(operands, delim);
                 Instruction_storage[addr].rd = getRegNum(op);

                 op = strtok(NULL, delim);
                 Instruction_storage[addr].rs = getRegNum(op);

                 op = strtok(NULL, delim);
                 Instruction_storage[addr].rt = getRegNum(op);
         }
         else if (strcmp(inst, "lb") == 0 || strcmp(inst, "sb") == 0 ||
                 strcmp(inst, "lw") == 0 || strcmp(inst, "sw") == 0)
         {
                 Instruction_storage[addr].rd = 0;

                 op = strtok(operands, delim);
                 Instruction_storage[addr].rt = getRegNum(op);

                 // IMM($rs)
                 op = strtok(NULL, " \t()");
                 if (op[0] == '$')
                 {
                         Instruction_storage[addr].immediate = 0;
                         Instruction_storage[addr].rs = getRegNum(op);
                 }
                 else
                 {
                         Instruction_storage[addr].immediate = mod_atoi(op);

                         op = strtok(NULL, " )\t\r\n");
                         Instruction_storage[addr].rs = getRegNum(op);
                 }
         }
         else if (strcmp(inst, "addi") == 0 || strcmp(inst, "andi") == 0 ||
                         strcmp(inst, "ori") == 0 || strcmp(inst, "slti") == 0 ||
                         strcmp(inst, "sll") == 0 || strcmp(inst, "srl") == 0 ||
                         strcmp(inst, "sra") == 0)
         {
                 Instruction_storage[addr].rd = 0;

                 op = strtok(operands, delim);
                 Instruction_storage[addr].rt = getRegNum(op);

                 op = strtok(NULL, delim);
                 Instruction_storage[addr].rs = getRegNum(op);

                 op = strtok(NULL, delim);
                 Instruction_storage[addr].immediate = mod_atoi(op);
         }
         else if (strcmp(inst, "beq") == 0 || strcmp(inst, "bne") == 0 ||
                         strcmp(inst, "bgt") == 0 || strcmp(inst, "blt") == 0 ||
                         strcmp(inst, "bge") == 0 || strcmp(inst, "ble") == 0)
         {
                 Instruction_storage[addr].rd = 0;

                 op = strtok(operands, delim);
                 Instruction_storage[addr].rs = getRegNum(op);

                 op = strtok(NULL, delim);
                 Instruction_storage[addr].rt = getRegNum(op);

                 op = strtok(NULL, delim);
                 // Instruction_storage[addr].address = ((findLabel(op) - (addr + 4)) >> 2); // this is not right.
                 Instruction_storage[addr].address = findLabel(op) >> 2;  // similar to j instruction.
         }
         else if (strcmp(inst, "li") == 0)
         {
                 op = strtok(operands, delim);
                 Instruction_storage[addr].rd = 0;

                 Instruction_storage[addr].rt = getRegNum(op);
                 Instruction_storage[addr].rs = 0;

                 op = strtok(NULL, delim);
                 Instruction_storage[addr].immediate = mod_atoi(op);
         }
         else if ((strcmp(inst, "lui") == 0) ||
                  (strcmp(inst, "lhi") == 0))
         {
                 op = strtok(operands, delim);
                 Instruction_storage[addr].rd = 0;

                 Instruction_storage[addr].rt = getRegNum(op);
                 Instruction_storage[addr].rs = 0;

                 op = strtok(NULL, delim);
                 Instruction_storage[addr].immediate = mod_atoi(op);
         }
         else if (strcmp(inst, "la") == 0)  // la is not a regular instruction. it is pseudo code.
         {
                 // processPseudoCode("la"); // This will be defined later on.
                 op = strtok(operands, delim);
                 Instruction_storage[addr].rd = 0;

                 Instruction_storage[addr].rt = getRegNum(op);
                 Instruction_storage[addr].rs = 0;

                 op = strtok(NULL, delim);
                 // assume target address is within 16-bit (64k) bytes range for now.
                 Instruction_storage[addr].immediate = findLabel(op);    // consider immediate as address.
         }
         else if (strcmp(inst, "j") == 0)
         {
                 Instruction_storage[addr].rd = 0;
                 Instruction_storage[addr].rt = 0;
                 Instruction_storage[addr].rs = 0;

                 op = strtok(operands, delim);

                 if (findLabel(op) >= 0)
                         Instruction_storage[addr].address = findLabel(op) >> 2;
                 else
                         Instruction_storage[addr].address = mod_atoi(op);  // could be absolote number. needs to remove base address here.
         }
         else if (strcmp(inst, "jal") == 0)
         {
                 Instruction_storage[addr].rd = 0;
                 Instruction_storage[addr].rt = 0;
                 Instruction_storage[addr].rs = 0;

                 op = strtok(operands, delim);
                 if (findLabel(op) >= 0)
                         Instruction_storage[addr].address = findLabel(op) >> 2;
                 else
                         Instruction_storage[addr].address = mod_atoi(op);// could be absolote number. needs to remove base address here.
         }
         else if (strcmp(inst, "b") == 0)
         {
                 Instruction_storage[addr].rd = 0;
                 Instruction_storage[addr].rt = 0;
                 Instruction_storage[addr].rs = 0;

                 op = strtok(operands, delim);
                 Instruction_storage[addr].address = findLabel(op);
         }
         else if (strcmp(inst, "jr") == 0)
         {
                 Instruction_storage[addr].rd = 0;
                 Instruction_storage[addr].rt = 0;

                 op = strtok(operands, delim);
                 Instruction_storage[addr].rs = getRegNum(op);
         }
         else if (strcmp(inst, "mfhi") == 0 ||
                                 strcmp(inst, "mflo") == 0)
         {
                 Instruction_storage[addr].rs = 0;
                 Instruction_storage[addr].rt = 0;

                 op = strtok(operands, delim);
                 Instruction_storage[addr].rd = getRegNum(op);
         }
         else if (strcmp(inst, "move") == 0)
         {
                 op = strtok(operands, delim);
                 Instruction_storage[addr].rd = getRegNum(op);

                 Instruction_storage[addr].rt = 0;

                 op = strtok(NULL, delim);
                 Instruction_storage[addr].rs = getRegNum(op);
         }
         else if (strcmp(inst, "mult") == 0 || strcmp(inst, "div") == 0)
         {
                 Instruction_storage[addr].rd = 0;

                 op = strtok(operands, delim);
                 Instruction_storage[addr].rs = getRegNum(op);

                 op = strtok(NULL, delim);
                 Instruction_storage[addr].rt = getRegNum(op);
         }
         else if (strcmp(inst, "mul") == 0)
         {
                 op = strtok(operands, delim);
                 Instruction_storage[addr].rd = getRegNum(op);

                 op = strtok(NULL, delim);
                 Instruction_storage[addr].rs = getRegNum(op);

                 op = strtok(NULL, delim);
                 Instruction_storage[addr].rt = getRegNum(op);
         }
         else if (strcmp(inst, "syscall") == 0)
         {
         // DO NOTHING
         }
         else if (strcmp(inst, "END") == 0)
         {
         // DO NOTHING
         }
         else
         {
                 // reach here? error
                 printf("parse error: unknown instruction %s\n", inst);
         }
 }


 /**
  * printout the ASM code after parser.
  *
  */
 void printAllInstructions(){
    int i =0;
    printf("-------- The following MIPS instructions are parsed: -------- \n");
    for (i = 0; i< MAX_SIZE; i++){
        if (strcmp(Instruction_storage[i].instruction, "syscall") == 0 )
            break; //exit the loop when end
        printf("Instruction %d:    %s\n", i+1, Instruction_storage[i].instruction);
    }
    printf("-------- End of code section. --------\n");
 }

 /**
  * printout the data section after parser.
  *
  */
 void printAllData(){
    int i =0, j=0;
    printf("-------- The following data are parsed: -------- \n");
    for (i = 0; i< ((int)totalDataByte/16)+1; i++){
        printf("Byte %02d:    ", i*16);
        for (j = 0; j<16; j++){    // every 16 bytes change the line.
            if (i*16+j < totalDataByte)
                printf("%02X ", (unsigned char)Data_storage[i*16+j]);
            else{
                printf("\n");
                break;
            }
        }
        printf("\n");
    }
    printf("-------- End of data section. --------\n");
 }
  /**
  * parse the ASM file pointed by fp.
  *
  * @param fp the file pointer pointed to ASM File.
  */
void parse_MIPS (FILE *fp) {
// Make sure correct number of arguments input
     char buffer[BUFMAX+1] = {0};  // each line should have less than 128 bytes
     char *token = NULL;
     char *delim = " \t\r\n";
     static int Segmentation = DATA;  // assume Data section.
     static int textOffset = 0;
     static int dataOffset = 0;

        // Open MIPS ASM file.
        // Check that files opened properly
         // PASS 1: get labels and variable declarations
         while (fgets(buffer, BUFMAX, fp))
         {
                 // Indicator
                 for (token = strlwr((char*)strtok(buffer, delim)); token; token = strlwr((char*)strtok(NULL, delim)))
                 {
                         // COMMENT
                         if (token[0] == '#')  // If this is comment line.
                                 break;        // ignore all the comments
                         if (strcmp(token, ".text") == 0){ // If that is code start
                              // Code Segment
                              addLabel("TEXT", 0, 0);    // make sure text has an address.
                              Segmentation = TEXT;// next few lines will be code.
                              break;
                         }
                         else if (strcmp(token, ".data") == 0){// If that is data start
                             // Data Segment
                             addLabel("DATA", 0, 0);
                             Segmentation = DATA;  // next few lines will be data.
                             break;
                         }

                        if (strcmp(token, ".globl") == 0)  // ignore globl for now
                                 break;
                         // LABEL
                         // if the last character is ':', then it is a label.
                         if (token[strlen(token)-1] == ':'){
                            token[strlen(token)-1] = '\0';
                            addLabel(token, Segmentation, (Segmentation == TEXT) ? textOffset:dataOffset);
                            continue;
                         }

                         // variable starts with a '.'
                         if ((token[0] == '.') && (Segmentation == DATA)){
                            // VARIABLE DECLARATION
                            dataOffset += addData(token, token+strlen(token)+1, dataOffset);
                            totalDataByte = dataOffset; // update the size of the data section.
                            break;
                         }
                         else if (Segmentation == TEXT){  // make sure it is Code segment.
                            // INSTRUCTION
                            // addInst(token, token+strlen(token)+1, textOffset);
                            textOffset += WORD_SIZE;
                            break;
                         }
                 }
                 memset(buffer, 0, BUFMAX);
         }
         // PASS 2: actual parsing
         textOffset = 0;
         memset(buffer, 0, BUFMAX);
         fseek(fp, 0, SEEK_SET);
         while (fgets(buffer, BUFMAX, fp))
         {
                 // seperate tokens in a line
                 for (token = (char*) strtok(buffer, delim); token; token = (char*) strtok(NULL, delim))
                 {
                         // COMMENT
                         if (token[0] == '#')   // ignore all comments.
                                 break;

                         // SEGMENT
                         if (strcmp(token, ".text") == 0) // code starts here.
                         {
                                 Segmentation = TEXT;
                                 break;
                         }
                         else if (strcmp(token, ".data") == 0)// data starts here.
                         {
                                 Segmentation = DATA;
                                 break;
                         }

                         if (strcmp(token, ".globl") == 0)  // ignore globl for now
                                 break;

                         // skip everything in data segment
                         if (Segmentation == DATA)
                                 break;

                         // skip labels
                         if (token[strlen(token)-1] == ':')
                                 continue;

                         // add INSTRUCTIONs anyways
                         if (Segmentation == TEXT)
                         {
                            addInst(token, token+strlen(token)+1, textOffset);
                            textOffset += WORD_SIZE;
                         }
                 }
                 memset(buffer, 0, BUFMAX);
         }
         addInst("END", NULL, textOffset);  // The last instruction will be END




        //  parseInstructions2ndRound();  // needs to update the label address for second time.
                                       // in case labels are shown after instrution.
         if (DEBUG_CODE){
         // Print the all instructions defined in code.
         printAllInstructions();
         // Print the all data defined in code.
         printAllData();
         }
         
}
char *strlwr(char *s)
{
    char *p;

    if (s == NULL)
        return NULL;

    p = s;
    while (*p) {
        *p = tolower((unsigned char)*p);
        p++;
    }
    return s;
}
