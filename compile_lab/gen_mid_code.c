#include <string.h>
#include <stdarg.h>
#include "stdio.h"
#include "stdlib.h"
#include "semantic.c"
void translate_if_else(struct node* _if);
void translate_if(struct node* _if);
void translate_while(struct node* _while);
void translate_Exp(struct node* Exp,char* t);
char* get_ir_num(char* num);
char* _path;  // 输出中间代码的相对路径
struct InterCode* IR;
struct relation* r;
struct p* params;
int counter1 = 1;
int counter2=1;

/* 单条中间代码的数据结构 */
struct InterCode{
    char* symbol1;
    char* symbol2;
    char* symbol3;
    char* symbol4;
    char* symbol5;
    char* symbol6;
    struct InterCode* next;
};
/* ir 和源代码变量的对应关系 */
struct relation{
    char* src_var;
    char* ir_var;
    struct relation* next;
};
/* 记录函数调用的所有实参 */
struct p{
    char* ir_var;
    struct p* next;
    struct p* pre;
};

/* 向实参列表中加入实参 */
void add_ir_param(struct p* param){
    if(params==NULL) params = param;
    else{
        struct p* index=params;
        while(index->next){
            index=index->next;
        }
        index->next=param;
        param->pre=index;
    }
}
/* 打印中间代码 */
void print_code(struct InterCode* code,FILE*fp){
    if(code->symbol1!=NULL) {
        printf("%s", code->symbol1);
        fputs(code->symbol1,fp);
    }
    if(code->symbol2!=NULL){
        printf(" ");
        fputs(" ",fp);
        printf("%s", code->symbol2);
        fputs(code->symbol2,fp);
    }
    if(code->symbol3!=NULL){
        printf(" ");
        fputs(" ",fp);
        printf("%s", code->symbol3);
        fputs(code->symbol3,fp);
    }
    if(code->symbol4!=NULL){
        printf(" ");
        fputs(" ",fp);
        printf("%s", code->symbol4);
        fputs(code->symbol4,fp);
    }
    if(code->symbol5!=NULL){
        printf(" ");
        fputs(" ",fp);
        printf("%s", code->symbol5);
        fputs(code->symbol5,fp);
    }
    if(code->symbol6!=NULL){
        printf(" ");
        fputs(" ",fp);
        printf("%s", code->symbol6);
        fputs(code->symbol6,fp);
    }
    printf("\n");
    fputs("\n",fp);
}
/* 打印一条关系 */
void print_re(struct relation* re){
    printf("%s %s\n",re->src_var, re->ir_var);
}
/* 产生一个新的符号 */
char* new_temp(){
    int bit_num=0;
    char* res=(char*)malloc(10);
    int temp=counter1;
    while(temp){
        bit_num++;
        temp/=10;
    }
    temp=counter1;
    for(int i=bit_num;i>0;i--){
        res[i]=temp%10+'0';
        temp/=10;
    }
    res[0]='t';
    res[bit_num+1]='\0';
    counter1++;
    return res;
}
char* new_label(){
    int bit_num=0;
    char* res=(char*)malloc(10);
    int temp=counter1;
    while(temp){
        bit_num++;
        temp/=10;
    }
    temp=counter2;
    for(int i=bit_num+4;i>4;i--){
        res[i]=temp%10+'0';
        temp/=10;
    }
    res[0]='l';
    res[1]='a';
    res[2]='b';
    res[3]='e';
    res[4]='l';
    res[bit_num+5]='\0';
    counter2++;
    return res;
}

/* 向一条中间代码插入符号 */
void add_symbol(struct InterCode* code, char* symbol){
    if(code->symbol1==NULL){
        code->symbol1=symbol;
        return;
    }else if(code->symbol2==NULL){
        code->symbol2=symbol;
        return;
    }else if(code->symbol3==NULL){
        code->symbol3=symbol;
        return;
    }else if(code->symbol4==NULL){
        code->symbol4=symbol;
        return;
    }else if(code->symbol5==NULL){
        code->symbol5=symbol;
        return;
    } else{
        code->symbol6=symbol;
        return;
    }
}
/* 生成单条中间代码 */
struct InterCode* gen_InterCode(int num, ...){
    struct InterCode* code = (struct InterCode*)malloc(sizeof(struct InterCode));
    code->next=NULL;
    va_list valist;
    va_start(valist, num);
    for(int i=1;i<=num;i++){
        char* symbol = va_arg(valist, char*);
        add_symbol(code, symbol);
    }
    return code;
}
/* 插入一条中间代码 */
void add_code(struct InterCode* code){
    if(IR==NULL)IR=code;
    else{
        struct InterCode* index=IR;
        while(index->next)index=index->next;
        index->next=code;
    }
}
/* 生成一条对应关系 */
struct relation* gen_re(char* src_var, char* ir_var){
    struct relation* re=(struct relation*)malloc(sizeof(struct relation));
    re->src_var=src_var;
    re->ir_var=ir_var;
    re->next=NULL;
}
/* 插入一条关系 */
void add_relation(struct relation* re){
    if(r==NULL)r=re;
    else{
        struct relation* index=r;
        while(index->next)index=index->next;
        index->next=re;
    }
}
/* 在对应表中找出源代码变量对应的中间代码变量 */
char* get_ir_by_src(char* src_var){
    struct relation* index=r;
    while(index){
        if(!strcmp(index->src_var,src_var))return index->ir_var;
        index=index->next;
    }
    return NULL;
}
/* 将read函数和write函数添加到符号表中 */
void add_read_write(){
    struct id_entry* r_entry = (struct id_entry*)malloc(sizeof(struct id_entry));
    r_entry->id = "read";
    r_entry->type = "int";
    r_entry->kind = "FUNC";
    r_entry->func = "null";
    r_entry->struct_name = "null";
    r_entry->next = NULL;
    r_entry->array_vec = NULL;
    add_entry(r_entry); // 添加read
    struct id_entry* w_entry = (struct id_entry*)malloc(sizeof(struct id_entry));
    w_entry->id = "write";
    w_entry->type = "int";
    w_entry->kind = "FUNC";
    w_entry->func = "null";
    w_entry->struct_name = "null";
    w_entry->next = NULL;
    w_entry->array_vec = NULL;
    add_entry(w_entry);  // 添加write
    struct id_entry* w_param = (struct id_entry*)malloc(sizeof(struct id_entry));
    w_param->id = "any";
    w_param->type = "int";
    w_param->kind = "VAR";
    w_param->func = "write";
    w_param->struct_name = "null";
    w_param->next = NULL;
    w_param->array_vec = NULL;
    add_entry(w_param);  // 添加write的参数
}
/* 翻译函数形参 */
void translate_args(struct node* VarList){
    struct node* temp=VarList;
    if(temp==NULL)return;
    if(!strcmp(temp->id,"ParamDec")){
        char* t=new_temp();
        char* param_name=temp->child_nodes->right_brother->child_nodes->value;
        struct InterCode* code = gen_InterCode(2,"PARAM",t);
        add_code(code);
        struct relation* re= gen_re(param_name,t);
        add_relation(re);
    }
    translate_args(VarList->child_nodes);
    translate_args(VarList->right_brother);
}
/* 翻译函数定义 */
void translate_func_def(struct node* ExtDef){
    struct node* FunDec=ExtDef->child_nodes->right_brother;
    char* func_name = FunDec->child_nodes->value;
    struct InterCode* code = gen_InterCode(3, "FUNCTION", func_name, ":");
    add_code(code);
    struct node* VarList=FunDec->child_nodes->right_brother->right_brother;
    if(!strcmp(VarList->id, "VarList")){  // 函数有形参
        translate_args(VarList);
    }
}
/* 将对应表中遍历郎赋值 */
void assign(char* src_var,char* value){
    struct relation* index=r;
    while(index){
        if(!strcmp(index->src_var,src_var)){
            struct InterCode* code= gen_InterCode(3,index->ir_var,":=",value);
            add_code(code);
        }
        index=index->next;
    }
}
/* 翻译赋值语句 */
void translate_assign(struct node* Exp,char*t){
    struct node* left=Exp->child_nodes;
    struct node* right=left->right_brother->right_brother;
    if(!strcmp(right->child_nodes->id,"ID")&&right->child_nodes->right_brother&&!strcmp(right->child_nodes->value,"read")){
        if(!strcmp(left->child_nodes->id,"ID")&&!left->child_nodes->right_brother){
            char* src_var=left->child_nodes->value;
            char* ir_var= get_ir_by_src(src_var);
            if(ir_var) {
                struct InterCode *code = gen_InterCode(2, "READ", ir_var);
                add_code(code);
            } else{
                struct InterCode *code = gen_InterCode(2, "READ", t);
                add_code(code);
                struct relation* re= gen_re(src_var,t);
                add_relation(re);
            }
        }
        return;
    }
    char* t2=new_temp();
    translate_Exp(right,t2);
    if(!strcmp(left->child_nodes->id,"ID")&&!left->child_nodes->right_brother){ // 左侧是普通变量
        char* src_var=left->child_nodes->value;
        assign(src_var,t2);
        struct InterCode* code= gen_InterCode(3,t,":=",t2);
        add_code(code);
        struct relation* re= gen_re(src_var,t);
        add_relation(re);
    }
}
/* 翻译函数实参 */
void translate_Args(struct node* Args){
    if(Args==NULL)return;
    struct node* temp=Args;
    if(!strcmp(temp->id,"Args")){
        temp=temp->child_nodes;
        char* t=new_temp();
        translate_Exp(temp,t);
        struct p* param=(struct p*)malloc(sizeof(struct p));
        param->ir_var=t;
        param->next=NULL;
        param->pre=NULL;
        add_ir_param(param);
    }
    translate_Args(Args->child_nodes);
    translate_Args(Args->right_brother);
}
/* 翻译函数调用 */
void translate_func_call(struct node* Exp, char* t){
    struct node* child=Exp->child_nodes;
    char* func_name=child->value;
    if(!strcmp(func_name,"read"))return; // 调用read函数在赋值阶段分析
    if(!strcmp(child->right_brother->right_brother->id,"Args")){ // 带参数
        if(!strcmp(func_name,"write")){
            struct node* arg = child->right_brother->right_brother->child_nodes->child_nodes;
            char* num=arg->value;
            char* type=arg->id;
            if(!strcmp(type,"INT")) num = get_ir_num(num);
            else num= get_ir_by_src(num);
            char* t1=new_temp();
            struct InterCode* code= gen_InterCode(3,t1,":=", num);
            add_code(code);
            struct InterCode* code1= gen_InterCode(2,"WRITE",t1);
            add_code(code1);
        } else{
            struct node* Args=child->right_brother->right_brother;
            params=NULL;
            translate_Args(Args);
            struct p* index=params;
            while(index->next)index=index->next;
            while(index){
                struct InterCode* code= gen_InterCode(2,"ARG",index->ir_var);
                add_code(code);
                index=index->pre;
            }
            struct InterCode* code= gen_InterCode(4,t,":=","CALL",func_name);
            add_code(code);
        }
    } else{ // 无参数

    }
}
/* 翻译二元运算 */
void translate_dual(struct node* Exp,char* t){
    struct node* left=Exp->child_nodes;
    struct node* right=left->right_brother->right_brother;
    char* t1=new_temp();
    char* t2=new_temp();
    translate_Exp(left,t1);
    translate_Exp(right,t2);
    char* op=left->right_brother->value;
    if(!strcmp(op,"- "))op="-";
    struct InterCode* code= gen_InterCode(5,t,":=",t1,op,t2);
    add_code(code);
}
/* 获得中间代码中的整数表示 */
char* get_ir_num(char* num){
    char* res=(char*)malloc(1+strlen(num));
    strcpy(res,"#");
    strcat(res,num);
    return res;
}
/* 翻译表达式 */
void translate_Exp(struct node* Exp,char* t){
    if(Exp->child_nodes->right_brother&&!strcmp(Exp->child_nodes->right_brother->id,"ASSIGNOP")){  // 赋值语句
        translate_assign(Exp, t);
    } else if(!strcmp(Exp->child_nodes->id,"ID")&&!Exp->child_nodes->right_brother){  // 普通变量
        char* var_name=Exp->child_nodes->value;
        char* ir_name= get_ir_by_src(var_name);
        struct InterCode* code=gen_InterCode(3,t,":=",ir_name);
        add_code(code);
    } else if(!strcmp(Exp->child_nodes->id,"INT")){  // 整数
        struct InterCode* code= gen_InterCode(3,t,":=", get_ir_num(Exp->child_nodes->value));
        add_code(code);
    } else if(!strcmp(Exp->child_nodes->id,"ID") && Exp->child_nodes->right_brother){  // 函数调用
        translate_func_call(Exp, t);
    } else if(Exp->child_nodes->right_brother
    && !strcmp(Exp->child_nodes->id,"Exp")
    && strcmp(Exp->child_nodes->right_brother->id,"LB")!=0
    && strcmp(Exp->child_nodes->right_brother->id,"DOT")!=0){  // 二元运算
        translate_dual(Exp,t);
    } else if(!strcmp(Exp->child_nodes->id,"LP")){  // 括号
        translate_Exp(Exp->child_nodes->right_brother,t);
    }
}
/* 翻译条件表达式 */
void translate_cond(struct node* cond, char* label_true, char* label_false){
    struct node* Exp=cond->child_nodes;
    if(Exp->right_brother&&!strcmp(Exp->right_brother->id,"RELOP")){ // Exp1 RELOP Exp2
        struct node* exp1=Exp;
        struct node* exp2=Exp->right_brother->right_brother;
        char* t1=new_temp();
        char* t2=new_temp();
        translate_Exp(exp1,t1);
        translate_Exp(exp2,t2);
        char* op=exp1->right_brother->value;
        struct InterCode* code1= gen_InterCode(6,"IF",t1,op,t2,"GOTO",label_true);
        add_code(code1);
        struct InterCode* code2= gen_InterCode(2,"GOTO",label_false);
        add_code(code2);
    } else if(!strcmp(Exp->id,"NOT")){ // NOT Exp
        Exp=Exp->right_brother;
        translate_cond(Exp,label_false,label_true);
    } else if(!strcmp(Exp->id,"LP")){ // (Exp)
        translate_cond(Exp->right_brother,label_true,label_false);
    } else if(Exp->right_brother&&!strcmp(Exp->right_brother->id,"AND")){ // Exp1 AND Exp2
        char* label1=new_label();
        struct node* Exp2=Exp->right_brother->right_brother;
        translate_cond(Exp,label1,label_false);
        struct InterCode* code=gen_InterCode(3,"LABEL",label1,":");
        add_code(code);
        translate_cond(Exp2,label_true,label_false);
    } else if(Exp->right_brother&&!strcmp(Exp->right_brother->id,"OR")){
        char* label=new_label();
        struct node* Exp2=Exp->right_brother->right_brother;
        translate_cond(Exp,label_true,label);
        struct InterCode* code= gen_InterCode(3,"LABEL",label,":");
        add_code(code);
        translate_cond(Exp2,label_true,label_false);
    }
}
/* 判断if后是否有else */
int has_else(struct node* _if){
    if(_if->right_brother->right_brother->right_brother->right_brother->right_brother)return 1;
    return 0;
}
/* 翻译语句 */
void translate_Stmt(struct node* Stmt){
    struct node* temp=Stmt;
    if(!strcmp(temp->child_nodes->id,"Exp")){  // Exp SEMI
        struct node* Exp=temp->child_nodes;
        char* t=new_temp();
        translate_Exp(Exp,t);
    }else if(!strcmp(temp->child_nodes->id,"CompSt")){
        /* CompSt todo */
    } else if(!strcmp(temp->child_nodes->id,"RETURN")){
        char* t=new_temp();
        struct node* Exp=temp->child_nodes->right_brother;
        translate_Exp(Exp,t);
        struct InterCode* code= gen_InterCode(2,"RETURN",t);
        add_code(code);
    } else if(!strcmp(temp->child_nodes->id,"IF")){
        struct node* _if=temp->child_nodes;
        int hasElse= has_else(_if);
        if(hasElse){ // 有else在后面
            translate_if_else(_if);
        } else{ // 没有else
            translate_if(_if);
        }
    } else if(!strcmp(temp->child_nodes->id,"WHILE")){ // while
        struct node* _while=temp->child_nodes;
        translate_while(_while);
    }
}
/* 翻译 if else 结构 */
void translate_if_else(struct node* _if){
    struct node* cond = _if->right_brother->right_brother;
    char* label_true = new_label();
    char* label_false=new_label();
    char* label3=new_label();
    translate_cond(cond,label_true,label_false);
    struct node* Stmt1=_if->right_brother->right_brother->right_brother->right_brother;
    struct node* Stmt2=Stmt1->right_brother->right_brother;
    struct InterCode* code1= gen_InterCode(3,"LABEL",label_true,":");
    add_code(code1);
    translate_Stmt(Stmt1);
    struct InterCode* code2= gen_InterCode(2,"GOTO",label3);
    add_code(code2);
    struct InterCode* code3= gen_InterCode(3,"LABEL",label_false,":");
    add_code(code3);
    translate_Stmt(Stmt2);
    struct InterCode* code4= gen_InterCode(3, "LABEL",label3,":");
    add_code(code4);
}
/* 翻译 if 结构 */
void translate_if(struct node* _if){
    struct node* cond=_if->right_brother->right_brother;
    char* label_true = new_label();
    char* label_false = new_label();
    translate_cond(cond,label_true,label_false);
    struct InterCode* code1= gen_InterCode(3,"LABEL",label_true,":");
    add_code(code1);
    struct node* Stmt=cond->right_brother->right_brother;
    translate_Stmt(Stmt);
    struct InterCode* code2= gen_InterCode(3,"LABEL",label_false,":");
    add_code(code2);
}
/* 翻译 while 结构 */
void translate_while(struct node* _while){
    char* label1=new_label();
    char* label2=new_label();
    char* label3=new_label();
    struct node* cond=_while->right_brother->right_brother;
    struct node* Stmt=cond->right_brother->right_brother;
    struct InterCode* code1= gen_InterCode(3,"LABEL",label1,":");
    add_code(code1);
    translate_cond(cond,label2,label3);
    struct InterCode* code2= gen_InterCode(3,"LABEL",label2,":");
    add_code(code2);
    translate_Stmt(Stmt);
    struct InterCode* code3= gen_InterCode(2,"GOTO",label1);
    add_code(code3);
    struct InterCode* code4= gen_InterCode(3,"LABEL",label3,":");
    add_code(code4);
}
/* 总翻译程序 */
void translate(struct node* root){
    if(root==NULL)return;
    struct node* temp = root;
    char* id=temp->id;
    if(!strcmp(id, "ExtDef")){  /* 高级定义 */
        int res= fun_or_struct(temp);
        if(res==0){ // 函数定义
            translate_func_def(temp);
        } else{

        }
    } else if(!strcmp(id,"StmtList")){  /* 语句 */
        temp=temp->child_nodes;
        translate_Stmt(temp);
        char* child_id=temp->child_nodes->id;
    }
    translate(root->child_nodes);
    translate(root->right_brother);
}
/* 打印对应关系 */
void print_relations(){
    struct relation* re=r;
    while(re){
        print_re(re);
        re=re->next;
    }
}
/* 打印结果中间代码 */
void print_IR(FILE* fp){
    struct InterCode* code=IR;
    while(code){
        print_code(code,fp);
        code=code->next;
    }
}
/* 中间代码生成 */
void gen_mid_code(struct node* root){
    char* dir = "/home/lay/C_or_CPP/compile_lab/";
    char* ir_path = (char*)malloc(strlen(dir)+strlen(_path));
    strcpy(ir_path, dir);
    strcat(ir_path, _path);  // 拼接文件的绝对路径
    FILE* _fp = fopen(ir_path, "w");
    /* 翻译程序 */
    translate(root);
    printf("变量对应表\n");
    print_relations();
    printf("中间代码\n");
    print_IR(_fp);
    fclose(_fp);
}
