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
    subjects_[name] = s;
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
    tasks_.push_back(t);
}

bool CramTasker::removeTask(const std::string& title) {
    for (auto it = tasks_.begin(); it != tasks_.end(); ++it) {
        if (it->title == title) {
            tasks_.erase(it);
            return true;
        }
    }
    return false;
}

void CramTasker::updateGrade(const std::string& subjectName, int newGrade) {
    auto it = subjects_.find(subjectName);
    if (it == subjects_.end()) return;
    it->second.currentGrade = newGrade;
    recalcAllWeights_();
}

void CramTasker::recalcWeight_(Task& t) const {
    auto it = subjects_.find(t.subjectKey);
    if (it != subjects_.end())
        t.weight = it->second.calcPriority();
}

void CramTasker::recalcAllWeights_() {
    for (auto& t : tasks_)
        recalcWeight_(t);
}

// O(n log n)
void CramTasker::sortByEnd() {
    std::sort(tasks_.begin(), tasks_.end(), CmpEnd());
}

// greedy — pick max non-overlapping tasks, O(n log n)
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

// latest non-conflicting task, O(log n)
static int findLastNonConflicting(const std::vector<Task>& tasks, int idx) {
    int lo = 0, hi = idx - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (tasks[mid].endHour <= tasks[idx].startHour) {
            if (mid + 1 <= hi && tasks[mid + 1].endHour <= tasks[idx].startHour)
                lo = mid + 1;
            else
                return mid;
        } else {
            hi = mid - 1;
        }
    }
    return -1;
}

// DP weighted interval — O(n log n)
std::vector<Task> CramTasker::dpSchedule() {
    if (tasks_.empty()) return {};

    sortByEnd();
    size_t n = tasks_.size();

    std::vector<double> dp(n);
    dp[0] = tasks_[0].weight;

    for (size_t i = 1; i < n; ++i) {
        double incl = tasks_[i].weight;
        int p = findLastNonConflicting(tasks_, (int)i);
        if (p != -1)
            incl += dp[p];
        dp[i] = std::max(dp[i - 1], incl);
    }

    std::cout << "[DP] optimal weight = " << dp[n - 1] << "\n";
    return {};
}

void CramTasker::display() const {
    std::cout << "\n--- Schedule (" << tasks_.size() << " tasks) ---\n";
    for (const auto& t : tasks_) {
        std::cout << "  " << std::setw(2) << t.startHour << ":00-"
                  << std::setw(2) << t.endHour << ":00 | "
                  << t.title << " [" << t.subjectKey << "]"
                  << "  w=" << std::fixed << std::setprecision(0) << t.weight
                  << "\n";
    }
}

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