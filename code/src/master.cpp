//    ||sai ram||
//
//    JACO ARM Assignment
//    Author : Victor CHoudhary


#include <ros/ros.h>
//PCL Specific
#include <sensor_msgs/PointCloud2.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/TwistStamped.h>
#include <geometry_msgs/PoseStamped.h>
#include <tf/transform_listener.h>
#include <jaco_msgs/ArmPoseAction.h>
#include <jaco_msgs/SetFingersPositionAction.h>
#include <jaco_msgs/HomeArm.h>
#include <std_msgs/String.h>
#include <actionlib/client/simple_action_client.h>
#include <stack>
#include <deque>
#include <pcl/common/transforms.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>

using namespace std;

typedef actionlib::SimpleActionClient<jaco_msgs::ArmPoseAction> Client;

int OCCUPIED_CELL = 0;
int USER_CELL = 0;
int COMP_CELL = 0;

bool CELL_OCCUPIED[9] = {false};

char done = ' ';
bool flag = true;

struct position{
    double x;
    double y;
    double z;
    
    double o_x;
    double o_y;
    double o_z;
    double o_w;
};

struct location{
    double x;
    double y;
    double z;
};
location cube_1, cube_2, cube_3, cube_4, cube_5;
location cell[9];
stack<location> cube_stack;

ros::Publisher pub;

void move_it(int cell_loc);



#include "tic_tac.h"
#include "jaco_move.hpp"
#include "vision.hpp"
#include "finger_move.hpp"


void get_cubes(int argc,char** argv){
    ros::init(argc,argv,"matrix_transform");
    ros::NodeHandle nh;
    ros::Subscriber sb1 = nh.subscribe("/camera/depth_registered/points",1,work);
    pub = nh.advertise<sensor_msgs::PointCloud2>("transformed_output",1);
    ros::spin();
}

void open_finger(){
    move_finger(30,30,30);
}

void close_finger(){
    move_finger(45,45,30);
}

void define_locations();
void move_it(int cell);


int main(int argc,char** argv) {
    ros::init(argc,argv,"game_master");
    
    define_locations();
    init_matrix();
    
    get_cubes(argc,argv);
    
    return 1;
}

void move_it(int cell_loc){
    ros::NodeHandle n;
    
    // reach home
    ros::ServiceClient home_client = n.serviceClient<jaco_msgs::HomeArm>("/jaco_arm_driver/in/home_arm");
    jaco_msgs::HomeArm home_srv;
    home_client.call(home_srv);
    
    open_finger();
    cout << "home done" << endl;
    
    location cube = cube_stack.top();
    cube_stack.pop();
    
    position top_pos = {cube.x, cube.y, 0.4, 0.9240, 0.3822, 0.0016, 0.0085};
    navigate_to(top_pos);
    cout << "top: press enter";
    cin.get();
    
    position pick_pos = {cube.x, cube.y, cube.z, 0.9240, 0.3822, 0.0016, 0.0085};
    navigate_to(pick_pos);
    cout << "picking up : press enter";
    cin.get();
    close_finger();
    cout << "picked\n";
    
    location cell_pos = cell[cell_loc-1];
    
    position cell_top = {cell_pos.x,cell_pos.y, 0.4, 0.9240, 0.3822, 0.0016, 0.0085};
    navigate_to(cell_top);
    cout << "top: press enter";
    cin.get();
    
    position drop_pos = {cell_pos.x,cell_pos.y, cell_pos.z, 0.9240, 0.3822, 0.0016, 0.0085};
    navigate_to(drop_pos);
    cout << "dropping : press enter ";
    cin.get();
    open_finger();
    cout << "picked\n";
    
    navigate_to(cell_top);
    home_client.call(home_srv);
}

void define_locations(){
    cube_1.x = -0.13; cube_1.y = -0.45; cube_1.z = 0.18;
    cube_2.x = -0.05; cube_2.y = -0.55; cube_2.z = 0.2;
    cube_3.x = 0.05; cube_3.y = -0.65; cube_3.z = 0.22;
    cube_4.x = 0.33; cube_4.y = -0.3; cube_4.z = 0.18;
    cube_5.x = 0.23; cube_5.y = -0.23; cube_5.z = 0.16;
    
    cube_stack.push(cube_5);
    cube_stack.push(cube_4);
    cube_stack.push(cube_3);
    cube_stack.push(cube_2);
    cube_stack.push(cube_1);
    
    cell[0].x = 0.0078; cell[0].y = -0.3999; cell[0].z = 0.18;
    cell[1].x = 0.0702; cell[1].y = -0.3377; cell[1].z = 0.18;
    cell[2].x = 0.1228; cell[2].y = -0.2601; cell[2].z = 0.16;
    cell[3].x = 0.0591; cell[3].y = -0.4459; cell[3].z = 0.16;
    cell[4].x = 0.119; cell[4].y = -0.38   ; cell[4].z = 0.18;
    cell[5].x = 0.1847; cell[5].y = -0.3241; cell[5].z = 0.18;
    cell[6].x = 0.1084; cell[6].y = -0.5167; cell[6].z = 0.18;
    cell[7].x = 0.1837; cell[7].y = -0.4492; cell[7].z = 0.18;
    cell[8].x = 0.2462; cell[8].y = -0.3757; cell[8].z = 0.18;
}
