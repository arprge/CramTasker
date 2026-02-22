#include <iostream>
#include "scheduler.h"

int main() {
    using namespace CramCore;

    CramTasker planner;

    // предметы
    planner.addSubject("Calculus", 5, 60);
    planner.addSubject("History",  3, 75);
    planner.addSubject("OOP",      4, 50);
    planner.addSubject("Physics",  4, 40);

    // задачи
    planner.addTask("Math Study",       "Calculus", 10, 12, 7);
    planner.addTask("History Reading",  "History",   9, 10, 3);
    planner.addTask("Coding Lab",       "OOP",      14, 16, 6);
    planner.addTask("Physics Problems", "Physics",  11, 13, 8);
    planner.addTask("OOP Homework",     "OOP",      15, 17, 5);
    planner.addTask("Calculus Review",  "Calculus", 13, 14, 4);

    std::cout << "=== Before sort ===" << std::endl;
    planner.display();

    planner.sortByEnd();
    std::cout << "\n=== After sort ===" << std::endl;
    planner.display();

    // heap top-3
    planner.displayUrgent(3);

    // greedy
    std::cout << "\n=== Greedy (non-overlapping) ===" << std::endl;
    std::vector<Task> g = planner.greedySchedule();
    for (const auto& t : g)
        std::cout << "  " << t.startHour << "-" << t.endHour << " " << t.title << "\n";

    // dp — ещё не готов
    auto dp = planner.dpSchedule();
    if (dp.empty())
        std::cout << "\n[DP: not implemented yet]\n";

    return 0;
}