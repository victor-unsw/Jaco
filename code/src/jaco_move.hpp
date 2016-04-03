
#ifndef jaco_move_h
#define jaco_move_h



typedef actionlib::SimpleActionClient<jaco_msgs::ArmPoseAction> Client;

bool navigate_to(position pos){
  
  Client client("/jaco_arm_driver/arm_pose/arm_pose",true);
  client.waitForServer();

  jaco_msgs::ArmPoseGoal goal;
  goal.pose.header.frame_id = "jaco_api_origin";
  goal.pose.pose.position.x = pos.x;
  goal.pose.pose.position.y = pos.y;
  goal.pose.pose.position.z = pos.z;
  goal.pose.pose.orientation.x = pos.o_x;
  goal.pose.pose.orientation.y = pos.o_y;
  goal.pose.pose.orientation.z = pos.o_z;
  goal.pose.pose.orientation.w = pos.o_w;

  client.sendGoal(goal);
  client.waitForResult(ros::Duration(5.0));

  if(client.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
    printf("Yay");
  printf("Current state : %s\n",client.getState().toString().c_str());
  
}

#endif
