/*!
 * AgentState.h
 *
 * \brief Represents the current state of an agent: its current position,
 *        its plans, etc.).
 *
 * \author Eric Schneider <esch@hunter.cuny.edu>
 * \date 11/11/2011 Created
 */
#ifndef AGENTSTATE_H
#define AGENTSTATE_H

#include "Task.h"
#include "FORRAction.h"
#include "Position.h"
#include "FORRGeometry.h"

#include <time.h>
#include <unistd.h>

#include <vector>
#include <list>
#include <deque>
#include <set>

#include <ros/ros.h>
#include <ros/console.h>
#include <sensor_msgs/LaserScan.h>

using namespace std;

class AgentState
{
 public:
  /** \brief AgentState constructor */
  AgentState() : currentTask(NULL) {
	vetoedActions = new set<FORRAction>();
        pos_hist = new vector<Position>();
        action_set = new set<FORRAction>();
	forward_set = new set<FORRAction>();
	rotation_set = new set<FORRAction>();

    	for(int i = 1; i < 5; i++){
      		action_set->insert(FORRAction(LEFT_TURN, i));
      		action_set->insert(FORRAction(RIGHT_TURN, i));
		rotation_set->insert(FORRAction(LEFT_TURN, i));
      		rotation_set->insert(FORRAction(RIGHT_TURN, i));
    	}
    	for(int i = 1; i < 6; i++){
      		action_set->insert(FORRAction(FORWARD, i));
		forward_set->insert(FORRAction(FORWARD, i));
    	}
    	action_set->insert(FORRAction(PAUSE,0));
	forward_set->insert(FORRAction(PAUSE,0));
	//rotation_set->insert(FORRAction(PAUSE,0));
	double m[] = {0, 0.2, 0.4, 0.8, 1.6, 3.2};  
  	double r[] = {0, 0.25, 0.5, 1, 2};
	for(int i = 0 ; i < 6 ; i++) move[i] = m[i];
	for(int i = 0 ; i < 5 ; i++) rotate[i] = r[i];
	
  }
  
  // Best possible move towards the target
  FORRAction moveTowards();

  set<FORRAction> *getActionSet(){return action_set;}
  set<FORRAction> *getForwardActionSet(){return forward_set;}
  set<FORRAction> *getRotationActionSet(){return rotation_set;}

  vector<Position> *getPositionHistory(){return pos_hist;}

  void clearPositionHistory(){pos_hist->clear();}

  void savePosition(){
	if(pos_hist->size() < 1){
		pos_hist->push_back(currentPosition);
	}
	else{	
     		Position pos = pos_hist->back();
     		if(!(pos == currentPosition)) 
			pos_hist->push_back(currentPosition);
	}
  }

  double getDistanceToTarget(double x, double y){ 
    double dx = x - currentTask->getX();
    double dy = y - currentTask->getY();
    return sqrt((dx*dx) + (dy*dy));
  }

  
  // returns the euclidiean distance to target from the current robot position 
  double getDistanceToTarget(){
     return currentPosition.getDistance(currentTask->getX(), currentTask->getY());
  }
  
  bool isDestinationReached(int epsilon){
    if(getDistanceToTarget() < epsilon) {
        return true;
    }
    return false;
  }

  Position getCurrentPosition() { return currentPosition; }
  void setCurrentPosition(Position p) { 
	currentPosition = p;
	savePosition(); 
  }
  
  
  Task *getCurrentTask() { return currentTask; }
  void setCurrentTask(Task *task) { 
    currentTask = task; 
  }


  set<FORRAction> *getVetoedActions() { return vetoedActions;}
  void clearVetoedActions() { vetoedActions->clear();}
  
  void addTask(float x, float y) {
    Task *task = new Task(x,y);
    agenda.push_back(task); 
  }
  
  Task *getNextTask() { return agenda.front(); }
  
  list<Task*>& getAgenda() { return agenda; }
  
  //Merge finish task and skip task they are both doing the same right now
  void finishTask() {
    if (currentTask != NULL)
      agenda.remove(currentTask);

    currentTask = NULL;
    clearPositionHistory();
  }

  void skipTask() {
    if (currentTask != NULL)
      agenda.remove(currentTask);

    currentTask = NULL;
    clearPositionHistory();
  }

  bool isMissionComplete(){
	bool status = false; 
	if(getAgenda().size() == 0 && currentTask == NULL){
		status = true;
 	}
	return status;
  }

  sensor_msgs::LaserScan getCurrentLaserScan(){return currentLaserScan;}
  void setCurrentLaserScan(sensor_msgs::LaserScan scan){ 
     currentLaserScan = scan;
     transformToEndpoints();
  }

  Position getExpectedPositionAfterAction(FORRAction action);
  //Position getExpectedPositionAfterActions(Position initialPosition, vector<FORRAction> actions);

  //Default as currentPosition
  //Position getExpectedPositionAfterAction(FORRAction action){
	//return getExpectedPositionAfterAction(currentPosition, action);
  //}
  //Position getExpectedPositionAfterActions(vector<FORRAction> actions){
	//return getExpectedPositionAfterActions(currentPosition, actions);
  //}

  // Returns distance from obstacle 
  double getDistanceToNearestObstacle(Position pos);

  // returns distance to obstacle in the direction of rotation
  double getDistanceToObstacle(double rotation_angle);
  double getDistanceToForwardObstacle(){
	//ROS_DEBUG("in getDistance to forward obstacle");
	if(currentLaserScan.ranges.size() == 0)
		return 25;
	return currentLaserScan.ranges[currentLaserScan.ranges.size()/2];
  }

  FORRAction maxForwardAction();

  // Can a robot see a segment or a point using its laser scan data?
  bool canSeeSegment(CartesianPoint point1, CartesianPoint point2);
  bool canSeeSegment(vector<CartesianPoint> givenLaserEndpoints, CartesianPoint laserPos, CartesianPoint point1, CartesianPoint point2);
  bool canSeePoint(vector<CartesianPoint> givenLaserEndpoints, CartesianPoint laserPos, CartesianPoint point);
  bool canSeePoint(CartesianPoint point);

  double getMovement(int para){return move[para];}
  double getRotation(int para){return rotate[para];}
 

 private:

  // Stores the move and rotate action values
  double move[6];  
  double rotate[5];

  FORRAction get_max_allowed_forward_move();

  // Current position of the agent x, y, theta 
  Position currentPosition;

  // Position History : Set of all unique positions the robot has been in , while pursuing the target
  vector<Position> *pos_hist;

  // set of vetoed actions that the robot cant execute in its current state
  set<FORRAction> *vetoedActions;

  // Set of all actions that the robot has in its action set
  set<FORRAction> *action_set;

  // Set of all forward actions that the robot has in its action set
  set<FORRAction> *forward_set;

  // Set of all forward actions that the robot has in its action set
  set<FORRAction> *rotation_set;
  
  // aggregate decision making statistics of the agent
  // Total travel time
  double total_travel_time;

  // Total travel distance
  double total_travel_distance;

  // Total tier 1 decisions 
  int total_t1_decisions;

  // Total tier 2 decisions
  int total_t2_decisions;

  // Total tier 3 decisions
  int total_t3_decisions;
  
  /** \brief The agent's list of tasks */
  list<Task*> agenda;
  
  // Current task in progress
  Task *currentTask;

  // Currrent laser scan reading at the current position
  sensor_msgs::LaserScan currentLaserScan;

  // Current laser scan data as endpoints in the x-y coordinate frame
  vector<CartesianPoint> laserEndpoints;

  //Converts current laser range scanner to endpoints
  void transformToEndpoints();

  //after linear move
  Position afterLinearMove(Position initialPosition, double distance);
  Position afterAngularMove(Position initialPosition, double angle);

};

#endif
