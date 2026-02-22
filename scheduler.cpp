#include "scheduler.h"
#include <iostream>
#include <algorithm> 

void Scheduler::addTask(std::string title, std::string sub, int s, int e, int p) {
    Task newTask = {title, sub, s, e, p};
    allTasks.push_back(newTask); // векторга косу
}

// алгорит 1
void Scheduler::sortByTime() {
    std::sort(allTasks.begin(), allTasks.end(), [](Task a, Task b) {
        return a.end < b.end;
    });
}

void Scheduler::display() {
    std::cout << "--- Schedule ---" << std::endl;
    for (int i = 0; i < allTasks.size(); i++) {
        std::cout << allTasks[i].start << ":00 - " << allTasks[i].end << ":00 | " 
                  << allTasks[i].title << " (" << allTasks[i].subName << ")" << std::endl;
    }
}