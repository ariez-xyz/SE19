// Copyright (c) 2015, the Selfie Project authors. All rights reserved.
// Please see the AUTHORS file for details. Use of this source code is
// governed by a BSD license that can be found in the LICENSE file.
//
// Selfie is a project of the Computational Systems Group at the
// Department of Computer Sciences of the University of Salzburg
// in Austria. For further information and code please refer to:
//
// http://selfie.cs.uni-salzburg.at
//
// The Selfie Project provides an educational platform for teaching
// undergraduate and graduate students the design and implementation
// of programming languages and runtime systems. The focus is on the
// construction of compilers, libraries, operating systems, and even
// virtual machine monitors. The common theme is to identify and
// resolve self-reference in systems code which is seen as the key
// challenge when teaching systems engineering, hence the name.
//
// Selfie is a fully self-referential 4k-line C implementation of:
//
// 1. a self-compiling compiler called cstarc that compiles
//    a tiny but powerful subset of C called C Star (C*) to
//    a tiny but powerful subset of MIPS32 called MIPSter,
// 2. a self-executing emulator called mipster that executes
//    MIPSter code including itself when compiled with cstarc, and
// 3. a tiny C* library called libcstar utilized by cstarc and mipster.
//
// Selfie is kept minimal for simplicity and implemented in a single file.
// There is no linker, assembler, or debugger. However, there is minimal
// operating system support in the form of MIPS32 o32 system calls built
// into the emulator. Selfie is meant to be extended in numerous ways.
//
// C* is a tiny Turing-complete subset of C that includes dereferencing
// (the * operator) but excludes data structures, Boolean expressions, and
// many other features. There are only signed 32-bit integers and pointers,
// and character constants for constructing word-aligned strings manually.
// This choice turns out to be helpful for students to understand the
// true role of composite data structures such as arrays and records.
// Bitwise operations are implemented in libcstar using signed integer
// arithmetics helping students gain true understanding of two's complement.
// C* is supposed to be close to the minimum necessary for implementing
// a self-compiling, single-pass, recursive-descent compiler. C* can be
// taught in around two weeks of classes depending on student background.
//
// The compiler can readily be extended to compile features missing in C*
// and to improve performance of the generated code. The compiler generates
// MIPSter executables that can directly be executed by the emulator or
// written to a file in a simple, custom-defined format. Support of standard
// MIPS32 ELF binaries should be easy but remains future work.
//
// MIPSter is a tiny Turing-complete subset of the MIPS32 instruction set.
// It only features arithmetic, memory, and control-flow instructions but
// neither bitwise nor byte-level instructions. MIPSter can be properly
// explained in a single week of classes.
//
// The emulator implements minimal operating system support that is meant
// to be extended by students, first as part of the emulator, and then
// ported to run on top of it, similar to an actual operating system or
// virtual machine monitor. The fact that the emulator can execute itself
// helps exposing the self-referential nature of that challenge.
//
// Selfie is the result of many years of teaching systems engineering.
// The design of the compiler is inspired by the Oberon compiler of
// Professor Niklaus Wirth from ETH Zurich.

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ---------------------     L I B R A R Y     ---------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ----------------------- LIBRARY FUNCTIONS -----------------------
// -----------------------------------------------------------------

void initLibrary();

int twoToThePowerOf(int p);
int leftShift(int n, int b);
int rightShift(int n, int b);

int  stringLength(int *s);
void stringReverse(int *s);
int  stringCompare(int *s, int *t);
int  atoi(int *s);
int* itoa(int n, int *s, int b, int a);
void print(int *s);
void println();
void printCharacter(int character);
void printString(int *s);
void assignString(int *s, int c0, int c1, int c2, int c3, int c4,
    int c5, int c6, int c7, int c8, int c9,
    int c10, int c11, int c12, int c13, int c14,
    int c15, int c16, int c17, int c18, int c19);
int* createString(int c0, int c1, int c2, int c3, int c4,
    int c5, int c6, int c7, int c8, int c9,
    int c10, int c11, int c12, int c13, int c14,
    int c15, int c16, int c17, int c18, int c19);
void memset(int *a, int size, int v);

void exit(int code);
int* malloc(int size);

// ------------------------ GLOBAL CONSTANTS -----------------------

int *power_of_two_table;

int INT_MAX; // maximum numerical value of an integer
int INT_MIN; // minimum numerical value of an integer

int CHAR_EOF;
int CHAR_TAB;
int CHAR_LF;
int CHAR_CR;
int CHAR_SPACE;        //
int CHAR_SEMICOLON;    // ;
int CHAR_PLUS;         // +
int CHAR_DASH;         // -
int CHAR_ASTERISK;     // *
int CHAR_HASH;         // #
int CHAR_SLASH;        // /
int CHAR_UNDERSCORE;   // _
int CHAR_EQUAL;        // =
int CHAR_LPARENTHESIS; // (
int CHAR_RPARENTHESIS; // )
int CHAR_LBRACE;       // {
int CHAR_RBRACE;       // }
int CHAR_COMMA;        // ,
int CHAR_LT;           // <
int CHAR_GT;           // >
int CHAR_EXCLAMATION;  // !
int CHAR_PERCENTAGE;   // %
int CHAR_SINGLEQUOTE;  // '
int CHAR_DOUBLEQUOTE;  // "

int *string_buffer;

// ------------------------- INITIALIZATION ------------------------

void initLibrary() {
    int i;

    power_of_two_table = malloc(31*4);

    *power_of_two_table = 1; // 2^0

    i = 1;

    while (i < 31) {
        *(power_of_two_table + i) = *(power_of_two_table + (i - 1)) * 2;

        i = i + 1;
    }

    // computing INT_MAX and INT_MIN without overflows
    INT_MAX = (twoToThePowerOf(30) - 1) * 2 + 1;
    INT_MIN = -INT_MAX - 1;

    CHAR_EOF          = -1; // end of file
    CHAR_TAB          = 9;  // ASCII code 9  = tabulator
    CHAR_LF           = 10; // ASCII code 10 = line feed
    CHAR_CR           = 13; // ASCII code 13 = carriage return
    CHAR_SPACE        = ' ';
    CHAR_SEMICOLON    = ';';
    CHAR_PLUS         = '+';
    CHAR_DASH         = '-';
    CHAR_ASTERISK     = '*';
    CHAR_HASH         = '#';
    CHAR_SLASH        = '/';
    CHAR_UNDERSCORE   = '_';
    CHAR_EQUAL        = '=';
    CHAR_LPARENTHESIS = '(';
    CHAR_RPARENTHESIS = ')';
    CHAR_LBRACE       = '{';
    CHAR_RBRACE       = '}';
    CHAR_COMMA        = ',';
    CHAR_LT           = '<';
    CHAR_GT           = '>';
    CHAR_EXCLAMATION  = '!';
    CHAR_PERCENTAGE   = '%';
    CHAR_SINGLEQUOTE  = 39; // ASCII code 39 = '
    CHAR_DOUBLEQUOTE  = '"';

    // accommodate 32-bit numbers for itoa
    string_buffer = malloc(33);
}

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ---------------------     C O M P I L E R   ---------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ---------------------------- SCANNER ----------------------------
// -----------------------------------------------------------------

void initScanner();

void printSymbol(int symbol);
void printLineNumber(int* message);

void syntaxErrorMessage(int *message);
void syntaxErrorCharacter(int character);

int isCharacterWhitespace();
int findNextCharacter();
int isCharacterLetter();
int isCharacterDigit();
int isCharacterLetterOrDigitOrUnderscore();
int isNotDoubleQuoteOrEOF();
int identifierStringMatch(int stringIndex);
int identifierOrKeyword();
int getSymbol();

// ------------------------ GLOBAL CONSTANTS -----------------------

int SYM_EOF;          // end of file
int SYM_IDENTIFIER;   // identifier
int SYM_INTEGER;      // integer
int SYM_VOID;         // void
int SYM_INT;          // int
int SYM_SEMICOLON;    // ;
int SYM_IF;           // if
int SYM_ELSE;         // else
int SYM_PLUS;         // +
int SYM_MINUS;        // -
int SYM_ASTERISK;     // *
int SYM_DIV;          // /
int SYM_EQUALITY;     // ==
int SYM_ASSIGN;       // =
int SYM_LPARENTHESIS; // (
int SYM_RPARENTHESIS; // )
int SYM_LBRACE;       // {
int SYM_RBRACE;       // }
int SYM_WHILE;        // while
int SYM_RETURN;       // return
int SYM_COMMA;        // ,
int SYM_LT;           // <
int SYM_LEQ;          // <=
int SYM_GT;           // >
int SYM_GEQ;          // >=
int SYM_NOTEQ;        // !=
int SYM_MOD;          // %
int SYM_CHARACTER;    // character
int SYM_STRING;       // string

int *SYMBOL; // array of strings representing symbols

int maxIdentifierLength; // maximum number of characters in an identifier
int maxIntegerLength;    // maximum number of characters in an integer
int maxStringLength;     // maximum number of characters in a string

// ------------------------ GLOBAL VARIABLES -----------------------

int lineNumber; // Current Line Number for error reporting
int character;   // most recently read character
int symbol;      // most recently recognized symbol

int *identifier; // stores scanned identifier
int *integer;    // stores scanned integer as string
int *string;     // stores scanned string

int ivalue; // stores numerical value of scanned integer string

int mayBeINTMINConstant; // support INT_MIN constant

// ------------------------- INITIALIZATION ------------------------

void initScanner () {
    SYM_EOF              = -1;  // end of file
    SYM_IDENTIFIER       = 0;   // identifier
    SYM_INTEGER          = 1;   // a number
    SYM_VOID             = 2;   // VOID
    SYM_INT              = 3;   // INT
    SYM_SEMICOLON        = 4;   // ;
    SYM_IF               = 5;   // IF
    SYM_ELSE             = 6;   // ELSE
    SYM_PLUS             = 7;   // +
    SYM_MINUS            = 8;   // -
    SYM_ASTERISK         = 9;   // *
    SYM_DIV              = 10;  // /
    SYM_EQUALITY         = 11;  // ==
    SYM_ASSIGN           = 12;  // =
    SYM_LPARENTHESIS     = 13;  // (
    SYM_RPARENTHESIS     = 14;  // )
    SYM_LBRACE           = 15;  // {
    SYM_RBRACE           = 16;  // }
    SYM_WHILE            = 17;  // WHILE
    SYM_RETURN           = 18;  // RETURN
    SYM_COMMA            = 19;  // ,
    SYM_LT               = 20;  // <
    SYM_LEQ              = 21;  // <=
    SYM_GT               = 22;  // >
    SYM_GEQ              = 23;  // >=
    SYM_NOTEQ            = 24;  // !=
    SYM_MOD              = 25;  // %
    SYM_CHARACTER        = 26;  // character value
    SYM_STRING           = 27;  // string

    SYMBOL = malloc(28*4);

    *(SYMBOL + SYM_IDENTIFIER)   = (int) "identifier";
    *(SYMBOL + SYM_INTEGER)      = (int) "integer";
    *(SYMBOL + SYM_VOID)         = (int) "void";
    *(SYMBOL + SYM_INT)          = (int) "int";
    *(SYMBOL + SYM_SEMICOLON)    = (int) ";";
    *(SYMBOL + SYM_IF)           = (int) "if";
    *(SYMBOL + SYM_ELSE)         = (int) "else";
    *(SYMBOL + SYM_PLUS)         = (int) "+";
    *(SYMBOL + SYM_MINUS)        = (int) "-";
    *(SYMBOL + SYM_ASTERISK)     = (int) "*";
    *(SYMBOL + SYM_DIV)          = (int) "/";
    *(SYMBOL + SYM_EQUALITY)     = (int) "==";
    *(SYMBOL + SYM_ASSIGN)       = (int) "=";
    *(SYMBOL + SYM_LPARENTHESIS) = (int) "(";
    *(SYMBOL + SYM_RPARENTHESIS) = (int) ")";
    *(SYMBOL + SYM_LBRACE)       = (int) "{";
    *(SYMBOL + SYM_RBRACE)       = (int) "}";
    *(SYMBOL + SYM_WHILE)        = (int) "while";
    *(SYMBOL + SYM_RETURN)       = (int) "return";
    *(SYMBOL + SYM_COMMA)        = (int) ",";
    *(SYMBOL + SYM_LT)           = (int) "<";
    *(SYMBOL + SYM_LEQ)          = (int) "<=";
    *(SYMBOL + SYM_GT)           = (int) ">";
    *(SYMBOL + SYM_GEQ)          = (int) ">=";
    *(SYMBOL + SYM_NOTEQ)        = (int) "!=";
    *(SYMBOL + SYM_MOD)          = (int) "%";
    *(SYMBOL + SYM_CHARACTER)    = (int) "character";
    *(SYMBOL + SYM_STRING)       = (int) "string";

    maxIdentifierLength = 64;
    maxIntegerLength    = 10;
    maxStringLength     = 64;

    lineNumber = 1;
    character  = getchar();
    symbol     = -1;

    identifier = (int*) 0;
    integer    = (int*) 0;
    string     = (int*) 0;

    ivalue = 0;

    mayBeINTMINConstant = 0;
}

// -----------------------------------------------------------------
// ------------------------- SYMBOL TABLE --------------------------
// -----------------------------------------------------------------

void initSymbolTable();

void createSymbolTableEntry(int which, int *string, int data, int class, int type);
int* getSymbolTableEntry(int *string, int class, int *symbol_table);

int* getNext(int *entry);
int* getString(int *entry);
int  getData(int *entry);
int  getClass(int *entry);
int  getType(int *entry);
int  getRegister(int *entry);

void setNext(int *entry, int *next);
void setString(int *entry, int *identifier);
void setData(int *entry, int data);
void setClass(int *entry, int class);
void setType(int *entry, int type);
void setRegister(int *entry, int reg);

// ------------------------ GLOBAL CONSTANTS -----------------------

// classes
int UNKNOWN;  // 0
int CONSTANT; // 1
int VARIABLE; // 2
int FUNCTION; // 3
int STRING;   // 4

// types
int INT_T;     // 1
int INTSTAR_T; // 2
int VOID_T;    // 3

// symbol tables
int GLOBAL_TABLE;
int LOCAL_TABLE;

// ------------------------ GLOBAL VARIABLES -----------------------

// table pointer
int *global_symbol_table;
int *local_symbol_table;

// ------------------------- INITIALIZATION ------------------------

void initSymbolTable() {
    // classes
    UNKNOWN  = 0;
    CONSTANT = 1;
    VARIABLE = 2;
    FUNCTION = 3;
    STRING   = 4;

    // types
    INT_T     = 1;
    INTSTAR_T = 2;
    VOID_T    = 3;

    // symbol tables
    GLOBAL_TABLE = 1;
    LOCAL_TABLE  = 2;

    // table pointers
    global_symbol_table = (int*) 0;
    local_symbol_table  = (int*) 0;
}

// -----------------------------------------------------------------
// ---------------------------- PARSER -----------------------------
// -----------------------------------------------------------------

void initParser();

int isNotRbraceOrEOF();
int isExpression();
int isStarOrDivOrModulo();
int isPlusOrMinus();
int isComparison();
int isVariableOrProcedure();

int lookForFactor();
int lookForStatement();
int lookForVariable();

void save_registers();
void restore_registers(int numberOfRegisters);

void syntaxErrorSymbol(int expected);
void syntaxErrorUnexpected();
int* putType(int type);
void typeWarning(int expected, int found);

int* getVariable(int *variable);
int  load_variable(int *variable);
void load_integer();
void load_string();

int  help_call_codegen(int *entry, int *procedure);
void help_procedure_prologue(int localVariables);
void help_procedure_epilogue(int parameters, int functionStart, int functionType);

int  gr_call(int *procedure);
int  gr_factor();
int  gr_term();
int  gr_simpleExpression();
int  gr_expression();
void gr_while();
void gr_if();
void gr_return(int returnType);
void gr_statement();
void gr_variable(int offset);
void gr_procedure(int *procedure, int returnType);
void gr_cstar();

// ------------------------ GLOBAL CONSTANTS -----------------------

int maxBinaryLength;

// ------------------------ GLOBAL VARIABLES -----------------------

int allocatedRegisters; // number of allocated registers
int allocatedMemory;    // number of words for global variables and strings

int mainJumpAddress;
int returnBranches;

int *currentProcedureName; // holds the name of currently parsed procedure

// ------------------------- INITIALIZATION ------------------------

void
initParser() {
    // set maximum code length for emitting code
    maxBinaryLength = 32768;

    allocatedRegisters = 0;
    allocatedMemory    = 0;

    mainJumpAddress = 0;
    returnBranches  = 0;

    currentProcedureName = (int*) 0;
}

// -----------------------------------------------------------------
// ---------------------- MACHINE CODE LIBRARY ---------------------
// -----------------------------------------------------------------

void emitLeftShiftBy(int b);
void emitMainEntry();

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// -------------------     I N T E R F A C E     -------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ---------------------------- REGISTER ---------------------------
// -----------------------------------------------------------------

// ------------------------ GLOBAL CONSTANTS -----------------------

int REG_ZR;
int REG_V0;
int REG_A0;
int REG_A1;
int REG_A2;
int REG_A3;
int REG_RR;
int REG_K1;
int REG_GP;
int REG_SP;
int REG_FP;
int REG_LINK;

// ------------------------- INITIALIZATION ------------------------

void initRegister() {
    REG_ZR   = 0;
    REG_V0   = 2;
    REG_A0   = 4;
    REG_A1   = 5;
    REG_A2   = 6;
    REG_A3   = 7;
    REG_RR   = 26;
    REG_K1   = 27;
    REG_GP   = 28;
    REG_SP   = 29;
    REG_FP   = 30;
    REG_LINK = 31;
}

// -----------------------------------------------------------------
// ---------------------------- ENCODER ----------------------------
// -----------------------------------------------------------------

int encodeRFormat(int opcode, int rs, int rt, int rd, int function);
int encodeIFormat(int opcode, int rs, int rt, int immediate);
int encodeJFormat(int opcode, int instr_index);

int getOpcode(int instruction);
int getRS(int instruction);
int getRT(int instruction);
int getRD(int instruction);
int getFunction(int instruction);
int getImmediate(int instruction);
int getInstrIndex(int instruction);
int signExtend(int immediate);

// -----------------------------------------------------------------
// ---------------------------- DECODER ----------------------------
// -----------------------------------------------------------------

void initDecoder();

void decode();
void decodeRFormat();
void decodeIFormat();
void decodeJFormat();

// ------------------------ GLOBAL CONSTANTS -----------------------

int OP_SPECIAL;
int FCT_SYSCALL;
int FCT_MFHI;
int FCT_MFLO;
int FCT_MULTU;
int FCT_DIVU;
int OP_ADDIU;
int FCT_ADDU;
int FCT_SUBU;
int OP_LW;
int OP_SW;
int OP_BEQ;
int OP_BNE;
int FCT_SLT;
int FCT_NOP;
int FCT_JR;
int OP_JAL;
int OP_J;
int FCT_TEQ;

// ------------------------ GLOBAL VARIABLES -----------------------

int opcode;
int rs;
int rt;
int rd;
int immediate;
int function;
int instr_index;

// ------------------------- INITIALIZATION ------------------------

void initDecoder() {
    OP_SPECIAL  = 0;
    FCT_NOP     = 0;
    OP_JAL      = 3;
    OP_J        = 2;
    OP_BEQ      = 4;
    OP_BNE      = 5;
    OP_ADDIU    = 9;
    FCT_JR      = 8;
    FCT_SYSCALL = 12;
    FCT_MFHI    = 16;
    FCT_MFLO    = 18;
    FCT_MULTU   = 25;
    FCT_DIVU    = 27;
    FCT_ADDU    = 33;
    FCT_SUBU    = 35;
    OP_LW       = 35;
    FCT_SLT     = 42;
    OP_SW       = 43;
    FCT_TEQ     = 52;

    opcode      = 0;
    rs          = 0;
    rt          = 0;
    rd          = 0;
    immediate   = 0;
    function    = 0;
    instr_index = 0;
}

// -----------------------------------------------------------------
// ---------------------------- MEMORY -----------------------------
// -----------------------------------------------------------------

void initMemory(int size, int* name);

// ------------------------ GLOBAL VARIABLES -----------------------

int *memory;
int  memorySize;

int *binaryName;
int  binaryLength;

// ------------------------- INITIALIZATION ------------------------

void initMemory(int size, int *name) {
    memory     = malloc(size);
    memorySize = size;

    binaryName   = name;
    binaryLength = 0;
}

// -----------------------------------------------------------------
// ---------------------------- BINARY -----------------------------
// -----------------------------------------------------------------

void emitInstruction(int instruction);
void emitRFormat(int opcode, int rs, int rt, int rd, int function);
void emitIFormat(int opcode, int rs, int rt, int immediate);
void emitJFormat(int opcode, int instr_index);

void fixup_relative(int fromAddress);
void fixup_absolute(int fromAddress, int toAddress);
void fixlink_absolute(int fromAddress, int toAddress);

int copyStringToMemory(int *s, int a);

void emitBinary();
void loadBinary();

// -----------------------------------------------------------------
// --------------------------- SYSCALLS ----------------------------
// -----------------------------------------------------------------

void initSyscalls();

void emitExit();
void syscall_exit();

void emitRead();
void syscall_read();

void emitWrite();
void syscall_write();

void emitOpen();
void syscall_open();

void emitMalloc();
void syscall_malloc();

void emitGetchar();
void syscall_getchar();

void emitPutchar();

// ------------------------ GLOBAL CONSTANTS -----------------------

int SYSCALL_EXIT;
int SYSCALL_READ;
int SYSCALL_WRITE;
int SYSCALL_OPEN;
int SYSCALL_MALLOC;
int SYSCALL_GETCHAR;

// ------------------------- INITIALIZATION ------------------------

void initSyscalls() {
    SYSCALL_EXIT    = 4001;
    SYSCALL_READ    = 4003;
    SYSCALL_WRITE   = 4004;
    SYSCALL_OPEN    = 4005;
    SYSCALL_MALLOC  = 5001;
    SYSCALL_GETCHAR = 5002;
}

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ---------------------     E M U L A T O R   ---------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ------------------------- INSTRUCTIONS --------------------------
// -----------------------------------------------------------------

void fct_syscall();
void fct_nop();
void op_jal();
void op_j();
void op_beq();
void op_bne();
void op_addiu();
void fct_jr();
void op_lui();
void fct_mfhi();
void fct_mflo();
void fct_multu();
void fct_divu();
void fct_addu();
void fct_subu();
void op_lw();
void fct_slt();
void op_sw();
void op_teq();

// -----------------------------------------------------------------
// -------------------------- INTERPRETER --------------------------
// -----------------------------------------------------------------

void initInterpreter();

void exception_handler(int enumber);

int addressTranslation(int vaddr);

void pre_debug();
void post_debug();

void fetch();
void execute();
void run();

void parse_args(int argc, int *argv);

void up_push(int value);
int  up_malloc(int size);
void up_copyArguments(int argc, int *argv);

int main_emulator(int argc, int *argv);

// ------------------------ GLOBAL CONSTANTS -----------------------

int *register_strings; // static strings for register names
int *op_strings;       // static strings for debug_disassemble
int *fct_strings;

int debug_load;

int debug_read;
int debug_write;
int debug_open;
int debug_malloc;
int debug_getchar;

int debug_registers;
int debug_disassemble;

int EXCEPTION_SIGNAL;
int EXCEPTION_ADDRESSERROR;
int EXCEPTION_UNKNOWNINSTRUCTION;
int EXCEPTION_HEAPOVERFLOW;
int EXCEPTION_UNKNOWNSYSCALL;
int EXCEPTION_UNKNOWNFUNCTION;

// ------------------------ GLOBAL VARIABLES -----------------------

int *registers; // general purpose registers

int pc; // program counter
int ir; // instruction record

int reg_hi; // hi register for multiplication/division
int reg_lo; // lo register for multiplication/division

// ------------------------- INITIALIZATION ------------------------

void initInterpreter() {
    register_strings = malloc(4*32);
    op_strings       = malloc(4*64);
    fct_strings      = malloc(4*64);

    *(register_strings + 0)  = (int) createString('z','e','r','o',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 1)  = (int) createString('a','t',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 2)  = (int) createString('v','0',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 3)  = (int) createString('v','1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 4)  = (int) createString('a','0',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 5)  = (int) createString('a','1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 6)  = (int) createString('a','2',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 7)  = (int) createString('a','3',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 8)  = (int) createString('t','0',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 9)  = (int) createString('t','1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 10) = (int) createString('t','2',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 11) = (int) createString('t','3',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 12) = (int) createString('t','4',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 13) = (int) createString('t','5',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 14) = (int) createString('t','6',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 15) = (int) createString('t','7',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 16) = (int) createString('s','0',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 17) = (int) createString('s','1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 18) = (int) createString('s','2',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 19) = (int) createString('s','3',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 20) = (int) createString('s','4',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 21) = (int) createString('s','5',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 22) = (int) createString('s','6',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 23) = (int) createString('s','7',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 24) = (int) createString('t','8',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 25) = (int) createString('t','9',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 26) = (int) createString('k','0',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 27) = (int) createString('k','1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 28) = (int) createString('g','p',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 29) = (int) createString('s','p',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 30) = (int) createString('f','p',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(register_strings + 31) = (int) createString('r','a',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

    *(fct_strings + 0) = (int) createString('n','o','p',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(fct_strings + 8) = (int) createString('j','r',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(fct_strings + 12) = (int) createString('s','y','s','c','a','l','l',0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(fct_strings + 16) = (int) createString('m','f','h','i',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(fct_strings + 18) = (int) createString('m','f','l','o',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(fct_strings + 25) = (int) createString('m','u','l','t','u',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(fct_strings + 27) = (int) createString('d','i','v','u',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(fct_strings + 33) = (int) createString('a','d','d','u',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(fct_strings + 35) = (int) createString('s','u','b','u',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(fct_strings + 42) = (int) createString('s','l','t',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(fct_strings + 52) = (int) createString('t','e','q',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

    *(op_strings + 0) = (int) createString('n','o','p',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(op_strings + 3) = (int) createString('j','a','l',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(op_strings + 2) = (int) createString('j',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(op_strings + 4) = (int) createString('b','e','q',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(op_strings + 5) = (int) createString('b','n','e',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(op_strings + 9) = (int) createString('a','d','d','i','u',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(op_strings + 15) = (int) createString('l','u','i',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(op_strings + 35) = (int) createString('l','w',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    *(op_strings + 43) = (int) createString('s','w',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

    debug_load = 0;
    
    debug_read    = 0;
    debug_write   = 0;
    debug_open    = 0;
    debug_malloc  = 0;
    debug_getchar = 0;

    debug_registers   = 0;
    debug_disassemble = 0;

    EXCEPTION_SIGNAL             = 1;
    EXCEPTION_ADDRESSERROR       = 2;
    EXCEPTION_UNKNOWNINSTRUCTION = 3;
    EXCEPTION_HEAPOVERFLOW       = 4;
    EXCEPTION_UNKNOWNSYSCALL     = 5;
    EXCEPTION_UNKNOWNFUNCTION    = 6;

    registers = malloc(32*4);

    pc = 0;
    ir = 0;

    reg_hi = 0;
    reg_lo = 0;
}

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ---------------------     L I B R A R Y     ---------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ----------------------- LIBRARY FUNCTIONS -----------------------
// -----------------------------------------------------------------

int twoToThePowerOf(int p) {
    // assert: 0 <= p < 31
    return *(power_of_two_table + p);
}

int leftShift(int n, int b) {
    // assert: b >= 0;

    if (b > 30)
        return 0;
    else
        return n * twoToThePowerOf(b);
}

int rightShift(int n, int b) {
    // assert: b >= 0

    if (b > 30)
        return 0;
    else if (n >= 0)
        return n / twoToThePowerOf(b);
    else
        // works even if n == INT_MIN
        return ((n + 1) + INT_MAX) / twoToThePowerOf(b) +
            (INT_MAX / twoToThePowerOf(b) + 1);
}

int getCharacter(int *s, int i) {
    // assert: i >= 0
    int a;

    a = i / 4;

    return rightShift(leftShift(*(s + a), 24 - (i % 4) * 8), 24);
}

int* putCharacter(int *s, int i, int c) {
    // assert: i >= 0, all characters are 7-bit
    int a;

    a = i / 4;

    *(s + a) = (*(s + a) - leftShift(getCharacter(s, i), (i % 4) * 8)) + leftShift(c, (i % 4) * 8);
    
    return s;
}

int stringLength(int *s) {
    int i;

    i = 0;

    while (getCharacter(s, i) != 0)
        i = i + 1;

    return i;
}

void stringReverse(int *s) {
    int i;
    int j;
    int tmp;

    i = 0;
    j = stringLength(s) - 1;

    while (i < j) {
        tmp = getCharacter(s, i);
        
        putCharacter(s, i, getCharacter(s, j));
        putCharacter(s, j, tmp);

        i = i + 1;
        j = j - 1;
    }
}

int stringCompare(int *s, int *t) {
    int i;

    i = 0;

    while (1)
        if (getCharacter(s, i) == 0)
            if (getCharacter(t, i) == 0)
                return 1;
            else
                return 0;
        else if (getCharacter(s, i) == getCharacter(t, i))
            i = i + 1;
        else
            return 0;
}

int atoi(int *s) {
    int i;
    int n;

    i = 0;

    n = 0;

    while (getCharacter(s, i) != 0) {
        n = n * 10 + getCharacter(s, i) - '0';
        
        i = i + 1;
    }

    return n;
}

int* itoa(int n, int *s, int b, int a) {
    // assert: b in {2,4,8,10,16}

    int i;
    int sign;

    i = 0;

    sign = 0;

    if (n == 0) {
        putCharacter(s, 0, '0');

        i = 1;
    } else if (n < 0) {
        sign = 1;

        if (b == 10) {
            if (n == INT_MIN) {
                // rightmost decimal digit of 32-bit INT_MIN
                putCharacter(s, 0, '8');

                n = -(n / 10);
                i = i + 1;
            } else
                n = -n;
        } else {
            if (n == INT_MIN) {
                // rightmost non-decimal digit of INT_MIN
                putCharacter(s, 0, '0');

                n = (rightShift(INT_MIN, 1) / b) * 2;
                i = i + 1;
            } else
                n = rightShift(leftShift(n, 1), 1);
        }
    }

    while (n != 0) {
        if (n % b > 9)
            putCharacter(s, i, n % b - 10 + 'A');
        else
            putCharacter(s, i, n % b + '0');

        n = n / b;
        i = i + 1;

        if (i == 1) {
            if (sign) {
                if (b != 10)
                    n = n + (rightShift(INT_MIN, 1) / b) * 2;
            }
        }
    }

    if (b != 10) {
        while (i < a) {
            putCharacter(s, i, '0'); // align with zeros

            i = i + 1;
        }

        if (b == 16) {
            putCharacter(s, i, 'x');
            putCharacter(s, i + 1, '0');

            i = i + 2;
        }
    } else if (sign) {
        putCharacter(s, i, '-');

        i = i + 1;
    }

    putCharacter(s, i, 0); // null terminated string

    stringReverse(s);

    return s;
}

void print(int *s) {
    int i;

    i = 0;

    while (getCharacter(s, i) != 0) {
        putchar(getCharacter(s, i));

        i = i + 1;
    }
}

void println() {
    putchar(CHAR_LF);
}

void printCharacter(int character) {
    putchar(CHAR_SINGLEQUOTE);

    if (character == CHAR_EOF)
        print((int*) "end of file");
    else if (character == CHAR_TAB)
        print((int*) "tabulator");
    else if (character == CHAR_LF)
        print((int*) "line feed");
    else if (character == CHAR_CR)
        print((int*) "carriage return");
    else
        putchar(character);

    putchar(CHAR_SINGLEQUOTE);
}

void printString(int *s) {
    putchar(CHAR_DOUBLEQUOTE);

    print(s);
    
    putchar(CHAR_DOUBLEQUOTE);
}

void assignString(int *s, int c0, int c1, int c2, int c3, int c4,
        int c5, int c6, int c7, int c8, int c9,
        int c10, int c11, int c12, int c13, int c14,
        int c15, int c16, int c17, int c18, int c19) {

    putCharacter(s, 0, c0);
    putCharacter(s, 1, c1);
    putCharacter(s, 2, c2);
    putCharacter(s, 3, c3);
    putCharacter(s, 4, c4);
    putCharacter(s, 5, c5);
    putCharacter(s, 6, c6);
    putCharacter(s, 7, c7);
    putCharacter(s, 8, c8);
    putCharacter(s, 9, c9);
    putCharacter(s, 10, c10);
    putCharacter(s, 11, c11);
    putCharacter(s, 12, c12);
    putCharacter(s, 13, c13);
    putCharacter(s, 14, c14);
    putCharacter(s, 15, c15);
    putCharacter(s, 16, c16);
    putCharacter(s, 17, c17);
    putCharacter(s, 18, c18);
    putCharacter(s, 19, c19);

    putCharacter(s, 20, 0); // null terminated string
}

int* createString(int c0, int c1, int c2, int c3, int c4,
        int c5, int c6, int c7, int c8, int c9,
        int c10, int c11, int c12, int c13, int c14,
        int c15, int c16, int c17, int c18, int c19) {
    int *s;

    s = malloc(21);
    assignString(s, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9,
                 c10, c11, c12, c13, c14, c15, c16, c17, c18, c19);
    return s;
}

void memset(int *a, int size, int v) {
    while (size > 0) {
        size = size - 1;
        *(a+size) = v;
    }
}

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ---------------------     C O M P I L E R   ---------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ---------------------------- SCANNER ----------------------------
// -----------------------------------------------------------------

void printSymbol(int symbol) {
    putchar(CHAR_DOUBLEQUOTE);

    if (symbol == SYM_EOF)
        print((int*) "end of file");
    else
        print((int*) *(SYMBOL + symbol));

    putchar(CHAR_DOUBLEQUOTE);
}

void printLineNumber(int* message) {
    print((int*) "cstarc: ");
    print(message);
    print((int*) " in line ");
    print(itoa(lineNumber, string_buffer, 10, 0));
    print((int*) ": ");
}

void syntaxErrorMessage(int *message) {
    printLineNumber((int*) "error");

    print(message);
    
    println();
}

void syntaxErrorCharacter(int expected) {
    printLineNumber((int*) "error");

    printCharacter(expected);
    print((int*) " expected but ");

    printCharacter(character);
    print((int*) " found");

    println();
}

int isCharacterWhitespace() {
    if (character == CHAR_SPACE)
        return 1;
    else if (character == CHAR_TAB)
        return 1;
    else if (character == CHAR_LF)
        return 1;
    else if (character == CHAR_CR)
        return 1;
    else
        return 0;
}

int findNextCharacter() {
    int inComment;

    inComment = 0;

    while (1) {
        if (inComment) {
            character = getchar();

            if (character == CHAR_LF)
                inComment = 0;
            else if (character == CHAR_CR)
                inComment = 0;
            else if (character == CHAR_EOF)
                return character;

        } else if (isCharacterWhitespace()) {
            if (character == CHAR_LF)
                lineNumber = lineNumber + 1;
            else if (character == CHAR_CR)
                lineNumber = lineNumber + 1;

            character = getchar();

        } else if (character == CHAR_HASH) {
            character = getchar();
            inComment = 1;

        } else if (character == CHAR_SLASH) {
            character = getchar();
            if (character == CHAR_SLASH)
                inComment = 1;
            else {
                symbol = SYM_DIV;
                return character;
            }

        } else
            return character;
    }
}

int isCharacterLetter() {
    if (character >= 'a')
        if (character <= 'z')
            return 1;
        else
            return 0;
    else if (character >= 'A')
        if (character <= 'Z')
            return 1;
        else
            return 0;
    else
        return 0;
}

int isCharacterDigit() {
    if (character >= '0')
        if (character <= '9')
            return 1;
        else
            return 0;
    else
        return 0;
}

int isCharacterLetterOrDigitOrUnderscore() {
    if (isCharacterLetter())
        return 1;
    else if (isCharacterDigit())
        return 1;
    else if (character == CHAR_UNDERSCORE)
        return 1;
    else
        return 0;
}

int isNotDoubleQuoteOrEOF() {
    if (character == CHAR_DOUBLEQUOTE)
        return 0;
    else if (character == CHAR_EOF)
        return 0;
    else
        return 1;
}

int identifierStringMatch(int keyword) {
    return stringCompare(identifier, (int*) *(SYMBOL + keyword));
}

int identifierOrKeyword() {
    if (identifierStringMatch(SYM_WHILE))
        return SYM_WHILE;
    if (identifierStringMatch(SYM_IF))
        return SYM_IF;
    if (identifierStringMatch(SYM_INT))
        return SYM_INT;
    if (identifierStringMatch(SYM_ELSE))
        return SYM_ELSE;
    if (identifierStringMatch(SYM_RETURN))
        return SYM_RETURN;
    if (identifierStringMatch(SYM_VOID))
        return SYM_VOID;
    else
        return SYM_IDENTIFIER;
}

int getSymbol() {
    int i;

    symbol = SYM_EOF;

    if (findNextCharacter() == CHAR_EOF)
        return SYM_EOF;
    else if (symbol == SYM_DIV)
        // check here because / was recognized instead of //
        return SYM_DIV;

    if (isCharacterLetter()) {
        identifier = malloc(maxIdentifierLength + 1);

        i = 0;

        while (isCharacterLetterOrDigitOrUnderscore()) {
            if (i >= maxIdentifierLength) {
                syntaxErrorMessage((int*) "identifier too long");
                exit(-1);
            }

            putCharacter(identifier, i, character);

            i = i + 1;

            character = getchar();
        }

        putCharacter(identifier, i, 0); // null terminated string

        symbol = identifierOrKeyword();

    } else if (isCharacterDigit()) {
        integer = malloc(maxIntegerLength + 1);

        i = 0;

        while (isCharacterDigit()) {
            if (i >= maxIntegerLength) {
                syntaxErrorMessage((int*) "integer out of bound");
                exit(-1);
            }

            putCharacter(integer, i, character);

            i = i + 1;
            
            character = getchar();
        }

        putCharacter(integer, i, 0); // null terminated string

        ivalue = atoi(integer);

        if (ivalue < 0) {
            if (ivalue == INT_MIN) {
                if (mayBeINTMINConstant == 0) {
                    syntaxErrorMessage((int*) "integer out of bound");
                    exit(-1);
                }
            } else {
                syntaxErrorMessage((int*) "integer out of bound");
                exit(-1);
            }
        }

        symbol = SYM_INTEGER;

    } else if (character == CHAR_DOUBLEQUOTE) {
        character = getchar();

        string = malloc(maxStringLength + 1);

        i = 0;

        while (isNotDoubleQuoteOrEOF()) {
            if (i >= maxStringLength) {
                syntaxErrorMessage((int*) "string too long");
                exit(-1);
            }

            putCharacter(string, i, character);

            i = i + 1;
            
            character = getchar();
        }

        if (character == CHAR_DOUBLEQUOTE)
            character = getchar();
        else {
            syntaxErrorCharacter(CHAR_DOUBLEQUOTE);

            exit(-1);
        }

        putCharacter(string, i, 0); // null terminated string

        symbol = SYM_STRING;

    } else if (character == CHAR_SINGLEQUOTE) {
        character = getchar();

        ivalue = 0;

        if (character == CHAR_EOF) {
            syntaxErrorMessage((int*) "reached end of file looking for a character constant");

            exit(-1);
        } else
            ivalue = character;

        character = getchar();

        if (character == CHAR_SINGLEQUOTE)
            character = getchar();
        else if (character == CHAR_EOF) {
            syntaxErrorCharacter(CHAR_SINGLEQUOTE);

            exit(-1);
        } else
            syntaxErrorCharacter(CHAR_SINGLEQUOTE);

        symbol = SYM_CHARACTER;

    } else if (character == CHAR_SEMICOLON) {
        character = getchar();
        symbol = SYM_SEMICOLON;

    } else if (character == CHAR_PLUS) {
        character = getchar();
        symbol = SYM_PLUS;

    } else if (character == CHAR_DASH) {
        character = getchar();
        symbol = SYM_MINUS;

    } else if (character == CHAR_ASTERISK) {
        character = getchar();
        symbol = SYM_ASTERISK;

    } else if (character == CHAR_EQUAL) {
        character = getchar();
        if (character == CHAR_EQUAL) {
            character = getchar();
            symbol = SYM_EQUALITY;
        } else
            symbol = SYM_ASSIGN;

    } else if (character == CHAR_LPARENTHESIS) {
        character = getchar();
        symbol = SYM_LPARENTHESIS;

    } else if (character == CHAR_RPARENTHESIS) {
        character = getchar();
        symbol = SYM_RPARENTHESIS;

    } else if (character == CHAR_LBRACE) {
        character = getchar();
        symbol = SYM_LBRACE;

    } else if (character == CHAR_RBRACE) {
        character = getchar();
        symbol = SYM_RBRACE;

    } else if (character == CHAR_COMMA) {
        character = getchar();
        symbol = SYM_COMMA;

    } else if (character == CHAR_LT) {
        character = getchar();
        if (character == CHAR_EQUAL) {
            character = getchar();
            symbol = SYM_LEQ;
        } else
            symbol = SYM_LT;

    } else if (character == CHAR_GT) {
        character = getchar();
        if (character == CHAR_EQUAL) {
            character = getchar();
            symbol = SYM_GEQ;
        } else
            symbol = SYM_GT;

    } else if (character == CHAR_EXCLAMATION) {
        character = getchar();
        if (character == CHAR_EQUAL)
            character = getchar();
        else
            syntaxErrorCharacter(CHAR_EQUAL);

        symbol = SYM_NOTEQ;

    } else if (character == CHAR_PERCENTAGE) {
        character = getchar();
        symbol = SYM_MOD;

    } else {
        printLineNumber((int*) "error");
        print((int*) "found unknown character ");
        printCharacter(character);
        
        println();

        exit(-1);
    }

    return symbol;
}

// -----------------------------------------------------------------
// ------------------------- SYMBOL TABLE --------------------------
// -----------------------------------------------------------------

void createSymbolTableEntry(int which, int *string, int data, int class, int type) {
    int *newEntry;

    // symbol table entry:
    // +----+----------+
    // |  0 | next     | pointer to next entry
    // |  1 | string   | identifier string, string constant
    // |  2 | data     | VARIABLE: offset, FUNCTION: address, STRING: offset
    // |  3 | class    | VARIABLE, FUNCTION, STRING
    // |  4 | type     | INT_T, INTSTAR_T
    // |  5 | register | REG_GP, REG_FP
    // +----+----------+

    newEntry = malloc(6 * 4);

    setString(newEntry, string);
    setData(newEntry, data);
    setClass(newEntry, class);
    setType(newEntry, type);

    // create entry at head of symbol table
    if (which == GLOBAL_TABLE) {
        setRegister(newEntry, REG_GP);
        setNext(newEntry, global_symbol_table);
        global_symbol_table = newEntry;
    } else {
        setRegister(newEntry, REG_FP);
        setNext(newEntry, local_symbol_table);
        local_symbol_table = newEntry;
    }
}

int* getSymbolTableEntry(int *string, int class, int *symbol_table) {
    while ((int) symbol_table != 0) {
        if (stringCompare(string, getString(symbol_table)))
            if (class == getClass(symbol_table))
                return symbol_table;
        
        // keep looking
        symbol_table = getNext(symbol_table);
    }

    return (int*) 0;
}

int* getNext(int *entry) {
    // cast only works if size of int and int* is equivalent
    return (int*) *entry;
}

int* getString(int *entry) {
    // cast only works if size of int and int* is equivalent
    return (int*) *(entry + 1);
}

int getData(int *entry) {
    return *(entry + 2);
}

int getClass(int *entry) {
    return *(entry + 3);
}

int getType(int *entry) {
    return *(entry + 4);
}

int getRegister(int *entry) {
    return *(entry + 5);
}

void setNext(int *entry, int *next) {
    // cast only works if size of int and int* is equivalent
    *entry = (int) next;
}

void setString(int *entry, int *identifier) {
    // cast only works if size of int and int* is equivalent
    *(entry + 1) = (int) identifier;
}

void setData(int *entry, int data) {
    *(entry + 2) = data;
}

void setClass(int *entry, int class) {
    *(entry + 3) = class;
}

void setType(int *entry, int type) {
    *(entry + 4) = type;
}

void setRegister(int *entry, int reg) {
    *(entry + 5) = reg;
}

// -----------------------------------------------------------------
// ---------------------------- PARSER -----------------------------
// -----------------------------------------------------------------

int isNotRbraceOrEOF() {
    if (symbol == SYM_RBRACE)
        return 0;
    else if(symbol == SYM_EOF)
        return 0;
    else
        return 1;
}

int isExpression() {
    if (symbol == SYM_MINUS)
        return 1;
    else if (symbol == SYM_LPARENTHESIS)
        return 1;
    else if (symbol == SYM_IDENTIFIER)
        return 1;
    else if (symbol == SYM_INTEGER)
        return 1;
    else if (symbol == SYM_ASTERISK)
        return 1;
    else if (symbol == SYM_STRING)
        return 1;
    else if (symbol == SYM_CHARACTER)
        return 1;
    else
        return 0;
}

int isStarOrDivOrModulo() {
    if (symbol == SYM_ASTERISK)
        return 1;
    else if (symbol == SYM_DIV)
        return 1;
    else if (symbol == SYM_MOD)
        return 1;
    else
        return 0;
}

int isPlusOrMinus() {
    if (symbol == SYM_MINUS)
        return 1;
    else if (symbol == SYM_PLUS)
        return 1;
    else
        return 0;
}

int isComparison() {
    if (symbol == SYM_EQUALITY)
        return 1;
    else if (symbol == SYM_NOTEQ)
        return 1;
    else if (symbol == SYM_LT)
        return 1;
    else if (symbol == SYM_GT)
        return 1;
    else if (symbol == SYM_LEQ)
        return 1;
    else if (symbol == SYM_GEQ)
        return 1;
    else
        return 0;
}

int isVariableOrProcedure() {
    if (symbol == SYM_INT)
        return 1;
    else if (symbol == SYM_VOID)
        return 1;
    else
        return 0;
}

int lookForFactor() {
    if (symbol == SYM_LPARENTHESIS)
        return 0;
    else if (symbol == SYM_ASTERISK)
        return 0;
    else if (symbol == SYM_IDENTIFIER)
        return 0;
    else if (symbol == SYM_INTEGER)
        return 0;
    else if (symbol == SYM_STRING)
        return 0;
    else if (symbol == SYM_CHARACTER)
        return 0;
    else if (symbol == SYM_EOF)
        return 0;
    else
        return 1;
}

int lookForStatement() {
    if (symbol == SYM_ASTERISK)
        return 0;
    else if (symbol == SYM_IDENTIFIER)
        return 0;
    else if (symbol == SYM_WHILE)
        return 0;
    else if (symbol == SYM_IF)
        return 0;
    else if (symbol == SYM_RETURN)
        return 0;
    else if (symbol == SYM_EOF)
        return 0;
    else
        return 1;
}

int lookForVariable() {
    if (symbol == SYM_INT)
        return 0;
    else if (symbol == SYM_EOF)
        return 0;
    else
        return 1;
}

void save_registers() {
    while (allocatedRegisters > 0) {
        emitIFormat(OP_ADDIU, REG_SP, REG_SP, -4);
        emitIFormat(OP_SW, REG_SP, allocatedRegisters, 0);

        allocatedRegisters = allocatedRegisters - 1;
    }
}

void restore_registers(int numberOfRegisters) {

    while (allocatedRegisters < numberOfRegisters) {
        allocatedRegisters = allocatedRegisters + 1;

        emitIFormat(OP_LW, REG_SP, allocatedRegisters, 0);
        emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);
    }
}

void syntaxErrorSymbol(int expected) {
    printLineNumber((int*) "error");

    printSymbol(expected);
    print((int*) " expected but ");

    printSymbol(symbol);
    print((int*) " found");

    println();
}

void syntaxErrorUnexpected() {
    printLineNumber((int*) "error");

    print((int*) "unexpected symbol ");
    printSymbol(symbol);
    print((int*) " found");

    println();
}

int* putType(int type) {
    if (type == INT_T)
        return (int*) "int";
    else if (type == INTSTAR_T)
        return (int*) "int*";
    else if (type == VOID_T)
        return (int*) "void";
    else
        return (int*) "unknown";
}

void typeWarning(int expected, int found) {
    printLineNumber((int*) "warning");

    print((int*) "type mismatch, ");

    print(putType(expected));

    print((int*) " expected but ");

    print(putType(found));

    print((int*) " found");

    println();
}

int* getVariable(int *variable) {
    int *entry;

    entry = getSymbolTableEntry(variable, VARIABLE, local_symbol_table);

    if (entry == (int*) 0) {
        entry = getSymbolTableEntry(variable, VARIABLE, global_symbol_table);

        if (entry == (int*) 0) {
            printLineNumber((int*) "error");

            print(variable);

            print((int*) " undeclared");
            println();

            exit(-1);
        }
    }

    return entry;
}

int load_variable(int *variable) {
    int *entry;

    entry = getVariable(variable);

    allocatedRegisters = allocatedRegisters + 1;

    emitIFormat(OP_LW, getRegister(entry), allocatedRegisters, getData(entry));

    return getType(entry);
}

void load_integer() {
    // assert: ivalue >= 0 or ivalue == INT_MIN

    allocatedRegisters = allocatedRegisters + 1;

    if (ivalue >= 0) {
        mayBeINTMINConstant = 0;

        if (ivalue < twoToThePowerOf(15))
            // ADDIU can only load numbers < 2^15 without sign extension
            emitIFormat(OP_ADDIU, REG_ZR, allocatedRegisters, ivalue);
        else if (ivalue < twoToThePowerOf(28)) {
            // load 14 msbs of a 28-bit number first
            emitIFormat(OP_ADDIU, REG_ZR, allocatedRegisters, rightShift(ivalue, 14));

            // shift left by 14 bits
            emitLeftShiftBy(14);

            // and finally add 14 lsbs
            emitIFormat(OP_ADDIU, allocatedRegisters, allocatedRegisters, rightShift(leftShift(ivalue, 18), 18));
        } else {
            // load 14 msbs of a 31-bit number first
            emitIFormat(OP_ADDIU, REG_ZR, allocatedRegisters, rightShift(ivalue, 17));

            emitLeftShiftBy(14);

            // then add the next 14 msbs
            emitIFormat(OP_ADDIU, allocatedRegisters, allocatedRegisters, rightShift(leftShift(ivalue, 15), 18));

            emitLeftShiftBy(3);

            // and finally add the remaining 3 lsbs
            emitIFormat(OP_ADDIU, allocatedRegisters, allocatedRegisters, rightShift(leftShift(ivalue, 29), 29));
        }
    } else {
        // load largest positive 16-bit number with a single bit set: 2^14
        emitIFormat(OP_ADDIU, REG_ZR, allocatedRegisters, twoToThePowerOf(14));

        // and then multiply 2^14 by 2^14*2^3 to get to 2^31 == INT_MIN
        emitLeftShiftBy(14);
        emitLeftShiftBy(3);
    }

    getSymbol();
}

void load_string() {
    int l;

    l = stringLength(string) + 1;

    allocatedMemory = allocatedMemory + l / 4;

    if (l % 4 != 0)
        allocatedMemory = allocatedMemory + 1;

    createSymbolTableEntry(GLOBAL_TABLE, string, -4 * allocatedMemory, STRING, INTSTAR_T);

    allocatedRegisters = allocatedRegisters + 1;

    emitIFormat(OP_ADDIU, REG_GP, allocatedRegisters, -4 * allocatedMemory);

    getSymbol();
}

int help_call_codegen(int *entry, int *procedure) {
    int type;

    type = UNKNOWN;

    if (entry == (int*) 0) {
        // CASE 1: function call, no definition, no declaration.
        createSymbolTableEntry(GLOBAL_TABLE, procedure, binaryLength, FUNCTION, INT_T);
        emitJFormat(OP_JAL, 0);
        type = INT_T; //assume default return type 'int'

    } else {
        type = getType(entry);

        if (getData(entry) == 0) {
            // CASE 2: function call, no definition, but declared.
            setData(entry, binaryLength);
            emitJFormat(OP_JAL, 0);
        } else if (getOpcode(*(memory + getData(entry))) == OP_JAL) {
            // CASE 3: function call, no declaration
            emitJFormat(OP_JAL, getData(entry));
            setData(entry, binaryLength - 2);
        } else
            // CASE 4: function defined, use the address
            emitJFormat(OP_JAL, getData(entry));
    }

    return type;
}

void help_procedure_prologue(int localVariables) {
    // allocate space for return address
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, -4);

    // save return address
    emitIFormat(OP_SW, REG_SP, REG_LINK, 0);

    // allocate space for caller's frame pointer
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, -4);

    // save caller's frame pointer
    emitIFormat(OP_SW, REG_SP, REG_FP, 0);

    // set callee's frame pointer
    emitIFormat(OP_ADDIU, REG_SP, REG_FP, 0);

    // allocate space for callee's local variables
    if (localVariables != 0)
        emitIFormat(OP_ADDIU, REG_SP, REG_SP, -4 * localVariables);
}

void help_procedure_epilogue(int parameters, int functionStart, int functionType) {
    // deallocate space for callee's frame pointer and local variables
    emitIFormat(OP_ADDIU, REG_FP, REG_SP, 0);

    // restore caller's frame pointer
    emitIFormat(OP_LW, REG_SP, REG_FP, 0);

    // deallocate space for caller's frame pointer
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    // restore return address
    emitIFormat(OP_LW, REG_SP, REG_LINK, 0);

    // deallocate space for return address and parameters
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, (parameters + 1) * 4);

    // return
    emitRFormat(OP_SPECIAL, REG_LINK, 0, 0, FCT_JR);
}

int gr_call(int *procedure) {
    int *entry;
    int numberOfRegisters;
    int type;

    // assert: n = allocatedRegisters

    entry = getSymbolTableEntry(procedure, FUNCTION, global_symbol_table);

    numberOfRegisters = allocatedRegisters;

    save_registers();

    // assert: allocatedRegisters == 0

    if (isExpression()) {
        gr_expression();

        // TODO: check if types/number of parameters is correct
        // PSH first parameter onto stack
        emitIFormat(OP_ADDIU, REG_SP, REG_SP, -4);
        emitIFormat(OP_SW, REG_SP, allocatedRegisters, 0);

        allocatedRegisters = allocatedRegisters - 1;

        while (symbol == SYM_COMMA) {
            getSymbol();

            gr_expression();

            // PSH more parameters onto stack
            emitIFormat(OP_ADDIU, REG_SP, REG_SP, -4);
            emitIFormat(OP_SW, REG_SP, allocatedRegisters, 0);

            allocatedRegisters = allocatedRegisters - 1;
        }

        if (symbol == SYM_RPARENTHESIS) {
            getSymbol();
            
            type = help_call_codegen(entry, procedure);
        } else {
            syntaxErrorSymbol(SYM_RPARENTHESIS);

            type = INT_T;
        }
    } else if (symbol == SYM_RPARENTHESIS) {
        getSymbol();

        type = help_call_codegen(entry, procedure);
    } else {
        syntaxErrorSymbol(SYM_RPARENTHESIS);

        type = INT_T;
    }

    // assert: allocatedRegisters == 0

    restore_registers(numberOfRegisters);

    // assert: allocatedRegisters == n
    return type;
}

int gr_factor() {
    int cast;
    int type;
    int *variableOrProcedureName;

    // assert: n = allocatedRegisters

    cast = 0; // turn off cast by default

    while (lookForFactor()) {
        syntaxErrorUnexpected();

        if (symbol == SYM_EOF)
            exit(-1);
        else
            getSymbol();
    }

    // optional cast: [ cast ]
    if (symbol == SYM_LPARENTHESIS) {
        getSymbol();

        mayBeINTMINConstant = 0;

        // (int)
        if (symbol == SYM_INT) {
            getSymbol();

            cast = INT_T;

            // (int*)
            if (symbol == SYM_ASTERISK) {
                getSymbol();

                cast = INTSTAR_T;
            }

            if (symbol == SYM_RPARENTHESIS)
                getSymbol();
            else
                syntaxErrorSymbol(SYM_RPARENTHESIS);

        // Not a cast: "(" expression ")"
        } else {
            type = gr_expression();

            if (symbol == SYM_RPARENTHESIS)
                getSymbol();
            else
                syntaxErrorSymbol(SYM_RPARENTHESIS);

            // assert: allocatedRegisters == n + 1

            return type;
        }
    }

    // dereference?
    if (symbol == SYM_ASTERISK) {
        getSymbol();

        mayBeINTMINConstant = 0;

        // ["*"] identifier
        if (symbol == SYM_IDENTIFIER) {
            type = load_variable(identifier);

            getSymbol();

        // * "(" simpleExpression ")"
        } else if (symbol == SYM_LPARENTHESIS) {
            getSymbol();

            type = gr_expression();

            if (symbol == SYM_RPARENTHESIS)
                getSymbol();
            else
                syntaxErrorSymbol(SYM_RPARENTHESIS);
        } else
            syntaxErrorUnexpected();

        // dereference
        emitIFormat(OP_LW, allocatedRegisters, allocatedRegisters, 0);

        if (type != INTSTAR_T)
            typeWarning(INTSTAR_T, type);

        type = INT_T;

    // identifier?
    } else if (symbol == SYM_IDENTIFIER) {
        variableOrProcedureName = identifier;

        getSymbol();

        mayBeINTMINConstant = 0;

        if (symbol == SYM_LPARENTHESIS) {
            getSymbol();

            // function call:  identifier "(" ... ")" ...
            type = gr_call(variableOrProcedureName);

            allocatedRegisters = allocatedRegisters + 1;

            emitIFormat(OP_ADDIU, REG_RR, allocatedRegisters, 0);
        } else
            // else.. it is just an 'identifier'
            type = load_variable(variableOrProcedureName);

    // integer?
    } else if (symbol == SYM_INTEGER) {
        load_integer();

        type = INT_T;

    // string?
    } else if (symbol == SYM_STRING) {
        load_string();

        type = INTSTAR_T;

    // character?
    } else if (symbol == SYM_CHARACTER) {
        mayBeINTMINConstant = 0;

        allocatedRegisters = allocatedRegisters + 1;

        emitIFormat(OP_ADDIU, REG_ZR, allocatedRegisters, ivalue);

        getSymbol();

        type = INT_T;

    //  "(" expression ")"
    } else if (symbol == SYM_LPARENTHESIS) {
        mayBeINTMINConstant = 0;
        
        getSymbol();

        type = gr_expression();

        if (symbol == SYM_RPARENTHESIS)
            getSymbol();
        else
            syntaxErrorSymbol(SYM_RPARENTHESIS);
    }

    // assert: allocatedRegisters == n + 1

    if (cast == 0)
        return type;
    else
        return cast;
}

int gr_term() {
    int ltype;
    int operatorSymbol;
    int rtype;

    // assert: n = allocatedRegisters

    ltype = gr_factor();

    // assert: allocatedRegisters == n + 1

    // * / or % ?
    while (isStarOrDivOrModulo()) {
        operatorSymbol = symbol;

        getSymbol();

        rtype = gr_factor();

        // assert: allocatedRegisters == n + 2
        
        if (ltype != rtype)
            typeWarning(ltype, rtype);

        if (operatorSymbol == SYM_ASTERISK) {
            emitRFormat(OP_SPECIAL, allocatedRegisters-1, allocatedRegisters, 0, FCT_MULTU);
            emitRFormat(OP_SPECIAL, 0, 0, allocatedRegisters-1, FCT_MFLO);

        } else if (operatorSymbol == SYM_DIV) {
            emitRFormat(OP_SPECIAL, allocatedRegisters-1, allocatedRegisters, 0, FCT_DIVU);
            emitRFormat(OP_SPECIAL, 0, 0, allocatedRegisters-1, FCT_MFLO);

        } else if (operatorSymbol == SYM_MOD) {
            emitRFormat(OP_SPECIAL, allocatedRegisters-1, allocatedRegisters, 0, FCT_DIVU);
            emitRFormat(OP_SPECIAL, 0, 0, allocatedRegisters-1, FCT_MFHI);
        }

        allocatedRegisters = allocatedRegisters - 1;
    }

    // assert: allocatedRegisters == n + 1

    return ltype;
}

int gr_simpleExpression() {
    int sign;
    int ltype;
    int operatorSymbol;
    int rtype;

    // assert: n = allocatedRegisters

    // optional: -
    if (symbol == SYM_MINUS) {
        sign = 1;
        mayBeINTMINConstant = 1;
        getSymbol();
    } else
        sign = 0;

    ltype = gr_term();

    // assert: allocatedRegisters == n + 1

    if (sign == 1) {
        if (mayBeINTMINConstant)
            // avoids 0-INT_MIN overflow when bootstrapping
            // even though 0-INT_MIN == INT_MIN
            mayBeINTMINConstant = 0;
        else
            emitRFormat(OP_SPECIAL, REG_ZR, allocatedRegisters, allocatedRegisters, FCT_SUBU);
    }

    // + or -?
    while (isPlusOrMinus()) {
        operatorSymbol = symbol;

        getSymbol();

        rtype = gr_term();

        // assert: allocatedRegisters == n + 2

        if (operatorSymbol == SYM_PLUS) {
            if (ltype == INTSTAR_T) {
                if (rtype == INT_T)
                    // pointer arithmetic: factor of 2^2 of integer operand
                    emitLeftShiftBy(2);
            } else if (rtype == INTSTAR_T)
                typeWarning(ltype, rtype);

            emitRFormat(OP_SPECIAL, allocatedRegisters-1, allocatedRegisters, allocatedRegisters-1, FCT_ADDU);

        } else if (operatorSymbol == SYM_MINUS) {
            if (ltype != rtype)
                typeWarning(ltype, rtype);

            emitRFormat(OP_SPECIAL, allocatedRegisters-1, allocatedRegisters, allocatedRegisters-1, FCT_SUBU);
        }

        allocatedRegisters = allocatedRegisters - 1;
    }

    // assert: allocatedRegisters == n + 1

    return ltype;
}

int gr_expression() {
    int ltype;
    int operatorSymbol;
    int rtype;

    // assert: n = allocatedRegisters

    ltype = gr_simpleExpression();

    // assert: allocatedRegisters == n + 1

    //optional: ==, !=, <, >, <=, >= simpleExpression
    if (isComparison()) {
        operatorSymbol = symbol;

        getSymbol();

        rtype = gr_simpleExpression();

        // assert: allocatedRegisters == n + 2

        if (ltype != rtype)
            typeWarning(ltype, rtype);

        if (operatorSymbol == SYM_EQUALITY) {
            // subtract, if result = 0 then 1, else 0
            emitRFormat(OP_SPECIAL, allocatedRegisters-1, allocatedRegisters, allocatedRegisters-1, FCT_SUBU);

            allocatedRegisters = allocatedRegisters - 1;

            emitIFormat(OP_BEQ, REG_ZR, allocatedRegisters, 4);
            emitIFormat(OP_ADDIU, REG_ZR, allocatedRegisters, 0);
            emitIFormat(OP_BEQ, REG_ZR, allocatedRegisters, 2);
            emitIFormat(OP_ADDIU, REG_ZR, allocatedRegisters, 1);

        } else if (operatorSymbol == SYM_NOTEQ) {
            // subtract, if result = 0 then 0, else 1
            emitRFormat(OP_SPECIAL, allocatedRegisters-1, allocatedRegisters, allocatedRegisters-1, FCT_SUBU);

            allocatedRegisters = allocatedRegisters - 1;

            emitIFormat(OP_BNE, REG_ZR, allocatedRegisters, 4);
            emitIFormat(OP_ADDIU, REG_ZR, allocatedRegisters, 0);
            emitIFormat(OP_BEQ, REG_ZR, allocatedRegisters, 2);
            emitIFormat(OP_ADDIU, REG_ZR, allocatedRegisters, 1);

        } else if (operatorSymbol == SYM_LT) {
            // set to 1 if a < b, else 0
            emitRFormat(OP_SPECIAL, allocatedRegisters-1, allocatedRegisters, allocatedRegisters-1, FCT_SLT);

            allocatedRegisters = allocatedRegisters - 1;

        } else if (operatorSymbol == SYM_GT) {
            // set to 1 if b < a, else 0
            emitRFormat(OP_SPECIAL, allocatedRegisters, allocatedRegisters-1, allocatedRegisters-1, FCT_SLT);

            allocatedRegisters = allocatedRegisters - 1;

        } else if (operatorSymbol == SYM_LEQ) {
            // if b < a set 0, else 1
            emitRFormat(OP_SPECIAL, allocatedRegisters, allocatedRegisters-1, allocatedRegisters-1, FCT_SLT);

            allocatedRegisters = allocatedRegisters - 1;

            emitIFormat(OP_BNE, REG_ZR, allocatedRegisters, 4);
            emitIFormat(OP_ADDIU, REG_ZR, allocatedRegisters, 1);
            emitIFormat(OP_BEQ, REG_ZR, REG_ZR, 2);
            emitIFormat(OP_ADDIU, REG_ZR, allocatedRegisters, 0);

        } else if (operatorSymbol == SYM_GEQ) {
            // if a < b set 0, else 1
            emitRFormat(OP_SPECIAL, allocatedRegisters-1, allocatedRegisters, allocatedRegisters-1, FCT_SLT);

            allocatedRegisters = allocatedRegisters - 1;

            emitIFormat(OP_BNE, REG_ZR, allocatedRegisters, 4);
            emitIFormat(OP_ADDIU, REG_ZR, allocatedRegisters, 1);
            emitIFormat(OP_BEQ, REG_ZR, REG_ZR, 2);
            emitIFormat(OP_ADDIU, REG_ZR, allocatedRegisters, 0);
        }
    }
    
    // assert: allocatedRegisters == n + 1

    return ltype;
}

void gr_while() {
    int brBackToWhile;
    int brForwardToEnd;

    // assert: allocatedRegisters == 0

    brBackToWhile = binaryLength;

    // while ( expression )
    if (symbol == SYM_WHILE) {
        getSymbol();

        if (symbol == SYM_LPARENTHESIS) {
            getSymbol();

            gr_expression();

            // do not know where to branch, fixup later
            brForwardToEnd = binaryLength;

            emitIFormat(OP_BEQ, REG_ZR, allocatedRegisters, 0);

            allocatedRegisters = allocatedRegisters - 1;

            if (symbol == SYM_RPARENTHESIS) {
                getSymbol();

                // zero or more statements: { statement }
                if (symbol == SYM_LBRACE) {
                    getSymbol();

                    while (isNotRbraceOrEOF())
                        gr_statement();

                    if (symbol == SYM_RBRACE)
                        getSymbol();
                    else {
                        syntaxErrorSymbol(SYM_RBRACE);

                        exit(-1);
                    }
                }
                // only one statement without {}
                else
                    gr_statement();
            } else
                syntaxErrorSymbol(SYM_RPARENTHESIS);
        } else
            syntaxErrorSymbol(SYM_LPARENTHESIS);
    } else
        syntaxErrorSymbol(SYM_WHILE);

    // unconditional branch to beginning of while
    emitIFormat(OP_BEQ, 0, 0, brBackToWhile - binaryLength - 1);

    // first instr after loop comes here, now we have
    // our address for the conditional jump from above
    fixup_relative(brForwardToEnd);

    // assert: allocatedRegisters == 0
}

void gr_if() {
    int brForwardToElseOrEnd;
    int brForwardToEnd;

    // assert: allocatedRegisters == 0

    // if ( expression )
    if (symbol == SYM_IF) {
        getSymbol();

        if (symbol == SYM_LPARENTHESIS) {
            getSymbol();

            gr_expression();

            // if the "if" case is not true, we jump to "else" (if provided)
            brForwardToElseOrEnd = binaryLength;

            emitIFormat(OP_BEQ, REG_ZR, allocatedRegisters, 0);

            allocatedRegisters = allocatedRegisters - 1;

            if (symbol == SYM_RPARENTHESIS) {
                getSymbol();

                // zero or more statements: { statement }
                if (symbol == SYM_LBRACE) {
                    getSymbol();

                    while (isNotRbraceOrEOF())
                        gr_statement();

                    if (symbol == SYM_RBRACE)
                        getSymbol();
                    else {
                        syntaxErrorSymbol(SYM_RBRACE);

                        exit(-1);
                    }
                }
                // only one statement without {}
                else
                    gr_statement();

                //optional: else
                if (symbol == SYM_ELSE) {
                    getSymbol();

                    // if the "if" case was true, we jump to the end
                    brForwardToEnd = binaryLength;
                    emitIFormat(OP_BEQ, 0, 0, 0);

                    // if the "if" case was not true, we jump here
                    fixup_relative(brForwardToElseOrEnd);

                    // zero or more statements: { statement }
                    if (symbol == SYM_LBRACE) {
                        getSymbol();

                        while (isNotRbraceOrEOF())
                            gr_statement();

                        if (symbol == SYM_RBRACE)
                            getSymbol();
                        else {
                            syntaxErrorSymbol(SYM_RBRACE);

                            exit(-1);
                        }

                    // only one statement without {}
                    } else
                        gr_statement();

                    // if the "if" case was true, we jump here
                    fixup_relative(brForwardToEnd);
                } else
                    // if the "if" case was not true, we jump here
                    fixup_relative(brForwardToElseOrEnd);
            } else
                syntaxErrorSymbol(SYM_RPARENTHESIS);
        } else
            syntaxErrorSymbol(SYM_LPARENTHESIS);
    } else
        syntaxErrorSymbol(SYM_IF);

    // assert: allocatedRegisters == 0
}

void gr_return(int returnType) {
    int type;

    // assert: allocatedRegisters == 0

    if (symbol == SYM_RETURN)
        getSymbol();
    else
        syntaxErrorSymbol(SYM_RETURN);

    // optional: expression
    if (symbol != SYM_SEMICOLON) {
        type = gr_expression();

        if (returnType == VOID_T)
            typeWarning(type, returnType);
        else if (type != returnType)
            typeWarning(returnType, type);

        // save value of expression in return register
        emitRFormat(OP_SPECIAL, REG_ZR, allocatedRegisters, REG_RR, FCT_ADDU);
        
        allocatedRegisters = allocatedRegisters - 1;
    }

    // unconditional branch to procedure epilogue
    // maintain fixup chain for later fixup
    emitJFormat(OP_J, returnBranches);

    // new head of fixup chain, offest is 2 because of delay slot NOP
    returnBranches = binaryLength - 2;

    // assert: allocatedRegisters == 0
}

void gr_statement() {
    int ltype;
    int rtype;
    int *variableOrProcedureName;
    int *entry;

    // assert: allocatedRegisters == 0;

    while (lookForStatement()) {
        syntaxErrorUnexpected();

        if (symbol == SYM_EOF)
            exit(-1);
        else
            getSymbol();
    }

    // ["*"]
    if (symbol == SYM_ASTERISK) {
        getSymbol();

        // "*" identifier
        if (symbol == SYM_IDENTIFIER) {
            ltype = load_variable(identifier);

            if (ltype != INTSTAR_T)
                typeWarning(INTSTAR_T, ltype);

            getSymbol();

            // "*" identifier "="
            if (symbol == SYM_ASSIGN) {
                getSymbol();

                rtype = gr_expression();

                if (rtype != INT_T)
                    typeWarning(INT_T, rtype);

                emitIFormat(OP_SW, allocatedRegisters-1, allocatedRegisters, 0);

                allocatedRegisters = allocatedRegisters - 2;
            } else
                syntaxErrorSymbol(SYM_ASSIGN);

            if (symbol == SYM_SEMICOLON)
                getSymbol();
            else
                syntaxErrorSymbol(SYM_SEMICOLON);

        // "*" "(" identifier [ "+" integer ]
        } else if (symbol == SYM_LPARENTHESIS) {
            getSymbol();

            if (symbol == SYM_IDENTIFIER) {
                ltype = load_variable(identifier);

                if (ltype != INTSTAR_T)
                    typeWarning(INTSTAR_T, ltype);

                getSymbol();

                if (symbol == SYM_PLUS) {
                    getSymbol();

                    if (symbol == SYM_IDENTIFIER) {
                        rtype = load_variable(identifier);

                        if (rtype != INT_T)
                            typeWarning(INT_T, rtype);

                        getSymbol();
                    } else if (symbol == SYM_INTEGER)
                        load_integer();
                    else
                        syntaxErrorUnexpected();

                    // pointer arithmetic: factor of 2^2 of integer operand
                    emitLeftShiftBy(2);

                    emitRFormat(OP_SPECIAL, allocatedRegisters-1, allocatedRegisters, allocatedRegisters-1, FCT_ADDU);

                    allocatedRegisters = allocatedRegisters - 1;
                }

                if (symbol == SYM_RPARENTHESIS) {
                    getSymbol();

                    // "*" "(" identifier ["+" integer] ")" ="
                    if (symbol == SYM_ASSIGN) {
                        getSymbol();

                        rtype = gr_expression();

                        if (rtype != INT_T)
                            typeWarning(INT_T, rtype);

                        emitIFormat(OP_SW, allocatedRegisters-1, allocatedRegisters, 0);

                        allocatedRegisters = allocatedRegisters - 2;
                    } else
                        syntaxErrorSymbol(SYM_ASSIGN);

                    if (symbol == SYM_SEMICOLON)
                        getSymbol();
                    else
                        syntaxErrorSymbol(SYM_SEMICOLON);
                } else
                    syntaxErrorSymbol(SYM_RPARENTHESIS);
            } else
                syntaxErrorSymbol(SYM_IDENTIFIER);
        } else
            syntaxErrorSymbol(SYM_LPARENTHESIS);
    }
    // identifier "=" expression | call
    else if (symbol == SYM_IDENTIFIER) {
        variableOrProcedureName = identifier;

        getSymbol();

        // call
        if (symbol == SYM_LPARENTHESIS) {
            getSymbol();

            gr_call(variableOrProcedureName);

            if (symbol == SYM_SEMICOLON)
                getSymbol();
            else
                syntaxErrorSymbol(SYM_SEMICOLON);

        // identifier = expression
        } else if (symbol == SYM_ASSIGN) {
            entry = getVariable(variableOrProcedureName);

            ltype = getType(entry);

            getSymbol();

            rtype = gr_expression();

            if (ltype != rtype)
                typeWarning(ltype, rtype);

            emitIFormat(OP_SW, getRegister(entry), allocatedRegisters, getData(entry));

            allocatedRegisters = allocatedRegisters - 1;

            if (symbol == SYM_SEMICOLON)
                getSymbol();
            else
                syntaxErrorSymbol(SYM_SEMICOLON);
        } else
            syntaxErrorUnexpected();
    }
    // while statement?
    else if (symbol == SYM_WHILE) {
        gr_while();
    }
    // if statement?
    else if (symbol == SYM_IF) {
        gr_if();
    }
    // return statement?
    else if (symbol == SYM_RETURN) {
        entry = getSymbolTableEntry(currentProcedureName, FUNCTION, global_symbol_table);

        gr_return(getType(entry));

        if (symbol == SYM_SEMICOLON)
            getSymbol();
        else
            syntaxErrorSymbol(SYM_SEMICOLON);
    }
}

void gr_variable(int offset) {
    int type;

    while (lookForVariable()) {
        syntaxErrorUnexpected();

        if (symbol == SYM_EOF)
            exit(-1);
        else
            getSymbol();
    }

    if (symbol == SYM_INT) {
        type = INT_T;

        getSymbol();

        if (symbol == SYM_ASTERISK) {
            type = INTSTAR_T;

            getSymbol();
        }

        if (symbol == SYM_IDENTIFIER) {
            createSymbolTableEntry(LOCAL_TABLE, identifier, offset, VARIABLE, type);

            getSymbol();
        } else {
            syntaxErrorSymbol(SYM_IDENTIFIER);

            createSymbolTableEntry(LOCAL_TABLE, (int*) "missing variable name", offset, VARIABLE, type);
        }
    } else
        syntaxErrorUnexpected();
}

void gr_procedure(int *procedure, int returnType) {
    int numberOfParameters;
    int parameters;
    int localVariables;
    int functionStart;
    int *entry;

    currentProcedureName = procedure;

    numberOfParameters = 0;

    // ( variable , variable ) ;
    if (symbol == SYM_LPARENTHESIS) {
        getSymbol();

        if (symbol != SYM_RPARENTHESIS) {
            gr_variable(0);

            numberOfParameters = 1;

            while (symbol == SYM_COMMA) {
                getSymbol();

                gr_variable(0);

                numberOfParameters = numberOfParameters + 1;
            }

            entry = local_symbol_table;

            parameters = 0;

            while (parameters < numberOfParameters) {
                // 8 bytes offset to skip frame pointer and link
                setData(entry, parameters * 4 + 8);

                parameters = parameters + 1;
                entry      = getNext(entry);
            }

            if (symbol == SYM_RPARENTHESIS)
                getSymbol();
            else
                syntaxErrorSymbol(SYM_RPARENTHESIS);
        } else
            getSymbol();
    } else
        syntaxErrorSymbol(SYM_LPARENTHESIS);

    if (symbol == SYM_SEMICOLON) {
        entry = getSymbolTableEntry(currentProcedureName, FUNCTION, global_symbol_table);

        if (entry == (int*) 0)
            createSymbolTableEntry(GLOBAL_TABLE, currentProcedureName, 0, FUNCTION, returnType);

        getSymbol();

    // ( variable, variable ) { variable; variable; statement }
    } else if (symbol == SYM_LBRACE) {
        functionStart = binaryLength;
        
        getSymbol();

        entry = getSymbolTableEntry(currentProcedureName, FUNCTION, global_symbol_table);

        if (entry == (int*) 0)
            createSymbolTableEntry(GLOBAL_TABLE, currentProcedureName, binaryLength, FUNCTION, returnType);
        else {
            if (getData(entry) != 0) {
                if (getOpcode(*(memory + getData(entry))) == OP_JAL)
                    fixlink_absolute(getData(entry), functionStart);
                else {
                    printLineNumber((int*) "error");

                    print((int*) "multiple definitions of ");

                    print(currentProcedureName);

                    println();
                }
            }

            setData(entry, functionStart);

            if (getType(entry) != returnType)
                typeWarning(getType(entry), returnType);

            setType(entry, returnType);
        }

        localVariables = 0;

        while (symbol == SYM_INT) {
            localVariables = localVariables + 1;

            gr_variable(-4 * localVariables);

            if (symbol == SYM_SEMICOLON)
                getSymbol();
            else
                syntaxErrorSymbol(SYM_SEMICOLON);
        }

        help_procedure_prologue(localVariables);

        // create a fixup chain for return statements
        returnBranches = 0;

        while (isNotRbraceOrEOF())
            gr_statement();

        if (symbol == SYM_RBRACE)
            getSymbol();
        else {
            syntaxErrorSymbol(SYM_RBRACE);

            exit(-1);
        }

        fixlink_absolute(returnBranches, binaryLength);

        returnBranches = 0;

        help_procedure_epilogue(numberOfParameters, functionStart, returnType);

    } else
        syntaxErrorUnexpected();

    local_symbol_table = (int*) 0;

    // assert: allocatedRegisters == 0
}

void gr_cstar() {
    int type;
    int *variableOrProcedureName;

    while (isVariableOrProcedure()) {
        // type identifier
        if (symbol == SYM_INT) {
            type = INT_T;

            getSymbol();

            if (symbol == SYM_ASTERISK) {
                type = INTSTAR_T;

                getSymbol();
            }

            if (symbol == SYM_IDENTIFIER) {
                variableOrProcedureName = identifier;

                getSymbol();

                // type identifier ; global variable declaration
                if (symbol == SYM_SEMICOLON) {
                    getSymbol();

                    allocatedMemory = allocatedMemory + 1;

                    createSymbolTableEntry(GLOBAL_TABLE, variableOrProcedureName, -4 * allocatedMemory, VARIABLE, type);
                }
                // type identifier ( procedure declaration or definition
                else
                    gr_procedure(variableOrProcedureName, type);
            } else
                syntaxErrorSymbol(SYM_IDENTIFIER);

        // void identifier procedure
        } else if (symbol == SYM_VOID) {
            type = VOID_T;

            getSymbol();

            if (symbol == SYM_IDENTIFIER) {
                variableOrProcedureName = identifier;

                getSymbol();

                gr_procedure(variableOrProcedureName, type);
            }
        } else
            syntaxErrorUnexpected();
    }

    // we do not expect any more unparsed code but
    // if there is some it is a syntax error
    if (symbol != SYM_EOF)
        syntaxErrorUnexpected();
}

// -----------------------------------------------------------------
// ------------------------ MACHINE CODE LIBRARY -------------------
// -----------------------------------------------------------------

void emitLeftShiftBy(int b) {
    // assert: 0 <= b < 15

    // load multiplication factor less than 2^15 to avoid sign extension
    emitIFormat(OP_ADDIU, REG_ZR, allocatedRegisters+1, twoToThePowerOf(b));
    emitRFormat(OP_SPECIAL, allocatedRegisters, allocatedRegisters+1, 0, FCT_MULTU);
    emitRFormat(OP_SPECIAL, 0, 0, allocatedRegisters, FCT_MFLO);
}

void emitMainEntry() {
    // instruction at address zero cannot be fixed up
    emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_NOP);

    createSymbolTableEntry(GLOBAL_TABLE, (int*) "main", binaryLength, FUNCTION, INT_T);

    mainJumpAddress = binaryLength;

    emitJFormat(OP_JAL, 0);
}

// -----------------------------------------------------------------
// ----------------------------- MAIN ------------------------------
// -----------------------------------------------------------------

int main_compiler() {
    initScanner();
    initSymbolTable();
    initParser();

    // memory in bytes and executable file name "out"
    initMemory(maxBinaryLength * 4, (int*) "out");

    getSymbol();

    // jump to main
    emitMainEntry();

    // library:
    // exit must be first to exit main
    // if exit call in main is missing
    emitExit();
    emitRead();
    emitWrite();
    emitOpen();
    emitMalloc();
    emitGetchar();
    emitPutchar();

    // parser
    gr_cstar();

    if (getInstrIndex(*(memory + mainJumpAddress)) != 0)
        emitBinary();
    else {
        print((int*) "cstarc: main function missing");
        println();
    }

    exit(0);
}

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// -------------------     I N T E R F A C E     -------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ---------------------------- ENCODER ----------------------------
// -----------------------------------------------------------------

// -----------------------------------------------------------------
// 32 bit
//
// +------+-----+-----+-----+-----+------+
// |opcode|  rs |  rt |  rd |00000|fction|
// +------+-----+-----+-----+-----+------+
//    6      5     5     5     5     6
int encodeRFormat(int opcode, int rs, int rt, int rd, int function) {
    // assert: 0 <= opcode < 2^6
    // assert: 0 <= rs < 2^5
    // assert: 0 <= rt < 2^5
    // assert: 0 <= rd < 2^5
    // assert: 0 <= function < 2^6
    return leftShift(leftShift(leftShift(leftShift(opcode, 5) + rs, 5) + rt, 5) + rd, 11) + function;
}

// -----------------------------------------------------------------
// 32 bit
//
// +------+-----+-----+----------------+
// |opcode|  rs |  rt |    immediate   |
// +------+-----+-----+----------------+
//    6      5     5          16
int encodeIFormat(int opcode, int rs, int rt, int immediate) {
    // assert: 0 <= opcode < 2^6
    // assert: 0 <= rs < 2^5
    // assert: 0 <= rt < 2^5
    // assert: -2^15 <= immediate < 2^15
    if (immediate < 0)
        // convert from 32-bit to 16-bit two's complement
        immediate = immediate + twoToThePowerOf(16);

    return leftShift(leftShift(leftShift(opcode, 5) + rs, 5) + rt, 16) + immediate;
}

// --------------------------------------------------------------
// 32 bit
//
// +------+--------------------------+
// |opcode|       instr_index        |
// +------+--------------------------+
//    6                26
int encodeJFormat(int opcode, int instr_index) {
    // assert: 0 <= opcode < 2^6
    // assert: 0 <= immediate < 2^26
    return leftShift(opcode, 26) + instr_index;
}

int getOpcode(int instruction) {
    return rightShift(instruction, 26);
}

int getRS(int instruction) {
    return rightShift(leftShift(instruction, 6), 27);
}

int getRT(int instruction) {
    return rightShift(leftShift(instruction, 11), 27);
}

int getRD(int instruction) {
    return rightShift(leftShift(instruction, 16), 27);
}

int getFunction(int instruction) {
    return rightShift(leftShift(instruction, 26), 26);
}

int getImmediate(int instruction) {
    return rightShift(leftShift(instruction, 16), 16);
}

int getInstrIndex(int instruction) {
    return rightShift(leftShift(instruction, 6), 6);
}

int signExtend(int immediate) {
    // sign-extend from 16-bit to 32-bit two's complement
    if (immediate < twoToThePowerOf(15))
        return immediate;
    else
        return immediate - twoToThePowerOf(16);
}

// -----------------------------------------------------------------
// ---------------------------- DECODER ----------------------------
// -----------------------------------------------------------------

void decode() {
    opcode = getOpcode(ir);

    if (opcode == 0)
        decodeRFormat();
    else if (opcode == OP_JAL)
        decodeJFormat();
    else if (opcode == OP_J)
        decodeJFormat();
    else
        decodeIFormat();
}

// --------------------------------------------------------------
// 32 bit
//
// +------+-----+-----+-----+-----+------+
// |opcode|  rs |  rt |  rd |00000|fction|
// +------+-----+-----+-----+-----+------+
//    6      5     5     5     5     6
void decodeRFormat() {
    rs          = getRS(ir);
    rt          = getRT(ir);
    rd          = getRD(ir);
    immediate   = 0;
    function    = getFunction(ir);
    instr_index = 0;
}

// --------------------------------------------------------------
// 32 bit
//
// +------+-----+-----+----------------+
// |opcode|  rs |  rt |    immediate   |
// +------+-----+-----+----------------+
//    6      5     5          16
void decodeIFormat() {
    rs          = getRS(ir);
    rt          = getRT(ir);
    rd          = 0;
    immediate   = getImmediate(ir);
    function    = 0;
    instr_index = 0;
}

// --------------------------------------------------------------
// 32 bit
//
// +------+--------------------------+
// |opcode|       instr_index        |
// +------+--------------------------+
//    6                26
void decodeJFormat() {
    rs          = 0;
    rt          = 0;
    rd          = 0;
    immediate   = 0;
    function    = 0;
    instr_index = getInstrIndex(ir);
}

// -----------------------------------------------------------------
// ---------------------------- BINARY -----------------------------
// -----------------------------------------------------------------

void emitInstruction(int instruction) {
    if (binaryLength >= maxBinaryLength) {
        syntaxErrorMessage((int*) "exceeded maximum binary length");
        exit(-1);
    } else {
        *(memory + binaryLength) = instruction;
        
        binaryLength = binaryLength + 1;
    }
}

void emitRFormat(int opcode, int rs, int rt, int rd, int function) {
    emitInstruction(encodeRFormat(opcode, rs, rt, rd, function));

    if (opcode == OP_SPECIAL) {
        if (function == FCT_JR)
            emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_NOP); // delay slot
        else if (function == FCT_MFLO) {
            // In MIPS I-III two instructions after MFLO/MFHI
            // must not modify the LO/HI registers
            emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_NOP); // pipeline delay
            emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_NOP); // pipeline delay
        } else if (function == FCT_MFHI) {
            emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_NOP); // pipeline delay
            emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_NOP); // pipeline delay
        }
    }
}

void emitIFormat(int opcode, int rs, int rt, int immediate) {
    emitInstruction(encodeIFormat(opcode, rs, rt, immediate));

    if (opcode == OP_BEQ)
        emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_NOP); // delay slot
    else if (opcode == OP_BNE)
        emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_NOP); // delay slot
}

void emitJFormat(int opcode, int instr_index) {
    emitInstruction(encodeJFormat(opcode, instr_index));

    emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_NOP); // delay slot
}

void fixup_relative(int fromAddress) {
    *(memory + fromAddress) = encodeIFormat(
        getOpcode(*(memory + fromAddress)),
        getRS(*(memory + fromAddress)),
        getRT(*(memory + fromAddress)),
        binaryLength - fromAddress - 1);
}

void fixup_absolute(int fromAddress, int toAddress) {
    *(memory + fromAddress) =
        encodeJFormat(getOpcode(*(memory + fromAddress)), toAddress);
}

void fixlink_absolute(int fromAddress, int toAddress) {
    int previousAddress;

    while (fromAddress != 0) {
        previousAddress = getInstrIndex(*(memory + fromAddress));
        fixup_absolute(fromAddress, toAddress);
        fromAddress = previousAddress;
    }
}

int copyStringToMemory(int *s, int a) {
    int l;
    int w;

    l = stringLength(s) + 1;

    w = a + l / 4;

    if (l % 4 != 0)
        w = w + 1;

    while (a < w) {
        *(memory + a) = *s;

        s = s + 1;
        a = a + 1;
    }

    return w;
}

void emitBinary() {
    int *entry;
    int fd;

    entry = global_symbol_table;

    // allocate space for global variables and copy strings
    while ((int) entry != 0) {
        if (getClass(entry) == VARIABLE) {
            *(memory + binaryLength) = 0;

            binaryLength = binaryLength + 1;
        } else if (getClass(entry) == STRING)
            binaryLength = copyStringToMemory(getString(entry), binaryLength);

        entry = getNext(entry);
    }

    // assert: file with name binaryName exists prior to execution of compiler
    fd = open(binaryName, 1); // 1 = O_WRONLY

    if (fd < 0) {
        syntaxErrorMessage((int*) "output file not found");
        exit(-1);
    }

    // The mipster_syscall 4004 writes the code array into a file.
    // The syscall uses the "write" system call of the OS and compiler.
    // The write system call of our Linux uses little endian byte ordering.
    write(fd, memory, binaryLength * 4);
}

void loadBinary() {
    int fd;
    int numberOfReadBytes;

    fd = open(binaryName, 0); // 0 = O_RDONLY

    if (fd < 0)
        exit(-1);

    numberOfReadBytes = 4;

    while (numberOfReadBytes == 4) {
        numberOfReadBytes = read(fd, memory + binaryLength, 4);

        if (debug_load) {
            print(binaryName);
            print((int*) ": ");
            print(itoa(binaryLength * 4, string_buffer, 16, 8));
            print((int*) ": ");
            print(itoa(*(memory+binaryLength), string_buffer, 16, 8));
            println();
        }

        if (numberOfReadBytes == 4)
            binaryLength = binaryLength + 1;
    }
}

// -----------------------------------------------------------------
// --------------------------- SYSCALLS ----------------------------
// -----------------------------------------------------------------

void emitExit() {
    createSymbolTableEntry(GLOBAL_TABLE, (int*) "exit", binaryLength, FUNCTION, INT_T);

    emitIFormat(OP_ADDIU, REG_ZR, REG_A3, 0);
    emitIFormat(OP_ADDIU, REG_ZR, REG_A2, 0);
    emitIFormat(OP_ADDIU, REG_ZR, REG_A1, 0);

    emitIFormat(OP_LW, REG_SP, REG_A0, 0); // exit code
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_ADDIU, REG_ZR, REG_V0, SYSCALL_EXIT);
    emitRFormat(0, 0, 0, 0, FCT_SYSCALL);
}

void syscall_exit() {
    int exitCode;

    exitCode = *(registers+REG_A0);

    *(registers+REG_V0) = exitCode;

    print(binaryName);
    print((int*) ": exiting with error code ");
    print(itoa(exitCode, string_buffer, 10, 0));
    println();

    exit(0);
}

void emitRead() {
    createSymbolTableEntry(GLOBAL_TABLE, (int*) "read", binaryLength, FUNCTION, INT_T);

    emitIFormat(OP_ADDIU, REG_ZR, REG_A3, 0);

    emitIFormat(OP_LW, REG_SP, REG_A2, 0); // count
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_LW, REG_SP, REG_A1, 0); // *buffer
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_LW, REG_SP, REG_A0, 0); // fd
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_ADDIU, REG_ZR, REG_V0, SYSCALL_READ);
    emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_SYSCALL);

    emitIFormat(OP_ADDIU, REG_V0, REG_RR, 0);

    emitRFormat(OP_SPECIAL, REG_LINK, 0, 0, FCT_JR);
}

void syscall_read() {
    int count;
    int address;
    int fd;
    int *buffer;
    int size;

    count   = *(registers+REG_A2);
    address = *(registers+REG_A1) / 4;
    fd      = *(registers+REG_A0);

    buffer = memory + address;

    size = read(fd, buffer, count);

    *(registers+REG_V0) = size;

    if (debug_read) {
        print(binaryName);
        print((int*) ": read ");
        print(itoa(size, string_buffer, 10, 0));
        print((int*) " bytes from file with descriptor ");
        print(itoa(fd, string_buffer, 10, 0));
        print((int*) " into buffer at address ");
        print(itoa((int) buffer, string_buffer, 16, 8));
        println();
    }
}

void emitWrite() {
    createSymbolTableEntry(GLOBAL_TABLE, (int*) "write", binaryLength, FUNCTION, INT_T);

    emitIFormat(OP_ADDIU, REG_ZR, REG_A3, 0);

    emitIFormat(OP_LW, REG_SP, REG_A2, 0); // size
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_LW, REG_SP, REG_A1, 0); // *buffer
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_LW, REG_SP, REG_A0, 0); // fd
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_ADDIU, REG_ZR, REG_V0, SYSCALL_WRITE);
    emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_SYSCALL);

    emitIFormat(OP_ADDIU, REG_V0, REG_RR, 0);

    emitRFormat(OP_SPECIAL, REG_LINK, 0, 0, FCT_JR);
}

void syscall_write() {
    int size;
    int address;
    int fd;
    int *buffer;

    size    = *(registers+REG_A2);
    address = *(registers+REG_A1) / 4;
    fd      = *(registers+REG_A0);

    buffer = memory + address;

    size = write(fd, buffer, size);

    *(registers+REG_V0) = size;

    if (debug_write) {
        print(binaryName);
        print((int*) ": wrote ");
        print(itoa(size, string_buffer, 10, 0));
        print((int*) " bytes from buffer at address ");
        print(itoa((int) buffer, string_buffer, 16, 8));
        print((int*) " into file with descriptor ");
        print(itoa(fd, string_buffer, 10, 0));
        println();
    }
}

void emitOpen() {
    createSymbolTableEntry(GLOBAL_TABLE, (int*) "open", binaryLength, FUNCTION, INT_T);

    emitIFormat(OP_ADDIU, REG_ZR, REG_A3, 0);
    emitIFormat(OP_ADDIU, REG_ZR, REG_A2, 0);

    emitIFormat(OP_LW, REG_SP, REG_A1, 0); // flags
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_LW, REG_SP, REG_A0, 0); // filename
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_ADDIU, REG_ZR, REG_V0, SYSCALL_OPEN);
    emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_SYSCALL);

    emitIFormat(OP_ADDIU, REG_V0, REG_RR, 0);

    emitRFormat(OP_SPECIAL, REG_LINK, 0, 0, FCT_JR);
}

void syscall_open() {
    int flags;
    int address;
    int *filename;
    int fd;

    flags   = *(registers+REG_A1);
    address = *(registers+REG_A0) / 4;

    filename = memory + address;

    fd = open(filename, flags);

    *(registers+REG_V0) = fd;

    if (debug_open) {
        print(binaryName);
        print((int*) ": opened file ");
        printString(filename);
        print((int*) " with flags ");
        print(itoa(flags, string_buffer, 10, 0));
        print((int*) " returning file descriptor ");
        print(itoa(fd, string_buffer, 10, 0));
        println();
    }
}

void emitMalloc() {
    createSymbolTableEntry(GLOBAL_TABLE, (int*) "malloc", binaryLength, FUNCTION, INTSTAR_T);

    emitIFormat(OP_ADDIU, REG_ZR, REG_A3, 0);
    emitIFormat(OP_ADDIU, REG_ZR, REG_A2, 0);
    emitIFormat(OP_ADDIU, REG_ZR, REG_A1, 0);

    // load argument for malloc (size)
    emitIFormat(OP_LW, REG_SP, REG_A0, 0);

    // remove the argument from the stack
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    // load the correct syscall number and invoke syscall
    emitIFormat(OP_ADDIU, REG_ZR, REG_V0, SYSCALL_MALLOC);
    emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_SYSCALL);

    // put return value into return register
    emitIFormat(OP_ADDIU, REG_V0, REG_RR, 0);

    // jump back to caller
    emitRFormat(OP_SPECIAL, REG_LINK, 0, 0, FCT_JR);
}

void syscall_malloc() {
    int size;
    int bump;

    size = *(registers+REG_A0);

    if (size % 4 != 0)
        size = size + (4 - size % 4);

    bump = *(registers+REG_K1);

    if (bump + size >= *(registers+REG_SP))
        exception_handler(EXCEPTION_HEAPOVERFLOW);

    *(registers+REG_K1) = bump + size;
    *(registers+REG_V0) = bump;

    if (debug_malloc) {
        print(binaryName);
        print((int*) ": malloc ");
        print(itoa(size, string_buffer, 10, 0));
        print((int*) " bytes returning address ");
        print(itoa(bump, string_buffer, 16, 8));
        println();
    }
}

void emitGetchar() {
    createSymbolTableEntry(GLOBAL_TABLE, (int*) "getchar", binaryLength, FUNCTION, INT_T);

    emitIFormat(OP_ADDIU, REG_ZR, REG_A3, 0);
    emitIFormat(OP_ADDIU, REG_ZR, REG_A2, 0);
    emitIFormat(OP_ADDIU, REG_ZR, REG_A1, 0);
    emitIFormat(OP_ADDIU, REG_ZR, REG_A0, 0);

    emitIFormat(OP_ADDIU, REG_ZR, REG_V0, SYSCALL_GETCHAR);
    emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_SYSCALL);

    emitIFormat(OP_ADDIU, REG_V0, REG_RR, 0);

    emitRFormat(OP_SPECIAL, REG_LINK, 0, 0, FCT_JR);
}

void syscall_getchar() {
    int c;

    c = getchar();

    *(registers+REG_V0) = c;

    if (debug_getchar) {
        print(binaryName);
        print((int*) ": getchar ");
        printCharacter(c);
        println();
    }
}

void emitPutchar() {
    createSymbolTableEntry(GLOBAL_TABLE, (int*) "putchar", binaryLength, FUNCTION, INT_T);

    emitIFormat(OP_ADDIU, REG_ZR, REG_A3, 0);

    emitIFormat(OP_ADDIU, REG_ZR, REG_A2, 4); // write one integer, 4 bytes

    emitIFormat(OP_ADDIU, REG_SP, REG_A1, 0); // pointer to character
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_ADDIU, REG_ZR, REG_A0, 1); // stdout file descriptor

    emitIFormat(OP_ADDIU, REG_ZR, REG_V0, SYSCALL_WRITE);
    emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_SYSCALL);

    emitRFormat(OP_SPECIAL, REG_LINK, 0, 0, FCT_JR);
}

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ---------------------     E M U L A T O R   ---------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ------------------------- INSTRUCTIONS --------------------------
// -----------------------------------------------------------------

void fct_syscall() {
    if (debug_disassemble) {
        print((int*) *(fct_strings+function));
        println();
    }

    if (*(registers+REG_V0) == SYSCALL_EXIT) {
        syscall_exit();
    } else if (*(registers+REG_V0) == SYSCALL_READ) {
        syscall_read();
    } else if (*(registers+REG_V0) == SYSCALL_WRITE) {
        syscall_write();
    } else if (*(registers+REG_V0) == SYSCALL_OPEN) {
        syscall_open();
    } else if (*(registers+REG_V0) == SYSCALL_MALLOC) {
        syscall_malloc();
    } else if (*(registers+REG_V0) == SYSCALL_GETCHAR) {
        syscall_getchar();
    } else {
        exception_handler(EXCEPTION_UNKNOWNSYSCALL);
    }

    pc = pc + 1;
}

void fct_nop() {
    pc = pc + 1;

    if (debug_disassemble) {
        print((int*) *(fct_strings+function));
        println();
    }
}

void op_jal() {
    *(registers+REG_LINK) = pc * 4 + 8;

    pc = instr_index;

    // TODO: execute delay slot

    if (debug_disassemble) {
        print((int*) *(op_strings+opcode));

        print((int*) " ");
        memset(string_buffer, 9, 0);
        print(itoa(instr_index, string_buffer, 16, 8));
        println();
    }
}

void op_j() {
    pc = instr_index;

    // TODO: execute delay slot

    if (debug_disassemble) {
        print((int*) *(op_strings+opcode));

        print((int*) " ");
        memset(string_buffer, 9, 0);
        print(itoa(instr_index, string_buffer, 16, 8));
        println();
    }
}

void op_beq() {
    pc = pc + 1;

    if (*(registers+rs) == *(registers+rt)) {
        pc = pc + signExtend(immediate);
        // TODO: execute delay slot
    }

    if (debug_disassemble) {
        print((int*) *(op_strings+opcode));

        print((int*) " ");
        print((int*) *(register_strings+rs));
        putchar(',');
        print((int*) *(register_strings+rt));
        putchar(',');
        print(itoa(signExtend(immediate), string_buffer, 10, 0));

        println();
    }
}

void op_bne() {
    pc = pc + 1;

    if (*(registers+rs) != *(registers+rt)) {
        pc = pc + signExtend(immediate);
        // TODO: execute delay slot
    }

    if (debug_disassemble) {
        print((int*) *(op_strings+opcode));

        print((int*) " ");
        print((int*) *(register_strings+rs));
        putchar(',');
        print((int*) *(register_strings+rt));
        putchar(',');
        print(itoa(signExtend(immediate), string_buffer, 10, 0));

        println();
    }
}

void op_addiu() {
    *(registers+rt) = *(registers+rs) + signExtend(immediate);

    // TODO: check for overflow

    pc = pc + 1;

    if (debug_disassemble) {
        print((int*) *(op_strings+opcode));

        print((int*) " ");
        print((int*) *(register_strings+rt));
        putchar(',');
        print((int*) *(register_strings+rs));
        putchar(',');
        print(itoa(signExtend(immediate), string_buffer, 10, 0));

        println();
    }
}

void fct_jr() {
    pc = *(registers+rs) / 4;

    if (debug_disassemble) {
        print((int*) *(fct_strings+function));

        print((int*) " ");
        print((int*) *(register_strings+rs));

        println();
    }
}

void op_lui() {
    *(registers+rt) = leftShift(immediate, 16);

    pc = pc + 1;

    if (debug_disassemble) {
        print((int*) *(op_strings+opcode));

        print((int*) " ");

        print((int*) *(register_strings+rt));
        putchar(',');
        print(itoa(signExtend(immediate), string_buffer, 10, 0));

        println();
    }
}

void fct_mfhi() {
    *(registers+rd) = reg_hi;

    pc = pc + 1;

    if (debug_disassemble) {
        print((int*) *(fct_strings+function));

        print((int*) " ");
        print((int*) *(register_strings+rd));

        println();
    }
}

void fct_mflo() {
    *(registers+rd) = reg_lo;

    pc = pc + 1;

    if (debug_disassemble) {
        print((int*) *(fct_strings+function));

        print((int*) " ");
        print((int*) *(register_strings+rd));

        println();
    }
}

void fct_multu() {
    // TODO: 64-bit resolution currently not supported
    reg_lo = *(registers+rs) * *(registers+rt);

    pc = pc + 1;

    if (debug_disassemble) {
        print((int*) *(fct_strings+function));

        print((int*) " ");
        print((int*) *(register_strings+rs));
        putchar(',');
        print((int*) *(register_strings+rt));

        println();
    }
}

void fct_divu() {
    reg_lo = *(registers+rs) / *(registers+rt);
    reg_hi = *(registers+rs) % *(registers+rt);

    pc = pc + 1;

    if (debug_disassemble) {
        print((int*) *(fct_strings+function));

        print((int*) " ");
        print((int*) *(register_strings+rs));
        putchar(',');
        print((int*) *(register_strings+rt));

        println();
    }
}

void fct_addu() {
    *(registers+rd) = *(registers+rs) + *(registers+rt);

    pc = pc + 1;

    if (debug_disassemble) {
        print((int*) *(fct_strings+function));

        print((int*) " ");
        print((int*) *(register_strings+rd));
        putchar(',');
        print((int*) *(register_strings+rs));
        putchar(',');
        print((int*) *(register_strings+rt));
        println();
    }
}

void fct_subu() {
    *(registers+rd) = *(registers+rs) - *(registers+rt);

    pc = pc + 1;

    if (debug_disassemble) {
        print((int*) *(fct_strings+function));

        print((int*) " ");
        print((int*) *(register_strings+rd));
        putchar(',');
        print((int*) *(register_strings+rs));
        putchar(',');
        print((int*) *(register_strings+rt));
        println();

    }
}

void op_lw() {
    int vaddr;
    int paddr;

    vaddr = *(registers+rs) + signExtend(immediate);

    paddr = addressTranslation(vaddr) / 4;

    *(registers+rt) = *(memory+paddr);

    pc = pc + 1;

    if (debug_disassemble) {
        print((int*) *(op_strings+opcode));

        print((int*) " ");
        print((int*) *(register_strings+rt));
        putchar(',');
        print(itoa(signExtend(immediate), string_buffer, 10, 0));

        putchar('(');
        print((int*) *(register_strings+rs));
        putchar(')');

        println();
    }
}

void fct_slt() {
    if (*(registers+rs) < *(registers+rt))
        *(registers+rd) = 1;
    else
        *(registers+rd) = 0;

    pc = pc + 1;

    if (debug_disassemble) {
        print((int*) *(fct_strings+function));

        print((int*) " ");
        print((int*) *(register_strings+rd));
        putchar(',');
        print((int*) *(register_strings+rs));
        putchar(',');
        print((int*) *(register_strings+rt));

        println();
    }
}

void op_sw() {
    int vaddr;
    int paddr;
    int tmp;

    vaddr = *(registers+rs) + signExtend(immediate);

    paddr = addressTranslation(vaddr) / 4;

    *(memory+paddr) = *(registers+rt);

    pc = pc + 1;

    if (debug_disassemble) {
        print((int*) *(op_strings+opcode));

        print((int*) " ");
        print((int*) *(register_strings+rt));
        putchar(',');
        print(itoa(signExtend(immediate), string_buffer, 10, 0));

        putchar('(');
        print((int*) *(register_strings+rs));
        putchar(')');

        println();
    }
}

void fct_teq() {
    if (*(registers+rs) == *(registers+rt))
        exception_handler(EXCEPTION_SIGNAL);

    pc = pc + 1;

    if (debug_disassemble) {
        print((int*) *(fct_strings+function));

        print((int*) " ");
        print((int*) *(register_strings+rs));
        putchar(',');
        print((int*) *(register_strings+rt));

        println();
    }
}

// -----------------------------------------------------------------
// -------------------------- INTERPRETER --------------------------
// -----------------------------------------------------------------

void exception_handler(int enumber) {
    if (enumber == EXCEPTION_SIGNAL) {
        exit(EXCEPTION_SIGNAL);
    } else if (enumber == EXCEPTION_ADDRESSERROR) {
        exit(EXCEPTION_ADDRESSERROR);
    } else if (enumber == EXCEPTION_UNKNOWNINSTRUCTION) {
        exit(EXCEPTION_UNKNOWNINSTRUCTION);
    } else if (enumber == EXCEPTION_HEAPOVERFLOW) {
        exit(EXCEPTION_HEAPOVERFLOW);
    } else if (enumber == EXCEPTION_UNKNOWNSYSCALL) {
        exit(EXCEPTION_UNKNOWNSYSCALL);
    } else if (enumber == EXCEPTION_UNKNOWNFUNCTION) {
        exit(EXCEPTION_UNKNOWNFUNCTION);
    }
}

int addressTranslation(int vaddr) {
    if (vaddr % 4 != 0)
        exception_handler(EXCEPTION_ADDRESSERROR);

    return vaddr;
}

void pre_debug() {
    if (debug_disassemble) {
        memset(string_buffer, 9, 0);           // print current PC
        print(itoa(4 * pc, string_buffer, 16, 8));
        putchar(CHAR_TAB);
    }
}

void post_debug() {
    int i;
    if (debug_registers) {
        i = 0;

        while (i < 32) {
            if (*(registers+i) != 0) {
                print((int*) *(register_strings+i));
                putchar(CHAR_TAB);
                memset(string_buffer, 9, 0);
                print(itoa(*(registers+i), string_buffer, 16, 8));

                println();
            }
            i = i + 1;
        }
        println();
    }
}

void fetch() {
    ir = *(memory+pc);
}

void execute() {
    if (opcode == OP_SPECIAL) {
        if (function == FCT_NOP) {
            fct_nop();
        } else if (function == FCT_ADDU) {
            fct_addu();
        } else if (function == FCT_SUBU) {
            fct_subu();
        } else if (function == FCT_MULTU) {
            fct_multu();
        } else if (function == FCT_DIVU) {
            fct_divu();
        } else if (function == FCT_MFHI) {
            fct_mfhi();
        } else if (function == FCT_MFLO) {
            fct_mflo();
        } else if (function == FCT_SLT) {
            fct_slt();
        } else if (function == FCT_JR) {
            fct_jr();
        } else if (function == FCT_SYSCALL) {
            fct_syscall();
        } else if (function == FCT_TEQ) {
            fct_teq();
        } else {
            exception_handler(EXCEPTION_UNKNOWNINSTRUCTION);
        }
    } else if (opcode == OP_ADDIU) {
        op_addiu();
    } else if (opcode == OP_LW) {
        op_lw();
    } else if (opcode == OP_SW) {
        op_sw();
    } else if (opcode == OP_BEQ) {
        op_beq();
    } else if (opcode == OP_BNE) {
        op_bne();
    } else if (opcode == OP_JAL) {
        op_jal();
    } else if (opcode == OP_J) {
        op_j();
    } else {
        exception_handler(EXCEPTION_UNKNOWNINSTRUCTION);
    }
}

void run() {

    while (1) {
        fetch();
        decode();
        pre_debug();
        execute();
        post_debug();
    }
}

void parse_args(int argc, int *argv) {
    // assert: ./selfie -m size executable {-m size executable}

    // memory size in bytes and executable file name
    initMemory(atoi((int*) *(argv+2)) * 1024 * 1024, (int*) *(argv+3));

    // initialize stack pointer
    *(registers+REG_SP) = memorySize - 4;

    print(binaryName);
    print((int*) ": memory size ");
    print(itoa(memorySize / 1024 / 1024, string_buffer, 10, 0));
    print((int*) "MB");
    println();
}

void up_push(int value) {
    int address;

    // allocate space for one value on the stack
    *(registers+REG_SP) = *(registers+REG_SP) - 4;

    // compute address
    address = *(registers+REG_SP) / 4;

    // store value
    *(memory + address) = value;
}

int up_malloc(int size) {
    *(registers+REG_A0) = size;

    syscall_malloc();

    return *(registers+REG_V0);
}

void up_copyArguments(int argc, int *argv) {
    int address;

    up_push(argc);

    address = up_malloc(argc * 4);

    up_push(address);

    address = address / 4;

    while (argc > 0) {
        *(memory + address) = up_malloc(stringLength((int*) *argv) + 1);

        copyStringToMemory((int*) *argv, *(memory + address) / 4);

        address = address + 1;

        argv = argv + 1;
        argc = argc - 1;
    }
}

int main_emulator(int argc, int *argv) {
    initInterpreter();

    parse_args(argc, argv);

    loadBinary();

    *(registers+REG_GP) = binaryLength * 4;

    *(registers+REG_K1) = *(registers+REG_GP);

    up_copyArguments(argc-3, argv+3);

    run();

    exit(0);
}

// -----------------------------------------------------------------
// ----------------------------- MAIN ------------------------------
// -----------------------------------------------------------------

int main(int argc, int *argv) {
    int *firstParameter;

    initLibrary();

    initRegister();
    initDecoder();
    initSyscalls();

    if (argc > 1) {
        firstParameter = (int*) *(argv+1);

        if (getCharacter(firstParameter, 0) == '-') {
            if (getCharacter(firstParameter, 1) == 'c')
                main_compiler();
            else if (getCharacter(firstParameter, 1) == 'm') {
                if (argc > 3)
                    main_emulator(argc, (int*) argv);
                else
                    exit(-1);
            }
            else {
                exit(-1);
            }
        } else {
            exit(-1);
        }
    } else
        // default: compiler
        main_compiler();
}