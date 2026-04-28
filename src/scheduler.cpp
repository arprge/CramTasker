#include "scheduler.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

namespace CramCore {

void CramTasker::addSubject(const std::string& name, int points, Date examDate, int credits) {
    Subject s;
    s.name = name;
    s.points = std::min(60, points);
    s.examDate = examDate;
    s.credits = std::min(6, credits);
    subjects_[name] = s;
}

void CramTasker::removeSubject(const std::string& name) {
    subjects_.erase(name);
}

void CramTasker::updateGrade(const std::string& subjectName, int newGrade) {
    auto it = subjects_.find(subjectName);
    if (it == subjects_.end()) return;
    it->second.points = std::min(60, newGrade);
}

std::vector<StudyDay> CramTasker::generateSchedule(Date today) const {
    std::vector<StudyDay> schedule;
    if (subjects_.empty()) return schedule;

    int maxExamDays = today.toDays();
    for (const auto& kv : subjects_) {
        maxExamDays = std::max(maxExamDays, kv.second.examDate.toDays());
    }

    for (int d = today.toDays(); d <= maxExamDays; ++d) {
        Date currentDay;
        currentDay.year = d / 365;
        currentDay.month = (d % 365) / 30;
        currentDay.day = (d % 365) % 30;
        
        double maxPriority = -1.0;
        std::string bestSubject;

        for (const auto& kv : subjects_) {
            if (kv.second.examDate.toDays() > d) {
                double priority = kv.second.calcPriority(currentDay);
                if (priority > maxPriority) {
                    maxPriority = priority;
                    bestSubject = kv.second.name;
                }
            }
        }
        
        if (maxPriority >= 0 && !bestSubject.empty()) {
            schedule.push_back({currentDay, bestSubject, maxPriority});
        }
    }
    return schedule;
}

} // namespace CramCore