#include <stdio.h>
#include <stdlib.h>
#include "include/extensions.h"

#define proper_usage_argc 3

void ensure_proper_usage(int argc, char* argv[], int error_code);
void manipulate_image(int height,int width, RGBTRIPLE image[height][width]);
void print_start();
FILE* open_file(const char* filename, const char* mode);
void file_open_fail(FILE* inp, FILE* out, int error_code);
void image_allocation_fail(FILE* in, FILE* out, int error_code);
void read_headers(BITMAPFILEHEADER* bfh, BITMAPINFOHEADER* bih, FILE* stream);
void write_headers(BITMAPFILEHEADER* bfh, BITMAPINFOHEADER* bih, FILE* stream);
void read_image(int padding, int height, int width, RGBTRIPLE image[height][width], FILE* stream);
void write_image(int padding, int height, int width, RGBTRIPLE image[height][width], FILE* stream);
 


int main(int argc, char* argv[])
{
    print_start();
    ensure_proper_usage(argc, argv, 701);
    char* inname = argv[1];
    char* outname = argv[2];

    FILE* inp = open_file(inname, "rb");
    if (inp == NULL)
    {
        file_open_fail(inp, NULL, 702);
    }
    FILE* out = open_file(outname, "wb");
    if (out == NULL)
    {
        file_open_fail(inp, out, 703);
    }

    BITMAPFILEHEADER bfheader;
    BITMAPINFOHEADER biheader;
    read_headers(&bfheader, &biheader, inp);

    int height = biheader.biHeight;
    int width = biheader.biWidth;
    RGBTRIPLE (*image)[width] = calloc(height, width * sizeof(RGBTRIPLE));
    if (image == NULL)
    {
        image_allocation_fail(inp, out, 704);
    }

    int padding = (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;

    read_image(padding, height, width, image, inp);

    manipulate_image(height, width, image);

    write_headers(&bfheader, &biheader, out);
    write_image(padding, height, width, image, out);
   
    printf("Image unitinu-ified successfully! result: %s\n", outname);
    free(image);
    fclose(inp);
    fclose(out);

    return 0;
}

void manipulate_image(int height,int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = width / 2; j < width; j++)
        {
            image[i][j] = image[i][width - j - 1];
        }
    }
}

void print_start()
{
    puts("Welcome to the Unitinu meme generator.");
    puts("NOTE: Program only accepts .bmp files");
}

void ensure_proper_usage(int argc, char* argv[], int error_code)
{
    if (argc != proper_usage_argc) 
    {
        printf("Usage: %s input_filename output_filename\n", argv[0]);
        exit(error_code);
    }
}

FILE* open_file(const char* filename, const char* mode)
{
    FILE* file = fopen(filename, mode);
    if (file == NULL)
    {
        printf("Failed to open file: %s", filename);
    }
    return file;
}

void file_open_fail(FILE* inp, FILE* out, int error_code) {
    if (inp != NULL) {
        fclose(inp);
    }
    if (out != NULL) {
        fclose(out);
    }
    exit(error_code);
}

void image_allocation_fail(FILE* in, FILE* out, int error_code)
{
    printf("Failed to allocate image space.");
    fclose(in);
    fclose(out);
    exit(error_code);
}

void read_headers(BITMAPFILEHEADER* bfh, BITMAPINFOHEADER* bih, FILE* stream)
{
    fread(bfh, sizeof(BITMAPFILEHEADER), 1, stream);
    fread(bih, sizeof(BITMAPINFOHEADER), 1, stream);
}

void write_headers(BITMAPFILEHEADER* bfh, BITMAPINFOHEADER* bih, FILE* stream)
{
    fwrite(bfh, sizeof(BITMAPFILEHEADER), 1, stream);
    fwrite(bih, sizeof(BITMAPINFOHEADER), 1, stream);
}

void read_image(int padding, int height, int width, RGBTRIPLE image[height][width], FILE* stream)
{
    for (int i = 0; i < height; i++)
    {
        fread(image[i], sizeof(RGBTRIPLE), width, stream);
        fseek(stream, padding, SEEK_CUR);
    }
}

void write_image(int padding, int height, int width, RGBTRIPLE image[height][width], FILE* stream)
{
    for (int i = 0; i < height; i++)
    {
        fwrite(image[i], sizeof(RGBTRIPLE), width, stream);
        
        for (int i = 0; i < padding; i++)
        {
            fputc(0x00, stream);
        }
    }
}

