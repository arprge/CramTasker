#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <queue>
#include <string>
#include "task.h"

namespace CramCore {

class CramTasker {
public:
    void addSubject(const std::string& name, int credits, int grade);
    void addTask(const std::string& title, const std::string& subKey,
                 int startH, int endH, int prio = 5);
    bool removeTask(const std::string& title);
    void toggleTaskCompletion(const std::string& title);
    void updateGrade(const std::string& subjectName, int newGrade);

    void sortByEnd();                       // O(n log n)
    std::vector<Task> greedySchedule();     // O(n log n)
    std::vector<Task> dpSchedule();         // WIP

    void display() const;
    void displayUrgent(int topN = 5) const;

    const std::vector<Task>&  getTasks()    const { return tasks_; }
    const SubjectMap&         getSubjects() const { return subjects_; }
    size_t taskCount()   const { return tasks_.size(); }
    size_t subjectCount() const { return subjects_.size(); }

private:
    std::vector<Task> tasks_;
    SubjectMap        subjects_;

    void recalcWeight_(Task& t) const;
    void recalcAllWeights_();
};

} // namespace CramCore

#endif