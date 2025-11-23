# Отчет по лабораторной работе №3
## Реализация сортировки вставкой с системой событий
**Выполнил:** Трясков Алексей 22пи2

### 1. Общая постановка задачи

Необходимо было реализовать алгоритм сортировки вставкой в COM-компоненте с системой событий, демонстрирующих пошаговую работу алгоритма. Компонент должен предоставлять интерфейс для подключения приемников событий (sinks) через механизм Connection Points.

### 2. Реализуемый алгоритм сортировки вставкой

**Алгоритм сортировки вставкой** работает следующим образом:
- Массив условно делится на отсортированную и неотсортированную части
- На каждом шаге берется первый элемент из неотсортированной части и вставляется в правильную позицию в отсортированной части
- Элементы большие чем вставляемый сдвигаются вправо

**Основные шаги алгоритма:**
1. Начинаем со второго элемента (i = 1)
2. Сохраняем текущий элемент как key
3. Находим правильную позицию для key в отсортированной части
4. Сдвигаем элементы большие чем key вправо
5. Вставляем key в найденную позицию
6. Повторяем для всех элементов

### 3. Асимптотика

**Временная сложность:**
- Худший случай: O(n²) - когда массив отсортирован в обратном порядке
- Лучший случай: O(n) - когда массив уже отсортирован
- Средний случай: O(n²)

**Сложность по памяти:** O(1) - сортировка на месте

### 4. Реализация системы событий

#### 4.1. Интерфейс событий IEcoLab1Events

```c
typedef struct IEcoLab1VTblEvents {
    /* IUnknown */
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ struct IEcoLab1Events* me, /* in */ const EcoGUID* riid, /* out */ void** ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ struct IEcoLab1Events* me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ struct IEcoLab1Events* me);
    
    /* IEcoLab1Events for Insertion Sort */
    int16_t (ECOCALLMETHOD *OnInsertionSortCalled)(/* in */ struct IEcoLab1Events* me, /* in */ const void *startPtr, size_t elem_count);
    int16_t (ECOCALLMETHOD *OnElementPick)(/* in */ struct IEcoLab1Events* me, /* in */ const void *element, /* in */ size_t current_index);
    int16_t (ECOCALLMETHOD *OnElementShift)(/* in */ struct IEcoLab1Events* me, /* in */ const void *element, /* in */ size_t from_index, /* in */ size_t to_index);
    int16_t (ECOCALLMETHOD *OnElementInserted)(/* in */ struct IEcoLab1Events* me, /* in */ const void *element, /* in */ size_t final_position);
    int16_t (ECOCALLMETHOD *OnIterationCompleted)(/* in */ struct IEcoLab1Events* me, /* in */ const void *startPtr, size_t elem_count);
} IEcoLab1VTblEvents, *IEcoLab1VTblEventsPtr;
```

### 4.2. Описание событий

1. **OnInsertionSortCalled** - вызывается при начале сортировки, передает исходный массив
2. **OnElementPick** - вызывается когда выбран элемент для вставки  
3. **OnElementShift** - вызывается при сдвиге элемента внутри массива
4. **OnElementInserted** - вызывается когда элемент нашел свою окончательную позицию
5. **OnIterationCompleted** - вызывается после каждой итерации внешнего цикла

### 5. Пример работы программы

```bash
Insertion Sort Started - Array: [6, 3, 9, 6, 3, 3, 4, 5]
Picking element [3] at index 1 for insertion
Shifting element [6] from index 0 to 1
Element [3] inserted at final position 0
Iteration Completed - Current Array: [3, 6, 9, 6, 3, 3, 4, 5]
Picking element [9] at index 2 for insertion
Element [9] inserted at final position 2
Iteration Completed - Current Array: [3, 6, 9, 6, 3, 3, 4, 5]
Picking element [6] at index 3 for insertion
Shifting element [9] from index 2 to 3
Element [6] inserted at final position 2
Iteration Completed - Current Array: [3, 6, 6, 9, 3, 3, 4, 5]
Picking element [3] at index 4 for insertion
Shifting element [9] from index 3 to 4
Shifting element [6] from index 2 to 3
Shifting element [6] from index 1 to 2
Element [3] inserted at final position 1
Iteration Completed - Current Array: [3, 3, 6, 6, 9, 3, 4, 5]
Picking element [3] at index 5 for insertion
Shifting element [9] from index 4 to 5
Shifting element [6] from index 3 to 4
Shifting element [6] from index 2 to 3
Element [3] inserted at final position 2
Iteration Completed - Current Array: [3, 3, 3, 6, 6, 9, 4, 5]
Picking element [4] at index 6 for insertion
Shifting element [9] from index 5 to 6
Shifting element [6] from index 4 to 5
Shifting element [6] from index 3 to 4
Element [4] inserted at final position 3
Iteration Completed - Current Array: [3, 3, 3, 4, 6, 6, 9, 5]
Picking element [5] at index 7 for insertion
Shifting element [9] from index 6 to 7
Shifting element [6] from index 5 to 6
Shifting element [6] from index 4 to 5
Element [5] inserted at final position 4
Iteration Completed - Current Array: [3, 3, 3, 4, 5, 6, 6, 9]
```


### 6. Демонстрация работы событий

Каждое событие предоставляет детальную информацию о текущем состоянии алгоритма:

- **OnElementPick** показывает какой элемент перемещается и его исходную позицию
- **OnElementShift** отображает сдвиги элементов при поиске места для вставки
- **OnElementInserted** фиксирует окончательную позицию элемента
- **OnIterationCompleted** показывает состояние массива после каждой итерации

### 7. Анализ и выводы

1. **Полнота отслеживания:** Система событий позволяет полностью отследить работу алгоритма сортировки вставкой
2. **Гибкость архитектуры:** Механизм Connection Points позволяет легко подключать sinks
3. **Информативность:** События предоставляют достаточную информацию для визуализации и отладки


### 8. Заключение

В ходе работы успешно реализован COM-компонент с алгоритмом сортировки вставкой и системой событий. Реализована схема подключения через механизм Connection Points. Созданная система позволяет детально отслеживать работу алгоритма и может быть использована для образовательных целей и отладки сложных алгоритмов сортировки.