/* Inclusion of header files */
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("Width of the image = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("Height of the image = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/* Validating the i/p args
 * Input : Source image, Secret text file and Stego image
 * Output : Validate the file extensions
 * Decription : Validate and fetch the filenames in the respective char pointers 
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    /* get .bmp file -- ./a.out -e beautiful.bmp secret.txt */
    if((argv[2] != NULL) && strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        // fetch the name of image file
        encInfo -> src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }
    /* get .txt file -- ./a.out -e beautiful.bmp secret.txt*/
    if((argv[3] != NULL) && strcmp(strstr(argv[3], "."), ".txt") == 0)
    {
        // fetch the name of the secret text file
        encInfo -> secret_fname = argv[3];
    }
    else
    {
        return e_failure;
    }
    /* Check for optional file name*/
    if(argv[4] != NULL)
    {
        encInfo -> stego_image_fname = argv[4]; 
        puts("optional filename read");
    }
    else
    {
        encInfo -> stego_image_fname = "stego.bmp";
    }

    return e_success;
}

/* Get secret file size
 * Input : Secret text FILE pointer
 * Output : size of secret text file
 * Description : Move the FILE pointer to EOF and fetch its position
 */
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

/* Checking image capacity
 * Input : pointer to a structure variable
 * Output : e_success or e_failure
 * Description : Checks capacity of image if it can encode the secret data or not 
 */
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);
    if(encInfo -> image_capacity > (54 + 16 + 32 + 32 + 32 + (encInfo -> size_secret_file * 8)))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/* Copying bmp header
 * Input : source image FILE pointer 
 * and destination image FILE pointer
 * Output : returns e_success after copying the header 
 * Description : Fetch 54 bytes from source image and write it in destination image 
 */
Status copy_bmp_header(FILE *fptr_src_img, FILE *fptr_dest_img)
{
    char str[54];
    fseek(fptr_src_img, 0, SEEK_SET);
    fread(str, sizeof(char), 54, fptr_src_img);
    fwrite(str, sizeof(char), 54, fptr_dest_img);
    return e_success;
}

/* Encoding Byte of data in 8 LSB bytes
 * Input : 1 byte character data and pointer to 8bytes of src image data 
 * Output : character data is encoded in the src image data
 * Description : Fetch 8 bytes of src image data,
 * encode each bit of character data into each LSB of 8bytes data 
 */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    uint mask = 1 << 7;

    for(int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data & mask) >> (7 - i));
        mask = mask >> 1;
    }
}

/* Encoding Data to Image
 * Input : const character pointer, size, FILE pointer of src image, 
 * FILE pointer of dest image, pointer to structure variable
 * Output : e_success, after encode_byte_to_lsb is written in dest image
 * Description : Fetch 8 bytes of src image data, pass the data and a character to encode_byte_to_lsb,
 * which will encode the character data into 8 LSB bytes, this encoded data is written in dest image.
 */
Status encode_data_to_image(const char *data, int size, FILE *fptr_src, FILE *fptr_stego, EncodeInfo *encInfo)
{
    for(int i = 0; i < size; i++)
    {
        // Read 8 bytes of RGB from beautiful.bmp
        fread(encInfo -> image_data, 8, sizeof(char), fptr_src);
        encode_byte_to_lsb(data[i], encInfo -> image_data);
        fwrite(encInfo -> image_data, 8, sizeof(char), fptr_stego);
    }
    return e_success;
}

/* Encoding Magic String
 * Input : character pointer to magic_string, pointer to structure variable
 * Output : e_success, after encoding magic_string into the destination image
 * Description : Pass the magic string, length of magic string,
 * src and dest image FILE pointers to encode_data_to image function 
 * which will encode the magic_string. 
 */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

/* Encoding size(integer data) 
 * Input : character pointer to buffer data, integer size
 * Output : Encodes 32 bits of information in 32 bytes of LSB data
 * Description : Fetches 32 bytes of data and
 * encodes each bit of 32 bit size into the LSB of 32 bytes
 */
Status encode_size_to_lsb(char *buffer, int size)
{
    uint mask = 1 << 31;
    for(int i = 0; i < 32; i++)
    {
        buffer[i] = (buffer[i] & 0xFE) | ((size & mask) >> (31 - i));
        mask = mask >> 1;
    }
}

/* Encoding size
 * Input : integer size, source and dest image FILE pointers
 * Output : e_success, after encoding size
 * Decription : Fetches 32 bytes of src image data, encodes size in the data,
 * writes the encoded 32 bytes of data into dest image data
 */
Status encode_size(int size, FILE *fptr_src_img, FILE *fptr_stego_img)
{
    char str[32];

    fread(str, 32, sizeof(char), fptr_src_img);
    encode_size_to_lsb(str, size);
    fwrite(str, 32, sizeof(char), fptr_stego_img);
    return e_success;
}

/* Encoding secret file extension
 * Input : character pointer to secret file extension, pointer to structre variable
 * Output : e_success, after encoding the secret file extension into destination image
 * Description : Pass file_ext, length of file_ext, FILE pointer to src and dest image and
 * structure variable to encode_data_to_image function which will encode the secret file extension 
 */
Status encode_secret_file_extn(const char *file_ext, EncodeInfo *encInfo)
{
    file_ext = ".txt";
    encode_data_to_image(file_ext, strlen(file_ext), encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo);
    return e_success;
}

/* Encoding secret file size
 * Input : Secret file size , pointer to structure variable
 * Output : e_success, after encoding secret file size
 * Description : Fetches 32 bytes of src image data and encodes size of secret file,
 * writes the 32bytes of data into dest image data
 */
Status encode_secret_file_size(long int file_size, EncodeInfo *encInfo)
{
    char str[32];

    fread(str, 32, sizeof(char), encInfo-> fptr_src_image);
    encode_size_to_lsb(str, file_size);
    fwrite(str, 32, sizeof(char), encInfo->fptr_stego_image);
    return e_success;
}

/* Encoding secret file data
 * Input : pointer to structure variable
 * Output : e_success, after encoding secret file data
 * Description : Fetch each character of the secret data, fetch 8 bytes of src image data,
 * encode the character in LSB of 8 Bytes of data, write the encoded data into dest image 
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;

    fseek(encInfo ->fptr_secret, 0, SEEK_SET);
    for(int i = 0; i < encInfo->size_secret_file ; i++)
    {
        fread(encInfo ->image_data, 8, sizeof(char), encInfo -> fptr_src_image);
        fread(&ch, 1, sizeof(char), encInfo -> fptr_secret);
        encode_byte_to_lsb(ch, encInfo -> image_data);
        fwrite(encInfo->image_data, 8, sizeof(char), encInfo->fptr_stego_image);
    }
    return e_success;
}

/* Copying remaining data
 * Input : source and destinaton image FILE pointers
 * Output : e_success
 * Description : Fetch the remaining src image data and write it into the dest image data
 */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_stego)
{
    char ch;
    while(fread(&ch, 1, 1, fptr_src) > 0)
    {
        fwrite(&ch, 1, 1, fptr_stego);
    }

    return e_success;
}

/* Do_encoding
 * Input : pointer to structure variable
 * Output : e_success or e_failure, based on the function output
 * Description : Performs all necessary operations for encoding secret data into bmp image
 */
Status do_encoding(EncodeInfo *encInfo)
{
    // Call the rest of the function for encoding
    // Open the files
    if(open_files(encInfo) == e_success)
    {
        puts("STATUS :: SUCCESS!!! Opened all the files");
        puts("STATUS :: Started Encoding");
        // Check capacity
        if(check_capacity(encInfo) == e_success)
        {
            puts("STATUS :: SUCCESS!!! Image is capable to hide the secret data");
            // Copy bmp header info
            if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
            {
                puts("STATUS :: SUCCESS!!! Copied bmp header");
                // Encode Magic String
                if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    puts("STATUS :: SUCCESS!!! Encoded Magic string");
                    // Encode secret file extension size
                    if(encode_size(strlen(".txt"), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                    {
                        puts("STATUS :: SUCCESS!!! Encoded Secret file extension size");
                        // Encode secret file extension
                        if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
                        {
                            puts("STATUS :: SUCCESS!!! Encoded secret file extension");
                            // Encode secret file size
                            if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                            {
                                puts("STATUS :: SUCCESS!!! Encoded secret file size");
                                // Encode secret file data
                                if(encode_secret_file_data(encInfo) == e_success)
                                {
                                    puts("STATUS :: SUCCESS!!! Encoded secret data");
                                    // Copy remaining image data
                                    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                                    {
                                        puts("STATUS :: SUCCESS!!! Copied remaining RGB data");
                                    }
                                    else
                                    {
                                        puts("STATUS :: FAILED!!! Copying remaining data");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    puts("STATUS :: FAILED!!! Encoding secret data");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                puts("STATUS :: FAILED!!! Encoding secret file size");
                                return e_failure;
                            }
                        }
                        else
                        {
                            puts("STATUS :: FAILED!!! Encoding secret file extension");
                            return e_failure;
                        }
                    }
                    else
                    {
                        puts("STATUS :: FAILED!!! Encoding secret file extension");
                        return e_failure;
                    }
                }
                else
                {
                    puts("STATUS :: FAILED!!! Encoding magic string");
                    return e_failure;
                }
            }
            else
            {
                puts("STATUS :: FAILED!!! Copying the header");
                return e_failure;
            }
        }
        else
        {
            puts("STATUS :: FAILED!!! Image size is not sufficient for Encoding");
            return e_failure;
        }
    }
    else
    {
        puts("STATUS :: FAILED!!! Opening the files");
        return e_failure;
    }
    return e_success;
}
