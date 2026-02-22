#ifndef TASK_H
#define TASK_H

#include <string>

// дисциплина инфа
struct Subject {
    std::string name;
    int credits;      //вес
    int grade;
};

// 1 таск инфа  
struct Task {
    std::string title;
    std::string subName;
    int start;        
    int end;          
    int priority;  
    double weight;   
};

#endif