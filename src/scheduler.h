#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <queue>
#include <string>
#include "task.h"

namespace CramCore {

struct StudyDay {
    Date date;
    std::string subjectName;
    double priority;
};

class CramTasker {
public:
    void addSubject(const std::string& name, int points, Date examDate, int credits);
    void removeSubject(const std::string& name);
    void updateGrade(const std::string& subjectName, int newGrade);

    std::vector<StudyDay> generateSchedule(Date today) const;

    const SubjectMap& getSubjects() const { return subjects_; }
    size_t subjectCount() const { return subjects_.size(); }

private:
    SubjectMap subjects_;
};

} // namespace CramCore

#endif