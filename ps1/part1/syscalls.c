#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>

int main() {

    /*FILE *fp = fopen("output.txt", "w");*/
    open("output.txt", O_RDONLY);
    FILE *fp = fopen("output.txt", "w");

    /*int blah; */
    /*scanf("enter in a number %d", &blah);*/

    
    
    //scanf
    /*int blah; */
    /*scanf("enter in a number %d", &blah);*/

    //atoi
    /*int test = atoi("as8");*/

    //isalpha
    /*int test = isalpha('5');*/
    
    //str error
    /*char* strerror(int err);*/

    //fopen
    /*FILE *fp = fopen("output.txt", "w");*/

    //malloc
    /*int a = 32;*/
    /*int ptr = (int*)malloc(a * sizeof(int));*/

    //sqrt
    /*double sq = sqrt(10.66);*/


    // strcpy
    /*char* hi = "hello";*/
    /*char d[4096];*/

    /*strcpy(d, hi);*/


    //fputc
    /*FILE *fp = fopen("output.txt", "w");*/
    /*if (fp == NULL) {*/
        /*perror("Error opening file");*/
        /*return 1;*/
    /*}*/

    /*fputc('A', fp);*/

    /*fclose(fp);*/
    return 0;
}

