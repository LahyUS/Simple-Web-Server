#include <string>
#include <vector>
#include <iostream>

std::vector<std::string> split(std::string, std::string);

std::vector<std::string> parse_Req(std::string msg);

std::string getURL(std::string firstLineReq);

std::string readFileText(std::string filePath);
std::string readFileBin(std::string filePath);

std::string checkTypeFile(std::string filePath);

void handle_Request(char* lpBuffRevc, std::string& content, std::string& contentType);
