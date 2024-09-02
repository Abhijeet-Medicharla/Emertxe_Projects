#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    /* Call check_operation */
    if(check_operation_type(argv) == e_encode)
    {
        puts("INFO: SELECTED -- Encoding");
        EncodeInfo encInfo;
        /* Read and Validate encoding arguments */
        if(read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            puts("STATUS :: SUCCESS!!! Reading and Validating encoding arguments");
            /* Start Encoding */
            if(do_encoding(&encInfo) == e_success)
            {
                puts("###********* ENCODING DONE SUCCESSFULLY **********###");
            }
            else
            {
                puts("###*************** FAILED ENCODING ***************###");
            }
        }
        else
        {
            puts("STATUS :: FAILED!!! Reading and Validating decoding arguments");
        }

    }
    else if(check_operation_type(argv) == e_decode)
    {
        puts("INFO: SELECTED -- Decoding");
        DecodeInfo decInfo;
        /* Read and Validate decoding arguments */
        if(read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            puts("STATUS :: SUCCESS!!! Reading and Validating decoding arguments");
            /* Start Decoding*/
            if(do_decoding(&decInfo) == e_success)
            {
                puts("###********* DECODING DONE SUCCESSFULLY **********###");
            }
            else
            {
                puts("###*************** FAILED DECODING ***************###");
            }
        }    
        else
        {
            puts("STATUS :: FAILED!!! Reading and Validating decoding arguments");
        }    
    }
    else
    {
        puts("Invalid Option");
        puts("*************************USAGE*************************");
        puts("ENCODING: ./a.out -e beautiful.bmp secret.txt stego.bmp");
        puts("DECODING: ./a.out -d stego.bmp output.txt");
    }

    return 0;
}

/* Check Operation type
 * Input : argument vector of command line args 
 * Output : e_encode or e_decode or e_unsupported based on argv[1] 
 * Description : Checks the argv[1] and executes the program accordingly 
 */
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    else if(argv[1] || argv[1] == NULL)
    {
        return e_unsupported;
    }
}
