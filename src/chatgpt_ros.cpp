#include "chatgpt_ros/chatgpt_ros.h"

ChatGPT_ROS::ChatGPT_ROS(ros::NodeHandle &nh, ros::NodeHandle &pn)
{
    std::string chatgpt_service;
    pn.param<std::string>("chatgpt_service", chatgpt_service, "chatgpt_service");

    pn.param<std::string>("url", _url, "https://api.openai.com/v1/completions");
    pn.param<std::string>("api_key", _api_key, "API KEY");
    pn.param<std::string>("model", _model, "text-davinci-003");
    pn.param<int>("max_tokens", _max_tokens, 4000);
    pn.param<double>("temperature", _temperature, 1.0);

    _chatgpt_server = nh.advertiseService(chatgpt_service, &ChatGPT_ROS::chatgpt_cb, this);
}

std::string ChatGPT_ROS::getTextFromJson(const std::string& json)
{
    std::regex pattern("\"text\"\\s*:\\s*\"([^\"]+)\"");

    std::smatch match;
    if (std::regex_search(json, match, pattern) && match.size() > 1 && match.str(1).length() > 4)
    {
        std::string text = match.str(1);
        std::regex pattern_("[A-Z].*");
        std::smatch match_;
        if (std::regex_search(text, match_, pattern_)) return match_.str();
        return text;
    }

    return "";
}

size_t ChatGPT_ROS::response_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool ChatGPT_ROS::chatgpt_cb(chatgpt_ros::chatgpt_service::Request& req, chatgpt_ros::chatgpt_service::Response& res)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    _curl = curl_easy_init();
    if (!_curl) { curl_easy_cleanup(_curl); curl_global_cleanup(); return false; }

    std::string post_data = "{\"model\": \"" + _model + "\", \"prompt\": \"" + req.message + "\", \"max_tokens\": " + std::to_string(_max_tokens) + ", \"temperature\": " + std::to_string(_temperature) + "}";

    curl_easy_setopt(_curl, CURLOPT_URL, _url.c_str());
    curl_easy_setopt(_curl, CURLOPT_POST, 1L);
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, post_data.length());

    struct curl_slist *headers=NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("Authorization: Bearer " + _api_key).c_str());
    curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, headers);

    std::string responseBuffer;
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, this->response_cb);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &responseBuffer);
    curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 1L);

    _res = curl_easy_perform(_curl);
    
    curl_easy_cleanup(_curl);
    curl_global_cleanup();

    if (_res != CURLE_OK) return false;

    std::cout << getTextFromJson(responseBuffer) << std::endl;
    res.response = getTextFromJson(responseBuffer);

    return true;
}