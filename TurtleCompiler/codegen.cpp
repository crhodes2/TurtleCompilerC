// codegen.c
// pete myers and Christian Rhodes
// OIT Fall 2007

//
// Assign 4 Handout
//


#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"
#include "tree.h"
#include "symtable.h"
#include "TurtleYacc.h"


//
// functions to help generate program
//


TURTLE_OPERATION func_to_turtle_op(FUNCTION_TYPE func)
{
	switch (func)
	{
		case FT_COLOR:
			return TURTLE_OPERATION_COLOR;
		case FT_XCOR:
			return TURTLE_OPERATION_XCOR;
		case FT_YCOR:
			return TURTLE_OPERATION_YCOR;
		case FT_RANDOM:
			return TURTLE_OPERATION_RANDOM;
		case FT_HEADING:
			return TURTLE_OPERATION_HEADING;
		default:
			return (TURTLE_OPERATION)-1;
	}
}

TURTLE_OPERATION turtle_cmd_to_turtle_op(TURTLE_CMD cmd)
{
	switch (cmd)
	{
		case CMD_HOME:
			return TURTLE_OPERATION_HOME;
		case CMD_PU:
			return TURTLE_OPERATION_PU;
		case CMD_PD:
			return TURTLE_OPERATION_PD;
		case CMD_FD:
			return TURTLE_OPERATION_FD;
		case CMD_BK:
			return TURTLE_OPERATION_BK;
		case CMD_RT:
			return TURTLE_OPERATION_RT;
		case CMD_LT:
			return TURTLE_OPERATION_LT;
		case CMD_SETC:
			return TURTLE_OPERATION_SETC;
		case CMD_SETXY:
			return TURTLE_OPERATION_SETXY;
		case CMD_SETH:
			return TURTLE_OPERATION_SETH;
		case CMD_SETX:
			return TURTLE_OPERATION_SETX;
		case CMD_SETY:
			return TURTLE_OPERATION_SETY;
		case CMD_HT:
			return TURTLE_OPERATION_HT;
		case CMD_ST:
			return TURTLE_OPERATION_ST;
		default:
			return (TURTLE_OPERATION)-1;
	}
}

COLORS color_type_to_colors(COLOR_TYPE c)
{
	switch(c)
	{
		case BLACK:
			return COLOR_BLACK;
		case WHITE:
			return COLOR_WHITE;
		case ORANGE:
			return COLOR_ORANGE;
		case YELLOW:
			return COLOR_YELLOW;
		case LIME:
			return COLOR_LIME;
		case CYAN:
			return COLOR_CYAN;
		case BLUE:
			return COLOR_BLUE;
		case MAGENTA:
			return COLOR_MAGENTA;
		case RED:
			return COLOR_RED;
		case BROWN:
			return COLOR_BROWN;
		case GREEN:
			return COLOR_GREEN;
		case TURQUOISE:
			return COLOR_TURQUOISE;
		case SKY:
			return COLOR_SKY;
		case VIOLET:
			return COLOR_VIOLET;
		case PINK:
			return COLOR_PINK;
		default:
			return (COLORS)-1;
	}
}



//
// C++ Version Jan 2017
//


TurtleProgram::TurtleProgram(int stackSize)
{
	this->stackSize = stackSize;
	this->program = NULL;
}

TurtleProgram::~TurtleProgram()
{
	if (program != NULL)
		delete[] program;
}

void TurtleProgram::Generate(BlockTreeNode * root)
{
	program = new unsigned char[MAX_PROGRAM];
	int pc = 0;

	// header
	pc += 8;

	// static variables
	program[2] = hibyte(pc);
	program[3] = lobyte(pc);
	pc = GenerateStaticVariables(pc);

	// program
	program[4] = hibyte(pc);
	program[5] = lobyte(pc);
	pc = root->GenerateNode(program, pc);
	program[pc++] = OPCODE_EXIT;

	// stack
	program[6] = hibyte(pc);
	program[7] = lobyte(pc);
	pc += stackSize;

	// fix up header's size member
	program[0] = hibyte(pc);
	program[1] = lobyte(pc);
}

int TurtleProgram::GenerateStaticVariables(int pc)
{
	// for each variable in symtable, determine its static address
	std::list<SymbolTable::Entry*> symbols = SymbolTable::GetInstance()->GetAllEntries();
	std::list<SymbolTable::Entry*>::iterator iter;
	for (iter = symbols.begin(); iter != symbols.end(); iter++)
	{
		// TODO Assign4: determine and set address for each variable
	}

	return pc;
}

int TurtleProgram::GetSize()
{
	return makeint(program[0], program[1]);
}

void TurtleProgram::PrintProgram(FILE * f)
{
	int i;
	OPCODE op;

	// header
	int size = makeint(program[0], program[1]);
	int variables = makeint(program[2], program[3]);
	int code = makeint(program[4], program[5]);
	int stack = makeint(program[6], program[7]);
	fprintf(f, "header->\n");
	fprintf(f, "    size      = 0x%04X\n", size);
	fprintf(f, "    variables = 0x%04X\n", variables);
	fprintf(f, "    code      = 0x%04X\n", code);
	fprintf(f, "    stack     = 0x%04X\n", stack);
	fprintf(f, "<-header\n");

	// static variables
	fprintf(f, "static variables->\n");
	fprintf(f, "    %d variables\n", (code - variables) / 2);
	if (code > variables)
	{
		fprintf(f, "    addr   = initial_value\n");
		fprintf(f, "    ------   -------------\n");
		for (i = variables; i < code; i += 2)
		{
			fprintf(f, "    0x%04X = 0x%04X\n", i, makeint(program[i], program[i + 1]));
		}
	}
	fprintf(f, "<-static variables\n");

	// code
	fprintf(f, "code->\n");
	i = code;
	do
	{
		op = (OPCODE)program[i];
		i = PrintOpcode(f, i);
	} while (op != OPCODE_EXIT);
	fprintf(f, "<-code\n");

	// stack
	fprintf(f, "stack->\n");
	fprintf(f, "    0x%04X bytes reserved\n", size - stack);
	fprintf(f, "<-stack\n");
}

int TurtleProgram::PrintOpcode(FILE * f, int i)
{
	REGISTER r1;
	REGISTER r2;
	unsigned char valh;
	unsigned char vall;
	unsigned char var1h;
	unsigned char var1l;
	unsigned char var2h;
	unsigned char var2l;
	unsigned char deltah;
	unsigned char deltal;
	int delta;
	unsigned char addrh;
	unsigned char addrl;
	TURTLE_OPERATION t;

	// print memory address first
	fprintf(f, "    0x%04X ", i & 0xffff);

	// find the opcode
	OPCODE op = (OPCODE)program[i++];

	switch (op)
	{
	case OPCODE_LOAD_R:
		r1 = (REGISTER)program[i++];
		valh = program[i++];
		vall = program[i++];
		fprintf(f, "LOAD_R  %s %d\n", RegisterName(r1), makeint(valh, vall));
		break;

	case OPCODE_LOAD_V:
		var1h = program[i++];
		var1l = program[i++];
		valh = program[i++];
		vall = program[i++];
		fprintf(f, "LOAD_V  %04X %d\n", makeint(var1h, var1l), makeint(valh, vall));
		break;

	case OPCODE_MOVE_RR:
		r1 = (REGISTER)program[i++];
		r2 = (REGISTER)program[i++];
		fprintf(f, "MOVE_RR %s %s\n", RegisterName(r1), RegisterName(r2));
		break;

	case OPCODE_MOVE_RV:
		r1 = (REGISTER)program[i++];
		var1h = program[i++];
		var1l = program[i++];
		fprintf(f, "MOVE_RV %s 0x%04X\n", RegisterName(r1), makeint(var1h, var1l));
		break;

	case OPCODE_MOVE_VR:
		var1h = program[i++];
		var1l = program[i++];
		r1 = (REGISTER)program[i++];
		fprintf(f, "MOVE_VR 0x%04X %s\n", makeint(var1h, var1l), RegisterName(r1));
		break;

	case OPCODE_MOVE_VV:
		var1h = program[i++];
		var1l = program[i++];
		var2h = program[i++];
		var2l = program[i++];
		fprintf(f, "MOVE_VV 0x%04X 0x%04X\n", makeint(var1h, var1l), makeint(var2h, var2l));
		break;

	case OPCODE_JMPRe:
		deltah = program[i++];
		deltal = program[i++];
		delta = makeint(deltah, deltal);
		fprintf(f, "JMPRe   0x%04X (to 0x%04X)\n", delta, i + delta);
		break;

	case OPCODE_JMPTo:
		addrh = program[i++];
		addrl = program[i++];
		fprintf(f, "JMPTo   0x%04X\n", makeint(addrh, addrl));
		break;

	case OPCODE_JEq:
		deltah = program[i++];
		deltal = program[i++];
		delta = makeint(deltah, deltal);
		fprintf(f, "JEq     0x%04X (to 0x%04X)\n", delta, i + delta);
		break;

	case OPCODE_JNe:
		deltah = program[i++];
		deltal = program[i++];
		delta = makeint(deltah, deltal);
		fprintf(f, "JNe     0x%04X (to 0x%04X)\n", delta, i + delta);
		break;

	case OPCODE_JGt:
		deltah = program[i++];
		deltal = program[i++];
		delta = makeint(deltah, deltal);
		fprintf(f, "JGt     0x%04X (to 0x%04X)\n", delta, i + delta);
		break;

	case OPCODE_JLt:
		deltah = program[i++];
		deltal = program[i++];
		delta = makeint(deltah, deltal);
		fprintf(f, "JLt     0x%04X (to 0x%04X)\n", delta, i + delta);
		break;

	case OPCODE_JZe:
		deltah = program[i++];
		deltal = program[i++];
		delta = makeint(deltah, deltal);
		fprintf(f, "JZe     0x%04X (to 0x%04X)\n", delta, i + delta);
		break;

	case OPCODE_JNz:
		deltah = program[i++];
		deltal = program[i++];
		delta = makeint(deltah, deltal);
		fprintf(f, "JNz     0x%04X (to 0x%04X)\n", delta, i + delta);
		break;

	case OPCODE_CMP_RR:
		r1 = (REGISTER)program[i++];
		r2 = (REGISTER)program[i++];
		fprintf(f, "CMP_RR  %s %s\n", RegisterName(r1), RegisterName(r2));
		break;

	case OPCODE_CMP_RV:
		r1 = (REGISTER)program[i++];
		var2h = program[i++];
		var2l = program[i++];
		fprintf(f, "CMP_RR  %s 0x%04X\n", RegisterName(r1), makeint(var2h, var2l));
		break;

	case OPCODE_CMP_VR:
		var1h = program[i++];
		var1l = program[i++];
		r2 = (REGISTER)program[i++];
		fprintf(f, "CMP_RR  0x%04X %s\n", makeint(var1h, var1l), RegisterName(r2));
		break;

	case OPCODE_CMP_VV:
		var1h = program[i++];
		var1l = program[i++];
		var2h = program[i++];
		var2l = program[i++];
		fprintf(f, "CMP_RR  0x%04X 0x%04X\n", makeint(var1h, var1l), makeint(var2h, var2l));
		break;

	case OPCODE_INC_R:
		r1 = (REGISTER)program[i++];
		fprintf(f, "INC_R   %s\n", RegisterName(r1));
		break;

	case OPCODE_DEC_R:
		r1 = (REGISTER)program[i++];
		fprintf(f, "DEC_R   %s\n", RegisterName(r1));
		break;

	case OPCODE_ADD_R:
		r1 = (REGISTER)program[i++];
		r2 = (REGISTER)program[i++];
		fprintf(f, "ADD_R   %s %s\n", RegisterName(r1), RegisterName(r2));
		break;

	case OPCODE_ADD_V:
		r1 = (REGISTER)program[i++];
		var2h = program[i++];
		var2l = program[i++];
		fprintf(f, "ADD_V   %s 0x%04X\n", RegisterName(r1), makeint(var2h, var2l));
		break;

	case OPCODE_SUB_R:
		r1 = (REGISTER)program[i++];
		r2 = (REGISTER)program[i++];
		fprintf(f, "SUB_R   %s %s\n", RegisterName(r1), RegisterName(r2));
		break;

	case OPCODE_SUB_V:
		r1 = (REGISTER)program[i++];
		var2h = program[i++];
		var2l = program[i++];
		fprintf(f, "SUB_V   %s 0x%04X\n", RegisterName(r1), makeint(var2h, var2l));
		break;

	case OPCODE_MUL_R:
		r1 = (REGISTER)program[i++];
		r2 = (REGISTER)program[i++];
		fprintf(f, "MUL_R   %s %s\n", RegisterName(r1), RegisterName(r2));
		break;

	case OPCODE_MUL_V:
		r1 = (REGISTER)program[i++];
		var2h = program[i++];
		var2l = program[i++];
		fprintf(f, "MUL_V   %s 0x%04X\n", RegisterName(r1), makeint(var2h, var2l));
		break;

	case OPCODE_DIV_R:
		r1 = (REGISTER)program[i++];
		r2 = (REGISTER)program[i++];
		fprintf(f, "DIV_R   %s %s\n", RegisterName(r1), RegisterName(r2));
		break;

	case OPCODE_DIV_V:
		r1 = (REGISTER)program[i++];
		var2h = program[i++];
		var2l = program[i++];
		fprintf(f, "DIV_V   %s 0x%04X\n", RegisterName(r1), makeint(var2h, var2l));
		break;

	case OPCODE_PUSH_R:
		r1 = (REGISTER)program[i++];
		fprintf(f, "PUSH_R  %s\n", RegisterName(r1));
		break;

	case OPCODE_POP_R:
		r1 = (REGISTER)program[i++];
		fprintf(f, "POP_R   %s\n", RegisterName(r1));
		break;

	case OPCODE_PEEK_R:
		r1 = (REGISTER)program[i++];
		fprintf(f, "PEEK_R  %s\n", RegisterName(r1));
		break;

	case OPCODE_TURTLE:
		t = (TURTLE_OPERATION)program[i++];
		fprintf(f, "TURTLE  %s\n", TurtleOperationString(t));
		break;

	case OPCODE_EXIT:
		fprintf(f, "EXIT\n");
		break;

	default:
		fprintf(f, "UNKNOWN\n");
		break;
	}

	return i;
}

char * TurtleProgram::RegisterName(REGISTER r)
{
	switch (r)
	{
	case REGISTER_P1:
		return "p1";
	case REGISTER_P2:
		return "p2";
	case REGISTER_RE:
		return "re";
	case REGISTER_G1:
		return "g1";
	case REGISTER_G2:
		return "g2";
	case REGISTER_G3:
		return "g3";
	case REGISTER_G4:
		return "g4";
	case REGISTER_PC:
		return "pc";
	case REGISTER_ST:
		return "st";
	case REGISTER_FL:
		return "fl";
	default:
		return "UNKNOWN";
	}
}

char * TurtleProgram::TurtleOperationString(TURTLE_OPERATION t)
{
	switch (t)
	{
	case TURTLE_OPERATION_HOME:
		return "HOME";
	case TURTLE_OPERATION_FD:
		return "FD";
	case TURTLE_OPERATION_BK:
		return "BK";
	case TURTLE_OPERATION_RT:
		return "RT";
	case TURTLE_OPERATION_LT:
		return "LT";
	case TURTLE_OPERATION_PU:
		return "PU";
	case TURTLE_OPERATION_PD:
		return "PD";
	case TURTLE_OPERATION_SETC:
		return "SETC";
	case TURTLE_OPERATION_SETXY:
		return "SETXY";
	case TURTLE_OPERATION_COLOR:
		return "COLOR";
	case TURTLE_OPERATION_XCOR:
		return "XCOR";
	case TURTLE_OPERATION_YCOR:
		return "YCOR";
	case TURTLE_OPERATION_RANDOM:
		return "RANDOM";
	case TURTLE_OPERATION_SETH:
		return "SETH";
	case TURTLE_OPERATION_SETX:
		return "SETX";
	case TURTLE_OPERATION_SETY:
		return "SETY";
	case TURTLE_OPERATION_HT:
		return "HT";
	case TURTLE_OPERATION_ST:
		return "ST";
	case TURTLE_OPERATION_HEADING:
		return "HEADING";
	default:
		return "UNKNOWN";
	}
}

void TurtleProgram::WriteBinary(FILE * f)
{
	fwrite(program, 1, GetSize(), f);
}


// Node classes for generation

int GenBlockTreeNode::GenerateNode(unsigned char * program, int pc)
{
	std::list<TreeNode*> children = GetChildren();
	std::list<TreeNode*>::iterator iter;
	for (iter = children.begin(); iter != children.end(); iter++)
	{
		pc = (*iter)->GenerateNode(program, pc);
	}

	return pc;
}

int GenNumberTreeNode::GenerateNode(unsigned char * program, int pc)
{
	// LOAD_R G1 value
	program[pc++] = OPCODE_LOAD_R;
	program[pc++] = REGISTER_G1;
	program[pc++] = TurtleProgram::hibyte(Value());
	program[pc++] = TurtleProgram::lobyte(Value());

	// PUSH_R G1
	program[pc++] = OPCODE_PUSH_R;
	program[pc++] = REGISTER_G1;

	return pc;
}

int GenColorNameTreeNode::GenerateNode(unsigned char * program, int pc)
{
	COLORS color_const = color_type_to_colors(Color());

	program[pc++] = OPCODE_LOAD_R;
	program[pc++] = REGISTER_G1;
	program[pc++] = TurtleProgram::hibyte(color_const);
	program[pc++] = TurtleProgram::lobyte(color_const);

	program[pc++] = OPCODE_PUSH_R;
	program[pc++] = REGISTER_G1;

	return pc;
}

int GenFunctionTreeNode::GenerateNode(unsigned char * program, int pc)
{
	switch (Function())
	{
	case FT_COLOR:
	case FT_XCOR:
	case FT_YCOR:
	case FT_HEADING:
	{
		program[pc++] = OPCODE_TURTLE;
		program[pc++] = func_to_turtle_op(Function());
	}
	break;

	case FT_RANDOM:
	{
		TreeNode * param1 = FirstChild();
		pc = param1->GenerateNode(program, pc);

		program[pc++] = OPCODE_POP_R;
		program[pc++] = REGISTER_P1;

		program[pc++] = OPCODE_TURTLE;
		program[pc++] = func_to_turtle_op(Function());
	}
	break;

	}

	program[pc++] = OPCODE_PUSH_R;
	program[pc++] = REGISTER_RE;

	return pc;
}

int GenTurtleCmdTreeNode::GenerateNode(unsigned char * program, int pc)
{
	switch (Command())
	{
	case CMD_SETXY:
	{
		TreeNode * param1 = FirstChild();
		TreeNode * param2 = SecondChild();
		pc = param1->GenerateNode(program, pc);
		pc = param2->GenerateNode(program, pc);

		// POP_R P2
		program[pc++] = OPCODE_POP_R;
		program[pc++] = REGISTER_P2;

		// POP_R P1
		program[pc++] = OPCODE_POP_R;
		program[pc++] = REGISTER_P1;

		// TURTLE cmd
		program[pc++] = OPCODE_TURTLE;
		program[pc++] = turtle_cmd_to_turtle_op(Command());
	}
	break;

	case CMD_FD:
	case CMD_BK:
	case CMD_LT:
	case CMD_RT:
	case CMD_SETX:
	case CMD_SETY:
	case CMD_SETH:
	case CMD_SETC:
	{
		TreeNode * param1 = FirstChild();
		pc = param1->GenerateNode(program, pc);

		// POP_R P1
		program[pc++] = OPCODE_POP_R;
		program[pc++] = REGISTER_P1;

		// TURTLE cmd
		program[pc++] = OPCODE_TURTLE;
		program[pc++] = turtle_cmd_to_turtle_op(Command());
	}
	break;

	case CMD_HOME:
	case CMD_PU:
	case CMD_PD:
	case CMD_ST:
	case CMD_HT:
		// TURTLE cmd
		program[pc++] = OPCODE_TURTLE;
		program[pc++] = turtle_cmd_to_turtle_op(Command());
		break;

	default:
		break;
	}

	return pc;
}

int GenIfTreeNode::GenerateNode(unsigned char * program, int pc)
{
	TreeNode * condition = FirstChild();

	pc = condition->GenerateNode(program, pc);

	program[pc++] = OPCODE_POP_R;
	program[pc++] = REGISTER_G1;

	program[pc++] = OPCODE_LOAD_R;
	program[pc++] = REGISTER_G2;
	program[pc++] = 0;
	program[pc++] = 0;

	program[pc++] = OPCODE_CMP_RR;
	program[pc++] = REGISTER_G1;
	program[pc++] = REGISTER_G2;

	program[pc++] = OPCODE_JEq;
	pc += 2;
	int after_jeq = pc;

	TreeNode * trueBlock = SecondChild();
	pc = trueBlock->GenerateNode(program, pc);

	int delta = pc - after_jeq;
	program[after_jeq -2] = TurtleProgram::hibyte(delta);
	program[after_jeq -1] = TurtleProgram::lobyte(delta);

	return pc;
}

int GenIfElseTreeNode::GenerateNode(unsigned char * program, int pc)
{
	TreeNode * condition = FirstChild();

	pc = condition->GenerateNode(program, pc);

	program[pc++] = OPCODE_POP_R;
	program[pc++] = REGISTER_G1;

	program[pc++] = OPCODE_LOAD_R;
	program[pc++] = REGISTER_G2;
	program[pc++] = 0;
	program[pc++] = 0;

	program[pc++] = OPCODE_CMP_RR;
	program[pc++] = REGISTER_G1;
	program[pc++] = REGISTER_G2;

	program[pc++] = OPCODE_JEq;
	pc += 2;
	int after_jeq = pc;

	TreeNode * trueBlock = SecondChild();
	pc = trueBlock->GenerateNode(program, pc);

	program[pc++] = OPCODE_JMPRe;
	pc += 2;
	int after_jmpre = pc;

	int delta = pc - after_jeq;
	program[after_jeq - 2] = TurtleProgram::hibyte(delta);
	program[after_jeq - 1] = TurtleProgram::lobyte(delta);

	TreeNode * falseBlock = ThirdChild();
	pc = falseBlock->GenerateNode(program, pc);

	delta = pc - after_jmpre;
	program[after_jmpre - 2] = TurtleProgram::hibyte(delta);
	program[after_jmpre - 1] = TurtleProgram::lobyte(delta);

	return pc;
}

int GenOperatorTreeNode::GenerateNode(unsigned char * program, int pc)
{
	TreeNode * lhs = FirstChild();
	TreeNode * rhs = SecondChild();

	// lhs child
	pc = lhs->GenerateNode(program, pc);

	// rhs child
	pc = rhs->GenerateNode(program, pc);

	// POP_R G2
	program[pc++] = OPCODE_POP_R;
	program[pc++] = REGISTER_G2;

	// POP_R G1
	program[pc++] = OPCODE_POP_R;
	program[pc++] = REGISTER_G1;

	// op
	switch (Operator())
	{
	case OT_PLUS:
		// ADD_R G1 G2
		program[pc++] = OPCODE_ADD_R;
		program[pc++] = REGISTER_G1;
		program[pc++] = REGISTER_G2;
		break;
	case OT_MINUS:
		// SUB_R G1 G2
		program[pc++] = OPCODE_SUB_R;
		program[pc++] = REGISTER_G1;
		program[pc++] = REGISTER_G2;
		break;
	case OT_TIMES:
		// MUL_R G1 G2
		program[pc++] = OPCODE_MUL_R;
		program[pc++] = REGISTER_G1;
		program[pc++] = REGISTER_G2;
		break;
	case OT_DIVIDE:
		// DIV_R G1 G2
		program[pc++] = OPCODE_DIV_R;
		program[pc++] = REGISTER_G1;
		program[pc++] = REGISTER_G2;
		break;

	case OT_EQUALS:
		// CMP
		program[pc++] = OPCODE_CMP_RR;
		program[pc++] = REGISTER_G1;
		program[pc++] = REGISTER_G2;
		
		// LOAD TRUE
		program[pc++] = OPCODE_LOAD_R;
		program[pc++] = REGISTER_G1;
		program[pc++] = 0;
		program[pc++] = 1;

		//JEq
		program[pc++] = OPCODE_JEq;
		program[pc++] = 0;
		program[pc++] = 4;

		// LOAD FALSE
		program[pc++] = OPCODE_LOAD_R;
		program[pc++] = REGISTER_G1;
		program[pc++] = 0;
		program[pc++] = 0;
		break;

	case OT_LESSTHAN:
		// CMP
		program[pc++] = OPCODE_CMP_RR;
		program[pc++] = REGISTER_G1;
		program[pc++] = REGISTER_G2;

		// LOAD TRUE
		program[pc++] = OPCODE_LOAD_R;
		program[pc++] = REGISTER_G1;
		program[pc++] = 0;
		program[pc++] = 1;

		//JLT
		program[pc++] = OPCODE_JLt;
		program[pc++] = 0;
		program[pc++] = 4;

		// LOAD FALSE
		program[pc++] = OPCODE_LOAD_R;
		program[pc++] = REGISTER_G1;
		program[pc++] = 0;
		program[pc++] = 0;
		break;

	case OT_GREATERTHAN:
		// CMP
		program[pc++] = OPCODE_CMP_RR;
		program[pc++] = REGISTER_G1;
		program[pc++] = REGISTER_G2;

		// LOAD TRUE
		program[pc++] = OPCODE_LOAD_R;
		program[pc++] = REGISTER_G1;
		program[pc++] = 0;
		program[pc++] = 1;

		//JGt
		program[pc++] = OPCODE_JGt;
		program[pc++] = 0;
		program[pc++] = 4;

		// LOAD FALSE
		program[pc++] = OPCODE_LOAD_R;
		program[pc++] = REGISTER_G1;
		program[pc++] = 0;
		program[pc++] = 0;
		break;

	default:
		break;
	}

	// PUSH_R G1
	program[pc++] = OPCODE_PUSH_R;
	program[pc++] = REGISTER_G1;

	return pc;
}

int GenRepeatTreeNode::GenerateNode(unsigned char * program, int pc)
{
	TreeNode * loopFor = FirstChild();
	pc = loopFor->GenerateNode(program, pc);

	int loopTop = pc;

	program[pc++] = OPCODE_PEEK_R;
	program[pc++] = REGISTER_G1;

	// LOAD_R
	program[pc++] = OPCODE_LOAD_R;
	program[pc++] = REGISTER_G2;
	program[pc++] = 0;
	program[pc++] = 1;

	// CMP_RR
	program[pc++] = OPCODE_CMP_RR;
	program[pc++] = REGISTER_G1;
	program[pc++] = REGISTER_G2;

	// Jlt
	program[pc++] = OPCODE_JLt;
	pc += 2;
	int after_jlt = pc;

	TreeNode * loopBlock = SecondChild();
	pc = loopBlock->GenerateNode(program, pc);

	program[pc++] = OPCODE_POP_R;
	program[pc++] = REGISTER_G1;

	program[pc++] = OPCODE_DEC_R;
	program[pc++] = REGISTER_G1;

	program[pc++] = OPCODE_PUSH_R;
	program[pc++] = REGISTER_G1;

	program[pc++] = OPCODE_JMPTo;
	program[pc++] = TurtleProgram::hibyte(loopTop);
	program[pc++] = TurtleProgram::lobyte(loopTop);

	int loopEnd = pc;

	int delta = loopEnd - after_jlt;
	program[after_jlt - 2] = TurtleProgram::hibyte(delta);
	program[after_jlt - 1] = TurtleProgram::lobyte(delta);

	program[pc++] = OPCODE_POP_R;
	program[pc++] = REGISTER_G1;

	return pc;
}

int GenDeclarationTreeNode::GenerateNode(unsigned char * program, int pc)
{
	// TODO Assign4: extra credit

	return pc;
}

int GenVariableTreeNode::GenerateNode(unsigned char * program, int pc)
{
	// TODO Assign4: extra credit

	return pc;
}

int GenAssignmentTreeNode::GenerateNode(unsigned char * program, int pc)
{
	// TODO Assign4: extra credit

	return pc;
}
