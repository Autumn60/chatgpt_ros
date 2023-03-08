#include "chatgpt_ros/chatgpt_ros.h"

int main(int argc, char **argv)
{
    ros::init(argc, argv, "chatgpt_node");
    ros::NodeHandle nh;
    ros::NodeHandle pn("~");

    int frequency;
    pn.param<int>("frequency", frequency, 10);

    ros::Rate loop_rate(frequency);

    ChatGPT_ROS chatgpt(nh, pn);

    while(ros::ok())
    {
        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}