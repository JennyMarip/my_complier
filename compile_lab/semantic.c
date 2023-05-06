#include "stdio.h"
#include "stdlib.h"

// 多叉树的节点
struct node{
    int is_null;  // 是否为空
    char* id;  //语(词)法单元名
    char* value; //值（如果是id或type）
    int line_no;  //第一个词素的行号
    struct node* child_nodes;  //指向第一个子结点的指针，所有子结点的链表
    struct node* right_brother;  //指向右兄弟的指针
};

/* 两个结构体是创建符号表的数据结构 */
// 数组的维
struct dimension{
    int size; // 元素个数
    struct dimension* next; // 指向下一维
};
// 符号表的条目
struct id_entry{
    char* id;  // 符号名
    char* kind;  // 符号种属 (VAR,FUNC,ARRAY,STRUCT,STRUCT_FIELD)
    char* type;  // 符号类型 (int,float,结构体id)
    char* func;  // 参数所属函数的名字 (如果不是函数参数,则为null)
    char* struct_name;  // 如果是结构体字段,则为结构体名,否则为null
    struct dimension* array_vec;  // 数组的内情向量
    struct id_entry* next;  // 指向下一个条目
};

// 函数参数类型
struct param{
    char* type;
    struct param* next;
};

// 符号表
struct id_entry* sym_table;

/* 向符号表中插入一条记录 */
void add_entry(struct id_entry* entry){
    if(sym_table==NULL)sym_table=entry;
    else{
        struct id_entry* index=sym_table;
        while(index->next!=NULL)index=index->next;
        index->next=entry;
    }
}
/* 判断一个ExtDef(高级定义)是函数定义(返回 0 )还是结构体定义(返回 1 )还是变量定义(返回 2 ) */
int fun_or_struct(struct node* ExtDef){
    struct node* sec_son = ExtDef->child_nodes->right_brother;
    if(strcmp(sec_son->id, "FunDec")==0){
        return 0;
    }else if(strcmp(sec_son->id, "SEMI")==0){
        return 1;
    }else
        return 2;
}
/* 确定类型 */
char* get_type(struct node* Specifier){
    char* res=NULL;
    if(!strcmp(Specifier->child_nodes->id,"TYPE"))res=Specifier->child_nodes->value;
    else{
        struct node* temp=Specifier->child_nodes->child_nodes->right_brother;
        res=temp->child_nodes->value;
    }
    return res;
}
/* 查看变量是否已经被定义(定义过返回 1,否则 0) */
int var_exist(char* name){
    struct id_entry* index=sym_table;
    while(index!=NULL){
        if(!strcmp(index->kind,"VAR")&&!strcmp(index->id,name))return 1;
        index=index->next;
    }
    return 0;
}
/* 查看函数是否已经被定义(定义过返回 1,否则 0) */
int fun_exist(char* name){
    struct id_entry* index=sym_table;
    while(index!=NULL){
        if(!strcmp(index->kind,"FUNC")&&!strcmp(index->id,name))return 1;
        index=index->next;
    }
    return 0;
}
/* 查看结构体是否已经被定义(定义过返回 1,否则 0) */
int struct_exist(char* name){
    struct id_entry* index=sym_table;
    while(index!=NULL){
        if(!strcmp(index->kind,"STRUCT")&&!strcmp(index->id,name))return 1;
        index=index->next;
    }
    return 0;
}
/* 查找函数的类型 */
char* get_fun_type(char* name){
    struct id_entry* index=sym_table;
    while(index!=NULL){
        if(!strcmp(index->kind,"FUNC")&&!strcmp(index->id,name))return index->type;
        index=index->next;
    }
    return "null";
}
/* 查找数组类型 */
char* get_array_type(struct node* Exp){
    // todo
    return "int";
}
/* 得到数组名 */
char* get_array_name(struct node* Exp){
    struct node* temp=Exp;
    while(strcmp(temp->id,"ID")!=0)
        temp=temp->child_nodes;
    return temp->value;
}
/* 判断一个标识符是否为数组(是:1，不是:0，无此符号:-1) */
int is_array(char* name){
    struct id_entry* index=sym_table;
    while(index){
        if(!strcmp(index->id,name)){
            if(!strcmp(index->kind,"ARRAY"))return 1;
            else return 0;
        }
        index=index->next;
    }
    return -1;
}
/* 查找变量类型 */
char* get_var_type(char* name){
    struct id_entry* index=sym_table;
    while(index!=NULL){
        if(!strcmp(index->kind,"VAR")&&!strcmp(index->id,name))return index->type;
        index=index->next;
    }
    return "null";
}
/* 查找结构体类型 */
char* get_struct_type(char* name){
    struct id_entry* index=sym_table;
    while(index!=NULL){
        if(!strcmp(index->id,name))return index->type;
        index=index->next;
    }
    return "null";
}
/* 检查结构体字段是否出现过 */
int field_exist(char* name){
    struct id_entry* index=sym_table;
    while(index){
        if(!strcmp(index->id,name)&&(!strcmp(index->kind,"VAR")||!strcmp(index->kind,"STRUCT_FIELD")))return 1;
        index=index->next;
    }
    return 0;
}
/* 遍历结构体字段定义序列 */
void add_decList2(char* struct_name,char* type,struct node* n){
    if(n==NULL)return;
    if(!strcmp(n->id,"Dec")&&!strcmp(n->child_nodes->child_nodes->id,"ID")){
        char* name=n->child_nodes->child_nodes->value;
        struct id_entry* entry=(struct id_entry*)malloc(sizeof(struct id_entry));
        entry->id=name;
        entry->type=type;
        entry->kind="STRUCT_FIELD";
        entry->func="do_not_param";
        entry->struct_name=struct_name;
        entry->next=NULL;
        entry->array_vec=NULL;
        if(field_exist(name)){
            printf("Error type 15 at Line %d: Redefined field \"%s\".\n",n->child_nodes->child_nodes->line_no,name);
        }else{
            add_entry(entry);
        }
    }else if(!strcmp(n->id,"Dec")&&!strcmp(n->child_nodes->child_nodes->right_brother->id,"LB")){//局部数组
        // todo
    }
    add_decList2(struct_name,type,n->child_nodes);
    add_decList2(struct_name,type,n->right_brother);
}
/* 将形参加入到符号表中 */
void add_param(char* func_name,struct node* p){
    if(p==NULL)return;
    if(!strcmp(p->id,"ParamDec")){
        char* type= get_type(p->child_nodes);
        char* name=NULL;
        struct node* VarDec=p->child_nodes->right_brother;
        if(!strcmp(VarDec->child_nodes->id,"ID")){//形参非数组
            name=VarDec->child_nodes->value;
        }else{//形参为数组
            name="Hello,world!";
        }
        struct id_entry* entry=(struct id_entry*)malloc(sizeof(struct id_entry));
        entry->id=name;
        entry->type=type;
        entry->kind="VAR";
        entry->func=func_name;
        entry->struct_name="null";
        entry->next=NULL;
        entry->array_vec=NULL;
        // 检查变量是否已经被定义
        if(var_exist(name))printf("变量已经被定义!\n");
        else add_entry(entry);
    }
    add_param(func_name,p->child_nodes);
    add_param(func_name,p->right_brother);
}
/* 将结构体字段加入到符号表中 */
void add_field(char* name,struct node* DefList){
    struct node* temp=DefList;
    if(temp==NULL)return;
    if(!strcmp(temp->id,"Def")){
        char* type= get_type(temp->child_nodes);
        struct node* DecList=temp->child_nodes->right_brother;
        add_decList2(name,type,DecList);
    }
    add_field(name,temp->child_nodes);
    add_field(name,temp->right_brother);
}
/* 函数登记:成功则返回 0,失败(重复定义)则返回 1 */
int fun_regis(struct node* ExtDef){
    // 查看是否定义过
    char* name = ExtDef->child_nodes->right_brother->child_nodes->value;
    if(fun_exist(name)){
        printf("Error type 4 at Line %d: Redefined function \"%s\".\n",ExtDef->child_nodes->right_brother->child_nodes->line_no,name);
        return 1;
    }
    // 确定函数返回类型
    char* type= get_type(ExtDef->child_nodes);
    // 构造符号表条目
    struct id_entry* entry = (struct id_entry*)malloc(sizeof(struct id_entry));
    entry->id = name;
    entry->kind = "FUNC";
    entry->type = type;
    entry->func="do_not_param";
    entry->struct_name="null";
    entry->next = NULL;
    entry->array_vec = NULL;
    add_entry(entry);
    // 将函数的参数列表加入到符号表中
    struct node* args = ExtDef->child_nodes->right_brother->child_nodes->right_brother->right_brother;
    if(!strcmp(args->id, "VarList")){// 说明需要将形参加入到符号表中
        add_param(name,args);
    }
    return 0;
}
/* 结构体登记(成功返回0,失败返回1) */
int struct_regis(struct node* t){
    struct node* StructSpecifier=t->child_nodes->child_nodes;
    struct node* id=StructSpecifier->child_nodes->right_brother->child_nodes;
    char* name;
    if(!id->is_null){
        /* 检查结构体是否已经定义 */
        name=id->value;
        if(struct_exist(name))return 1;
    } else name="no_name";
    // 构造符号表条目
    struct id_entry* entry=(struct id_entry*)malloc(sizeof(struct id_entry));
    entry->id=name;
    entry->kind="STRUCT";
    entry->type=name;
    entry->next=NULL;
    entry->func="do_not_param";
    entry->struct_name="null";
    entry->array_vec=NULL;
    add_entry(entry);
    // 将结构体字段添加到符号表
    if(!strcmp(StructSpecifier->child_nodes->right_brother->id,"OptTag")){
        struct node* DefList=StructSpecifier->child_nodes->right_brother->right_brother->right_brother;
        add_field(name,DefList);
    }
    return 0;
}

/* 局部数组登记 */
struct id_entry* create_array_entry(char* type,struct node* VarDec){
    struct id_entry* res=(struct id_entry*)malloc(sizeof(struct id_entry));
    res->kind="ARRAY";
    res->type=type;
    res->func="do_not_param";
    res->struct_name="null";
    res->next=NULL;
    struct node* index=VarDec->child_nodes;
    struct dimension* vec=(struct dimension*)malloc(sizeof(struct dimension));
    struct dimension* end=vec;
    while(index->child_nodes){
        struct dimension* t=(struct dimension*)malloc(sizeof(struct dimension));
        char* stop;
        t->size=(int)strtol(index->right_brother->right_brother->value,&stop,10);
        t->next=NULL;
        printf("%d\n",t->size);
        end->next=t;
        end=end->next;
        index=index->child_nodes;
    }
    end=vec;
    vec=vec->next;
    free(end);
    res->id=index->value;
    res->array_vec=vec;
    return res;
}
/* 检查结构体类型是否存在 */
int struct_type_exist(char* name){
    struct id_entry* index=sym_table;
    while(index){
        if(!strcmp(index->id,name)&&!strcmp(index->kind,"STRUCT"))return 1;
        index=index->next;
    }
    return 0;
}
/* 遍历局部数据定义序列 */
void add_decList(char* type,struct node* n){
    if(strcmp(type,"int")!=0&&strcmp(type,"float")!=0&& !struct_type_exist(type)){
        printf("Error type 17 at Line %d: Undefined structure.\n",n->line_no);
        return;
    }
    if(n==NULL)return;
    if(!strcmp(n->id,"Dec")&&!strcmp(n->child_nodes->child_nodes->id,"ID")){//局部变量
        char* name=n->child_nodes->child_nodes->value;
        struct id_entry* entry=(struct id_entry*)malloc(sizeof(struct id_entry));
        entry->id=name;
        entry->type=type;
        entry->kind="VAR";
        entry->func="do_not_param";
        entry->struct_name="null";
        entry->next=NULL;
        entry->array_vec=NULL;
        if(var_exist(name)){
            printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",n->child_nodes->child_nodes->line_no,name);
        }else{
            add_entry(entry);
        }
    }else if(!strcmp(n->id,"Dec")&&!strcmp(n->child_nodes->child_nodes->right_brother->id,"LB")){//局部数组
        struct node* VarDec=n->child_nodes;
        struct id_entry* entry= create_array_entry(type,VarDec);
        add_entry(entry);
    }
    add_decList(type,n->child_nodes);
    add_decList(type,n->right_brother);
}

/* 局部数据登记 */
void local_regis(struct node* Def){
    //获取声明中的类型
    char* type= get_type(Def->child_nodes);
    struct node* DecList=Def->child_nodes->right_brother;
    add_decList(type,DecList);
}
void local_regis0(struct node* DefList){
    struct node* temp=DefList;
    if(temp==NULL)return;
    if(!strcmp(temp->id,"Def"))local_regis(temp);
    local_regis0(temp->child_nodes);
    local_regis0(temp->right_brother);
}
/* 判断Exp是函数调用(0),数组元素引用(1),变量(3),常数(4),还是结构体字段引用(2),其他则为 -1 */
int judge(struct node* Exp){
    struct node* son=Exp->child_nodes;
    if(!strcmp(son->id,"INT")||!strcmp(son->id,"FLOAT"))return 4;
    if(!strcmp(son->id,"ID")
    && son->right_brother!=NULL
    && !strcmp(son->right_brother->id,"LP"))return 0;
    struct node* right=Exp->right_brother;
    if(right==NULL||strcmp(right->id,"LB")!=0){
        if(!strcmp(son->id,"Exp")
        && son->right_brother!=NULL
        && !strcmp(son->right_brother->id,"LB"))return 1;
    }
    if(!strcmp(son->id,"Exp")
    &&son->right_brother
    &&!strcmp(son->right_brother->id,"DOT"))return 2;
    if(!strcmp(son->id,"ID")){
        if(right==NULL||strcmp(right->id,"LB")!=0&&strcmp(right->id,"DOT")!=0)return 3;
    }
    return -1;
}
/* 判断Exp的类型 */
char* get_ExpType(struct node* Exp){
    int res= judge(Exp);
    struct node* son=Exp->child_nodes;
    if(res==0){ // 函数调用
        char* fun_name=son->value;
        return get_fun_type(fun_name);
    }else if(res==1){ // 数组元素引用
        return get_array_type(Exp);
    }else if(res==2){ // 结构体字段
        struct node* id=son->right_brother->right_brother;
        char* name=id->value;
        // todo(获取结构体字段的类型)
    }else if(res==3){ // 变量
        char* var_name=son->value;
        return get_var_type(var_name);
    }else if(res==4){ // 常数
        if(!strcmp(son->id,"INT"))return "int";
        return "float";
    }else{ // 其他
        if(!strcmp(son->id,"Exp"))return get_ExpType(son);
        return get_ExpType(son->right_brother);
    }
    return "null";
}
/* 判断Exp是否为赋值语句 */
int is_assign(struct node* Exp){
    struct node* son=Exp->child_nodes;
    if(son->right_brother!=NULL&&!strcmp(son->right_brother->id,"ASSIGNOP"))return 1;
    return 0;
}
/* 判断Exp是否为四则运算语句 */
int is_operation(struct node* Exp){
    struct node* son=Exp->child_nodes;
    if(son->right_brother!=NULL){
        char* id=son->right_brother->id;
        if(!strcmp(id,"PLUS")||!strcmp(id,"MINUS")||!strcmp(id,"STAR")||!strcmp(id,"DIV"))return 1;
    }
    return 0;
}
/* 判断Stmt是否为返回语句 */
int is_return(struct node* Stmt){
    struct node* son=Stmt->child_nodes;
    if(!strcmp(son->id,"RETURN"))return 1;
    return 0;
}
/* 查找函数中的返回语句 */
void search_return(char* fun_type,struct node* n){
    if(n==NULL)return;
    if(!strcmp(n->id,"Stmt")&&is_return(n)){
        struct node* Exp=n->child_nodes->right_brother;
        char* type= get_ExpType(Exp);
        if(strcmp(type,fun_type)!=0)printf("Error type 8 at Line %d: Type mismatched for return.\n",n->line_no);
    }
    search_return(fun_type,n->child_nodes);
    search_return(fun_type,n->right_brother);
}
/* 检查函数返回值类型是否正确 */
void check_fun_type(struct node* ExtDef){
    struct node* son=ExtDef->child_nodes;
    char* fun_type= get_type(son);
    search_return(fun_type,ExtDef);
}
/* 得到函数的形参链 */
struct param* get_fun_chain1(char* func_name){
    struct param* res=(struct param*)malloc(sizeof(struct param));
    struct param* end=res;
    struct id_entry* index=sym_table;
    while(index!=NULL){
        if(!strcmp(index->func,func_name)){
            struct param* n=(struct param*)malloc(sizeof(struct param));
            n->type=index->type;
            n->next=NULL;
            end->next=n;
            end=end->next;
        }
        index=index->next;
    }
    end=res;
    res=res->next;
    free(end);
    return res;
}
/* 得到函数的实参链 */
void create_chain(struct param* end,struct node* n){
    while(1){
        struct node* Exp = n->child_nodes;
        struct param* t = (struct param*)malloc(sizeof(struct param));
        t->type = get_ExpType(Exp);
        t->next = NULL;
        end->next=t;
        end=end->next;
        if(Exp->right_brother){
            n=Exp->right_brother->right_brother;
        }else break;
    }
}
struct param* get_fun_chain2(struct node* Exp){
    struct node* args=Exp->child_nodes->right_brother->right_brother;
    if(!strcmp(args->id,"Args")){
        struct param* res=(struct param*)malloc(sizeof(struct param));
        struct param* end=res;
        create_chain(end,args);
        end=res;
        res=res->next;
        free(end);
        return res;
    }else return NULL;
}
/* 检查函数调用是否合法 */
void check_fun_call(struct node* Exp){
    char* func_name=Exp->child_nodes->value;
    struct param* chain1= get_fun_chain1(func_name);
    struct param* chain2= get_fun_chain2(Exp);
    struct param* index1=chain1;
    struct param* index2=chain2;
    while(index1!=NULL&&index2!=NULL){
        if(strcmp(index1->type,index2->type)!=0)printf("(%d),参数类型不匹配!\n",Exp->line_no);
        index1=index1->next;
        index2=index2->next;
    }
    if(index1!=NULL||index2!=NULL)printf("Error type 9 at Line %d: not applicable for arguments.\n",Exp->line_no);
}
/* 根据名字获取种属 */
char* get_kind_by_name(char* name){
    struct id_entry* index=sym_table;
    while(index!=NULL){
        if(!strcmp(index->id,name))return index->kind;
        index=index->next;
    }
    return "null";
}
/* 检查数组索引是否为整数 */
void check_array_index(struct node* t){
    struct node* index=t->child_nodes;
    while(index&&strcmp(index->id,"ID")!=0){
        struct node* Exp=index->right_brother->right_brother;
        if(strcmp(get_ExpType(Exp),"int"))printf("Error type 12 at Line %d: not a integer.\n",t->line_no);
        index=index->child_nodes;
    }
}
/* 检查结构体字段引用 */
int is_struct(char* name){
    struct id_entry* index=sym_table;
    while(index){
        if(!strcmp(index->id,name)){
            if(!strcmp(index->kind,"VAR")&& struct_type_exist(index->type))return 1;
            return 0;
        }
        index=index->next;
    }
    return -1;
}
int has_field(char* struct_name,char* field_name){
    struct id_entry* index=sym_table;
    while(index){
        if(!strcmp(field_name,index->id)&&!strcmp(index->kind,"STRUCT_FIELD")&&!strcmp(index->struct_name,struct_name))
            return 1;
        index=index->next;
    }
    return 0;
}
void check_struct(struct node* Exp){
    struct node* id=Exp->child_nodes->child_nodes;
    struct node* field=Exp->child_nodes->right_brother->right_brother;
    char* id_name=id->value;
    int res=is_struct(id_name);
    if(res==-1)printf("(%d),未定义符号!\n",Exp->line_no);
    else if(res==0)printf("Error type 13 at Line %d: Illegal use of \".\".\n",Exp->line_no);
    else{
        // 是结构体
        char* field_name=field->value;
        char* struct_type= get_struct_type(id_name);
        if(!has_field(struct_type,field_name))printf("Error type 14 at Line %d: Non-existent field.\n",Exp->line_no);
    }
}
/* 打印符号表 */
void print_table(){
    struct id_entry* index=sym_table;
    while(index!=NULL){
        printf("%s:%s %s\n",index->id,index->kind,index->type);
        index=index->next;
    }
}
/* 语义分析函数 */
void semantic1(struct node* root){
    if(root==NULL)return;
    struct node* temp=root;
    /* 高级定义 */
    if(!strcmp(temp->id,"ExtDef")) {
        if (fun_or_struct(temp) == 0) {
            /* 函数定义 */
            if (fun_regis(temp))
                printf("函数已被定义过!\n");
        }else if(fun_or_struct(temp)==1){
            /* 结构体定义 */
            if(struct_regis(temp))
                printf("Error type 16 at Line %d: Duplicated name.\n",temp->line_no);
        }
    }
    /* 局部定义 */
    if(!strcmp(temp->id,"CompSt")){
        struct node* DefList=temp->child_nodes->right_brother;
        local_regis0(DefList);
    }
    if(!strcmp(temp->id,"Exp")){
        struct node* son=temp->child_nodes;
        if(judge(temp)==3){
            if(var_exist(son->value)==0)
                printf("Error type 1 at Line %d: Undefined variable \"%s\".\n",son->line_no,son->value);
        }
        else if(judge(temp)==0){
            char* func_name=son->value;
            char* kind= get_kind_by_name(func_name);
            if(strcmp(kind,"null")!=0&&strcmp(kind,"FUNC")!=0)printf("Error type 11 at Line %d: not a function.\n",temp->line_no);
            else if(!strcmp(kind,"null")&&fun_exist(func_name)==0){
                printf("Error type 2 at Line %d: Undefined function \"%s\".\n",son->line_no,son->value);
            }else check_fun_call(temp);  // 检查函数调用的实参是否合法
        }else if(judge(temp)==1){
            char* array_name=get_array_name(temp);
            int res=is_array(array_name);
            if(res==0)printf("Error type 10 at Line %d: not an array.\n",temp->line_no);
            else if(res==-1)printf("(%d),未定义符号!\n",temp->line_no);
            else{
                /* 检查数组索引是否为整数 */
                check_array_index(temp);
            }
        }else if(judge(temp)==2){
            /* 检查结构体字段引用 */
            check_struct(temp);
        }
        if(is_assign(temp)){
            /* 检查赋值语句左面是否出现右值 */
            int res= judge(son);
            if(res==-1||res==0||res==4)printf("Error type 6 at Line %d: The left-hand side if an assignment must be a variable.\n",temp->line_no);
            /* 检查赋值语句两侧类型是否匹配 */
            struct node* left=son;
            struct node* right=son->right_brother->right_brother;
            char* left_type=get_ExpType(left);
            char* right_type= get_ExpType(right);
            if(strcmp(left_type,"null")!=0
            && strcmp(right_type,"null")!=0
            && strcmp(left_type, right_type)!=0)printf("Error type 5 at Line %d: Type mismatched for assignment.\n",son->line_no);
        }
        /* 检查运算符两侧的类型是否相同 */
        if(is_operation(temp)){
            struct node* first=son;
            struct node* second=son->right_brother->right_brother;
            char* first_type=get_ExpType(first);
            char* second_type= get_ExpType(second);
            if(strcmp(first_type,second_type)!=0)printf("Error type 7 at Line %d: Type mismatched for operands.\n",temp->line_no);
        }
    }
    semantic1(root->child_nodes);
    semantic1(root->right_brother);
}
void semantic2(struct node* root){
    struct node* temp=root;
    if(temp==NULL)return;
    /* 检查函数返回类型是否正确 */
    if(!strcmp(temp->id,"ExtDef")&&!fun_or_struct(temp))check_fun_type(temp);
    semantic2(temp->child_nodes);
    semantic2(temp->right_brother);
}
void semantic(struct node* root){
    semantic1(root);
    semantic2(root);
}
