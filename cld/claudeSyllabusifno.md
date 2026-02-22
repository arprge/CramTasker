Честный ответ: твой текущий промпт очень хорош, но чтобы он на 100% «выжал» из Клода соответствие методичке, нам нужно сделать его более специфичным в плане инвентаризации тем.

В твоем силлабусе есть темы, которые не всегда очевидны для планировщика (например, Графы на 9-10 неделе или Деревья на 6-й). Если мы не подскажем модели, как эти темы вплести в архитектуру сейчас, потом придется «перекраивать» весь проект.

Давай внесем три критических дополнения, чтобы промпт закрыл абсолютно все темы семестра:
Что нужно добавить в промпт (дополненные блоки)
1. Карта соответствия темам (Syllabus Mapping)

Добавь этот блок в Context.md, чтобы модель понимала, куда и какую структуру данных «приткнуть»:

    Syllabus-to-Code Mapping:

        Week 2 (Arrays/Lists): std::vector for task storage.

        Week 3 (Stacks/Queues): Task undo/redo stack or processing queue.

        Week 4 (Sorting): Quick/Merge sort for timeline ordering.

        Week 5 (Hash Tables): std::unordered_map for O(1) Subject lookup.

        Week 6-7 (Trees/Heaps): std::priority_queue (Heap) for urgent tasks; potential Tree for category hierarchy.

        Week 9-10 (Graphs): Representing time-slots as nodes for finding optimal paths (Free-time analysis).

        Week 11-12 (DP/Greedy): Core scheduling logic.

2. Математическая модель "Profit"

Учителя любят, когда есть формула. Добавь это:

    Optimization Formula:
    Task Weight W=Credits×(100−CurrentGrade)×DifficultyFactor.
    The goal is to maximize ∑W within the constraints of free time windows.

3. Архитектурный задел под iCal

Так как ты планируешь подписку по ссылке, нужно указать это как внешний модуль:

    External Integration:
    The Parser module must have a placeholder for libcurl (networking) and a string-processing engine to handle .ics files.

Обновленный (финальный) "Супер-промпт" для Клода

Скопируй это и отправь вместе со своим кодом:

«Я строю проект ExamAI на C++ согласно университетскому силлабусу (15 недель). Мне нужно, чтобы ты провел рефакторинг моего текущего кода, создав масштабируемый фундамент.

Критически важные требования из методички:

    Структуры данных: Заложи архитектуру так, чтобы я мог внедрить:

        std::vector (хранение),

        std::unordered_map (база предметов),

        std::priority_queue (очередь дедлайнов),

        и оставил место для Графа (анализ свободных окон времени).

    Алгоритмы: Подготовь интерфейсы для:

        Sort (сортировка по времени),

        Greedy (Activity Selection),

        Dynamic Programming (Weighted Interval Scheduling).

    Архитектура: - Раздели на task.h, scheduler.h, scheduler.cpp, main.cpp.

        Используй namespace ExamCore.

        Вместо готовой реализации напиши подробные TODO-комментарии с указанием Big-O сложности, которую я должен буду достичь.