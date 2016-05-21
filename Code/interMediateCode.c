#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interMediateCode.h"

extern Node* get_root();
int current = 0;
int func_num = 0;
int args_num[10];
Operand_ args_list[10][10];

void travel_tree_inter(Node* root);
void translate_ExtDefList(Node* p);
void translate_ExtDef(Node* p);
void translate_ExtDecList(Node* p);
void translate_VarDec(Node* p,structCode s);
void translate_FunDec(Node* p);
void create_FuncCode(char* name);
void translate_CompSt(Node* p);
void translate_StmtList(Node* p);
void create_FuncCode(char* name);
void translate_DecList(Node* p,structCode s);
void translate_DefList(Node* p,structCode s);
void translate_Dec(Node* p,structCode s);
void translate_Def(Node* p,structCode s);
void print_codes();

void interMediateCode(){
	Node* root = get_root();

	if(root == NULL){
		//printf("the program is empty\n");
		exit(-1);
	}
	init_structCode();
	travel_tree_inter(root);
	//print_codes();
}

void travel_tree_inter(Node* root){
	if(root->node_type == -1 && root->children == NULL)
		return;
	//printf("%s\n",root->name);

	//if(root->children == NULL)
	//	return;
	if(strcmp(root->name,"ExtDef") == 0){
		translate_ExtDef(root);
	}

	if(root->children != NULL){
		travel_tree_inter(root->children);
		Node* temp = root->children;
		while(temp->sibling != NULL){
			travel_tree_inter(temp->sibling);
			temp = temp->sibling;
		}
	}
}

void translate_ExtDefList(Node* p){
	p = p->children;
	if(p != NULL){
		translate_ExtDef(p);
		translate_ExtDefList(p->sibling);
	}
	else{
		return;
	}
}

void translate_ExtDef(Node* p){
	p = p->children;
	if(strcmp(p->sibling->name,"ExtDecList") == 0){
		translate_Specifier(p);
		translate_ExtDecList(p->sibling);
	}
	else if(strcmp(p->sibling->name,"FunDec") == 0){
		translate_FunDec(p->sibling);
		translate_CompSt(p->sibling->sibling);
	}
	else{
		translate_Specifier(p);
	}
}

void translate_Specifier(Node* p){
	Node* head = p;
	p = p->children;
	if(strcmp(p->name,"TYPE") != 0){
		Node* ch = p->children;
		if(ch->sibling->sibling == NULL){

			structCode s = get_struct(ch->sibling->children->type_char);
			
			/*ParamDec:Specifier VarDec*/
			if(strcmp(head->sibling->name,"VarDec") == 0){
				Operand temp = (Operand)malloc(sizeof(Operand_));
				temp->kind = STRUCTURE;
				//printf("~~~~%s\n",head->sibling->children->name );
				strcpy(temp->u.var_name,head->sibling->children->type_char);
				temp->u.var_no = var_num+1;
				temp->u.s = s;
				temp->u.is_param = 1;

				create_ParamCode(temp);

				varible[var_num] = *temp;
				var_num++;
				//printf("null %d %s\n",temp->u.var_no,varible[var_num-1].u.s->name);
			
			}
			else{
				translate_structDecList(head->sibling,s);
			}

			/*Operand temp = (Operand)malloc(sizeof(Operand_));
			temp->kind = STRUCTURE;
			strcpy(temp->u.var_name,ch->sibling->children->type_char);
			temp->u.var_no = var_num+1;
			temp->u.s = s;

			create_DecCode(s,temp);

			varible[var_num] = *temp;
			printf("null %s\n",varible[var_num].u.var_name);
			exit(-1);
			var_num++;*/
		}
		else{
			structCode s = (structCode)malloc(sizeof(structCode_));
			s = st[struct_num];

			s->size = 0;
			strcpy(s->name,ch->sibling->children->type_char);
			//printf("======STRUCT %d======\n",struct_num);

			translate_DefList(ch->sibling->sibling->sibling,s);
			struct_num++;
		}
	}
}

void translate_structDecList(Node* p,structCode s){
	p = p->children;
	if(p->sibling == NULL){
		translate_structDec(p,s);
	}
	else{
		translate_structDec(p,s);
		translate_structDecList(p->sibling->sibling,s);
	}
}

void translate_structDec(Node* p,structCode s){
	p = p->children;
	if(p->sibling == NULL){
		Node* ch = p->children;
		/*ID*/
		if(ch->sibling == NULL){
			Operand temp = (Operand)malloc(sizeof(Operand_));
			temp->kind = STRUCTURE;
			strcpy(temp->u.var_name,ch->type_char);
			temp->u.var_no = var_num+1;
			temp->u.s = s;

			create_DecCode(s,temp);

			varible[var_num] = *temp;
			//printf("null %d %s\n",var_num,varible[var_num].u.s->name);
			
			var_num++;
		}
		/*struct array*/
		else{

		}
	}
}

void translate_ExtDecList(Node* p){
	p = p->children;
	if(p->sibling == NULL){
		translate_VarDec(p,NULL);
	}
}

void translate_VarDec(Node* p,structCode s){
	p = p->children;
	if(s == NULL){
		if(p->sibling == NULL){
			create_varible(p);
		}
		else{
			Operand temp1 = (Operand)malloc(sizeof(Operand_));
			create_temp(temp1);

			temp1->kind = ARRAY;
			temp1->u.var_no = var_num+1;
			strcpy(temp1->u.var_name,p->children->type_char);

			int x = p->sibling->sibling->type_int;
			temp1->u.array_size = x * 4;

			create_DecCode(s,temp1);
			varible[var_num] = *temp1;
			var_num++;
		}
	}
	else{
		if(p->sibling == NULL){
			Operand_ op;
			op.kind = VARIABLE;
			strcpy(op.u.var_name,p->type_char);
			s->List[s->list_num] = op;
			s->list_num++;
			s->size += 4; 
		}
		else{
			Operand_ op;
			op.kind = ARRAY;
			strcpy(op.u.var_name,p->type_char);
			op.u.array_size = p->sibling->sibling->type_int * 4;
			s->List[s->list_num] = op;
			s->list_num++;
			s->size += op.u.array_size;
		}
	}
}

void translate_FunDec(Node* p){
	p = p->children;
	if(p->sibling->sibling->sibling == NULL){
		create_FuncCode(p->type_char);
	}
	else{
		create_FuncCode(p->type_char);

		translate_VarList(p->sibling->sibling);

	}
}

void translate_VarList(Node* p){
	p = p->children;
	if(p->sibling == NULL){
		translate_ParamDec(p);
	}
	else{
		translate_ParamDec(p);
		translate_VarList(p->sibling->sibling);
	}
}

void translate_ParamDec(Node* p){
	p = p->children;
	Node* varDec = p->sibling;
	Node* ch = varDec->children;

	translate_Specifier(p);
	//printf("Param %s\n",p->children->name);
	if(strcmp(p->children->name,"TYPE") == 0){
		if(ch->sibling == NULL){
			create_varible(ch);

			Operand temp1 = (Operand)malloc(sizeof(Operand_));
			create_temp(temp1);

			temp1->kind = VARIABLE;
			strcpy(temp1->u.var_name,ch->type_char);
			if(get_varible(ch->type_char) == -1){
				//printf("NO SUCH VARIABLE!\n");
				return;
			}
			temp_num_minus();
			temp1->u.var_no = get_varible(ch->type_char);
			create_ParamCode(temp1);
		}
		else{
			printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
		}
	}
}

void translate_CompSt(Node* p){
	p = p->children;
	translate_DefList(p->sibling,NULL);
	translate_StmtList(p->sibling->sibling);
}

void translate_DefList(Node* p,structCode s){
	p = p->children;
	if(p != NULL){
		translate_Def(p,s);
		translate_DefList(p->sibling,s);
	}
	else{
		return;
	}
}

void translate_StmtList(Node* p){
	p = p->children;
	if(p == NULL){
		return;
	}
	else{
		translate_Stmt(p);
		translate_StmtList(p->sibling);
	}
}

void translate_Stmt(Node* p){
	p = p->children;
	if(strcmp(p->name,"Exp") == 0){
		translate_Exp(p,NULL);
	}
	else if(strcmp(p->name,"CompSt") == 0){
		translate_CompSt(p);
	}
	else if(strcmp(p->name,"WHILE") == 0){
		int label1 = create_label();
		int label2 = create_label();
		int label3 = create_label();

		create_labelCode(label1);
		translate_Cond(label2,label3,p->sibling->sibling);
		create_labelCode(label2);
		translate_Stmt(p->sibling->sibling->sibling->sibling);
		create_GotoCode(label1);
		create_labelCode(label3);
	}
	else if(strcmp(p->name,"RETURN") == 0){
		Operand temp1 = (Operand)malloc(sizeof(Operand_));
		create_temp(temp1);

		translate_Exp(p->sibling,temp1);
		create_ReturnCode(temp1);
	}
	else{
		if(p->sibling->sibling->sibling->sibling->sibling != NULL){
			int label1 = create_label();
			int label2 = create_label();
			int label3 = create_label();

			translate_Cond(label1,label2,p->sibling->sibling);
			create_labelCode(label1);

			translate_Stmt(p->sibling->sibling->sibling->sibling);
			create_GotoCode(label3);
			create_labelCode(label2);
			translate_Stmt(p->sibling->sibling->sibling->sibling->sibling->sibling);
			create_labelCode(label3);
		}
		else{
			int label1 = create_label();
			int label2 = create_label();

			translate_Cond(label1,label2,p->sibling->sibling);
			create_labelCode(label1);

			translate_Stmt(p->sibling->sibling->sibling->sibling);
			create_labelCode(label2);
		}
	}
}

void translate_Cond(int label1,int label2,Node* p){
	p = p->children;
	if(p->sibling->sibling == NULL){
		translate_Cond(label2,label1,p->sibling);
	}
	else{
		if(strcmp(p->sibling->name,"RELOP") == 0){
			Operand temp1 = (Operand)malloc(sizeof(Operand_));
			Operand temp2 = (Operand)malloc(sizeof(Operand_));
			create_temp(temp1);
			create_temp(temp2);

			translate_Exp(p,temp1);
			translate_Exp(p->sibling->sibling,temp2);

			int op_type = 0;
			if(strcmp(p->sibling->type_char,"==") == 0){
				op_type = 1;
			}
			else if(strcmp(p->sibling->type_char,">") == 0){
				op_type = 2;
			}
			else if(strcmp(p->sibling->type_char,"<") == 0){
				op_type = 3;
			}
			else if(strcmp(p->sibling->type_char,">=") == 0){
				op_type = 4;
			}
			else if(strcmp(p->sibling->type_char,"<=") == 0){
				op_type = 5;
			}
			else if(strcmp(p->sibling->type_char,"!=") == 0){
				op_type = 6;
			}

			create_ifGotoCode(temp1,temp2,op_type,label1);
			create_GotoCode(label2);
		}
		else if(strcmp(p->sibling->name,"AND") == 0){
			int label3 = create_label();

			translate_Cond(label3,label2,p);
			create_labelCode(label3);
			translate_Cond(label1,label2,p->sibling->sibling);

		}
		else if(strcmp(p->sibling->name,"OR") == 0){
			int label3 = create_label();

			translate_Cond(label1,label3,p);
			create_labelCode(label3);
			translate_Cond(label1,label2,p->sibling->sibling);
		}
	}
}

void translate_Def(Node* p,structCode s){
	p = p->children;
	translate_Specifier(p);
	if(strcmp(p->children->name,"TYPE") == 0){
		translate_DecList(p->sibling,s);
	}
}

void translate_DecList(Node* p,structCode s){
	p = p->children;
	if(p->sibling == NULL){
		translate_Dec(p,s);
	}
	else{
		translate_Dec(p,s);
		translate_DecList(p->sibling->sibling,s);
	}
}

void translate_Dec(Node* p,structCode s){
	p = p->children;
	if(p->sibling == NULL){
		translate_VarDec(p,s);
	}
	else{
		Node* p1 = p->children;
		if(p1->sibling == NULL){
			Operand var = create_varible(p1);
			Operand temp = (Operand)malloc(sizeof(Operand_));
			create_temp(temp);
			translate_Exp(p->sibling->sibling,temp);
			create_AssignCode(var,temp);
		}
	}
}

void translate_Exp(Node* p,Operand temp){
	p = p->children;
	if(p->sibling == NULL){
		if(temp == NULL){
			return;
		}
		if(strcmp(p->name,"INT") == 0){
			temp->kind = CONSTANT;
			temp->u.value = p->type_int;
			temp_num_minus();
			//printf("%d\n",p->type_int);
			
		}
		else if(strcmp(p->name,"ID") == 0){
			temp->kind = VARIABLE;
			strcpy(temp->u.var_name,p->type_char);
			if(get_varible(p->type_char) == -1){
				//printf("NO SUCH VARIABLE!\n");
				return;
			}

			int n = get_varible(p->type_char);
			if(varible[n-1].kind == STRUCTURE){
				temp->kind = STRUCTURE;
			}
			temp_num_minus();
			temp->u.var_no = get_varible(p->type_char);
		}
		else if(strcmp(p->name,"FLOAT") == 0){
			return;
		}
	}
	else if(p->sibling->sibling == NULL){
		if(strcmp(p->name,"MINUS") == 0){
			Operand temp1 = (Operand)malloc(sizeof(Operand_));
			Operand temp2 = (Operand)malloc(sizeof(Operand_));
			create_temp(temp2);

			temp1->kind = CONSTANT;
			temp1->u.value = 0;
			translate_Exp(p->sibling,temp2);
			create_CalCode(temp1,temp2,temp,2);
		}
		else{

		}
	}
	else if(p->sibling->sibling->sibling == NULL){
		if(strcmp(p->sibling->name,"ASSIGNOP") == 0){
			Node* ch = p->children;
			if(ch->sibling == NULL){
				Operand var = (Operand)malloc(sizeof(Operand_));
				int var_no = get_varible(ch->type_char);

				var->kind = VARIABLE;
				var->u.var_no = var_no;

				Operand temp1 = (Operand)malloc(sizeof(Operand_));
				create_temp(temp1);

				translate_Exp(p->sibling->sibling,temp1);
				create_AssignCode(var,temp1);
			}
			else if(ch->sibling->sibling->sibling == NULL){
				/*struct assign*/
				Operand temp1 = (Operand)malloc(sizeof(Operand_));
				create_temp(temp1);

				translate_Exp(p,temp1);
				Operand temp2 = (Operand)malloc(sizeof(Operand_));
				create_temp(temp2);
				translate_Exp(p->sibling->sibling,temp2);
				create_LeftAddrCode(temp1,temp2);
			}
			else{
				/*array assign*/
				Operand temp1 = (Operand)malloc(sizeof(Operand_));
				create_temp(temp1);

				translate_Exp(p,temp1);
				Operand temp2 = (Operand)malloc(sizeof(Operand_));
				create_temp(temp2);
				translate_Exp(p->sibling->sibling,temp2);
				create_LeftAddrCode(temp1,temp2);
			}
		}
		else if(strcmp(p->name,"ID") == 0){
			if(strcmp(p->type_char,"read") == 0){
				create_callCode(temp,p->type_char);
			}
			else{
				create_callCode(temp,p->type_char);
			}
		}
		else if(strcmp(p->sibling->name,"DOT") == 0){
			Node* ch = p->children;
			Operand var = (Operand)malloc(sizeof(Operand_));
			int var_no = get_varible(ch->type_char);
			var->kind = STRUCTURE;
			var->u.var_no = var_no;
			if(varible[var_no-1].u.is_param == 0){
				Node* ch1 = p->sibling->sibling;
				int gap = find_in_struct(ch1->type_char,var_no);
				Operand temp1 = (Operand)malloc(sizeof(Operand_));
				temp1->kind = TEMP;
				temp1->u.var_no = temp->u.var_no;

				temp->kind = ADDRESS;
				create_GetAddrCode(temp1,var,gap);
			}
			else{
				Node* ch1 = p->sibling->sibling;

				int gap = find_in_struct(ch1->type_char,var_no);
				if(gap != 0){
					Operand temp1 = (Operand)malloc(sizeof(Operand_));
					create_temp(temp1);
					create_GapCode(temp1,var,gap);

					create_RightAddrCode(temp,temp1);
				}
				else{
					create_RightAddrCode(temp,var);
				}


			}
		}
		else if(strcmp(p->sibling->name,"Exp") == 0){
			translate_Exp(p->sibling,temp);
		}
		else if(strcmp(p->sibling->name,"RELOP") == 0 || strcmp(p->sibling->name,"AND") == 0 || strcmp(p->sibling->name,"OR")==0){

		}
		else{
			Operand temp1 = (Operand)malloc(sizeof(Operand_));
			Operand temp2 = (Operand)malloc(sizeof(Operand_));
			create_temp(temp1);
			translate_Exp(p,temp1);
			create_temp(temp2);

			translate_Exp(p->sibling->sibling,temp2);

			int type = 0;
			if(strcmp(p->sibling->name,"PLUS") == 0){
				type = 1;
			}
			else if(strcmp(p->sibling->name,"MINUS") == 0){
				type = 2;
			}
			else if(strcmp(p->sibling->name,"STAR") == 0){
				type = 3;
			}
			else if(strcmp(p->sibling->name,"DIV") == 0){
				type = 4;
			}
			create_CalCode(temp1,temp2,temp,type);
		}
	}
	//children num = 4
	else{
		if(strcmp(p->name,"ID") == 0){
			if(strcmp(p->type_char,"write") == 0){
				Node* ch = p->sibling->sibling->children;

				Operand temp1 = (Operand)malloc(sizeof(Operand_));
				create_temp(temp1);

				translate_Exp(ch,temp1);
				codes[current].kind = WRITE;
				codes[current].sign = 1;	
				codes[current].u.op = temp1;
				current++;
			}
			else{
				translate_Args(p->sibling->sibling);
				func_num++;

				create_callCode(temp,p->type_char);
			}
		}
		else{
			Node* ch = p->children;
			if(ch->sibling != NULL){
				printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
			}
			else{
				int var_no = get_varible(ch->type_char);
				//printf("%d\n", var_no);
				Operand var = (Operand)malloc(sizeof(Operand_));
				var->kind = ARRAY;
				var->u.var_no = var_no;

				Operand temp1 = (Operand)malloc(sizeof(Operand_));
				create_temp(temp1);
				translate_Exp(p->sibling->sibling,temp1);

				Operand temp2 = (Operand)malloc(sizeof(Operand_));
				create_temp(temp2);
				create_arrayGapCode(temp2,temp1);

				Operand temp3 = (Operand)malloc(sizeof(Operand_));
				temp3->u.var_no = temp->u.var_no;
				temp3->kind = TEMP;

				temp->kind = ADDRESS;
				create_tempGapCode(temp3,var,temp2);
			}

		}
	}
}

void translate_Args(Node* p){
	p = p->children;
	if(p->sibling == NULL){
		Operand temp1 = (Operand)malloc(sizeof(Operand_));
		create_temp(temp1);
		
		translate_Exp(p,temp1);

		args_list[func_num][args_num[func_num]] = *temp1;
		args_num[func_num]++;
		for(int i = args_num[func_num]-1;i >= 0;i--){
			create_Args(&args_list[func_num][i]);
		}
	}
	else{
		Operand temp1 = (Operand)malloc(sizeof(Operand_));
		create_temp(temp1);

		translate_Exp(p,temp1);
		args_list[func_num][args_num[func_num]] = *temp1;
		args_num[func_num]++;
		translate_Args(p->sibling->sibling);
	}
}

void print_codes(){
	int i = 0;
	while(codes[i].sign != 0){
		if(codes[i].kind == FUNCTION){
			printf("FUNCTION %s :\n",codes[i].u.funcName);
		}
		if(codes[i].kind == ASSIGN){
			print_Operand(codes[i].u.assign.left);
			printf(" := ");
			print_Operand(codes[i].u.assign.right);
			printf("\n");
		}
		if(codes[i].kind == ADD){
			print_Operand(codes[i].u.binop.result);
			printf(" := ");
			print_Operand(codes[i].u.binop.op1);
			printf(" + ");
			print_Operand(codes[i].u.binop.op2);
			printf("\n");
		}
		if(codes[i].kind == SUB){
			print_Operand(codes[i].u.binop.result);
			printf(" := ");
			print_Operand(codes[i].u.binop.op1);
			printf(" - ");
			print_Operand(codes[i].u.binop.op2);
			printf("\n");
		}
		if(codes[i].kind == MUL){
			print_Operand(codes[i].u.binop.result);
			printf(" := ");
			print_Operand(codes[i].u.binop.op1);
			printf(" * ");
			print_Operand(codes[i].u.binop.op2);
			printf("\n");
		}
		if(codes[i].kind == DIV){
			print_Operand(codes[i].u.binop.result);
			printf(" := ");
			print_Operand(codes[i].u.binop.op1);
			printf(" / ");
			print_Operand(codes[i].u.binop.op2);
			printf("\n");
		}
		if(codes[i].kind == CALL){
			print_Operand(codes[i].u.callop.op);
			printf(" := CALL ");
			printf("%s\n",codes[i].u.callop.func_name);
		}
		if(codes[i].kind == RETURN){
			printf("RETURN ");
			print_Operand(codes[i].u.op);
			printf("\n");
		}
		if(codes[i].kind == READ){
			printf("READ ");
			print_Operand(codes[i].u.callop.op);
			printf("\n");
		}
		if(codes[i].kind == WRITE){
			printf("WRITE ");
			print_Operand(codes[i].u.op);
			printf("\n");
		}
		if(codes[i].kind == IF){
			printf("IF ");
			print_Operand(codes[i].u.ifGotoOp.left);
			print_relop(codes[i].u.ifGotoOp.type);
			print_Operand(codes[i].u.ifGotoOp.right);
			printf(" GOTO label");
			printf("%d\n",codes[i].u.ifGotoOp.label);
		}
		if(codes[i].kind == GOTO){
			printf("GOTO label");
			printf("%d\n",codes[i].u.label);
		}
		if(codes[i].kind == LABEL){
			printf("LABEL label%d :\n",codes[i].u.label);
		}
		if(codes[i].kind == ARGS){
			printf("ARG ");
			if(codes[i].u.op->kind == STRUCTURE){
				printf("&");
			}
			print_Operand(codes[i].u.op);
			printf("\n");
		}
		if(codes[i].kind == PARAM){
			printf("PARAM ");
			print_Operand(codes[i].u.op);
			printf("\n");
		}
		if(codes[i].kind == DEC){
			printf("DEC ");
			print_Operand(codes[i].u.decop.op);
			printf(" %d\n",codes[i].u.decop.size);
		}
		if(codes[i].kind == GETADDR){
			print_Operand(codes[i].u.getaddrop.left);
			printf(" := &");
			print_Operand(codes[i].u.getaddrop.right);
			if(codes[i].u.getaddrop.gap != 0){
				printf(" + ");
				printf("#%d",codes[i].u.getaddrop.gap);
			}
			printf("\n");
		}
		if(codes[i].kind == LEFTADDR){
			print_Operand(codes[i].u.assign.left);
			printf(" := ");
			print_Operand(codes[i].u.assign.right);
			printf("\n");
		}
		if(codes[i].kind == GAP){
			print_Operand(codes[i].u.getaddrop.left);
			printf(" := ");
			print_Operand(codes[i].u.getaddrop.right);
			printf(" + ");
			printf("#%d\n",codes[i].u.getaddrop.gap);
		}
		if(codes[i].kind == TEMPGAP){
			print_Operand(codes[i].u.binop.result);
			printf(" := &");
			print_Operand(codes[i].u.binop.op1);
			printf(" + ");
			print_Operand(codes[i].u.binop.op2);
			printf("\n");
		}
		if(codes[i].kind == ARRAYGAP){
			print_Operand(codes[i].u.assign.left);
			printf(" := ");
			print_Operand(codes[i].u.assign.right);
			printf(" * #4\n");
		}
		if(codes[i].kind == RIGHTADDR){
			print_Operand(codes[i].u.assign.left);
			printf(" := *");
			print_Operand(codes[i].u.assign.right);
			printf("\n");
		}
		i++;
	}
}

void create_FuncCode(char* name){
	codes[current].kind = FUNCTION;
	codes[current].sign = 1;
	strcpy(codes[current].u.funcName,name);
	current++;
}

void create_ParamCode(Operand op){
	codes[current].kind = PARAM;
	codes[current].sign = 1;
	codes[current].u.op = op;
	current++;
}

void create_callCode(Operand op,char* name){
	if(strcmp(name,"read") == 0){
		codes[current].kind = READ;
		codes[current].sign = 1;
		codes[current].u.callop.op = op;
		current++;
	}
	else{
		codes[current].kind = CALL;
		codes[current].sign = 1;
		codes[current].u.callop.op = op;
		strcpy(codes[current].u.callop.func_name,name);
		current++;
	}
}

void create_Args(Operand op){
	codes[current].kind = ARGS;
	codes[current].u.op = op;
	codes[current].sign = 1;
	current++;
}

void create_ifGotoCode(Operand left,Operand right,int op_type,int label){
	//printf("=====IFGOTO==========\n");
	codes[current].kind = IF;
	codes[current].u.ifGotoOp.left = left;
	codes[current].u.ifGotoOp.right = right;
	codes[current].u.ifGotoOp.type = op_type;
	codes[current].u.ifGotoOp.label = label;
	codes[current].sign = 1;
	current++;
}

void create_GotoCode(int label){
	codes[current].kind = GOTO;
	codes[current].u.label = label;
	codes[current].sign = 1;
	current++;
}

void create_labelCode(int label){
	codes[current].kind = LABEL;
	codes[current].u.label = label;
	codes[current].sign = 1;
	current++;
}

void create_DecCode(structCode s,Operand op){
	codes[current].kind = DEC;
	if(s != NULL){
		codes[current].u.decop.size = s->size;
	}
	else{
		codes[current].u.decop.size = op->u.array_size;
	}
	codes[current].u.decop.op = op;
	codes[current].sign = 1;
	current++;
}

void create_ReturnCode(Operand op){
	codes[current].kind = RETURN;
	codes[current].sign = 1;
	codes[current].u.op = op;
	current++;
}

void create_AssignCode(Operand left,Operand right){
	codes[current].kind = ASSIGN;
	codes[current].sign = 1;
	codes[current].u.assign.left = left;
	codes[current].u.assign.right = right;
	current++;
}

void create_LeftAddrCode(Operand left,Operand right){
	codes[current].kind = LEFTADDR;
	codes[current].sign = 1;
	codes[current].u.assign.left = left;
	codes[current].u.assign.right = right;
	current++;
}

void create_CalCode(Operand left,Operand right,Operand result,int type){
	if(type == 1){
		codes[current].kind = ADD;
	}
	else if(type == 2){
		codes[current].kind = SUB;
	}
	else if(type ==3){
		codes[current].kind = MUL;
	}
	else{
		codes[current].kind = DIV;	
	}
	codes[current].u.binop.result = result;
	codes[current].u.binop.op1 = left;
	codes[current].u.binop.op2 = right;
	codes[current].sign = 1;
	current++;
}

void create_GetAddrCode(Operand temp,Operand var,int gap){
	codes[current].kind = GETADDR;
	codes[current].u.getaddrop.gap = gap;
	codes[current].u.getaddrop.left = temp;
	codes[current].u.getaddrop.right = var;
	codes[current].sign = 1;
	current++;
}

void create_GapCode(Operand temp,Operand var,int gap){
	codes[current].kind = GAP;
	codes[current].u.getaddrop.gap = gap;
	codes[current].u.getaddrop.left = temp;
	codes[current].u.getaddrop.right = var;
	codes[current].sign = 1;
	current++;
}

void create_arrayGapCode(Operand left,Operand right){
	codes[current].kind = ARRAYGAP;
	codes[current].u.assign.left = left;
	codes[current].u.assign.right = right;
	codes[current].sign = 1;
	current++;
}

void create_tempGapCode(Operand temp,Operand var,Operand temp1){
	codes[current].kind = TEMPGAP;
	codes[current].u.binop.result = temp;
	codes[current].u.binop.op1 = var;
	codes[current].u.binop.op2 = temp1;
	codes[current].sign = 1;
	current++;
}

void create_RightAddrCode(Operand temp,Operand var){
	codes[current].kind = RIGHTADDR;
	codes[current].u.assign.left = temp;
	codes[current].u.assign.right = var;
	codes[current].sign = 1;
	current++;
}

void print_Operand(Operand op){
	if(op != NULL){
		switch(op->kind) {
			case VARIABLE:printf("v%d",op->u.var_no);break;
			case CONSTANT:printf("#%d",op->u.value);break;
			case ADDRESS:printf("*t%d",op->u.var_no);break;
			case TEMP:printf("t%d",op->u.var_no);break;
			case STRUCTURE:printf("v%d",op->u.var_no);break;
			case ARRAY:printf("v%d",op->u.var_no);break;
			default:return;
		}
	}
}