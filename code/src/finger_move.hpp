#ifndef finger_move_h
#define finger_move_h


typedef actionlib::SimpleActionClient<jaco_msgs::SetFingersPositionAction> finger_client;

bool move_finger(double angle1,double angle2, double angle3){
  finger_client client("/jaco_arm_driver/fingers/finger_positions",true);
  client.waitForServer();

  jaco_msgs::SetFingersPositionGoal goal;
  goal.fingers.finger1 = angle1;
  goal.fingers.finger2 = angle2;
  goal.fingers.finger3 = angle3;
  client.sendGoal(goal);
  client.waitForResult(ros::Duration(5.0));

  if(client.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
    printf("Yay");
  printf("Current state : %s\n",client.getState().toString().c_str());



}

#endif
