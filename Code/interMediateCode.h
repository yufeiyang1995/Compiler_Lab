#ifndef INTERMEDIATECODE_H
#define INTERMEDIATECODE_H

#define MAX_LINE 10000

#include "node.h"

typedef struct Operand_* Operand;
typedef struct structCode_* structCode;

int temp_num = 0;
int var_num = 0;
int label_num = 0;
int struct_num = 0;

typedef struct Operand_ {
	enum { VARIABLE, CONSTANT, ADDRESS, TEMP,ARRAY, STRUCTURE} kind;
 	struct {
		int var_no;
 		int value;
 		char var_name[30];
 		structCode s;
 		int is_param;
 		int array_size;
 		Operand addr;
 		/*, … */
	} u;
}Operand_;

typedef struct InterCode
{
	enum { ASSIGN, ADD, SUB, MUL, DIV, LABEL, GOTO, READ, WRITE, IF, RETURN, 
	FUNCTION, CALL, ARGS,PARAM, DEC,GETADDR,LEFTADDR,RIGHTADDR,GAP,ARRAYGAP,
	TEMPGAP} kind;
	union {
		struct { Operand right, left; } assign;
		struct { Operand result, op1, op2; } binop;
		struct { Operand op; char func_name[30];}callop;
		struct { Operand right, left; int type; int label;} ifGotoOp;
		Operand op;
		char funcName[30];
		char varName[30];
		int label;
		struct{int size; Operand op;}decop;
		struct{int gap; Operand left,right;}getaddrop;
	} u;
	int sign;
}InterCode;

typedef struct structCode_
{
	Operand_ List[10];
	int list_num;
	int size;
	char name[30];
}structCode_;

Operand_ varible[MAX_LINE];
InterCode codes[MAX_LINE];
structCode st[10];

void init_InterCode(){
	for(int i = 0;i < MAX_LINE;i++){
		codes[i].sign = 0;
	}
}

void init_structCode(){
	for(int i = 0;i < 10;i++){
		st[i] = (structCode)malloc(sizeof(structCode_));
	}
}

int find_in_struct(char* str,int var_no){
	structCode s = varible[var_no-1].u.s;
	//printf("========%s\n",s->List[1].u.var_name);
	
	int temp = 0;
	int i = 0;
	while(strcmp(s->List[i].u.var_name,str) != 0){
		if(s->List[i].kind == VARIABLE){
			temp = temp + 4;
		}
		else{
			/*array*/
			temp = temp + s->List[i].u.array_size;
		}
		i++;
	}
	return temp;
}

Operand create_varible(Node* p){
	varible[var_num].kind = VARIABLE;
	varible[var_num].u.var_no = var_num+1;
	//printf("======%d=====\n", varible[var_num].u.var_no);
	strcpy(varible[var_num].u.var_name,p->type_char);
	//printf("======%s=====\n",varible[var_num].u.var_name);
	//printf("======%d=====\n", varible[var_num].u.var_no);
	var_num++;
	//printf("====var %d\n",var_num);
	//printf("======%s=====\n",varible[var_num-1].u.var_name);
	return &varible[var_num-1];
}



void create_temp(Operand temp){
	temp_num++;
	///printf("==========\n");
	temp->kind = TEMP;
	temp->u.var_no = temp_num;
}

int create_label(){
	label_num++;
	return label_num;
}

int get_varible(char* s){
	for(int i = var_num-1;i >= 0;i--){
		//printf("======%s=====\n",varible[i].u.var_name);
		if(strcmp(varible[i].u.var_name,s) == 0){
			return i+1;
		}
	}
	return -1;
}

structCode get_struct(char* s){
	for(int i = 0;i < struct_num;i++){
		if(strcmp(st[i]->name,s) == 0){
			return st[i];
		}
	}
	return NULL;
}

void temp_num_minus(){
	temp_num--;
}

void print_relop(int relop){
	switch(relop){
		case 1:printf(" == ");break;
		case 2:printf(" > ");break;
		case 3:printf(" < ");break;
		case 4:printf(" >= ");break;
		case 5:printf(" <= ");break;
		case 6:printf(" != ");break;
		default:break;
	}
}

void print_relop_text(int relop,FILE* fp){
	switch(relop){
		case 1:fprintf(fp," == ");break;
		case 2:fprintf(fp," > ");break;
		case 3:fprintf(fp," < ");break;
		case 4:fprintf(fp," >= ");break;
		case 5:fprintf(fp," <= ");break;
		case 6:fprintf(fp," != ");break;
		default:break;
	}
}

void print_Operand_text(FILE* fp,Operand op){
	if(op != NULL){
		switch(op->kind) {
			case VARIABLE:fprintf(fp,"v%d",op->u.var_no);break;
			case CONSTANT:fprintf(fp,"#%d",op->u.value);break;
			case ADDRESS:fprintf(fp,"*t%d",op->u.var_no);break;
			case TEMP:fprintf(fp,"t%d",op->u.var_no);break;
			case STRUCTURE:fprintf(fp,"v%d",op->u.var_no);break;
			case ARRAY:fprintf(fp,"v%d",op->u.var_no);break;
			default:return;
		}
	}
}


void print_text(char* filename){
	FILE* fp = fopen(filename,"w");
	if(fp == NULL) {
		printf("Error when open the file %s\n",filename);
		exit(-1);
	}

	int i = 0;
	while(codes[i].sign != 0){
		if(codes[i].kind == FUNCTION){
			fprintf(fp, "FUNCTION %s :\n", codes[i].u.funcName);
		}
		if(codes[i].kind == ASSIGN){
			print_Operand_text(fp,codes[i].u.assign.left);
			fprintf(fp, " := ");
			print_Operand_text(fp,codes[i].u.assign.right);
			fprintf(fp, "\n");
		}
		if(codes[i].kind == ADD){
			print_Operand_text(fp,codes[i].u.binop.result);
			fprintf(fp, " := " );
			print_Operand_text(fp,codes[i].u.binop.op1);
			fprintf(fp, " + ");
			print_Operand_text(fp,codes[i].u.binop.op2);
			fprintf(fp, "\n");
		}
		if(codes[i].kind == SUB){
			print_Operand_text(fp,codes[i].u.binop.result);
			fprintf(fp, " := " );
			print_Operand_text(fp,codes[i].u.binop.op1);
			fprintf(fp, " - ");
			print_Operand_text(fp,codes[i].u.binop.op2);
			fprintf(fp, "\n");
		}
		if(codes[i].kind == MUL){
			print_Operand_text(fp,codes[i].u.binop.result);
			fprintf(fp, " := " );
			print_Operand_text(fp,codes[i].u.binop.op1);
			fprintf(fp, " * ");
			print_Operand_text(fp,codes[i].u.binop.op2);
			fprintf(fp, "\n");
		}
		if(codes[i].kind == DIV){
			print_Operand_text(fp,codes[i].u.binop.result);
			fprintf(fp, " := " );
			print_Operand_text(fp,codes[i].u.binop.op1);
			fprintf(fp, " / ");
			print_Operand_text(fp,codes[i].u.binop.op2);
			fprintf(fp, "\n");
		}
		if(codes[i].kind == CALL){
			print_Operand_text(fp,codes[i].u.callop.op);
			fprintf(fp, " := CALL " );
			fprintf(fp,"%s\n",codes[i].u.callop.func_name);
		}
		if(codes[i].kind == RETURN){
			fprintf(fp, "RETURN ");
			print_Operand_text(fp,codes[i].u.op);
			fprintf(fp, "\n");
		}
		if(codes[i].kind == READ){
			fprintf(fp, "READ ");
			print_Operand_text(fp,codes[i].u.callop.op);
			fprintf(fp, "\n");
		}
		if(codes[i].kind == WRITE){
			fprintf(fp, "WRITE ");
			print_Operand_text(fp,codes[i].u.op);
			fprintf(fp, "\n");
		}
		if(codes[i].kind == IF){
			fprintf(fp, "IF ");
			print_Operand_text(fp,codes[i].u.ifGotoOp.left);
			print_relop_text(codes[i].u.ifGotoOp.type,fp);
			print_Operand_text(fp,codes[i].u.ifGotoOp.right);
			fprintf(fp, " GOTO label");
			fprintf(fp, "%d\n",codes[i].u.ifGotoOp.label);
		}
		if(codes[i].kind == GOTO){
			fprintf(fp, "GOTO label");
			fprintf(fp, "%d\n", codes[i].u.label);
		}
		if(codes[i].kind == LABEL){
			fprintf(fp, "LABEL label%d :\n", codes[i].u.label);
		}
		if(codes[i].kind == ARGS){
			fprintf(fp, "ARG ");
			if(codes[i].u.op->kind == STRUCTURE){
				fprintf(fp, "&" );
			}
			print_Operand_text(fp,codes[i].u.op);
			fprintf(fp, "\n");
		}
		if(codes[i].kind == PARAM){
			fprintf(fp, "PARAM ");
			print_Operand_text(fp,codes[i].u.op);
			fprintf(fp, "\n" );
		}
		if(codes[i].kind == DEC){
			fprintf(fp, "DEC ");
			print_Operand_text(fp,codes[i].u.decop.op);
			fprintf(fp, " %d\n", codes[i].u.decop.size);
		}
		if(codes[i].kind == GETADDR){
			print_Operand_text(fp,codes[i].u.getaddrop.left);
			fprintf(fp, " := &");
			print_Operand_text(fp,codes[i].u.getaddrop.right);
			if(codes[i].u.getaddrop.gap != 0){
				fprintf(fp, " + ");
				fprintf(fp, "#%d",codes[i].u.getaddrop.gap );
			}
			fprintf(fp, "\n");
		}
		if(codes[i].kind == LEFTADDR){
			print_Operand_text(fp,codes[i].u.assign.left);
			fprintf(fp, " := ");
			print_Operand_text(fp,codes[i].u.assign.right);
			fprintf(fp, "\n");
		}
		if(codes[i].kind == GAP){
			print_Operand_text(fp,codes[i].u.getaddrop.left);
			fprintf(fp," := ");
			print_Operand_text(fp,codes[i].u.getaddrop.right);
			fprintf(fp," + ");
			fprintf(fp,"#%d\n",codes[i].u.getaddrop.gap);
		}
		if(codes[i].kind == TEMPGAP){
			print_Operand_text(fp,codes[i].u.binop.result);
			fprintf(fp," := &");
			print_Operand_text(fp,codes[i].u.binop.op1);
			fprintf(fp," + ");
			print_Operand_text(fp,codes[i].u.binop.op2);
			fprintf(fp,"\n");
		}
		if(codes[i].kind == ARRAYGAP){
			print_Operand_text(fp,codes[i].u.assign.left);
			fprintf(fp," := ");
			print_Operand_text(fp,codes[i].u.assign.right);
			fprintf(fp," * #4\n");
		}
		if(codes[i].kind == RIGHTADDR){
			print_Operand_text(fp,codes[i].u.assign.left);
			fprintf(fp," := *");
			print_Operand_text(fp,codes[i].u.assign.right);;
			fprintf(fp,"\n");
		}
		i++;

	}
}

void translate_VarList(Node* p);
void translate_ParamDec(Node* p);
void translate_Specifier(Node* p);
void translate_structDec(Node* p,structCode s);
void translate_structDecList(Node* p,structCode s);
void translate_Exp(Node* p,Operand temp);
void translate_Stmt(Node* p);
void translate_Cond(int label1,int label2,Node* p);
void translate_Args(Node* p);
void create_AssignCode(Operand left,Operand right);
void print_Operand(Operand op);
void create_CalCode(Operand left,Operand right,Operand result,int type);
void create_callCode(Operand op,char* name);
void create_Args(Operand op);
void create_ParamCode(Operand op);
void create_ifGotoCode(Operand left,Operand right,int op_type,int label);
void create_GotoCode(int label);
void create_labelCode(int label);
void create_ReturnCode(Operand op);
void create_DecCode(structCode s,Operand op);
void create_GetAddrCode(Operand temp,Operand var,int gap);
void create_LeftAddrCode(Operand left,Operand right);
void create_GapCode(Operand temp,Operand var,int gap);
void create_RightAddrCode(Operand temp,Operand var);
void create_arrayGapCode(Operand left,Operand right);
void create_tempGapCode(Operand temp,Operand var,Operand temp1);


#endif