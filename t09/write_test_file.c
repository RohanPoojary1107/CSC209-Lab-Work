#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        fprintf(stderr, "Usage: write_test_file filename\n");
        exit(1);
    }

    FILE *fp = fopen(argv[1], "wb");
    if (fp == NULL)
    {
        perror("fopen");
        exit(1);
    }

    for (int i = 0; i < 100; i++)
    {
        int random_um = (rand() % (99 - 0 + 1));
        if (fwrite(&random_um, sizeof(int), 1, fp) != 1)
        {
            fprintf(stderr, "error during reading");
            exit(1);
        }
    }

    if (fclose(fp)!= 0) {
        perror("fclose");
        return 1;
    }
    return 0;
}