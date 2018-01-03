#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{
    time_t timep;
    struct tm timec;
    FILE *pf;

    if(argc < 2) {
        printf("usage: mkdate outfile\n");
        return 1;
    }

    time(&timep);
    timec = *localtime(&timep);

    //pf = fopen("./lib/mkdate.c", "w");
    pf = fopen(argv[1], "w");
    if(NULL == pf) {
        printf("can not open out file.\n");
        return 1;
    }

    fprintf(pf, "const unsigned char _mk_year = %d;\n", timec.tm_year-100);
    fprintf(pf, "const unsigned char _mk_month = %d;\n", timec.tm_mon+1);
    fprintf(pf, "const unsigned char _mk_day = %d;\n", timec.tm_mday);
    fprintf(pf, "const unsigned char _mk_hour = %d;\n", timec.tm_hour);
    fprintf(pf, "const unsigned char _mk_minute = %d;\n", timec.tm_min);

    fclose(pf);

    return 0;
}
