//
// Created by phil on 24/01/18.
//

#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/gazebo_client.hh>
#include <gazebo/gazebo_config.h>
#include <nav_msgs/Odometry.h>
#include <ros/ros.h>
#include <geometry_msgs/Vector3.h>
#include <contact_republisher/contact_msg.h>
#include <contact_republisher/contacts_msg.h>
#include <iostream>
#include <vector>
ros::Publisher pub;

// Forces callback function
void forcesCb(ConstContactsPtr &_msg){
    contact_republisher::contacts_msg contacts_message;
    std::vector<contact_republisher::contact_msg> contacts_list;
    // What to do when callback
    for (int i = 0; i < _msg->contact_size(); ++i) {

        // publish only if digit is in contact name: save time
        if (_msg->contact(i).collision1().find("left_digit") != std::string::npos ||
            _msg->contact(i).collision2().find("left_digit") != std::string::npos) {
            
            contact_republisher::contact_msg contact_message;

            contact_message.collision_1 = _msg->contact(i).collision1();
            contact_message.collision_2 = _msg->contact(i).collision2();

            contact_message.normal[0] = _msg->contact(i).normal().Get(0).x();
            contact_message.normal[1] = _msg->contact(i).normal().Get(0).y();
            contact_message.normal[2] = _msg->contact(i).normal().Get(0).z();

            contact_message.position[0] = _msg->contact(i).position().Get(0).x();
            contact_message.position[1] = _msg->contact(i).position().Get(0).y();
            contact_message.position[2] = _msg->contact(i).position().Get(0).z();

            contact_message.forces[0] = _msg->contact(i).wrench(0).body_1_wrench().force().x();
            contact_message.forces[1] = _msg->contact(i).wrench(0).body_1_wrench().force().y();
            contact_message.forces[2] = _msg->contact(i).wrench(0).body_1_wrench().force().z();

            contact_message.depth = _msg->contact(i).depth().Get(0);

            contacts_list.push_back(contact_message);
        }
    }
    if ( _msg->contact_size()== 0 || contacts_list.size() == 0){
        contact_republisher::contact_msg contact_message;

        contact_message.collision_1 = "default";
        contact_message.collision_2 = "default";

        contact_message.normal[0] = 0;
        contact_message.normal[1] = 0;
        contact_message.normal[2] = 0;

        contact_message.position[0] = 0;
        contact_message.position[1] = 0;
        contact_message.position[2] = 0;

        contact_message.forces[0] = 0;
        contact_message.forces[1] = 0;
        contact_message.forces[2] = 0;

        contact_message.depth = 0;

        contacts_list.push_back(contact_message);
    }
    contacts_message.contacts = contacts_list;
    pub.publish(contacts_message);
}


int main(int _argc, char **_argv){

    // Load Gazebo & ROS
    gazebo::client::setup(_argc, _argv);
    ros::init(_argc, _argv, "force_measure");

    // Create Gazebo node and init
    gazebo::transport::NodePtr node(new gazebo::transport::Node());
    node->Init();

    // Create ROS node and init
    ros::NodeHandle n;
    pub = n.advertise<contact_republisher::contacts_msg>("forces/gazebo", 1000);

    // Listen to Gazebo contacts topic
    gazebo::transport::SubscriberPtr sub = node->Subscribe("/gazebo/default/physics/contacts", forcesCb);

    // Busy wait loop...replace with your own code as needed.
    while (true)
    {
        gazebo::common::Time::MSleep(20);

        // Spin ROS (needed for publisher) // (nope its actually for subscribers-calling callbacks ;-) )
        ros::spinOnce();


    // Mayke sure to shut everything down.

    }
    gazebo::client::shutdown();
}