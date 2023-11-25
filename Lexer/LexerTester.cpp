#include "Lexer.h"
using namespace std;

int main(){

    char path[80] = "./compilerFront/text/test/04.sy";
    FILE* fpin;
    char resultPath[80] = "./compilerFront/text/token/token04.txt";
    FILE* fpout;
    fpin = fopen(path, "r");
    fpout = fopen(resultPath, "w");

    if(fpin == NULL){
        cout << "Wrong path!" << endl;
        return 0;
    }

    cout << "The results of lexical analysis will be written to the txt file" << endl;
    vector<string> result = analyseLexer(fpin);
    print(result);
    print(result, fpout);

    fclose(fpin);
    fclose(fpout);
    cout << "Finish\n";

    system("pause");
    return 0;
}