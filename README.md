# Очередь на базе кольцевого буфера
[![License](https://img.shields.io/badge/LICENSE-The%20Unlicense-green?style=flat-square)](/LICENSE)  [![Version](https://img.shields.io/badge/VERSION-STABLE-green?style=flat-square)](https://github.com/averov90/circular-buffer-Queue/releases)
### :small_orange_diamond: [English version](/README-eng.md)

В данном репозитории представлена реализация очереди на основе кольчевого буфера. 
Использование кольцевого буфера позволяет этой структуре данных быть достаточно эффективной в предполагаемом режиме работы (добавление и удаление в среднем поровну).
Производительность представленной реализации аналогична производительности STL-варианта, скомпилированного в режиме Release (тестирование проводилось на битности x64).

В зависимости от особенностей использования, возможны дополнительные оптимизации некоторых сценариев работы, например, добавления. 
Если оказывается, что временные задержки на расширение структуры слишком велики, возможна реализация кольца более высокого уровня: кольца, содержащего в себе кольца с данными.
Такое решение позволит не использовать потенциально дорогостоящее *memcpy* для большого числа объектов, а также позволит избежать перемещения элементов во время нахождения в структуре (указатель на элемент меняться не будет).

Также возможна оптимизация реверсирования направления очереди. 
Сделать это можно, поменяв местами указатели начала и конца, а также добавив новоеполе, в котором будет храниться направление обхода элементов кольца.

Все это довольно полезные и интересные задачи по теме кодинга.