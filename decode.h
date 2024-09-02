#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h" 

#define MAX_STEGO_BUF_SIZE      8
#define MAX_FIL_BUF_SIZE        MAX_STEGO_BUF_SIZE * 4
/*
 * Structure to store information required for
 * decoding secret data into output file
 */
typedef struct _DecodeInfo
{
    /* Stego Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char stego_data[MAX_STEGO_BUF_SIZE];
    char dec_size[MAX_FIL_BUF_SIZE];
    int sec_file_ext_size;
    int sec_file_size;

    /* Output file Info */
    char *output_text_fname;
    FILE *fptr_out_file;

} DecodeInfo;

/* Decoding Function Prototypes */

/* Read and Validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the Decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointer for i/p and o/p files */
Status dec_open_files(DecodeInfo *decInfo);

/* Fetch Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Decode Data From Image */
Status decode_data_from_image(const char *data, int size, FILE *fptr_stego, DecodeInfo *decInfo);

/* Decode LSB to Byte */
char decode_lsb_to_byte(char *image_buffer);

/* Decode Secret file extension size */
Status decode_secret_file_ext_size(FILE *fptr_stego, DecodeInfo *decInfo);

/* Decode Secret file extension */
Status decode_sec_file_ext(FILE *fptr_stego, DecodeInfo *decInfo);

/* Decode LSB to Char */
Status decode_lsb_to_char(char *buffer, int size, DecodeInfo *decInfo);

/* Decode Secret file size */
Status decode_sec_file_size(FILE *fptr_stego, DecodeInfo *decInfo);

/* Decode Secret data */
Status decode_sec_data(int f_size, FILE *fptr_stego, FILE *fptr_output, DecodeInfo *decInfo);


#endif