#include <stdio.h>
#include <stdlib.h>

#include "mtft.h"
#include "debug.h"


#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

int main(int argc, char **argv)
{
    //int testValue=1;
    //double time_spent = 0.0;
    //clock_t begin = clock();
    // do some stuff here
    //sleep(3);

    if(validargs(argc, argv))
        USAGE(*argv, EXIT_FAILURE);
    if(global_options & HELP_OPTION)
        USAGE(*argv, EXIT_SUCCESS);
    int v = (global_options & (1<<30));
    int r;
    if(v){
        r = mtf_encode();
    }
    else{
        r = mtf_decode();
    }


    if(r==0){
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */
