#include <stdio.h>

#include "CuTest.h"
#include "TestUT.h"
#include "TestBasicAdv.h"

CuSuite* TestUTGetSuite();
CuSuite* TestBasicAdvGetSuite();

void RunAllTests(void) {
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();
    
    CuSuiteAddSuite(suite, TestUTGetSuite());
    CuSuiteAddSuite(suite, TestBasicAdvGetSuite());

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
}

int main(int argc, char **argv)
{
    RunAllTests();

    printf("\n\nPress any key to exit\n");
    getchar( ); 
    return 0;
}
