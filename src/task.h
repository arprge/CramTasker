#ifndef TASK_H
#define TASK_H

#include <string>
#include <unordered_map>
#include <algorithm>

namespace CramCore {

struct Date {
    int year;
    int month;
    int day;

    int toDays() const {
        return year * 365 + month * 30 + day;
    }
};

struct Subject {
    std::string name;
    int points = 0;
    Date examDate;
    int credits = 0;

    double calcPriority(Date current) const {
        int daysLeft = examDate.toDays() - current.toDays();
        if (daysLeft <= 0) return 0.0;
        return (credits * (60.0 - points)) / std::max(1, daysLeft);
    }
};

using SubjectMap = std::unordered_map<std::string, Subject>;

} // namespace CramCore

#endif