#include "scheduler.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

namespace CramCore {

void CramTasker::addSubject(const std::string& name, int credits, int grade) {
    Subject s;
    s.name = name;
    s.credits = credits;
    s.currentGrade = grade;
    subjects_[name] = s; // O(1) insert
}

void CramTasker::addTask(const std::string& title, const std::string& subKey,
                         int startH, int endH, int prio) {
    Task t;
    t.title = title;
    t.subjectKey = subKey;
    t.startHour = startH;
    t.endHour = endH;
    t.priority = prio;
    t.weight = 0.0;
    recalcWeight_(t);
    tasks_.push_back(t); // O(1) amortized
}

void CramTasker::recalcWeight_(Task& t) const {
    auto it = subjects_.find(t.subjectKey);
    if (it != subjects_.end())
        t.weight = it->second.calcPriority();
}

// sort по endHour — O(n log n)
void CramTasker::sortByEnd() {
    std::sort(tasks_.begin(), tasks_.end(), CmpEnd());
}

// greedy activity selection — O(n log n)
std::vector<Task> CramTasker::greedySchedule() {
    sortByEnd();
    std::vector<Task> sel;
    if (tasks_.empty()) return sel;

    sel.push_back(tasks_[0]);
    int lastEnd = tasks_[0].endHour;

    for (size_t i = 1; i < tasks_.size(); ++i) {
        if (tasks_[i].startHour >= lastEnd) {
            sel.push_back(tasks_[i]);
            lastEnd = tasks_[i].endHour;
        }
    }
    return sel;
}

// TODO (Week 11): DP weighted interval scheduling — O(n log n)
std::vector<Task> CramTasker::dpSchedule() {
    // 1. sort по endHour
    // 2. binary search для p(i)
    // 3. dp[i] = max(dp[i-1], weight[i] + dp[p(i)])
    // 4. backtrack
    return {};
}

void CramTasker::display() const {
    std::cout << "\n--- Schedule (" << tasks_.size() << " tasks) ---\n";
    for (size_t i = 0; i < tasks_.size(); ++i) {
        const Task& t = tasks_[i];
        std::cout << "  " << std::setw(2) << t.startHour << ":00-"
                  << std::setw(2) << t.endHour << ":00 | "
                  << t.title << " [" << t.subjectKey << "]"
                  << "  w=" << std::fixed << std::setprecision(0) << t.weight
                  << "\n";
    }
}

// priority_queue (heap) — показываем topN по weight
void CramTasker::displayUrgent(int topN) const {
    std::priority_queue<Task, std::vector<Task>, CmpWeight> pq(
        tasks_.begin(), tasks_.end());

    std::cout << "\n--- Top " << topN << " urgent ---\n";
    for (int i = 0; i < topN && !pq.empty(); ++i) {
        const Task& t = pq.top();
        std::cout << "  #" << (i+1) << " " << t.title
                  << " [" << t.subjectKey << "] w="
                  << std::fixed << std::setprecision(0) << t.weight << "\n";
        pq.pop();
    }
}

} // namespace CramCore