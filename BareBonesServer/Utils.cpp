#define _CRT_SECURE_NO_WARNINGS
#include "Utils.h"
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <iterator>
#include <algorithm>

std::vector<std::string> split(std::string str, std::string delimiter)
{
	std::vector<std::string> retVal;
	size_t pos = 0;
	std::string token;
	while ((pos = str.find(delimiter)) != std::string::npos) 
	{
		token = str.substr(0, pos);
		//std::cout << token << std::endl;
		retVal.push_back(token);
		str.erase(0, pos + delimiter.length());
	}
	retVal.push_back(str);
	return retVal;
}

std::vector<std::string> parse_Req(std::string msg)
{
	std::vector<std::string> retVal = split(msg, "\r\n");
	return retVal;
}

std::string getURL(std::string firstLineReq)
{
	std::vector<std::string> result = split(firstLineReq, " ");
	return result[1];
}

std::string readFileText(std::string filePath)
{
	std::string result;
	std::string line;
	std::ifstream myfile(filePath);
	if (myfile.is_open())
	{
		while (myfile.good())
		{
			getline(myfile, line);
			result += line;
		}
		myfile.close();
	}

	return result;
}

std::string readFileBin(std::string filePath)
{
	std::ifstream fileInput(filePath, std::ios::binary);
	std::string str;
	// Check if it opened and if it did, grab the entire contents
	if (fileInput.good())
	{
		str.assign((std::istreambuf_iterator<char>(fileInput)), std::istreambuf_iterator<char>());
	}

	fileInput.close();

	return str;
}

std::string checkTypeFile(std::string filePath)
{
	unsigned int posOfDot = filePath.find_last_of('.');
	std::string extens = filePath.substr(posOfDot + 1, filePath.length() - 1);

	if (extens == "png" || extens == "jpg" || extens == "jpeg")
		return "image/" + extens;
	else if (extens == "js")
		return "text/javascript";
	else if (extens == "css")
		return "text/css";
	else
		return "text/html";
}

void handle_Request(char* lpBuffRevc, std::string& content, std::string& contentType)
{
	// Parse request content
	std::vector<std::string> req = parse_Req(std::string(lpBuffRevc));

	// Get URL in request content
	std::string url = getURL(req[0]);
	std::string indexDir;

	if (url == "/")
	{
		url = "SignUpForm/index.html";
	}

	else if (url.length() > 1)
	{
		// URL like : /filename.type
		// Check folder of request file
		if (url[3] == 'f')	// Info file
		{	// Name of folder contain html page (include in project folder)
			indexDir = "Redirection"; //index direction
		}
		else if (url[3] == 'd')	// Index file
		{	// Name of folder contain html page (include in project folder)
			indexDir = "SignUpForm"; //index direction
		}
		else if (url[1] == '4') // 404 file
		{
			indexDir = "404";
		}

		url = indexDir + url; // relative path
	}

	contentType = checkTypeFile(url);	// Get content type

	if (contentType == "image/png" || contentType == "image/jpeg" || contentType == "image/jpg")
	{
		content = readFileBin(url);
	}
	else
		content = readFileText(url);
}

