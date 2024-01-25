# **ВЕРСИОННАЯ ПАМЯТЬ**
#### Требования
- Реализовать структуры данных с поддержкой одновременного многопоточного независимого изменения с возможностью слияния версий.
- Создание версии должно происходить при первом изменении без дополнительных указаний со стороны пользователя библиотеки, как если бы структура данных не была версионной.
- Должны поддерживаться такие структуры данных как множества, очереди и стеки.
- Способ реализации множества следует выбрать наиболее подходящим для
- реализации версионности.
- Отличие стека и очереди заключается в специфических операциях доступа, в
- соответствии с которыми должны применяться различные стратегии слияния версий.
- Демонстрация работоспособности должна быть проведена примере добавления данных конкурентными потоками исполнения, которые со случайными интервалами генерируют данные и помещают их в структуру (можно предложить свой вариант демонстрационной задачи)
#### Дополнительные требования
- Обеспечить детерминированность алгоритма слияния. Для одной программы вне зависимости от race-conditions должны получаться одни и те же результаты.
- Поддержка сбалансированных деревьев (особенность заключается в том, что после слияния дерево должно оставаться сбалансированным).
- Разработайте механизм для определения пользовательских стратегий объединения версий.
- Реализацию доп. требования продемонстрируйте на примере конкурентного построения дерева частотности.
