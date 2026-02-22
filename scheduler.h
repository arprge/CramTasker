#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include "task.h"

class Scheduler {
private:
    std::vector<Task> allTasks; // Тасклит / #1 дқ

public:
    void addTask(std::string title, std::string sub, int s, int e, int p);
    void addSubject(std::string name, int credits, int grade); 

    // Таймсорт / #1 алгорит
    void sortByTime();

    
    void display();
};

#endif