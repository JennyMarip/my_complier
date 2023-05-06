#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "syntax.tab.c"
extern char *right(char *dst,char *src,int n);
extern char *left(char *dst,char *src,int n);
extern FILE* yyin;
extern int yylineno;
extern void yyrestart(FILE*);
char* file_name;
int file_name_len;
FILE* fp;
int main(int argc, char** argv) {
    if(argc<=2){
        return 1;
    }

    // 获取文件名连同路径，并得到文件名长度
    file_name = (char*)malloc(20);
    file_name = argv[1];
    _path = argv[2];
    file_name_len = strlen(file_name)-5;

    FILE* f = fopen(argv[1], "r");
    if(!f){
        perror(argv[1]);
    }
    yylineno=1;
    yyrestart(f);

    //获取文件名filName(带后缀)
    char* fileName = (char*)malloc(10);
    right(fileName, file_name, file_name_len);
    //获取文件名fileName2(不带后缀)
    char* fileName2 = (char*)malloc(7);
    left(fileName2, fileName, file_name_len-4);
    //得到输出output文件的完整名(路径+带后缀文件名)
    char* dir = "/home/lay/C_or_CPP/compile_lab/output/";
    char* path = (char*)malloc(strlen(dir)+strlen(fileName2)+7);
    strcpy(path, dir);
    strcat(path, fileName2);
    strcat(path, ".output");

    fp = fopen(path, "w");
    yyparse();
    fclose(fp);
    return 0;
}
