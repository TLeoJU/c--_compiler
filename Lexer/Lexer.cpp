#include <bits/stdc++.h>
using namespace std;
#define maxSize 1024

static const char* const KW[6] = {"int", "void", "return", "const", "main", "struct"};   //关键字及其种别码
static const int   KW_array[6] = {1, 2, 3, 4, 5, 26};
static const int        KW_num = 6;

static const char* const OP[14] = {"+", "-", "*", "/", "%", "=", ">", "<", "==", "<=", ">=", "!=", "&&", "||"};  //运算符及其种别码
static const int   OP_array[14] = {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
static const int         OP_num = 14;

static const char        SE[6] = {'(', ')', '{', '}', ';', ','}; //界符及其种别码
static const int   SE_array[6] = {20, 21, 22, 23, 24, 25};
static const int        SE_num = 6;

static const char  FILTER[3] = {' ', '\n', '\t'};

int isKW(const char* s);        //判断是否为关键字，是则返回种别码，否则返回0
int isSE(const char c);         //判断是否为界符，是则返回种别码，否则返回0
int isOP1(const char c);        //判断是否为单字符运算符，是则返回种别码，否则返回0 ****对'!'符号特别处理****
int isOP2(const char* s);       //判断是否为双字符运算符，是则返回种别码，否则返回0
bool isDigit(const char c);     //判断是否为数字，是则返回true
bool isUnderline(const char c); //判断是否为下划线
bool isLetter(const char c);    //判断是否为字母


vector<string> analyse(FILE* fpin){

    char ch;       //当前输入字符
    char str[maxSize] = "";    //代码
    vector<string> token;  //输出token序列
    int typeCode = 0;   //种别码
    int index = 0;      //当前位置
    int front = 0;      //超前指针
    int max = 0;        //文本长度
    bool isError = false;   //可接受标志

    //读取代码保存在str
    while((ch = fgetc(fpin)) != EOF){
        str[max] = ch;
        max++;
    }

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
            char tc[2];
            // snprintf(tc, sizeof(typeCode), "%d", typeCode);
            itoa(typeCode, tc, 10);
            // token = (token + ch + "\t<SE," + tc + ">\n");
            string tk;
            tk.append(1,ch);
            tk.append("\t<SE,");
            tk.append(tc);
            tk.append(">");
            token.push_back(tk);
            index++;
            continue;
        }
        //若当前字符为数，则一直读取至第一个不为数的字符为止，输出token，index更新至第一个不为数的字符
        else if(isDigit(ch)){
            string s;
            s = s + ch;
            while(isDigit(ch = str[++front])){
                s = s + ch;
            }
            //若第一个不为数的字符是字母或下划线，则不被接受（整型后只能紧跟空白符、界符、运算符）
            if(isLetter(ch) || isUnderline(ch)){
                isError = true;
                break;
            }
            // token += (s + "\t<INT," + s + ">\n");
            string tk;
            tk.append(s);
            tk.append("\t<INT,");
            tk.append(s);
            tk.append(">");
            token.push_back(tk);
            index = front;
            continue;
        }
        //若当前字符为运算符，则往后读取一位，若为等号则调用二位运算符判断。输出相应token，更新相应index
        else if(typeCode = isOP1(ch)){
            front++;
            char nextCh = str[front];
            const char OP2[2] = {ch, nextCh};
            if(isOP2(OP2)){
                typeCode = isOP2(OP2);
                char tc[2];
                snprintf(tc, sizeof(typeCode), "%d", typeCode);
                // token = (token + OP2 + "\t<OP," + tc + ">\n");
                string tk;
                tk.append(OP2);
                tk.append("\t<OP,");
                tk.append(tc);
                tk.append(">");
                token.push_back(tk);
                index += 2;
                continue;
            }
            //若不为二位运算符：下一位依旧是运算符、或当前位为'!'，则不被接受
            else if(isOP1(nextCh) || ch == '!'){
                isError = true;
                break;
            }
            else{
                typeCode = isOP1(ch);
                char tc[2];
                snprintf(tc, sizeof(typeCode), "%d", typeCode);
                // token = (token + ch + "\t<OP," + tc + ">\n");
                string tk;
                tk.append(1,ch);
                tk.append("\t<OP,");
                tk.append(tc);
                tk.append(">");
                token.push_back(tk);
                index++;
                continue;
            }

        }
        //若当前字符为下划线，则只可能是标识符
        else if(isUnderline(ch)){
            string s;
            s = s + ch;
            front++;
            ch = str[front];
            while(isLetter(ch) || isDigit(ch) || isUnderline(ch)){
                s = s + ch;
                front++;
                ch = str[front];
            }
            // token += (s + "\t<IDN," + s + ">\n");
            string tk;
            tk.append(s);
            tk.append("\t<IDN,");
            tk.append(s);
            tk.append(">");
            token.push_back(tk);
            index = front;
            continue;
        }
        //若当前字符为字母，则可能是关键字或标识符
        else if(isLetter(ch)){
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
                snprintf(tc, sizeof(typeCode), "%d", typeCode);
                // token += (s + "\t<KW," + tc + ">\n");
                string tk;
                tk.append(s);
                tk.append("\t<KW,");
                tk.append(tc);
                tk.append(">");
                token.push_back(tk);
                index = front;
                continue;
            }
            //否则为标识符
            else{
                // token += (s + "\t<IDN," + s + ">\n");
                string tk;
                tk.append(s);
                tk.append("\t<IDN,");
                tk.append(s);
                tk.append(">");
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

    //若程序不被词法接受，返回错误信息及已生成的token序列
    if(isError){
        string message = "This program is not accepted by this vocabulary!\n";
        message += "Error occurs near the character ";
        char indexS[4];
        snprintf(indexS, sizeof(index), "%d", index);
        // message = (message + indexS + "\nThe following is the generated token sequence\n\n");
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
    fpout = fopen(resultPath, "w");

    if((fpin = fopen(path, "r")) != NULL){
        cout << "Wrong path!" << endl;
        system("pause");
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
