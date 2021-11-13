#ifndef STAGE1_H
#define STAGE1_H
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <stack>
using namespace std;
const char END_OF_FILE = '$'; // arbitrary choice
enum storeTypes {INTEGER, BOOLEAN, PROG_NAME, UNKNOWN};
enum modes {VARIABLE, CONSTANT};
enum allocation {YES, NO};
class SymbolTableEntry
{
public:
 SymbolTableEntry(string in, storeTypes st, modes m,
 string v, allocation a, int u)
 {
 setInternalName(in);
 setDataType(st);
 setMode(m);
 setValue(v);
 setAlloc(a);
 setUnits(u);
 }
 string getInternalName() const
 {
 return internalName;
 }
 storeTypes getDataType() const
 {
 return dataType;
 }
 modes getMode() const
 {
 return mode;
 }
 string getValue() const
 {
 return value;
 }
 allocation getAlloc() const
 {
 return alloc;
Overall Compiler Structure – Stage 1 Page 13
 }
 int getUnits() const
 {
 return units;
 }
 void setInternalName(string s)
 {
 internalName = s;
 }
 void setDataType(storeTypes st)
 {
 dataType = st;
 }
 void setMode(modes m)
 {
 mode = m;
 }
 void setValue(string s)
 {
 value = s;
 }
 void setAlloc(allocation a)
 {
 alloc = a;
 }
 void setUnits(int i)
 {
 units = i;
 }
private:
 string internalName;
 storeTypes dataType;
 modes mode;
 string value;
 allocation alloc;
 int units;
};
class Compiler
{
public:
 Compiler(char **argv); // constructor
 ~Compiler(); // destructor
 void createListingHeader();
 void parser();
 void createListingTrailer();
 // Methods implementing the grammar productions
Page 14 Overall Compiler Structure – Stage 1
 void prog(); // stage 0, production 1
 void progStmt(); // stage 0, production 2
 void consts(); // stage 0, production 3
 void vars(); // stage 0, production 4
 void beginEndStmt(); // stage 0, production 5
 void constStmts(); // stage 0, production 6
 void varStmts(); // stage 0, production 7
 string ids(); // stage 0, production 8
 void execStmts(); // stage 1, production 2
 void execStmt(); // stage 1, production 3
 void assignStmt(); // stage 1, production 4
 void readStmt(); // stage 1, production 5
 void writeStmt(); // stage 1, production 7
 void express(); // stage 1, production 9
 void expresses(); // stage 1, production 10
 void term(); // stage 1, production 11
 void terms(); // stage 1, production 12
 void factor(); // stage 1, production 13
 void factors(); // stage 1, production 14
 void part(); // stage 1, production 15
 // Helper functions for the Pascallite lexicon
 bool isKeyword(string s) const; // determines if s is a keyword
 bool isSpecialSymbol(char c) const; // determines if c is a special symbol
 bool isNonKeyId(string s) const; // determines if s is a non_key_id
 bool isInteger(string s) const; // determines if s is an integer
 bool isBoolean(string s) const; // determines if s is a boolean
 bool isLiteral(string s) const; // determines if s is a literal
 // Action routines
 void insert(string externalName, storeTypes inType, modes inMode,
 string inValue, allocation inAlloc, int inUnits);
 storeTypes whichType(string name); // tells which data type a name has
 string whichValue(string name); // tells which value a name has
 void code(string op, string operand1 = "", string operand2 = "");
 void pushOperator(string op);
 string popOperator();
 void pushOperand(string operand);
 string popOperand();
 // Emit Functions
 void emit(string label = "", string instruction = "", string operands = "",
 string comment = "");
 void emitPrologue(string progName, string = "");
 void emitEpilogue(string = "", string = "");
 void emitStorage();
 void emitReadCode(string operand, string = "");
 void emitWriteCode(string operand, string = "");
 void emitAssignCode(string operand1, string operand2); // op2 = op1
 void emitAdditionCode(string operand1, string operand2); // op2 + op1
 void emitSubtractionCode(string operand1, string operand2); // op2 - op1
 void emitMultiplicationCode(string operand1, string operand2); // op2 * op1
 void emitDivisionCode(string operand1, string operand2); // op2 / op1
 void emitModuloCode(string operand1, string operand2); // op2 % op1
 void emitNegationCode(string operand1, string = ""); // -op1
 void emitNotCode(string operand1, string = ""); // !op1
Overall Compiler Structure – Stage 1 Page 15
 void emitAndCode(string operand1, string operand2); // op2 && op1
 void emitOrCode(string operand1, string operand2); // op2 || op1
 void emitEqualityCode(string operand1, string operand2); // op2 == op1
 void emitInequalityCode(string operand1, string operand2); // op2 != op1
 void emitLessThanCode(string operand1, string operand2); // op2 < op1
 void emitLessThanOrEqualToCode(string operand1, string operand2); // op2 <=
op1
 void emitGreaterThanCode(string operand1, string operand2); // op2 > op1
 void emitGreaterThanOrEqualToCode(string operand1, string operand2); // op2
>= op1
 // Lexical routines
 char nextChar(); // returns the next character or END_OF_FILE marker
 string nextToken(); // returns the next token or END_OF_FILE marker
 // Other routines
 string genInternalName(storeTypes stype) const;
 void processError(string err);
 void freeTemp();
 string getTemp();
 string getLabel();
 bool isTemporary(string s) const; // determines if s represents a temporary
private:
 map<string, SymbolTableEntry> symbolTable;
 ifstream sourceFile;
 ofstream listingFile;
 ofstream objectFile;
 string token; // the next token
 char ch; // the next character of the source file
 uint errorCount = 0; // total number of errors encountered
 uint lineNo = 0; // line numbers for the listing
 stack<string> operatorStk; // operator stack
 stack<string> operandStk; // operand stack
 int currentTempNo = -1; // current temp number
 int maxTempNo = -1; // max temp number
 string contentsOfAReg; // symbolic contents of A register
};
#endif