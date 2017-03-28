/**!
  * Task.h
  * 
  * /author: Anoop Aroor
  *
  *          Defines tasks as simple target and stores its status and statistics of completion
  */

#ifndef TASK_H
#define TASK_H

#include "definitions.h"
#include "FORRAction.h"
#include <vector>
#include <map>
#include <algorithm>

class Task {
  
 public:
  
  Task(int x_in, int y_in)  
    {
      x = x_in;
      y = y_in; 
      status_ = ENROUTE; 
      decision_count = 0;
      decisionSequence = new std::vector<FORRAction>;
    }

  int getX() { return x; }
  
  int getY() { return y; }
  
  TASK_STATUS getStatus() { return status_; }
  
  void setStatus(TASK_STATUS stat) { status_ = stat; }

  int getDecisionCount(){return decision_count;} 
 
  int incrementDecisionCount() {decision_count += 1;}

  std::vector<FORRAction> getPreviousDecisions(){
	return *decisionSequence;
  }

  FORRAction saveDecision(FORRAction decision){
	decisionSequence->push_back(decision);
  }

 private:
  
  //<! expected task execution time in seconds 
  float time_taken;

  // distance covered by the robot to get to the target
  float distance_travelled; 

  // Sequence of decisions made
  std::vector<FORRAction> *decisionSequence; 

  // decision count
  int decision_count;

  // t1, t2, t3 counters
  int tier1_decisions, tier2_decisions, tier3_decisions;

  //<! The point in the map, that the robot needs to go in order to execute this task 
  float x,y;
  
  //<! States the execution status of the task
  TASK_STATUS status_; 
  
};

#endif
