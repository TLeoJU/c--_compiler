#include <stdio.h>

int main(){
    char gender;    //'m' or 'f'
    char lovePE;    //'y' or 'n'
    char goodDiet;  //'y' or 'n'
    int stature_Dad;
    int stature_Mon;

    printf("hello world");

    printf("please enter your gender ( f / m )\n");
    scanf(" %c", &gender);


    printf("please enter the height of your father (e.g. 163)\n");
    scanf("%d", &stature_Dad);

    printf("please enter the height of your mother (e.g. 163)\n");
    scanf("%d", &stature_Mon);

    printf("Do you love doing sports? ( y / n )\n");
    scanf(" %c", &lovePE);

    printf("Do you have a good diet? ( y / n )\n");
    scanf(" %c", &goodDiet);

    float height = 0.000;

    printf("Here is your height prediction: ");
    if(gender == 'm')
        height = 0.54 * (stature_Dad + stature_Mon);
    else    
        height = 0.5 * (0.923 * stature_Dad + stature_Mon); 

    if(goodDiet == 'y') height *= 1.015;
    if(lovePE == 'y') height *= 1.02;
    printf("%f cm", height);


    return 0;

}