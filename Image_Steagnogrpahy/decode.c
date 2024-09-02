/* Inclusion of header files */

#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Function definitions */
 
/* Get File pointers for i/p and o/p files
 * Input : Stego Image and Output text 
 * Output: FILE pointer for the above files
 * Return Value: e_success or e_failure, on file errors
 */
Status dec_open_files(DecodeInfo *decInfo)
{
    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Error Handling
    if(decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",decInfo->stego_image_fname);
        return e_failure;
    }

    // Output text file
    decInfo->fptr_out_file = fopen(decInfo->output_text_fname, "w");
    // Error Handling
    if(decInfo->fptr_out_file == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",decInfo->output_text_fname);
        return e_failure;
    }

    // No failure means Success
    return e_success;
}

/* Validating the i/p args
 * Input : Stego image, Output text file
 * Output : Validate the file extensions
 * Description : Validate and fetch the filenames in the respective char pointers 
 */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    /* get .bmp file -- ./a.out -d stego.bmp */
    if((argv[2] != NULL) && strcmp(strstr(argv[2], "."),".bmp") == 0)
    {
        //fetch the name of stego image file
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    if(argv[3] != NULL)
    {
        decInfo->output_text_fname = argv[3];
    }
    else
    {
        decInfo->output_text_fname = "output.txt";
    }

    return e_success;
}

/* Convert LSB to Byte 
 * Input : 8 Bytes of image data
 * Output : A single character
 * Description : Fetch 8 LSB bits from 8 Bytes of image data 
 * convert 8 LSB bits to a character  
 */
char decode_lsb_to_byte(char *image_buffer)
{
    unsigned char ch = 0, mask = 0x01;
    for(int i = 0; i < 8; i++)
    {
        ch = ch | ((image_buffer[i] & mask) << (7 - i) );
    }
    return ch;
}

/* Decode image data
 * Input : const string, string length, 
 * FILE pointer, Pointer to a structure variable
 * Output : e_success or e_failure, based on comparison of decoded data 
 * Description : Reads 8 bytes of stego_image data 
 * Compares a character from const char pointer and 
 * return character of decode_lsb_to_byte function
 */
Status decode_data_from_image(const char *data, int size, FILE *fptr_stego, DecodeInfo *decInfo)
{
    fseek(fptr_stego, 54, SEEK_SET);
    for(int i = 0; i < size; i++)
    {
        // Read 8 bytes of RGB from stego.bmp
        fread(decInfo->stego_data, 8 , sizeof(char),fptr_stego);
        if(data[i] != decode_lsb_to_byte(decInfo->stego_data))
        {
            return e_failure;
        }
    }
    return e_success;
}

/* Decode magic string
 * Input : const char pointer to Magic_String,
 * pointer to structure variable 
 * Output : e_success or e_failure 
 * Description : Pass magic_string, length of magic_string, FILE pointer of stego image,
 * pointer to structre variable to decode_data_from_image function which will return e_success or
 * e_failure based upon the comparison of magic_string and decoded magic_string
 */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    decode_data_from_image(magic_string, strlen(magic_string), decInfo->fptr_stego_image, decInfo);
    return e_success;
}

/* Decode size from LSB
 * Input : char pointer to data buffer
 * Output : integer size
 * Decription : Fetches 32 LSB bits from 32 bytes of image data in data buffer
 * Converts 32 bits of LSB into integer data and returns the integer data 
 */
int decode_lsb_to_size(char *buffer)
{
    uint size = 0, mask = 0x01;
    for(int i = 0; i < 32; i++)
    {
        size = size | ((buffer[i] & mask) << (31 - i));
    }
    return size;
}

/* Decode secret file extension size
 * Input : FILE pointer to stego image, pointer to structure variable
 * Output : e_success or e_failure, based on decoding of file extension size
 * Description : Fetches 32 Bytes Image data,passes the data to decode_lsb_to_size function
 * Stores the return value of the function 
 */
Status decode_secret_file_ext_size(FILE *fptr_stego, DecodeInfo *decInfo)
{
    fread(decInfo->dec_size, 8 , sizeof(int), decInfo->fptr_stego_image);
    decInfo -> sec_file_ext_size = decode_lsb_to_size(decInfo->dec_size);  
    printf("INFO: Size of Secret file extension - %d\n",decInfo -> sec_file_ext_size);
    return e_success;  
}

/* Decode character from LSB
 * Input : char pointer to data buffer, integer size 
 * and pointer to structure variable   
 * Output : String 
 * Description : Fetch 32 bits of LSB from 32 Bytes of stego image data
 * Group 32 LSB bits in single bytes and convert each byte of data to char 
 */
Status decode_lsb_to_char(char *buffer, int size, DecodeInfo *decInfo)
{
    unsigned char ch[size + 1], mask = 0x01;
    for(int i = 0; i < size; i++)
    {
        ch[i] = decode_lsb_to_byte(&buffer[8 * i]);
    }
    ch[size] = '\0';
    printf("INFO: Secret file extension - \"%s\"\n",ch);

    if(strlen(ch) == size)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
} 

/* Decode secret file extension
 * Input : FILE pointer to stego image, 
 * pointer to structure variable
 * Output : e_success or e_failure
 * Description : Fetch sec_file_ext_size bytes of stego image data 
 * and pass the data to decode_lsb_to_char function
 */
Status decode_sec_file_ext(FILE *fptr_stego, DecodeInfo *decInfo)
{
    fread(decInfo->dec_size , 8, sizeof(int), decInfo ->fptr_stego_image);
    if(decode_lsb_to_char(decInfo -> dec_size, decInfo->sec_file_ext_size, decInfo) == e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/* Decode secret file size
 * Input : FILE pointer to stego image and pointer to structure variable 
 * Output : e_success or e_failure
 * Description : Fetch 32 LSB bits from 32 bytes of stego image data 
 * and pass the data to decode_lsb_to_size function which returns size of the secret file
 */ 
Status decode_sec_file_size(FILE *fptr_stego, DecodeInfo *decInfo)
{
    fread(decInfo->dec_size, 8 , sizeof(int), decInfo->fptr_stego_image);
    decInfo -> sec_file_size = decode_lsb_to_size(decInfo->dec_size);  
    printf("INFO: Size of secret file - %d\n",decInfo -> sec_file_size);
    return e_success;
}

/* Decode secret data
 * Input : secret_file_data size, FILE pointer to stego image,
 * FILE pointer to output text file and pointer to structure variable 
 * Output : e_success or e_failure
 * Description : Fetch secret_file_size LSB bits 
 * from secret_file_size bytes of stego image data
 * and pass it to decode_lsb_to_byte function 
 */
Status decode_sec_data(int f_size, FILE *fptr_stego, FILE *fptr_output, DecodeInfo *decInfo)
{
    for(int i = 0; i < f_size; i++)
    {
        // Read 8 bytes of RGB fro stego.bmp
        fread(decInfo->stego_data, 8, sizeof(char), fptr_stego);
        fputc(decode_lsb_to_byte(decInfo -> stego_data),fptr_output);
    }
    return e_success;
}

/* Start Decoding
 * Input : pointer to structure variable
 * Output : e_success or e_failure 
 * Description : Call all the functions needed for decoding
 */
Status do_decoding(DecodeInfo *decInfo)
{
    // Call the rest of the functions for decoding
    // Open the files
    if(dec_open_files(decInfo) == e_success)
    {
        puts("STATUS :: SUCCESS!!! Opened files");
        puts("STATUS :: Started Decoding");
        // Decode Magic String
        if(decode_magic_string(MAGIC_STRING, decInfo) == e_success)
        {
            puts("STATUS :: SUCCESS!!! Magic String Verified");
            // Decode secret file extension size
            if(decode_secret_file_ext_size(decInfo->fptr_stego_image, decInfo) == e_success)
            {
                puts("STATUS :: SUCCESS!!! Decoded secret file extension size");
                // Decode secret file extension
                if(decode_sec_file_ext(decInfo->fptr_stego_image, decInfo) == e_success)
                {
                    puts("STATUS :: SUCCESS!!! Decoded secret file extension");
                    // Decode secret file size
                    if(decode_sec_file_size(decInfo->fptr_stego_image, decInfo) == e_success)
                    {
                        puts("STATUS :: SUCCESS!!! Decoded secret file size");
                        // Decode secret data
                        if(decode_sec_data(decInfo->sec_file_size, decInfo->fptr_stego_image, decInfo->fptr_out_file, decInfo) == e_success)
                        {
                            printf("STATUS :: SUCCESS!!! Decoded secret data from \"stego.bmp\"\n");
                            return e_success;
                        }
                        else
                        {
                            printf("STATUS :: FAILED!!! Decoding secret data from \"stego.bmp\"\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        puts("STATUS :: FAILED!!! Decoding secret file size");
                        return e_failure;
                    }
                }
                else
                {
                    puts("STATUS :: FAILED!!! Decoding file extension");
                    return e_failure;
                }
            }
            else
            {
                puts("STATUS :: FAILED!!! Decoding secret file extensio size");
                return e_failure;
            }
        }
        else
        {
            printf("STATUS :: FAILED!!! Magic String not verified, \"stego.bmp\" does not have encoded data\n");
            return e_failure;
        }
    }
    else
    {
        puts("STATUS :: FAILED!!! Opening the files");
        return e_failure;
    }
}