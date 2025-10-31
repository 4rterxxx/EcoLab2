/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   EcoLab1
 * </сводка>
 *
 * <описание>
 *   Данный исходный файл является точкой входа
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */


/* Eco OS */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IdEcoLab1.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>

#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IdEcoCalculatorA.h"
#include "IdEcoCalculatorB.h"
#include "IdEcoCalculatorD.h"
#include "IdEcoCalculatorE.h"

/* Функция сравнения для qsort с правильным соглашением вызовов */
int __cdecl compare(const void* a, const void* b) {
    int32_t int_a = *((int32_t*)a);
    int32_t int_b = *((int32_t*)b);
    
    if (int_a < int_b) return -1;
    if (int_a > int_b) return 1;
    return 0;
}

/*
 *
 * <сводка>
 *   Функция EcoMain
 * </сводка>
 *
 * <описание>
 *   Функция EcoMain - точка входа
 * </описание>
 *
 */
int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    /* Указатель на системный интерфейс */
    IEcoSystem1* pISys = 0;
    /* Указатель на интерфейс работы с системной интерфейсной шиной */
    IEcoInterfaceBus1* pIBus = 0;
    /* Указатель на интерфейс работы с памятью */
    IEcoMemoryAllocator1* pIMem = 0;
    
    /* Массивы для сортировки */
    int32_t* sortArrayCOM = 0;
    int32_t* sortArrayStdlib = 0;
    uint32_t arraySize = 0;
    uint32_t i = 0;
    uint32_t exponent = 0;
    int32_t isSorted = 1;
    
    /* Переменные для замера времени */
    clock_t startTimeCOM, endTimeCOM, totalTimeCOM;
    clock_t startTimeStdlib, endTimeStdlib, totalTimeStdlib;
    double speedRatio = 0.0;
    
    /* Указатели на тестируемые интерфейсы */
    IEcoLab1 *pIEcoLab1 = 0, *pOtherIEcoLab1 = 0;
    IEcoCalculatorX *pIX = 0, *pOtherIX = 0;
    IEcoCalculatorY *pIY = 0, *pOtherIY = 0;
    IEcoUnknown *pIUnknown = 0, *pOtherIUnknown = 0;

    /* Инициализация генератора случайных чисел */
    srand((unsigned int)time(NULL));

    /* Проверка и создание системного интрефейса */
    if (pISys == 0) {
        result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void **)&pISys);
        if (result != 0 && pISys == 0) {
        /* Освобождение системного интерфейса в случае ошибки */
            goto Release;
        }
    }

    /* Получение интерфейса для работы с интерфейсной шиной */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    if (result != 0 || pIBus == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

#ifdef ECO_LIB
    /* Регистрация компонентов */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_1F5DF16EE1BF43B999A434ED38FE8F3A);
    if (result != 0 ) {
        goto Release;
    }
    
    /* Регистрация компонентов калькуляторов */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorA, (IEcoUnknown*)GetIEcoComponentFactoryPtr_4828F6552E4540E78121EBD220DC360E);
    if (result != 0 ) {
        goto Release;
    }
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorB, (IEcoUnknown*)GetIEcoComponentFactoryPtr_AE202E543CE54550899603BD70C62565);
    if (result != 0 ) {
        goto Release;
    }
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorD, (IEcoUnknown*)GetIEcoComponentFactoryPtr_3A8E44677E82475CB4A3719ED8397E61);
    if (result != 0 ) {
        goto Release;
    }
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorE, (IEcoUnknown*)GetIEcoComponentFactoryPtr_872FEF1DE3314B87AD44D1E7C232C2F0);
    if (result != 0 ) {
        goto Release;
    }
#endif

    /* Получение интерфейса управления памятью */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);

    /* Проверка */
    if (result != 0 || pIMem == 0) {
        goto Release;
    }

    /* Получение тестируемого интерфейса */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0, &IID_IEcoLab1, (void**) &pIEcoLab1);
    if (result != 0 || pIEcoLab1 == 0) {
        goto Release;
    }

    printf("\n=== Calculator Interfaces Demonstration ===\n\n");

    /* IEcoCalculatorX напрямую из IEcoLab1 */
    if (pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorX, (void**)&pIX) == 0 && pIX) {
        printf("IEcoCalculatorX obtained directly from IEcoLab1\n");
        printf(" 1) 1337 + 812 = %d\n", pIX->pVTbl->Addition(pIX, 1337, 812));
        printf(" 2) 321 - 123 = %d\n", pIX->pVTbl->Subtraction(pIX, 321, 123));

        /* IEcoUnknown через IEcoCalculatorX */
        if (pIX->pVTbl->QueryInterface(pIX, &IID_IEcoUnknown, (void**)&pOtherIUnknown) == 0 && pOtherIUnknown) {
            printf("IEcoUnknown obtained from IEcoCalculatorX\n");
            pOtherIUnknown->pVTbl->Release(pOtherIUnknown);
        } else {
            printf("Error: Failed to get IEcoUnknown from IEcoCalculatorX\n");
        }

        /* IEcoCalculatorY через IEcoCalculatorX */
        if (pIX->pVTbl->QueryInterface(pIX, &IID_IEcoCalculatorY, (void**)&pOtherIY) == 0 && pOtherIY) {
            printf("IEcoCalculatorY obtained from IEcoCalculatorX\n");
            printf(" 1) 52 * 42 = %d\n", pOtherIY->pVTbl->Multiplication(pOtherIY, 52, 42));
            printf(" 2) 333 / 33 = %d\n", pOtherIY->pVTbl->Division(pOtherIY, 333, 33));
            pOtherIY->pVTbl->Release(pOtherIY);
        } else {
            printf("Error: Failed to get IEcoCalculatorY from IEcoCalculatorX\n");
        }

        /* IEcoLab1 через IEcoCalculatorX */
        if (pIX->pVTbl->QueryInterface(pIX, &IID_IEcoLab1, (void**)&pOtherIEcoLab1) == 0 && pOtherIEcoLab1) {
            printf("IEcoLab1 obtained from IEcoCalculatorX\n");
            pOtherIEcoLab1->pVTbl->Release(pOtherIEcoLab1);
        } else {
            printf("Error: Failed to get IEcoLab1 from IEcoCalculatorX\n");
        }

        pIX->pVTbl->Release(pIX);
    }
    else {
        printf("Error: Failed to get IEcoCalculatorX from IEcoLab1\n");
    }
    printf("\n");

    /* IEcoCalculatorY напрямую из IEcoLab1 */
    if (pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorY, (void**)&pIY) == 0 && pIY) {
        printf("IEcoCalculatorY obtained directly from IEcoLab1\n");
        printf(" 1) 13 * 100 = %d\n", pIY->pVTbl->Multiplication(pIY, 13, 100));
        printf(" 2) 99 / 11 = %d\n", pIY->pVTbl->Division(pIY, 99, 11));

        /* IEcoUnknown через IEcoCalculatorY */
        if (pIY->pVTbl->QueryInterface(pIY, &IID_IEcoUnknown, (void**)&pOtherIUnknown) == 0 && pOtherIUnknown) {
            printf("IEcoUnknown obtained from IEcoCalculatorY\n");
            pOtherIUnknown->pVTbl->Release(pOtherIUnknown);
        } else {
            printf("Error: Failed to get IEcoUnknown from IEcoCalculatorY\n");
        }

        /* IEcoCalculatorX через IEcoCalculatorY */
        if (pIY->pVTbl->QueryInterface(pIY, &IID_IEcoCalculatorX, (void**)&pOtherIX) == 0 && pOtherIX) {
            printf("IEcoCalculatorX obtained from IEcoCalculatorY\n");
            printf(" 1) 555 + 777 = %d\n", pOtherIX->pVTbl->Addition(pOtherIX, 555, 777));
            printf(" 2) 450 - 50 = %d\n", pOtherIX->pVTbl->Subtraction(pOtherIX, 450, 50));
            pOtherIX->pVTbl->Release(pOtherIX);
        } else {
            printf("Error: Failed to get IEcoCalculatorX from IEcoCalculatorY\n");
        }

        /* IEcoLab1 через IEcoCalculatorY */
        if (pIY->pVTbl->QueryInterface(pIY, &IID_IEcoLab1, (void**)&pOtherIEcoLab1) == 0 && pOtherIEcoLab1) {
            printf("IEcoLab1 obtained from IEcoCalculatorY\n");
            pOtherIEcoLab1->pVTbl->Release(pOtherIEcoLab1);
        } else {
            printf("Error: Failed to get IEcoLab1 from IEcoCalculatorY\n");
        }

        pIY->pVTbl->Release(pIY);
    }
    else {
        printf("Error: Failed to get IEcoCalculatorY from IEcoLab1\n");
    }
	 printf("\n");

	/* Получаем IEcoUnknown из IEcoLab1 */
    if (pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoUnknown, (void**)&pIUnknown) == 0 && pIUnknown) {
        printf("IEcoUnknown obtained from IEcoLab1\n");

        /* IEcoLab1 через IEcoUnknown */
        if (pIUnknown->pVTbl->QueryInterface(pIUnknown, &IID_IEcoLab1, (void**)&pOtherIEcoLab1) == 0 && pOtherIEcoLab1) {
            printf("IEcoLab1 obtained from IEcoUnknown\n");
            pOtherIEcoLab1->pVTbl->Release(pOtherIEcoLab1);
        } else {
            printf("Error: Failed to get IEcoLab1 from IEcoUnknown\n");
        }

        /* IEcoCalculatorX через IEcoUnknown */
        if (pIUnknown->pVTbl->QueryInterface(pIUnknown, &IID_IEcoCalculatorX, (void**)&pOtherIX) == 0 && pOtherIX) {
            printf("IEcoCalculatorX obtained from IEcoUnknown\n");
            printf(" 1) 44 + 66 = %d\n", pOtherIX->pVTbl->Addition(pOtherIX, 44, 66));
            printf(" 2) 999 - 9 = %d\n", pOtherIX->pVTbl->Subtraction(pOtherIX, 999, 9));
            pOtherIX->pVTbl->Release(pOtherIX);
        } else {
            printf("Error: Failed to get IEcoCalculatorX from IEcoUnknown\n");
        }

        /* IEcoCalculatorY через IEcoUnknown */
        if (pIUnknown->pVTbl->QueryInterface(pIUnknown, &IID_IEcoCalculatorY, (void**)&pOtherIY) == 0 && pOtherIY) {
            printf("IEcoCalculatorY obtained from IEcoUnknown\n");
            printf(" 1) 10 * 100 = %d\n", pOtherIY->pVTbl->Multiplication(pOtherIY, 10, 100));
            printf(" 2) 1000 / 5 = %d\n", pOtherIY->pVTbl->Division(pOtherIY, 1000, 5));
            pOtherIY->pVTbl->Release(pOtherIY);
        } else {
            printf("Error: Failed to get IEcoCalculatorY from IEcoUnknown\n");
        }

        pIUnknown->pVTbl->Release(pIUnknown);
    } else {
        printf("Error Failed to get IEcoUnknown from IEcoLab1\n");
    }

    printf("\n=== End of Interface Demonstration ===\n\n");

    /* === ТЕСТИРОВАНИЕ СОРТИРОВКИ (ваш оригинальный код) === */
    printf("=== Sorting Algorithms Testing ===\n");

    /* Table header */
    printf("Array Size     | COM (ticks) | stdlib (ticks) | Faster  | Ratio\n");
    printf("---------------|-------------|----------------|---------|----------\n");

    /* Testing for powers of 2 from 2 to 20 */
    exponent = 2;
    while (exponent <= 17) {
        arraySize = (uint32_t)pow(2, exponent);
        
        /* Выделение памяти для массивов */
        sortArrayCOM = (int32_t*)pIMem->pVTbl->Alloc(pIMem, arraySize * sizeof(int32_t));
        sortArrayStdlib = (int32_t*)pIMem->pVTbl->Alloc(pIMem, arraySize * sizeof(int32_t));

        /* Заполнение массивов случайными значениями */
        i = 0;
        while (i < arraySize) {
            int32_t randomValue = rand() % 10000 - 5000; /* Numbers from -5000 to 4999 */
            sortArrayCOM[i] = randomValue;
            sortArrayStdlib[i] = randomValue;
            i++;
        }

        /* COM COMPONENT TEST */
        startTimeCOM = clock();
        
        /* Вызов функции сортировки COM-компонента */
        result = pIEcoLab1->pVTbl->MyFunction(pIEcoLab1, sortArrayCOM, arraySize);
        if (result != 0) {
            /* Освобождение памяти перед выходом */
            pIMem->pVTbl->Free(pIMem, sortArrayCOM);
            pIMem->pVTbl->Free(pIMem, sortArrayStdlib);
            goto Release;
        }

        endTimeCOM = clock();
        totalTimeCOM = endTimeCOM - startTimeCOM;

        /* Проверка правильности сортировки COM */
        isSorted = 1;
        i = 0;
        while (i < arraySize - 1) {
            if (sortArrayCOM[i] > sortArrayCOM[i + 1]) {
                isSorted = 0;
                break;
            }
            i++;
        }

        if (isSorted == 0) {
            printf("ERROR: COM sorting failed for size %u\n", arraySize);
            pIMem->pVTbl->Free(pIMem, sortArrayCOM);
            pIMem->pVTbl->Free(pIMem, sortArrayStdlib);
            goto Release;
        }

        /* STANDARD LIBRARY TEST */
        startTimeStdlib = clock();
        
        /* Вызов стандартной функции сортировки */
        qsort(sortArrayStdlib, arraySize, sizeof(int32_t), compare);

        endTimeStdlib = clock();
        totalTimeStdlib = endTimeStdlib - startTimeStdlib;

        /* Проверка правильности сортировки stdlib */
        isSorted = 1;
        i = 0;
        while (i < arraySize - 1) {
            if (sortArrayStdlib[i] > sortArrayStdlib[i + 1]) {
                isSorted = 0;
                break;
            }
            i++;
        }

        if (isSorted == 0) {
            printf("ERROR: stdlib sorting failed for size %u\n", arraySize);
            pIMem->pVTbl->Free(pIMem, sortArrayCOM);
            pIMem->pVTbl->Free(pIMem, sortArrayStdlib);
            goto Release;
        }

        /* OUTPUT RESULTS */
        printf("2^%2u (%7u) | %11ld | %14ld | ", 
               exponent, arraySize, totalTimeCOM, totalTimeStdlib);

        /* Determine which sorting is faster with division by zero check */
        if (totalTimeCOM == 0 || totalTimeStdlib == 0) {
            printf("equal   | 1.00x\n");
        }        else if (totalTimeCOM < totalTimeStdlib) {
            speedRatio = (double)totalTimeStdlib / totalTimeCOM;
            printf("COM     | %.2fx\n", speedRatio);
        }
        else if (totalTimeCOM > totalTimeStdlib) {
            speedRatio = (double)totalTimeCOM / totalTimeStdlib;
            printf("stdlib  | %.2fx\n", speedRatio);
        }
        else {
            printf("equal   | 1.00x\n");
        }

        /* Освобождение памяти для текущего размера */
        pIMem->pVTbl->Free(pIMem, sortArrayCOM);
        pIMem->pVTbl->Free(pIMem, sortArrayStdlib);
        sortArrayCOM = 0;
        sortArrayStdlib = 0;

        exponent++;
    }

    printf("=========================================\n");
    printf("Testing completed successfully! Press any key to exit...\n");
    getchar();
    /* Если дошли сюда, все тесты прошли успешно */
    result = 0;

Release:

    /* Освобождение интерфейса для работы с интерфейсной шиной */
    if (pIBus != 0) pIBus->pVTbl->Release(pIBus);

    /* Освобождение интерфейса работы с памятью */
    if (pIMem != 0) pIMem->pVTbl->Release(pIMem);

    /* Освобождение тестируемого интерфейса */
    if (pIEcoLab1 != 0) pIEcoLab1->pVTbl->Release(pIEcoLab1);

    /* Освобождение системного интерфейса */
    if (pISys != 0) pISys->pVTbl->Release(pISys);

    return result;
}