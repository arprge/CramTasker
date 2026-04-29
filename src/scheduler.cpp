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

    // Создаем копию предметов для симуляции прогресса обучения в рамках расписания
    SubjectMap simSubjects = subjects_;

    for (int d = today.toDays(); d <= maxExamDays; ++d) {
        Date currentDay;
        currentDay.year = d / 365;
        currentDay.month = (d % 365) / 30;
        currentDay.day = (d % 365) % 30;
        
        double maxPriority = -1.0;
        std::string bestSubject;

        for (const auto& kv : subjects_) {
            if (kv.second.examDate.toDays() > d) {
        for (auto& kv : simSubjects) {
            int examDay = kv.second.examDate.toDays();
            // Планируем предмет, только если экзамен еще не прошел и знания не максимальные
            if (examDay > d && kv.second.points < 60) {
                double priority = kv.second.calcPriority(currentDay);
                if (priority > maxPriority) {
                // При равном приоритете отдаем предпочтение более близкому экзамену
                if (priority > maxPriority || 
                   (priority == maxPriority && !bestSubject.empty() && examDay < simSubjects[bestSubject].examDate.toDays())) {
                    maxPriority = priority;
                    bestSubject = kv.second.name;
                    bestSubject = kv.first;
                }
            }
        }
        
        if (maxPriority >= 0 && !bestSubject.empty()) {
        if (!bestSubject.empty()) {
            schedule.push_back({currentDay, bestSubject, maxPriority});
            // Симулируем учебу: 1 день занятий дает +5 баллов к уверенности (максимум до 60)
            simSubjects[bestSubject].points = std::min(60, simSubjects[bestSubject].points + 5);
        } else {
            // Если все предметы доведены до 60 баллов — добавляем свободный день для корректного календаря
            schedule.push_back({currentDay, "", 0.0});
        }
    }
    return schedule;
}

} // namespace CramCore