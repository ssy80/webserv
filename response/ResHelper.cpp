/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResHelper.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 09:42:21 by daong             #+#    #+#             */
/*   Updated: 2025/03/15 09:42:24 by daong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/ResHelper.hpp"
#include <unistd.h>
#include <sys/wait.h>

string createErrorResponse(ConfigServer& configServer, string errorCode){
	map<string, string> errorPageMap = configServer.getErrorPageMap();
	string filePath = "";
	map<string, string>::iterator it;
	string statusCode = SC404;
	// convert errorcode to statuscode
	if (errorCode == "403")
		statusCode = SC403;
	else if (errorCode == "404")
		statusCode = SC404;
	else if (errorCode == "405")
		statusCode = SC405;
	else if (errorCode == "406")
		statusCode = SC406;
	else if (errorCode == "413")
		statusCode = SC413;
	else if (errorCode == "422")
		statusCode = SC422;
	else if (errorCode == "500")
		statusCode = SC500;
	else if (errorCode == "501")
		statusCode = SC501;
	else if (errorCode == "505")
		statusCode = SC505;
	// find error page
	it = errorPageMap.find(errorCode);
	if (it != errorPageMap.end() && readServerFile(it->second) != "")
		filePath = it->second;
	//cannot find error page, send default error page 404
	else{
		map<string, string> defaultErrorPageMap = configServer.getDefaultErrorPageMap();
		it = defaultErrorPageMap.find("404");
		// if cannot find default error page, create on the spot
		if (it == defaultErrorPageMap.end()){
			return Response::ResBuilder()
							.sc(statusCode)
							->ct(MIME::KEY + MIME::HTML)
							->mc("Connection:", "close")
							->cl(strlen(SC404))
							->build()
							.toString() + SC404 + '\0';
		}
		filePath = it->second;
		statusCode = SC404;
	}	
	//read file
	string file = readServerFile(filePath);
	Response res = Response::ResBuilder()
								.sc(statusCode)
								->ct(MIME::KEY + MIME::HTML)
								->mc("Connection:", "close")
								->cl(file.size())
								->build();
	return res.toString() + file  + '\0';
}

vector<unsigned char> readRequestFile(const string& resource){
	ifstream file;
	vector<unsigned char> output;
	output.reserve(2048);
	file.open(resource.c_str(), ios::binary);
	if (!file)
		return output;
	file >> noskipws;
	while (!file.eof()){
		unsigned char ch;
		file >> ch;
		output.push_back(ch);
	}
	file.close();
	return output;	
}

string getFileExtension(const string& filename) {
	size_t dotPos = filename.find_last_of('.');
	if (dotPos == std::string::npos || dotPos == filename.length() - 1)
		return "";
	return filename.substr(dotPos + 1);
}

string filetype(const string& url){
	string ext = getFileExtension(url);
	if (ext.length() == 0)
		return "";
	static map<string,string> map;
	if (map.size() == 0){
		map["html"] = MIME::HTML;
		map["htm"] = MIME::HTML;
		map["shtml"] = MIME::HTML;
		map["xml"] = MIME::XML;
		map["gif"] = MIME::GIF;
		map["jpeg"] = MIME::JPEG;
		map["jpg"] = MIME::JPEG;
		map["js"] = MIME::JS;
		map["atom"] = MIME::ATOMXML;
		map["rss"] = MIME::RSSXML;
		map["mml"] = MIME::RSSXML;
		map["txt"] = MIME::TXT;
		map["avif"] = MIME::AVIF;
		map["png"] = MIME::PNG;
		map["svg"] = MIME::SVGXML;
		map["svgz"] = MIME::SVGXML;
		map["svgz"] = MIME::SVGXML;
		map["tif"] = MIME::TIFF;
		map["tiff"] = MIME::TIFF;
		map["webp"] = MIME::WEBP;
		map["ico"] = MIME::ICO;
		map["jng"] = MIME::JNG;
		map["bmp"] = MIME::BMP;
		map["jar"] = MIME::JAR;
		map["war"] = MIME::JAR;
		map["ear"] = MIME::JAR;
		map["json"] = MIME::JSON;
		map["pdf"] = MIME::PDF;
		map["rtf"] = MIME::RTF;
		map["doc"] = MIME::DOC;
		map["7z"] = MIME::Z7;
		map["rar"] = MIME::RAR;
		map["zip"] = MIME::ZIP;
		map["bin"] = MIME::BIN;
		map["exe"] = MIME::BIN;
		map["dll"] = MIME::BIN;
		map["deb"] = MIME::BIN;
		map["dmg"] = MIME::BIN;
		map["iso"] = MIME::BIN;
		map["img"] = MIME::BIN;
		map["msi"] = MIME::BIN;
		map["msm"] = MIME::BIN;
		map["midi"] = MIME::MIDI;
		map["mid"] = MIME::MIDI;
		map["kar"] = MIME::MIDI;
		map["mpg"] = MIME::MPEG;
		map["ogg"] = MIME::OGG;
		map["m4a"] = MIME::M4A;
		map["m4a"] = MIME::REALAUDIO;
		map["3gpp"] = MIME::GPP;
		map["3gp"] = MIME::GPP;
		map["ts"] = MIME::MP2T;
		map["mp4"] = MIME::MP4;
		map["mpeg"] = MIME::MPEGV;
		map["mpg"] = MIME::MPEGV;
		map["mov"] = MIME::QUICKTIME;
		map["webm"] = MIME::WEBM;
		map["flv"] = MIME::FLV;
		map["m4v"] = MIME::M4V;
		map["mng"] = MIME::MNG;
		map["asx"] = MIME::ASF;
		map["asf"] = MIME::ASF;
		map["wmv"] = MIME::WMV;
		map["avi"] = MIME::AVI;
	}
	if (map.find(ext) == map.end())
		return MIME::TXT;
	return map[ext];
}

static string bonusCookie(ConfigServer& configServer, Request& req){
	if (req.url=="/14789632"){
		if (req.headers["Cookie"].find("session=login") != string::npos){
			Response res = Response::ResBuilder()
				.sc(SC200)
				->ct(MIME::KEY + MIME::HTML)
				->cl(25)
				->mc("Connection", "close")
				->build();
			return res.toString() + "<h1>already logged in</h1>";
		}
		Response res = Response::ResBuilder()
						.sc(SC200)
						->mc("Set-Cookie","session=login")
						->ct(MIME::KEY + MIME::HTML)
						->cl(25)
						->mc("Connection", "close")
						->build();
		return res.toString() + "<h1>login successful</h1>";
	}
	if (req.url == "/secretpage"){
		if (req.headers["Cookie"].find("session=login") == string::npos){
			return createErrorResponse(configServer, "403");
		}
		return Response::ResBuilder()
			.sc(SC200)
			->ct(MIME::KEY + MIME::HTML)
			->cl(10)
			->mc("Connection", "close")
			->build()
			.toString() + "Authorized ";
	}
	return "";
}

string getHandler(Request& req, ConfigServer& configServer, ConfigLocation& configLocation) {
	// cookie management for bonus
	string bonusstr = bonusCookie(configServer, req);
	if (!bonusstr.empty())
		return bonusstr;

	string PATH_INFO = replacePath(req.url, configLocation.getRequestPath(), configLocation.getRoot());
	
	// cannot find file, return 404
	ifstream f((PATH_INFO).c_str());
	if (!f.good()) {
		std::cerr << "file not found" << std::endl;
		return createErrorResponse(configServer, "404");
	}
	
	// if autoindex is true
	if ((configLocation.getAutoIndex() == "on") && req.url[req.url.size() - 1] == '/') {
		string tmp  = listdir(PATH_INFO);
		vector<unsigned char> file = readRequestFile(PATH_INFO + configLocation.getIndex());
		if (file.empty()){
			string res = Response::ResBuilder()
				.sc(SC200)
				->ct(MIME::KEY + MIME::HTML)
				->cl(tmp.size())
				->mc("Connection", "close")
				->build()
				.toString();
			return (res + tmp);
		}
	}

	// getting index page
	if (req.url[req.url.size() - 1] == '/') {
		vector<unsigned char> file = readRequestFile(PATH_INFO + configLocation.getIndex());
		if (file.empty()) {
			return createErrorResponse(configServer, "404");
		}

		string res = Response::ResBuilder()
			.sc(SC200)
			->ct(MIME::KEY + MIME::HTML)
			->mc("Connection", "close")
			->cl(file.size())
			->build()
			.toString();

		res.insert(res.end(), file.begin(), file.end());
		return res;
	}

	// other path
	if (getFileExtension(req.url)==""){
		std::cerr << "unknown file extension" << std::endl;
		return createErrorResponse(configServer, "404");
	}

	vector<unsigned char> file = readRequestFile(PATH_INFO);
	if (file.empty()) {
		std::cerr << "file is empty" << std::endl;
		return createErrorResponse(configServer, "404");
	}
	
	string res = Response::ResBuilder()
		.sc(SC200)
		->ct(MIME::KEY + filetype(PATH_INFO))
		->mc("Connection", "close")
		->cl(file.size())
		->build()
		.toString();
	
	res.insert(res.end(), file.begin(), file.end());
	return res;
}

std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;
    
    while ((end = s.find(delimiter, start)) != std::string::npos) {
        tokens.push_back(s.substr(start, end - start));
        start = end + delimiter.length();
    }
    tokens.push_back(s.substr(start));

    return tokens;
}

// when delete handler is called, it will delete all files in the folder
string deleteHandler(std::string uploadDirectory) {
	// check if directory exists
	struct stat st;
	if (stat(uploadDirectory.c_str(), &st) != 0 || !S_ISDIR(st.st_mode)) {
		return Response::ResBuilder()
			.sc(SC200)
			->mc("Connection", "close")
			->build()
			.toString() + CLRF;
	}

	// delete files in directory
	DIR *dir = opendir(uploadDirectory.c_str());
	if (!dir) {
		return Response::ResBuilder()
			.sc(SC200)
			->mc("Connection", "close")
			->build()
			.toString() + CLRF;
	}
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		string file_name = entry->d_name;

		if (file_name == "." || file_name == "..") {
			continue;
		}

		string full_path = uploadDirectory + "/" + file_name;

		struct stat st;
		if (stat(full_path.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
			// Delete file
			std::cout << "Deleting file: " << full_path << std::endl;
			unlink(full_path.c_str());
		}
	}
	closedir(dir);
	return Response::ResBuilder()
		.sc(SC200)
		->mc("Connection", "close")
		->build()
		.toString() + CLRF;
}

string otherHandler(ConfigServer& configServer){
	return createErrorResponse(configServer, "501");
}

string listdir(const string& path){
	DIR* dir = opendir(path.c_str());
    string res = "";
	if (dir == NULL) {
		res += "Invalid directory or error accessing directory.";
        return res;
    }
	struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {
    string fileStr(entry->d_name);
    if (fileStr == "." || fileStr == ".."){
        // cout << entry->d_name << endl;
      continue;
    }
    string tmp(entry->d_name);
    if (entry->d_type == DT_REG){
      res += "<a href=\"" + tmp + "\">" +  tmp + "</a> <br/>";
      // cout << "this is file : " <<  entry->d_name << endl;
    }
    else if (entry->d_type == DT_DIR){
      res += "<a href=\"" + tmp + "/\">" +  tmp + "/</a> <br/>";
      // cout << "this is folder : " <<  entry->d_name << endl;
    }
  }
  closedir(dir);
	return res;
}

string postUploadHandler(Request& req, ConfigServer& configServer, ConfigLocation& configLocation){
  // (void) configServer;
  (void) configLocation;
  string PATH_INFO = replacePath(req.url, configLocation.getRequestPath(), configLocation.getRoot());
  cout << PATH_INFO << endl;
  struct stat sb;
  if (stat(PATH_INFO.c_str(), &sb) != 0)
    return createErrorResponse(configServer, "422");
  std::ofstream MyFile((PATH_INFO + req.formFields.begin()->second).c_str());
  string tmp = req.files.begin()->second.substr(2);
  tmp.erase(tmp.end()-1);
  MyFile << tmp;
  // Close the file
  MyFile.close();
  return Response::ResBuilder()
          .sc(SC201)
          ->mc("Connection", "close")
          ->build()
          .toString();
}

// curl -X POST -H "Content-Type: plain/text" --data alsoosss 
// curl -F 'file=@./Makefile' http://localhost:8080/upload/ -i
// curl -F 'file=@./www/images/42.png' http://localhost:8080/ -i
// curl --data-binary @./Makefile http://localhost:8080/cgi-bin/upload -i