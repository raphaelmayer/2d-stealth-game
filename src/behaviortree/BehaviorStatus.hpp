#pragma once

enum BehaviorStatus { 
	IDLE = 0,
	SUCCESS, 
	FAILURE, 
	RUNNING 
};

enum TreeNodeType { 
	UNDEFINED = 0, 
	ACTION, 
	CONDITION, 
	CONTROL, 
	DECORATOR, 
	SUBTREE 
};