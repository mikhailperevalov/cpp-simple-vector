# Vector
Реализован небольшой аналог шаблонного класса vector из стандартной библиотеки.
Данные хранятся в памяти, динамически выделяемой в куче. Память выделяется неинициализированная, инициализация происходит при фактическом добавлении элементов в вектор.
При добавлении новых элементов, если выделенной памяти недостаточно - выделяется новый участок памяти размером в два раза больше предыдущего, в который перемещаются (либо копируются) данные из старого участка памяти, после чего старый участок удаляется.

# Функционал класса
- Создание вектора
    *конструктор по-умолчанию, создающий пустой вектор
    *конструктор, создающий вектор заданного размера (инициализирован значениями по умолчанию)
    *копирующий конструктор и перемещающий конструктор
    *операторы копирующего присваивания и перемещающего присваивания
    *обмен двух векторов

- Методы
    *Добавление/удаление элементов в конец вектора
    *Конструирование элемента в векторе
    *Резервирование места в векторе под новые элементы
    *Изменение размера вектора
    *Итерирование по элементам вектора в for-range цикле

# Установка и использование
Cкопируйте файл vector.h в папку с вашим проектом и подключите через директиву #include<vector.h>

# Системные требования
Компилятор С++ с поддержкой стандарта C++17 или новее
