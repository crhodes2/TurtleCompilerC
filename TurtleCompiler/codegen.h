// codegen.h
// pete myers
// OIT Fall 2008

#ifndef _CODEGEN_H_
#define _CODEGEN_H_

#include "tree.h"

typedef enum
{
	REGISTER_P1 = 0x00,
	REGISTER_P2 = 0x01,
	REGISTER_RE = 0x02,
	REGISTER_G1 = 0x03,
	REGISTER_G2 = 0x04,
	REGISTER_G3 = 0x05,
	REGISTER_G4 = 0x06,
	REGISTER_ST = 0x07,
	REGISTER_PC = 0x08,
	REGISTER_FL = 0x09
} REGISTER;

typedef enum
{
	FLAGS_ZERO    = 0x0001,
	FLAGS_EQUAL   = 0x0002,
	FLAGS_GREATER = 0x0004
} FLAGS;

typedef enum
{
	OPCODE_LOAD_R = 0x00,
	OPCODE_LOAD_V = 0x01,
	OPCODE_MOVE_RR = 0x10,
	OPCODE_MOVE_RV = 0x11,
	OPCODE_MOVE_VR = 0x12,
	OPCODE_MOVE_VV = 0x13,
	OPCODE_JMPRe = 0x20,
	OPCODE_JMPTo = 0x21,
	OPCODE_JEq = 0x22,
	OPCODE_JNe = 0x23,
	OPCODE_JGt = 0x24,
	OPCODE_JLt = 0x25,
	OPCODE_JZe = 0x26,
	OPCODE_JNz = 0x27,
	OPCODE_CMP_RR = 0x30,
	OPCODE_CMP_RV = 0x31,
	OPCODE_CMP_VR = 0x32,
	OPCODE_CMP_VV = 0x33,
	OPCODE_INC_R = 0x40,
	OPCODE_DEC_R = 0x41,
	OPCODE_ADD_R = 0x42,
	OPCODE_ADD_V = 0x43,
	OPCODE_SUB_R = 0x44,
	OPCODE_SUB_V = 0x45,
	OPCODE_MUL_R = 0x46,
	OPCODE_MUL_V = 0x47,
	OPCODE_DIV_R = 0x48,
	OPCODE_DIV_V = 0x49,
	OPCODE_PUSH_R = 0x50,
	OPCODE_POP_R = 0x51, 
	OPCODE_PEEK_R = 0x52,
	OPCODE_TURTLE = 0x60,
	OPCODE_EXIT = 0xFF
} OPCODE;

typedef enum
{
	TURTLE_OPERATION_HOME = 0x00,    // no params
	TURTLE_OPERATION_FD = 0x01,      // p1 is distance
	TURTLE_OPERATION_BK = 0x02,      // p1 is distance
	TURTLE_OPERATION_RT = 0x03,      // p1 is degrees to turn
	TURTLE_OPERATION_LT = 0x04,      // p1 is degrees to turn
	TURTLE_OPERATION_SETC = 0x05,    // p1 is color
	TURTLE_OPERATION_COLOR = 0x06,   // sets re to current color
	TURTLE_OPERATION_SETXY = 0x07,   // p1 is x, p2 is y
	TURTLE_OPERATION_XCOR = 0x08,    // sets re to current x
	TURTLE_OPERATION_YCOR = 0x09,    // sets re to current y
	TURTLE_OPERATION_SETH = 0x0A,    // p1 is degrees
	TURTLE_OPERATION_PD = 0x0B,      // no params
	TURTLE_OPERATION_PU = 0x0C,      // no params
	TURTLE_OPERATION_RANDOM = 0x0D,  // sets re to random number between 1 and p1
	TURTLE_OPERATION_SETX = 0x0E,    // p1 is new x-coord
	TURTLE_OPERATION_SETY = 0x0F,    // p1 is new y-coord
	TURTLE_OPERATION_HT = 0x10,      // no params
	TURTLE_OPERATION_ST = 0x11,      // no params
	TURTLE_OPERATION_HEADING = 0x12  // sets re to current heading
} TURTLE_OPERATION;

typedef enum
{
	COLOR_BLACK = 0x00,
	COLOR_WHITE = 0x01,
	COLOR_ORANGE = 0x02,
	COLOR_YELLOW = 0x03,
	COLOR_LIME = 0x04,
	COLOR_CYAN = 0x05,
	COLOR_BLUE = 0x06,
	COLOR_MAGENTA = 0x07,
	COLOR_RED = 0x08,
	COLOR_BROWN = 0x09,
	COLOR_GREEN = 0x0A,
	COLOR_TURQUOISE = 0x0B,
	COLOR_SKY = 0x0C,
	COLOR_VIOLET = 0x0D,
	COLOR_PINK = 0x0E
} COLORS;


//
// C++ Version Jan 2017
//


class TurtleProgram
{
private:
	int stackSize;
	unsigned char * program;

public:
	// constants
	static const int MAX_PROGRAM = 0x10000;
	static const int DEFAULT_STACK_SIZE = 0x00010;

	TurtleProgram(int stackSize = DEFAULT_STACK_SIZE);
	~TurtleProgram();

	void Generate(BlockTreeNode * root);
	int GetSize();
	void PrintProgram(FILE * f);
	void WriteBinary(FILE * f);

private:
	int GenerateStaticVariables(int pc);
	int PrintOpcode(FILE * f, int i);
	char * RegisterName(REGISTER r);
	char * TurtleOperationString(TURTLE_OPERATION t);
	
public:
	// generation helpers
	static int makeint(unsigned char hi, unsigned char lo) { return (((int)hi << 8) & 0xff00) | (lo & 0x00ff); }
	static unsigned char hibyte(int x) { return (unsigned char)((x >> 8) & 0x00ff); }
	static unsigned char lobyte(int x) { return (unsigned char)(x & 0x00ff); }
};


// sub-classes of TreeNode classes that implement generation

class GenBlockTreeNode : public BlockTreeNode
{
public:
	GenBlockTreeNode() { }
	virtual int GenerateNode(unsigned char * program, int pc);
};

class GenNumberTreeNode : public NumberTreeNode
{
public:
	GenNumberTreeNode(int value) : NumberTreeNode(value) { }
	virtual int GenerateNode(unsigned char * program, int pc);
};

class GenColorNameTreeNode : public ColorNameTreeNode
{
public:
	GenColorNameTreeNode(COLOR_TYPE color) : ColorNameTreeNode(color) { }
	virtual int GenerateNode(unsigned char * program, int pc);
};

class GenFunctionTreeNode : public FunctionTreeNode
{
public:
	GenFunctionTreeNode(FUNCTION_TYPE func) : FunctionTreeNode(func) { }
	GenFunctionTreeNode(FUNCTION_TYPE func, TreeNode * param) : FunctionTreeNode(func, param) { }
	virtual int GenerateNode(unsigned char * program, int pc);
};

class GenTurtleCmdTreeNode : public TurtleCmdTreeNode
{
public:
	GenTurtleCmdTreeNode(TURTLE_CMD cmd) : TurtleCmdTreeNode(cmd) { }
	GenTurtleCmdTreeNode(TURTLE_CMD cmd, TreeNode * param) : TurtleCmdTreeNode(cmd, param) { }
	GenTurtleCmdTreeNode(TURTLE_CMD cmd, TreeNode * param1, TreeNode * param2) : TurtleCmdTreeNode(cmd, param1, param2) { }
	virtual int GenerateNode(unsigned char * program, int pc);
};

class GenIfTreeNode : public IfTreeNode
{
public:
	GenIfTreeNode(TreeNode * condition, BlockTreeNode * block) : IfTreeNode(condition, block) { }
	virtual int GenerateNode(unsigned char * program, int pc);
};

class GenIfElseTreeNode : public IfElseTreeNode
{
public:
	GenIfElseTreeNode(TreeNode * condition, BlockTreeNode * trueBlock, BlockTreeNode * falseBlock) : IfElseTreeNode(condition, trueBlock, falseBlock) { }
	virtual int GenerateNode(unsigned char * program, int pc);
};

class GenOperatorTreeNode : public OperatorTreeNode
{
public:
	GenOperatorTreeNode(OPERATOR_TYPE op, TreeNode * lhs, TreeNode * rhs) : OperatorTreeNode(op, lhs, rhs) { }
	virtual int GenerateNode(unsigned char * program, int pc);
};

class GenRepeatTreeNode : public RepeatTreeNode
{
public:
	GenRepeatTreeNode(TreeNode * loopFor, BlockTreeNode * block) : RepeatTreeNode(loopFor, block) { }
	virtual int GenerateNode(unsigned char * program, int pc);
};

class GenDeclarationTreeNode : public DeclarationTreeNode
{
public:
	GenDeclarationTreeNode(VARIABLE_TYPE vartype, VariableTreeNode * variable) : DeclarationTreeNode(vartype, variable) { }
	virtual int GenerateNode(unsigned char * program, int pc);
};

class GenVariableTreeNode : public VariableTreeNode
{
public:
	GenVariableTreeNode(SymbolTable::Entry * symentry) : VariableTreeNode(symentry) { }
	virtual int GenerateNode(unsigned char * program, int pc);
};

class GenAssignmentTreeNode : public AssignmentTreeNode
{
public:
	GenAssignmentTreeNode(VariableTreeNode * lhs, TreeNode * rhs) : AssignmentTreeNode(lhs, rhs) { }
	virtual int GenerateNode(unsigned char * program, int pc);
};


// factory
class GenTreeNodeFactory : public TreeNodeFactory
{
public:
	virtual BlockTreeNode * CreateBlock() { return new GenBlockTreeNode(); }
	virtual NumberTreeNode * CreateNumber(int value) { return new GenNumberTreeNode(value); }
	virtual ColorNameTreeNode * CreateColorName(COLOR_TYPE color) { return new GenColorNameTreeNode(color); }
	virtual FunctionTreeNode * CreateFunction(FUNCTION_TYPE func) { return new GenFunctionTreeNode(func); }
	virtual FunctionTreeNode * CreateFunction(FUNCTION_TYPE func, TreeNode * param) { return new GenFunctionTreeNode(func, param); }
	virtual TurtleCmdTreeNode * CreateTurtleCmd(TURTLE_CMD cmd) { return new GenTurtleCmdTreeNode(cmd); }
	virtual TurtleCmdTreeNode * CreateTurtleCmd(TURTLE_CMD cmd, TreeNode * param) { return new GenTurtleCmdTreeNode(cmd, param); }
	virtual TurtleCmdTreeNode * CreateTurtleCmd(TURTLE_CMD cmd, TreeNode * param1, TreeNode * param2) { return new GenTurtleCmdTreeNode(cmd, param1, param2); }
	virtual IfTreeNode * CreateIf(TreeNode * condition, BlockTreeNode * block) { return new GenIfTreeNode(condition, block); }
	virtual IfElseTreeNode * CreateIfElse(TreeNode * condition, BlockTreeNode * trueblock, BlockTreeNode * falseblock) { return new GenIfElseTreeNode(condition, trueblock, falseblock); }
	virtual OperatorTreeNode * CreateOperator(OPERATOR_TYPE op, TreeNode * lhs, TreeNode * rhs) { return new GenOperatorTreeNode(op, lhs, rhs); }
	virtual RepeatTreeNode * CreateRepeat(TreeNode * loopFor, BlockTreeNode * block) { return new GenRepeatTreeNode(loopFor, block); }
	virtual DeclarationTreeNode * CreateDeclaration(VARIABLE_TYPE vartype, VariableTreeNode * variable) { return new GenDeclarationTreeNode(vartype, variable); }
	virtual VariableTreeNode * CreateVariable(SymbolTable::Entry * symentry) { return new GenVariableTreeNode(symentry); }
	virtual AssignmentTreeNode * CreateAssignment(VariableTreeNode * lhs, TreeNode * rhs) { return new GenAssignmentTreeNode(lhs, rhs); }
};


#endif

