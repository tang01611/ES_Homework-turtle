#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <turtlesim/Pose.h>
 
#include<geometry_msgs/Twist.h> //运动速度结构体类型  geometry_msgs::Twist的定义文件
#include<iostream>
#include<cmath>
#include<random>


std::string turtle_name;
 
void poseCallback(const turtlesim::PoseConstPtr& msg)
{
    // tf broadcaster
    static tf::TransformBroadcaster br;
 
    // Set the coordinate transformation relative to the world coordinate system according to the current pose of the turtle。
    tf::Transform transform;
    transform.setOrigin(tf::Vector3(msg->x, msg->y, 0.0));
    tf::Quaternion q;
    q.setRPY(0, 0, msg->theta);
    transform.setRotation(q);
 
    // Post coordinate transformation.
    br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "world", turtle_name));
}
 
int main(int argc, char** argv)
{


    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<double> dist1(1.0,4.0);
    std::uniform_real_distribution<double> dist2(-4*3.14,4*3.14);



    // Initialize the node.
    ros::init(argc, argv, "my_tf_broadcaster");
    if (argc != 2)
    {
        ROS_ERROR("need turtle name as argument"); 
        return -1;
    };
    turtle_name = argv[1];
 
    // Subscribe to tortoise's pose information
    ros::NodeHandle n;
    ros::Subscriber sub = n.subscribe(turtle_name+"/pose", 10, &poseCallback);

    ros::Publisher vel_pub = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
    //vel_pub会在主题"/cmd_vel"(机器人速度控制主题)里广播geometry_msgs::Twist类型的数据
    ROS_INFO("draw_circle start...");//输出显示信息

    while(ros::ok())
    {
        geometry_msgs::Twist vel_cmd; //声明一个geometry_msgs::Twist 类型的对象vel_cmd，并将速度的值赋值到这个对象里面
 
        vel_cmd.linear.x = dist1(gen);//前后（+-） m/s
        vel_cmd.linear.y = 0.0;  //左右（+-） m/s
        vel_cmd.linear.z = 0.0;
 
        vel_cmd.angular.x = 0;
        vel_cmd.angular.y = 0;
        vel_cmd.angular.z = dist2(gen); //机器人的自转速度，+左转，-右转，单位是rad/s
        vel_pub.publish(vel_cmd); //赋值完毕后，发送到主题"/cmd_vel"。机器人的核心节点会从这个主题接受发送过去的速度值，并转发到硬件体上去执行
 
        ros::spinOnce();//调用此函数给其他回调函数得以执行(比例程未使用回调函数)
    }



    // Call the posecallback function cyclically
    ros::spin();
    return 0;
};
