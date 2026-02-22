#include <iostream>
#include "scheduler.h"

int main() {
    Scheduler myPlan;

    // Имитируем ввод данных студентом
    myPlan.addTask("Math Study", "Calculus", 10, 12, 5);
    myPlan.addTask("History Reading", "History", 9, 10, 2);
    myPlan.addTask("Coding Lab", "OOP", 14, 16, 4);

    // Сортируем (Week 4)
    myPlan.sortByTime();

    // Выводим результат
    myPlan.display();

    return 0;
}