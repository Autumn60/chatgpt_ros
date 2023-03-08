#ifndef CHATGPT_ROS_H
#define CHATGPT_ROS_H

#include <ros/ros.h>
#include <curl/curl.h>
#include <regex>
#include "chatgpt_ros/chatgpt_service.h"

class ChatGPT_ROS
{
    public:
        ChatGPT_ROS(ros::NodeHandle &nh, ros::NodeHandle &pn);
        
    private:
        std::string getTextFromJson(const std::string& json);
        static size_t response_cb(void *contents, size_t size, size_t nmemb, void *userp);
        bool chatgpt_cb(chatgpt_ros::chatgpt_service::Request& req, chatgpt_ros::chatgpt_service::Response& res);

        ros::ServiceServer _chatgpt_server;

        CURL *_curl;
        CURLcode _res;

        std::string _url;
        std::string _api_key;
        std::string _model;
        int _max_tokens;
        double _temperature;
};

#endif