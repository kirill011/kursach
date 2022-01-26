// StbSample.cpp : This file contains the 'main' function. Program execution begins and ends there.
//image[realChannelsNum * (y * width + x)]

#include "pch.h"
#include <iostream>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define realChannelsNum 3


unsigned char Crc8(unsigned char* pcBlock, unsigned int len) //������� ������� ����������� ����� ���������� crc8
{
    unsigned char crc = 0xFF;
    unsigned int i;

    while (len--)
    {
        crc ^= *pcBlock++;

        for (i = 0; i < 8; i++)
            crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;     
    }

    return crc;
}

int schetSim(FILE* fp) // ������� ������� ���������� �������� � ��������� �����
{
    int num = 0;
    while (getc(fp) != EOF)  
    {
        num++;                       
    }
    fseek(fp, 0, SEEK_SET);
    num++;
    return num;
}

void MassivSoSlovami(FILE* fp, unsigned char* arr)    //������� ���������� ����� �� ���������� ����� � ������
{
    int a;
    int i = 0;
    while ((a = getc(fp)) != EOF)       
    {
        *(arr + i) = a;
        i++;
    }
    arr[i] = 0;
    fseek(fp, 0, SEEK_SET);
}

int PobitoviyMassiv(char text, int bit, int i)//������� ���������� ��� ������� �� i ����� ������� text 
{
    bit = text & ((int)pow(2, i));                  
    return bit >> i;
}

void shifrovanie(unsigned char* image,unsigned char* text, int sim) // ������� ������� ����� � ��������
{
    int i = 0, g = 0;
    int x = 0, y = 0, bit = 0;
    for (x = 0; x < sim * 8; x++)
    {
        g++;
        if (i < 8)
        {
            g--;
            image[x] = image[x] & 254;               // �������� ��������� ��� ����� image[x]
            image[x] += PobitoviyMassiv(text[g], bit, i);       // ���������� � ��������� ��� ����� image[x] ��� ������� �� i ����� text[g] �������                  
            i++;
        }
        else {
            x--;
            i = 0;
        }
    }
    unsigned char crc = Crc8(image, sim * 8);     // ������� crc8
    i = 0;
    for (x = sim * 8; x < sim * 8 + 8; x++)
    {
        image[x] = image[x] & 254;
        image[x] += PobitoviyMassiv(crc, bit, i);    // ��������� crc8  ����� ���������� ������� ������
        i++;
    }
}


int binToDec(int* bit, FILE* fp, int kod)    // � ������� bit ����� ���� ����������� �� ��������. ������� ��������� 8 ��� � 1 ���������� ����� � ���������� ��� ����� � ���� �� ��������� fp
{
    unsigned char text = 0;
    static int sc = 0;
    for (int i = 0; i < 8; i++)
    {
        text = text + bit[i] * pow(2, i);
    }
    if (text == 0)
    {
        return 0;
    }
    else
    {
        if (kod != 0)
        {
            fprintf(fp, "%c", text);
            sc++;
            return 1;
        }
        else
        {
            return text;
        }
    }
}

unsigned char deshif(unsigned char* image, int* bit, int width, int height, FILE* fp) // ������� ��������� ���� �� �������� � ���������� � ������� binToDec
{
    int i = 0;
    int g = 0;
    int x = 0;
    int kod = 1;
    unsigned char crc;
    while (kod < 2)
    {
        if (i < 8 )
        {
            bit[i] = image[x] & 1;
            x++;
            i++;
        }
        else
        {
            i = 0;
            kod = binToDec(bit, fp, kod);
            if (kod != 1 && kod != 0)
            {
                crc = kod;
                fseek(fp, 0, SEEK_SET);
                return crc;
            }
        }
    }
}

int schetchikSim(unsigned char* image, int* bit)   //������� ������� ���������� �������� � ��������
{
    int i = 0;
    int x = 0;
    int kod = 1;
    int schetchik = 0;
    while (kod != 0)
    {
        if (i < 8)
        {
            bit[i] = image[x] & 1;
            x++;
            i++;
        }
        else
        {
            unsigned char text = 0;
            for (int i = 0; i < 8; i++)
            {
                text = text + bit[i] * pow(2, i);
            }
            schetchik++;
            i = 0;
            if (text == 0)
            {
                kod = 0;
            }
        }
    }
    return schetchik;
}


void proverka(void* fp) // ������� ��������� ���������
{
    if (fp == 0)
    {
        printf("���� �� ������"); 
        exit(2);
    }
}

int main()
{
    setlocale(LC_ALL, "");
    printf("�� ���� ��������� ���������� ��������\n(��� 2 ������ � �������������� �����������)\n � ������� bmp ��� png � ��� ��x����� ����� \n(��� ���������� � ���� ������ ���� � �������)\n\n ������� ����� ������:\n 1 - ������������ \n2 - ������������ \n\n���� �����: ");
    int rejim;
    scanf("%d", &rejim);
    if (rejim == 1)
    {
        printf("\n������� ��� �����������\n\n���� �����: ");
        char stroka[_MAX_PATH] = { 0 };
        scanf("%s", &stroka);
        int width, height, channels;
        unsigned char* image = stbi_load(stroka,
            &width,
            &height,
            &channels,
            STBI_rgb);
        printf("����������� ���������: %d x %d , %d �������\n", width, height, channels);
        if (!image)
        {
            printf("�� �������");
            return 1;
        }
        printf("\n������� ��� ���������� �����\n\n���� �����: ");
        char stroka1[_MAX_PATH] = { 0 };
        scanf("%s", &stroka);
        FILE* fp = fopen(stroka, "rt");
        proverka(fp);
        printf("������� �������� ������ ������(1-bmp, 2-png\n");
        int viv = 0;
        scanf("%d", &viv);
        int sim = schetSim(fp);
        unsigned char* text = (unsigned char*)malloc(sizeof(unsigned char) * sim);  
        proverka(text);
        MassivSoSlovami(fp, text);

        if (3 * width * height < sim * 8 - 8)
        {
            printf("����� ������� �������");
            return 1;
        }
        shifrovanie(image, text, sim);

        if (viv == 1)
        {
            stbi_write_bmp("out.bmp", width, height, realChannelsNum, image);
        }
        if (viv == 2)
        {
            stbi_write_png("out.png", width, height, realChannelsNum, image, realChannelsNum * width);
        }
        free(text);
        fclose(fp);
        stbi_image_free(image);
    }

    if (rejim == 2)
    {
        printf("\n������� ��� �����������\n\n���� �����: ");
        char stroka[_MAX_PATH] = { 0 };
        scanf("%s", &stroka);
        int width, height, channels;
        unsigned char* image = stbi_load(stroka,
            &width,
            &height,
            &channels,
            STBI_rgb);
        printf("����������� ���������: %d x %d , %d �������\n", width, height, channels);
        if (!image)
        {
            printf("����� ������� �������");
            return 1;
        }
        printf("\n������� ��� ���������� �����\n\n���� �����: ");
        char stroka1[_MAX_PATH] = { 0 };
        scanf("%s", &stroka);
        unsigned char crcdeshif, crcshif;
        int schetchik;
        FILE* fp = fopen(stroka, "w+t");
        proverka(fp);
        int bit[8];
        schetchik = schetchikSim(image, bit);
        crcdeshif = Crc8(image, schetchik * 8);
        crcshif = deshif(image, bit, width, height, fp);
        if (crcdeshif == crcshif)
        {
            printf("����� �� ��������");
        }
        else 
        {
            printf("����� ��������");
        }
        fclose(fp);
        stbi_image_free(image);
    }
    if(rejim != 1 && rejim != 2)
    {
        printf("�������� ����� 1 ��� 2");
    }

    return 1;
}