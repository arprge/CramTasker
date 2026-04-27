#ifndef TASK_H
#define TASK_H

#include <string>
#include <unordered_map>

namespace CramCore {

struct Subject {
    std::string name;
    int points = 0;
    int examOrder = 0;
    int credits = 0;

    // priority = credits * (100 - points) / examOrder etc (customize as needed)
    double calcPriority() const {
        return (credits * (100.0 - points)) / (examOrder > 0 ? examOrder : 1);
    }
};

using SubjectMap = std::unordered_map<std::string, Subject>;

struct Task {
    std::string title;
    std::string subjectKey;
    int startHour = 0;
    int endHour   = 0;
    int priority  = 0;
    double weight = 0.0;

    bool completed = false;

    int duration() const { return endHour - startHour; }
};

// max-heap by weight
struct CmpWeight {
    bool operator()(const Task& a, const Task& b) const {
        return a.weight < b.weight;
    }
};

// sort by end time
struct CmpEnd {
    bool operator()(const Task& a, const Task& b) const {
        return a.endHour < b.endHour;
    }
};

// struct CmpStart {
//     bool operator()(const Task& a, const Task& b) const {
//         return a.startHour < b.startHour;
//     }
// };

} // namespace CramCore

#endif