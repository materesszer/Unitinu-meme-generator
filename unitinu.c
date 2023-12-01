#include <stdio.h>
#include <stdlib.h>
#include "include/extensions.h"

void manipulate_image(int height,int width, RGBTRIPLE image[height][width]);

int main(int argc, char* argv[])
{
    puts("Welcome to the Unitinu meme generator.");

    if (argc != 3) 
    {
        printf("Usage: %s input_filename output_filename\n", argv[0]);
        return 701;
    }

    char* inname = argv[1];
    char* outname = argv[2];


    FILE* inp = fopen(inname, "rb");
    if (inp == NULL)
    {
        printf("Failed to open input file: %s\n", inname);
        return 702;
    }

    FILE* out = fopen(outname, "wb");

    if (out == NULL)
    {
        fclose(inp);
        printf("Failed to open output file: %s\n", outname);
        return 703;
    }

    BITMAPFILEHEADER bfheader;
    fread(&bfheader, sizeof(BITMAPFILEHEADER), 1, inp);

    BITMAPINFOHEADER biheader;
    fread(&biheader, sizeof(BITMAPINFOHEADER), 1, inp);

    int height = biheader.biHeight;
    int width = biheader.biWidth;

    RGBTRIPLE (*image)[width] = calloc(height, width * sizeof(RGBTRIPLE));

    if (image == NULL)
    {
        printf("Failed to allocate image space.");
        fclose(inp);
        fclose(out);
        return 704;
    }

    int padding = (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;

    for (int i = 0; i < height; i++)
    {
        fread(image[i], sizeof(RGBTRIPLE), width, inp);
        fseek(inp, padding, SEEK_CUR);
    }

    manipulate_image(height, width, image);

    fwrite(&bfheader, sizeof(BITMAPFILEHEADER), 1, out);
    fwrite(&biheader, sizeof(BITMAPINFOHEADER), 1, out);

    for (int i = 0; i < height; i++)
    {
        fwrite(image[i], sizeof(RGBTRIPLE), width, out);
        
        for (int i = 0; i < padding; i++)
        {
            fputc(0x00, out);
        }
    }

    printf("Image unitinu-ified successfully! result: %s\n", outname);

    free(image);

    fclose(inp);
    fclose(out);
}

void manipulate_image(int height,int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE* temp;

    for (int i = 0; i < height; i++)
    {
        for (int j = width / 2; j < width; j++)
        {
            image[i][j] = image[i][width - j - 1];
        }
    }

}
