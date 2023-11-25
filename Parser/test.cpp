#include "Parser.h"
using namespace std;

int main() {
    char readFile[80] = "./compilerFront/text/Grammar.txt";
    FILE* fpin = fopen(readFile, "r");
    if(fpin == NULL){
        cout << "wrong path!\n";
        system("pause");
        return 0;
    }
    initParser(fpin);
    fclose(fpin);

    char readToken[80] = "./compilerFront/text/token/token05.txt";
    FILE* fptoken = fopen(readToken, "r");
    if(fptoken == NULL){
        cout << "wrong path!\n";
        system("pause");
        return 0;
    }
    string text = getText(fptoken);
    regex pattern("\\n+");
    vector<string> token(sregex_token_iterator(text.begin(), text.end(), pattern, -1), sregex_token_iterator());
    if(analyseParser(token))
        cout << "Accepted!" << endl;
    else
        cout << "Error!" << endl;
    fclose(fptoken);

    char writeResult[80] = "./compilerFront/text/result/result05.txt";
    FILE* fpresult = fopen(writeResult, "w");
    if(fpresult == NULL){
        cout << "wrong path!\n";
        system("pause");
        return 0;
    }
    for(auto it : result){
        fprintf(fpresult, "%s\n", it.c_str());
    }
    fclose(fpresult);


    // 打印FIRST集和FOLLOW集
    // printFirst();
    // cout << endl;
    // printFollow();

    // 向文件写入每条产生式右侧符号串的FIRST集
    // char writeFirstX[80] = "./compilerFront/text/FirstX.txt";
    // FILE* wfirstX = fopen(writeFirstX, "w");
    // for(auto pro : production){
    //     for(auto pr : pro.second){
    //         fprintf(wfirstX, "FIRSTX( ");
    //         for(auto p : pr)
    //             fprintf(wfirstX, "%s ", p.c_str());
    //         fprintf(wfirstX, ")\t");
    //         getStringFirst(pr);
    //         set<string> fp = FIRST_STRING[pr];
    //         for(auto f : fp)
    //             fprintf(wfirstX, "%s\t", f.c_str());
    //         fputc('\n', wfirstX);
    //     }
    // }
    // fclose(wfirstX);

    // 向文件写入FIRST集
    // char writeFirst[80] = "./compilerFront/text/First.txt";
    // FILE* wfirst = fopen(writeFirst, "w");
    // printFirst(wfirst);
    // fclose(wfirst);

    // 向文件写入FOLLOW集
    // char writeFollow[80] = "./compilerFront/text/Follow.txt";
    // FILE* wfollow = fopen(writeFollow, "w");
    // printFollow(wfollow);
    // fclose(wfollow);

    system("pause");
    return 0;
}
