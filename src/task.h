#ifndef TASK_H
#define TASK_H

#include <string>
#include <unordered_map>

namespace CramCore {

// предмет (Week 5 — хранится в unordered_map)
struct Subject {
    std::string name;
    int credits      = 0;
    int currentGrade = 0;   // 0-100

    // Priority = Credits * (100 - CurrentGrade)
    double calcPriority() const {
        return credits * (100.0 - currentGrade);
    }
};

using SubjectMap = std::unordered_map<std::string, Subject>;

// одна задача (Week 2 — хранится в vector)
struct Task {
    std::string title;
    std::string subjectKey;
    int startHour = 0;
    int endHour   = 0;
    int priority  = 0;
    double weight = 0.0;   // считается через Subject::calcPriority

    int duration() const { return endHour - startHour; }
};

// компаратор для priority_queue (Week 6-7, max-heap по weight)
struct CmpWeight {
    bool operator()(const Task& a, const Task& b) const {
        return a.weight < b.weight;
    }
};

// компаратор для sort (Week 3-4, по endHour)
struct CmpEnd {
    bool operator()(const Task& a, const Task& b) const {
        return a.endHour < b.endHour;
    }
};

} // namespace CramCore

#endif