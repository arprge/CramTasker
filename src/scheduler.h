#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <queue>
#include <string>
#include "task.h"

namespace CramCore {

class CramTasker {
public:
    // данные
    void addSubject(const std::string& name, int credits, int grade);
    void addTask(const std::string& title, const std::string& subKey,
                 int startH, int endH, int prio = 5);

    // сортировка — O(n log n)
    void sortByEnd();

    // greedy activity selection — O(n log n)
    std::vector<Task> greedySchedule();

    // TODO (Week 11): DP weighted interval — O(n log n)
    std::vector<Task> dpSchedule();

    // вывод
    void display() const;
    void displayUrgent(int topN = 5) const;

    // геттеры
    const std::vector<Task>& getTasks() const { return tasks_; }

private:
    std::vector<Task> tasks_;       // Week 2: vector
    SubjectMap        subjects_;    // Week 5: unordered_map

    void recalcWeight_(Task& t) const;

    // TODO (Week 8-10): граф свободных слотов
};

} // namespace CramCore

#endif