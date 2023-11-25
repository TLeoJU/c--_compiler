/**
 * @file
 * @brief 语法分析器的实现 核心函数为getProduction(), First(), getStringFirst(), Follow(), createTable(), analyseParser()
 * @author zhn
 * @date Sat Nov 25 16:06:21 2023
*/

#include "ParserHead.h"
using namespace std;

/**
 * @brief 为了输入符号可能是标识符或常数而写的判断函数
 * @param X 栈顶符号
 * @param a 输入符号
 * @return 符号匹配则返回true，不匹配返回false
*/
bool isEqual(string X, string a){
    if(X == a)
        return true;
    regex pattern("^[0-9]+$");
    if(regex_match(a, pattern) && X == "INT")
        return true;
    else if(X == "Ident" && !VnSet.count(a) && !VtSet.count(a))
        return true;
    else return false;
}

/**
 * @brief 用于语法分析过程记录
 * @param X 栈顶符号
 * @param a 输入符号
 * @param action 执行动作
*/
void addResult(string X, string a, string action){
    string s = to_string(result.size() + 1) + "\t";
    s += (X + "#" + a + "\t" + action);
    result.push_back(s);
}

/**
 * @brief 分析token是否可被文法接受，将最左推导token所用的产生式序列存入result
 * @param token Laxer产生的token序列
 * @return 接受则返回true [序号][TAB][栈顶符号]#[面临输入符号][TAB][执行动作]
*/
bool analyseParser(vector<string> token){
    result.clear();
    //处理token信息
    for(vector<string>::iterator it=token.begin(); it!=token.end(); it++){
        int pos = it->find('\t');
        it->erase(pos, it->length()-1);
    }

    //初始化分析栈、指向token的指针it、栈顶符号X
    Stack.push("#");
    Stack.push(START);
    token.push_back("#");
    vector<string>::const_iterator it = token.begin();
    string X = Stack.top();
    //开始重复
    while(true){
        string a = *it;
        if(!VnSet.count(X) || X == "#"){
            if(isEqual(X, a) && X == "#"){
                addResult(X, a, "accept");
                return true;
            }
            else if(isEqual(X, a) && X != "#"){
                addResult(X, a, "move");
                Stack.pop();
                it++;
            }
            else{
                addResult(X, a, "error");
                return false;
            }
        }
        else{
            list<string> pros = find(X, a);
            if(pros.front() == "error"){
                addResult(X, a, "error");
                return false;
            }
            else{
                addResult(X, a, "reduction");
                Stack.pop();
                for(list<string>::reverse_iterator re = pros.rbegin(); re!=pros.rend(); re++)
                    if(*re != "$")
                        Stack.push(*re);
            }
        }
        X = Stack.top();
    }
    return false;
}

/**
 * @brief 根据栈顶符号及输入符号，给出指派的产生式
 * @param vn 栈顶符号
 * @param sym 输入符号
 * @return 产生式序列
*/
list<string> find(string vn, string sym){
    if(!VtSet.count(sym) && sym != "#"){
        regex pattern("^[0-9]+$");
        if(regex_match(sym, pattern))
            return table[vn]["INT"];
        else return table[vn]["Ident"];
    }

    return table[vn][sym];
}

/**
 * @brief 生成预测分析表
*/
void createTable(){
    if(!FIRST.size() || !FOLLOW.size())
        return;
    //对每个非终结符生成预测列表
    for(auto vn : VnSet){
        map<string, list<string>> column;   //当前非终结符的预测列表，<输入符号, 指派式>
        list<list<string>> pros = production[vn];   //当前非终结符的所有产生式
        //对每条产生式
        for(auto pro : pros){
            //生成其FIRST集
            // if(!FIRST_STRING.count(pro))
                getStringFirst(pro);
            //获取该产生式的FIRST集
            set<string> firstPros = FIRST_STRING[pro];
            //生成本非终结符的预测列表
            for(auto firstPro : firstPros){
                if(firstPro != "$")
                    column.insert(pair<string, list<string>>(firstPro, pro));
            }
            //若该FIRST集包含空字符
            if(firstPros.count("$")){
                //获取本非终结符vn的后随符号集
                set<string> followSyms = FOLLOW[vn];
                //扩充本非终结符的预测列表
                for(auto followSym : followSyms){
                    column.insert(pair<string, list<string>>(followSym, pro));
                }
            }

        }
        //将预测列表补全error值
        for(auto vt : VtSet){
            if(!column.count(vt)){
                list<string> error = {"error"};
                column.insert(pair<string, list<string>>(vt, error));
            }
        }
        //去除补全error时可能加入的"$"
        if(column.count("$"))
            column.erase("$");
        //将该符号的预测列表加入预测分析表中
        table.insert(pair<string, map<string, list<string>>>(vn, column));
    }
}

/**
 * @brief 重复对每个符号求First集直到没有新增
*/
void First(){
    if(!production.size())
        return;
    int count;
    do{
        count = 0;
        for(string vn:VnSet){
            count += getFirst(vn);
        }
    }while(count);
}

/**
 * @brief 重复对每个符号求First集直到没有新增
*/
void Follow(){
    if(!production.size())
        return;
    int count;
    do{
        count = 0;
        for(string vn:VnSet){
            count += getFollow(vn);
        }
    }while(count);
}

/**
 * @brief 获取vn的终结首符集，保存在FIRST[vn]中，反馈本操作是否扩充了FIRST
 * @param vn 待求取FIRST集的符号
 * @return 本操作总共扩充次数
*/
int getFirst(string vn){

    set<string> vset;    //vset表示符号vn所有终结首符
    if(vn == "$"){
        if(FIRST.count("$"))return 0;
        vset.insert("$");
        FIRST.insert(pair<string, set<string>>("$", vset));
        return 1;
    }
    //若vn是终结符，则将自身加入FIRST(vn)并返回
    if(VtSet.count(vn)){
        if(FIRST.count(vn))return 0;
        vset.insert(vn);
        FIRST.insert(make_pair(vn,vset));
        return 1;
    }

    list<list<string>> pros = production[vn];   //pros表示符号vn的所有产生式
    //若存在已有FIRST(vn)，则加入vset
    if(FIRST.count(vn)){
        vset = FIRST[vn];
    }
    int initSize = vset.size();     //记录是否有终结首符集扩大
    int finalSize = initSize;       //返回finalSize-initSize表示是否有终结首符集扩大
    //对vn的每条产生式
    for(auto pro : pros){
        //处理当前产生式的符号。当且仅当此符号终结首符集含有"$"时，继续处理下一个符号
        int flag = 0;   //当flag等于符号数时，即每个符号都含"$"，FIRST(vn)应当添加"$"
        for(auto it=pro.begin(); it!=pro.end(); ){
            string n = (*it);
            //递归求FIRST
            finalSize += getFirst(n);
            //保存此符号的FIRST
            set<string> nset = FIRST[n];
            //将此集加入FIRST(vn)
            for(auto nit=nset.begin(); nit!=nset.end(); nit++){
                if((*nit)!="$")
                    vset.insert(*nit);
            }
            //若此集包含空字符，继续处理下一字符；否则终止
            if(nset.count("$")){
                it++;
                flag++;
            }
            else
                break;
        }
        //若每个符号都含"$"，FIRST(vn)应当添加"$"
        if(flag == pro.size())
            vset.insert("$");
    }
    FIRST.erase(vn);
    FIRST.insert(pair<string,set<string>>(vn,vset));
    finalSize += (vset.size()-initSize);
    return finalSize - initSize;
}

/**
 * @brief 获取符号串ls的终结首符集，保存在FIRST_STRING[ls]中
 * @param ls 待求取FIRST集的符号串
*/
void getStringFirst(list<string> ls){
    
    //求符号串first不会用到自身递归，故查询到已有则直接返回
    if(FIRST_STRING.count(ls)){
        return;
    }
    
    set<string> lsset;
    if(ls.front() == "$"){
        lsset.insert("$");
        FIRST_STRING.insert(pair<list<string>, set<string>>(ls, lsset));
        return;
    }
    for(auto s : ls){
        int flag = 0;
        if(!FIRST.count(s))
            getFirst(s);
        set<string> sset = FIRST[s];
        for(auto sym : sset)
            if(sym != "$")
                lsset.insert(sym);
        
        if(sset.count("$"))
            flag++;
        else
            break;
        
        if(flag == ls.size())
            lsset.insert("$");
    }
    FIRST_STRING.insert(pair<list<string>,set<string>>(ls, lsset));
    return;

}

/**
 * @brief 获取vn的后随符号集，保存在FOLLOW[vn]中
 * @param vn 待求取FOLLOW集的符号
 * @return 本次函数是否扩充了FOLLOW集，不为0则有扩充
*/
int getFollow(string vn){
    set<string> vset;   ////vset表示符号vn所有后随符号
    if(FOLLOW.count(vn))
        vset = FOLLOW[vn];

    list<list<string>> pros = production[vn];   //vn的所有候选产生式
    //若为开始符号，则初始化加入"#"
    int initSize = vset.size();     //记录是否有终结首符集扩大
    int finalSize = initSize;       //返回finalSize-initSize表示是否有终结首符集扩大
    if(vn == START){
        vset.insert("#");
    }
    //在所有产生式中寻找跟在vn后的终结符
    for(auto v : VnSet){
        list<list<string>> vpros = production[v];
        for(list<list<string>>::const_iterator vpro = vpros.begin(); vpro!=vpros.end(); vpro++)
            for(list<string>::const_iterator sym=vpro->begin(); sym!=vpro->end(); sym++)
                if(*sym == vn ){
                    if( (++sym)!=vpro->end() && VtSet.count(*(sym))){
                    vset.insert(*sym);
                    }
                    sym--;
                }
    }
    FOLLOW.insert(pair<string,set<string>>(vn, vset));
    finalSize += vset.size() - initSize;
    //下面对vn的每条产生式视为A->αBβ处理
    for(auto pro : pros){
        //从式尾往前处理，当前处理符号记为(*sym)，即A->αBβ中的B
        list<string>::reverse_iterator sym = pro.rbegin();
        while(sym != pro.rend()){
            int symsetSize = 0; //记录*sym，即B的FOLLOW集是否增大
            //仅对*sym是非终结符时处理，即A->αBβ中的B；若*sym为终结符则跳过，视其为β的一部分
            if(VnSet.count(*sym)){

                //若sym不是式尾符号，即A->αBβ中β不为空
                if(sym != pro.rbegin()){
                    //symRight保存β
                    list<string> symRight;
                    for(list<string>::reverse_iterator it=pro.rbegin(); it!=sym; it++)
                        symRight.push_front(*it);
                    
                    //rset保存β的终结首符集
                    set<string> rset;
                    //若β仅有一个符号，则从FIRST中求集
                    if(symRight.size() == 1){
                        if(!FIRST.count(symRight.front()))
                            getFirst(symRight.front());
                        rset = FIRST[symRight.front()];
                    }
                    //若β是符号串，则从FIRST_STRING中求集
                    else{

                        if(!FIRST_STRING.count(symRight))
                            getStringFirst(symRight);
                        rset = FIRST_STRING[symRight];
                    }
                    //symset保存(*sym)的后随符号集，即A->αBβ中B的后随符号集
                    set<string> symset;
                    if(FOLLOW.count(*sym))
                        symset = FOLLOW[*sym];
                    symsetSize = symset.size();
                    //将β的所有非空首符加入B的后随符号集中
                    for(auto rsym : rset)
                        if(rsym != "$")
                            symset.insert(rsym);

                    //若β的终结首符集包含空字符，则把FOLLOWA加入FOLLOWB中，A->αBβ，此处A为vn，B为*sym，FOLLOWA为vset
                    if(rset.count("$"))
                        if(*sym != vn)  //A->αBβ中若A=B则无需进行此处加入
                            for(auto v : vset)
                                symset.insert(v);
                    //symset存入FOLLOW中
                    if(FOLLOW.count(*sym))
                        FOLLOW.erase(*sym);
                    FOLLOW.insert(pair<string, set<string>>(*sym, symset));
                    finalSize += (symset.size()-symsetSize);
                }
                //若sym是式尾符号，即A->αB，则把FOLLOWA加入FOLLOWB中
                else{
                    if(*sym != vn){  //A->αB中若A=B则无需进行此处加入
                        //获取FOLLOWB
                        set<string> symset;
                        if(FOLLOW.count(*sym))
                            symset = FOLLOW[*sym];
                        symsetSize = symset.size();
                        //加入FOLLOWA
                        for(auto v : vset)
                            symset.insert(v);
                        //保存FOLLOWB
                        if(FOLLOW.count(*sym))
                            FOLLOW.erase(*sym);
                        FOLLOW.insert(pair<string, set<string>>(*sym, symset));
                        finalSize += (symset.size()-symsetSize);
                    }
                }
            }
            sym++;
        }
    }
    
    int isExpanded =  finalSize - initSize;
    return isExpanded;
}

/**
 * @brief 从给定文件获取文法，保存在production中
 * @param fpin 指定文件流
*/
void getProduction(FILE* fpin){
    if(NULL == fpin)
        return;
    if(production.size() > 0)
        return;
    //读取文本为一条string text
    string text = getText(fpin);
    //将text按行分割得到单独的产生式，保存在list pro中
    regex pattern("\\n+");
    vector<string> pro(sregex_token_iterator(text.begin(), text.end(), pattern, -1), sregex_token_iterator());
        //将产生式按空格与tab分割，去除序号与"->"，得到产生式左侧非终结符与对应产生式右侧符号序列
    for(auto it=pro.begin(); it!=pro.end(); it++){
        regex pattern2("\\s+");
        list<string> proSplited(sregex_token_iterator((*it).begin(), (*it).end(), pattern2, -1), sregex_token_iterator());
        proSplited.pop_front(); //去除序号
        string proLeft = proSplited.front();    //保存产生式左侧
        VnSet.insert(proLeft);  //将非终结符保存到VnSet内
        proSplited.pop_front(); //去除产生式左侧
        proSplited.pop_front(); //去除"->"
        //将产生式右侧存在以左侧为key的值里
        if(!production.count(proLeft)){
            list<list<string>> lls;
            lls.push_back(proSplited);
            production.insert(make_pair(proLeft,lls));
        }
        else{
            production[proLeft].push_back(proSplited);
        }
    }
    //生成终结符集
    for(auto pros : production){        //每个非终结符
        for(auto prod : pros.second){   //每个候选产生式
            for(auto sym : prod){    //每个产生式右侧符号序列
                if(!VnSet.count(sym))
                    VtSet.insert(sym);
            }
        }
    }
    if(VtSet.count("$"))
        VtSet.erase("$");
    return;
}

/**
 * @brief 从给定文件读取所有文本到一条string内
 * @param fpin 指定文件流
 * @return 文件里的文本内容
*/
string getText(FILE* fpin){
    if(fpin == NULL)    return "";
    char str[maxSize] = {'\0'};
    char ch;
    int max = 0;
    while((ch = fgetc(fpin)) != EOF){
        str[max] = ch;
        max++;
    }
    string s;
    s = s + str;
    return s;
}

/**
 * @brief 从给定文件读取文法，初始化语法分析器
 * @param fpin 指定文件
*/
void initParser(FILE* fpin){
    if(NULL == fpin)
        return;
    getProduction(fpin);
    First();
    Follow();
    createTable();
}

/**
 * @brief 打印FIRST集
*/
void printFirst(){
    for(auto vn : FIRST){
        if(VtSet.count(vn.first)) continue;
        cout << "FIRST(" << vn.first << ")";
        if(vn.first.length() < 9 ) cout << '\t';
        cout << "\t=";
        for(auto it : vn.second){
            cout << "\t" << it;
        }
        cout << '\n';
    }
}

/**
 * @brief 向文件打印FIRST集
 * @param 指定写入文件流
*/
void printFirst(FILE* fpout){
    for(auto vn : FIRST){
        if(VtSet.count(vn.first)) continue;
        fprintf(fpout, "FIRST(%s)\t", vn.first.c_str());
        if(vn.first.length() < 5)
            fputc('\t', fpout);
        if(vn.first.length() < 9)
            fputc('\t', fpout);
        if(vn.first.length() < 13)
            fputc('\t', fpout);
        fprintf(fpout, "\t=");
        for(auto it : vn.second)
            fprintf(fpout, "\t%s", it.c_str());
        fputc('\n', fpout);
    }
}

/**
 * @brief 打印FOLLOW集
*/
void printFollow(){
    for(auto vn : FOLLOW){
        if(VtSet.count(vn.first)) continue;
        cout << "FOLLOW(" << vn.first << ")";
        if(vn.first.length() < 8 ) cout << '\t';
        cout << "\t=";
        for(auto it : vn.second){
            cout << "\t" << it;
        }
        cout << '\n';
    }
}

/**
 * @brief 向文件打印FOLLOW集
 * @param 指定写入文件流
*/
void printFollow(FILE* fpout){
    for(auto vn : FOLLOW){
        if(VtSet.count(vn.first)) continue;
        fprintf(fpout, "FOLLOW(%s)\t", vn.first.c_str());
        if(vn.first.length() < 4)
            fputc('\t', fpout);
        if(vn.first.length() < 8)
            fputc('\t', fpout);
        if(vn.first.length() < 12)
            fputc('\t', fpout);
        fprintf(fpout, "\t=");
        for(auto it : vn.second)
            fprintf(fpout, "\t%s", it.c_str());
        fputc('\n', fpout);
    }
}
