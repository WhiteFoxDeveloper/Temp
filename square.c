#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

void printDiagram(const char *name_input_bin, int x1, int x, double s)
{
    // Инициализация файла и переменных
    int fsize_is, buff, i, j, max, min;
    float *y, fmin, fmax;
    fpos_t pos;
    FILE *f_is = fopen(name_input_bin, "rb");

    // Проверка файловых ошибок и вывод ошибки
    if (!f_is)
    {
        perror("Open file error");
        return;
    }

    // Чтение длины файла
    fread(&fsize_is, sizeof(int), 1, f_is);

    // Проверка возможной ошибки выхода из диапазона
    if (fsize_is <= x1)
    {
        printf("Starting point has gone beyond the signal range\n");
        return;
    }

    // Замена области отрисовки
    buff = x1 + x >= fsize_is;
    if (buff || x == 0)
    {
        x = fsize_is - x1;
        if (buff)
        {
            printf("The required range went beyond the existing signal range\n");
        }
    }

    // Инициализация массива и выгрузка в него сигнала
    y = malloc(sizeof(float) * fsize_is);
    pos = sizeof(int) + x1 * sizeof(float);
    fsetpos(f_is, &pos);
    fread(y, sizeof(float), x, f_is);

    // Поиск минимального и максимального значения сигнала
    fmin = y[0], fmax = y[0];
    for (i = 0; i < x; i++)
    {
        if (fmin > y[i])
        {
            fmin = y[i];
        }
        else if (fmax < y[i])
        {
            fmax = y[i];
        }
    }

    // Вычисление минимального и максимального отображения выборки
    max = round(fmax * s), min = round(fmin * s);

    // Вывод графика в консоль
    for (i = 0; i < x; i++)
    {
        j = 0;
        buff = fabs(max - (int)round(y[i] * s));
        while (j < buff)
        {
            putchar('_');
            j++;
        }
        putchar('\n');
    }
    printf("min = %lf\nmax = %lf\n", fmin, fmax);
    free(y);
}

void square(const char *name_input_bin, const char *name_output_bin)
{
    // Инициализация файлов и переменных
    int fsize_is, fsize_os, i;
    float *is, *os;
    FILE *f_is, *f_os;
    f_is = fopen(name_input_bin, "rb");
    f_os = fopen(name_output_bin, "wb");

    // Проверка на ошибку и вывод ошибки
    if (!(f_is && f_os))
    {
        perror("Open files error");
        return;
    }

    // Запись длины всех файлов
    fread(&fsize_is, sizeof(int), 1, f_is);
    fsize_os = fsize_is;

    // Инициализация массивов для выгрузки данных с файлов и записи
    is = malloc(fsize_is * sizeof(float));
    os = malloc(fsize_os * sizeof(float));

    // Выгрузка файла в массив
    fread(is, sizeof(float), fsize_is, f_is);
    fclose(f_is);

    // Алгоритм square-обработки сигнала
    for (i = 0; i < fsize_os; i++)
    {
        os[i] = pow(is[i], 2) * -1;
    }

    // Запись результата в фаил
    fwrite(&fsize_os, sizeof(int), 1, f_os);
    fwrite(os, sizeof(float), fsize_os, f_os);

    // Освобождение памяти
    free(is);
    free(os);
}

void create_is(const char *name_bin, const int s)
{
    // Инициализация файла и переменных
    FILE *file = fopen(name_bin, "wb");
    float a[5], f[5], fr[5], y[s];
    int i, j;

    // Проверка на файловую ошибку и вывод ошибки
    if (!file)
    {
        perror("Open file error");
        return;
    }

    // Заполнение массива частоты
    fr[0] = (rand() % 10) / 100.0;
    while ((fr[1] = (rand() % 10) / 100.0) == fr[0])
    {
    }
    for (i = 0; i < 5; i++)
    {
        fr[i] = rand() % 2 ? fr[0] : fr[1];
    }

    // Заполнение массивов фазы и амплитуды
    for (i = 0; i < 5; i++)
    {
        a[i] = (rand() % 10) / 100.0;
        f[i] = (rand() % 100) / 50.0;
    }

    // Создание полигармонического сигнала из 5 гармонических
    for (i = 0; i < s; i++)
    {
        y[i] = 0.0;
        for (j = 0; j < 5; j++)
        {
            y[i] += a[j] * sin(fr[j] * (float)(j + 1) * (float)i + f[j]);
        }
    }

    // Запись сигнала в фаил
    fwrite(&s, sizeof(int), 1, file);
    fwrite(y, sizeof(float), s, file);
    fclose(file);
}

void main(int argc, char *argv[])
{
    // Инициализация переменных с нейтральным значением
    char *i = NULL, *o = NULL, *g = NULL, *d = NULL;
    int s = 0, x = 0, p = 0;
    double l = 1.0;
    int arg;

    // Запрет вывода optarg ошибок в консоль
    opterr = 0;

    // Поиск аргументов и запись в переменные
    while ((arg = getopt(argc, argv, "i:o:g:s:d:x:p:l:")) != -1)
    {
        switch (arg)
        {

        case 'i':
            i = optarg;
            break;
        case 'o':
            o = optarg;
            break;

        case 'g':
            g = optarg;
            break;
        case 's':
            s = atoi(optarg); // int
            break;

        case 'd':
            d = optarg;
            break;
        case 'x':
            x = atoi(optarg); // int
            break;
        case 'p':
            p = atoi(optarg); // int
            break;
        case 'l':
            l = strtod(optarg, NULL); // double
            break;

        default:
            break;
        }
    }

    // Запуск создания сигнала
    if (g && s)
    {
        printf("Create signal...\n");
        create_is(g, s);
        printf("Completion\n");
    }

    // Запуск square-обработки сигнала
    if (i && o)
    {
        printf("Launch generate square signal...\n");
        square(i, o);
        printf("Completion\n");
    }

    // Отрисовка графика сигнала
    if (d)
    {
        printf("Print graph signal...\n");
        printDiagram(d, x, p, l);
        printf("Completion\n");
    }
}
