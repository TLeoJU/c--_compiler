/**
 * @file
 * @brief 语法分析器的参数定义、数据结构声明与函数原型声明
 * @author zhn
 * @date Sat Nov 25 16:06:21 2023
*/

#include <bits/stdc++.h>
using namespace std;

#define maxSize 4096        //输入文本最大长度
#define START "program"     //开始符号

static set<string> VnSet;   //非终结符集合
static set<string> VtSet;   //终结符集合
static stack<string> Stack; //符号栈
static map<string, set<string>> FIRST;  //FIRST集合
static map<string, set<string>> FOLLOW; //FOLLOW集合
static map<list<string>, set<string>> FIRST_STRING;     //任意符号串的FIRST集合
static map<string, list<list<string>>> production;      //产生式，非终结符与其所有候选产生式
//第一层为map<string, map>，左为非终结符
//第二层为map<string, set>，左为输入符号，右保存产生式右侧符号序列
static map<string, map<string, list<string>>> table;    //预测分析表
static vector<string> result;   //分析token所用的产生式序列

string getText(FILE* fpin);         //从给定文件读取所有文本到一条string内
void getProduction(FILE* fpin);     //从给定文法文件中读取产生式，保存于production
void First();   //生成First集
int  getFirst(string vn);   //扩展单个非终结符的First集
void getStringFirst(list<string>);    //生成指定符号串的First集
void Follow();  //根据文法生成所有非终结符的FOLLOW集合，保存在FOLLOW里
int  getFollow(string vn);  //扩展单个非终结符的FOLLOW集合，若有扩展则返回1，无新增元素则返回0
void createTable(); //生成预测分析表
list<string> find(string vn, string sym);   //根据栈顶符号和输入符号指派产生式
void initParser(FILE* fpin);    //从文件流读取文法并初始化语法分析器
bool analyseParser(vector<string> token); //分析token是否可被文法接受，返回最左推导token所用的产生式序列


void printFirst();  //向输出流打印first集
void printFirst(FILE* fpout);   //向文件流打印first集
void printFollow(); //向输出流打印follow集
void printFollow(FILE* fpout);  //向文件流打印follow集
