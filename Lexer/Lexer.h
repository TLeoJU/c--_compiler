#include <bits/stdc++.h>
using namespace std;

#define maxSize 1024    //源文件代码最大长度

static const char* const KW[6] = {"int", "void", "return", "const", "main", "struct"};   //关键字及其种别码、个数
static const int   KW_array[6] = {1, 2, 3, 4, 5, 26};
static const int        KW_num = 6;

static const char* const OP[14] = {"+", "-", "*", "/", "%", "=", ">", "<", "==", "<=", ">=", "!=", "&&", "||"};  //运算符及其种别码、个数
static const int   OP_array[14] = {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
static const int         OP_num = 14;

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
vector<string> analyse(FILE* fpin);  //给定一个源文件，返回token序列，或者程序不被词法接受的通知


vector<string> analyse(FILE* fpin){

    char ch;       //当前输入字符
    char str[maxSize] = "";    //代码
    vector<string> token;  //输出token序列
    int typeCode = 0;   //种别码
    int index = 0;      //当前位置
    int front = 0;      //超前指针
    int max = 0;        //代码文本长度
    bool isError = false;   //可接受标志，为false说明可接受

    //读取代码保存在str
    while((ch = fgetc(fpin)) != EOF){
        str[max] = ch;
        max++;
    }

    //开始处理，当当前指针等于代码长度时结束处理，代码可接受，返回token序列
    while(index < max){

        typeCode = 0;
        ch = str[index];
        front = index;
        
        //跳过空白符
        if(ch == ' ' || ch == '\n' || ch == '\t'){
            index++;
            continue;
        }

        //若当前字符为界符，输出token，index+=1，之后是否出界交由while条件判断
        if(typeCode = isSE(ch)){
            //将种别码转化为char*类型以便生成token
            char tc[2];
            itoa(typeCode, tc, 10); 

            //生成本条序列，例如 "{\t<SE,22>"
            string tk;
            tk = tk + ch + "\t<SE," + tc + ">";
            
            //将本条token序列加入容器中
            token.push_back(tk);
            //当前指针后移一位，进入下一轮while
            index++;
            continue;
        }
        //若当前字符为数，则一直读取至第一个不为数的字符为止，输出token，index更新至第一个不为数的字符
        else if(isDigit(ch)){
            //s保存当前表示数的子串
            string s;
            s = s + ch;
            while(isDigit(ch = str[++front])){
                s = s + ch;
            }
            //若数后紧跟的字符为字母或下划线，则单词非法
            if(isLetter(ch) || isUnderline(ch)){
                isError = true;
                break;
            }
            //生成本条token
            string tk;
            tk += s + "\t<INT," + s + ">";
            //将本条token序列加入容器中，当前指针移向下一个待处理字符
            token.push_back(tk);
            index = front;
            continue;
        }
        //若当前字符为运算符，则往后读取一位，若为等号则调用二位运算符判断。输出相应token，更新相应index
        else if(typeCode = isOP1(ch)){
            front++;    //往后读一位
            char nextCh = str[front];   //保存下一位字符
            const char OP2[2] = {ch, nextCh};   //保存当前两位字符
            //若为二位运算符
            if(isOP2(OP2)){
                //将种别码化为char*类型
                typeCode = isOP2(OP2);
                char tc[2];
                itoa(typeCode, tc, 10);
                //生成本条token
                string tk;
                tk = tk + OP2 + "\t<OP," + tc + ">";
                //将本条token加入容器，指针下移2位
                token.push_back(tk);
                index += 2;
                continue;
            }
            //若不为二位运算符：下一位依旧是运算符、或当前位为'!'，则不被接受
            else if(isOP1(nextCh) || ch == '!'){
                isError = true;
                break;
            }
            //否则为单运算符
            else{
                //种别码转化为char*
                typeCode = isOP1(ch);
                char tc[2];
                itoa(typeCode, tc, 10);
                //生成本条token
                string tk;
                tk = tk + ch + "\t<OP," + tc + ">";
                //本条token加入容器，指针下移1位
                token.push_back(tk);
                index++;
                continue;
            }

        }
        //若当前字符为下划线，则只可能是标识符
        else if(isUnderline(ch)){
            //s保存当前标识符，遇到非数字字母下划线的字符则停止
            string s;
            s = s + ch;
            front++;
            ch = str[front];
            while(isLetter(ch) || isDigit(ch) || isUnderline(ch)){
                s = s + ch;
                front++;
                ch = str[front];
            }
            //生成本条token
            string tk;
            tk += s + "\t<IDN," + s + ">";
            //本条token加入容器，更新指针
            token.push_back(tk);
            index = front;
            continue;
        }
        //若当前字符为字母，则可能是关键字或标识符
        else if(isLetter(ch)){
            //s保存当前读取子串
            string s;
            s = s + ch;
            front++;
            ch = str[front];
            while(isLetter(ch) || isDigit(ch) || isUnderline(ch)){
                s = s + ch;
                front++;
                ch = str[front];
            }
            //若为关键字
            if(typeCode = isKW(s.c_str())){
                char tc[2];
                itoa(typeCode, tc, 10);
                string tk;
                tk += s + "\t<KW," + tc + ">";
                token.push_back(tk);
                index = front;
                continue;
            }
            //否则为标识符
            else{
                string tk;
                tk += s + "\t<IDN," + s + ">";
                token.push_back(tk);
                index = front;
                continue;
            }
        }
        //若当前字符不符合以上要求，则不被接受
        else{
            isError = true;
            break;
        }


    }

    //若程序不被词法接受，将错误信息及已生成token一并返回
    if(isError){
        string message = "This program is not accepted by this vocabulary!\n";
        message += "Error occurs near the character ";
        char indexS[4];
        itoa(index, indexS, 10);
        message.append(indexS);
        token.push_back(message);
    }
    return token;

}

int isKW(const char* s){
    for(int i=0; i<KW_num; i++){
        if(strcmp(s, KW[i]) == 0)
            return KW_array[i];
    }
    return 0;
}

int isSE(const char c){
    for(int i=0; i<SE_num; i++){
        if(c == SE[i])
            return SE_array[i];
    }
    return 0;
}

int isOP1(const char c){
    for(int i=0; i<OP_num; i++){
        if(c == OP[i][0])
            return OP_array[i];
    }
    if (c == '!')   return 17;
    return 0;
}

int isOP2(const char* s){
    for(int i=0; i<OP_num; i++){
        if(strcmp(s, OP[i]) == 0)
            return OP_array[i];
    }
    return 0;
}

bool isDigit(const char c){
    if(c>='0' && c<='9')
        return true;
    else return false;
}

bool isUnderline(const char c){
    if(c == '_')
        return true;
    else return false;
}

bool isLetter(const char c){
    if((c >= 'A' && c<= 'Z')||(c >= 'a' && c <= 'z'))
        return true;
    else return false;
}

int main(){

    char path[80] = "D:\\Code\\CPP\\cpp\\compilationAssignment\\Lexer\\hello.sy";
    FILE* fpin;
    char resultPath[80] = "D:\\Code\\CPP\\cpp\\compilationAssignment\\Lexer\\result.txt";
    FILE* fpout;
    fpin = fopen(path, "r");
    fpout = fopen(resultPath, "w");

    if(fpin == NULL){
        cout << "Wrong path!" << endl;
        return 0;
    }

    cout << "The results of lexical analysis will be written to the txt file" << endl;
    vector<string> result = analyse(fpin);
    for(vector<string>::iterator it=result.begin(); it!=result.end(); it++){
        string k = *it;
        fputs(k.c_str(), fpout);
        fputc('\n', fpout);
    }
    fclose(fpin);
    fclose(fpout);
    cout << "Finish\n";

    system("pause");
    return 0;
}
