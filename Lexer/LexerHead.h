/**
 * @file
 * @brief 词法分析器的参数定义和函数原型声明
 * @author zhn
 * @date Sat Nov 25 16:06:21 2023
*/

#include <bits/stdc++.h>
using namespace std;

#define maxSize 1024    //源文件代码最大长度

static const char* const KW[6] = {"int", "void", "return", "const", "main", "struct"};   //关键字及其种别码、个数
static const int   KW_array[6] = {1, 2, 3, 4, 5, 26};
static const int        KW_num = 6;

static const char* const OP[14] = {"+", "-", "*", "/", "%", "=", ">", "<", "==", "<=", ">=", "!=", "!"};  //运算符及其种别码、个数
static const int   OP_array[14] = {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 27};
static const int         OP_num = 13;

static const char        SE[6] = {'(', ')', '{', '}', ';', ','}; //界符及其种别码、个数
static const int   SE_array[6] = {20, 21, 22, 23, 24, 25};
static const int        SE_num = 6;

static const char  FILTER[3] = {' ', '\n', '\t'};   //空白符

int isKW(const char* s);        //判断是否为关键字，是则返回种别码，否则返回0
int isSE(const char c);         //判断是否为界符，是则返回种别码，否则返回0
int isOP1(const char c);        //判断是否为单字符运算符，是则返回种别码，否则返回0 ****对'!'符号特别处理****
int isOP2(const char* s);       //判断是否为双字符运算符，是则返回种别码，否则返回0
bool isDigit(const char c);     //判断是否为数字，是则返回true
bool isUnderline(const char c); //判断是否为下划线
bool isLetter(const char c);    //判断是否为字母
vector<string> analyseLexer(FILE* fpin);//给定一个源文件，返回token序列，或者程序不被词法接受的通知

void print(vector<string> result);      //向输出流打印token序列
void print(vector<string> result, FILE* fpout);  //向指定文件打印token序列