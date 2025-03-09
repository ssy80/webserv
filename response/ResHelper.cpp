#include "../header/ResHelper.hpp"
#include "../header/GlobalServer.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/epoll.h>

std::string createErrorResponse(ConfigServer& configServer, std::string errorCode)
{
	std::map<std::string, std::string> errorPageMap = configServer.getErrorPageMap();
    std::string filePath;
    std::map<std::string, std::string>::iterator it;
    std::string statusCode;

    it = errorPageMap.find(errorCode);                         //find error page
    if (it != errorPageMap.end())
    {
        filePath = it->second;
		if (errorCode == "403")
			statusCode == "403 Forbidden";
        else if (errorCode == "404")
            statusCode = "404 Not Found";
		else if (errorCode == "405")
			statusCode = "405 Not Allowed";
		else if (errorCode == "406")
			statusCode == "406 Not Acceptable";
		else if (errorCode == "413")
            statusCode = "413 Content Too Large";
		else if (errorCode == "500")
			statusCode == "500 Internal Server Error";
        else if (errorCode == "501")
            statusCode = "501 Not Implemented";
    }
    else                                                      //cannot find error page, send default error page 404
    {
        std::map<std::string, std::string> defaultErrorPageMap = configServer.getDefaultErrorPageMap();
        it = defaultErrorPageMap.find("404");
        if (it != defaultErrorPageMap.end())
        {
            filePath = it->second;
            statusCode = "404 Not Found";
        }
    }

    //read file
    std::string output; 
    std::string file = readServerFile(filePath);
    Response res = Response::ResBuilder()
									.sc(statusCode)
                                    ->ct(MIME::KEY + MIME::HTML)
									->mc("Connection:", "close")
									->cl(file.size())
									->build();
    output = res.toString();
    output = output + file + '\0';
    return (output);
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

// int readRequestCGI(const string& scriptPath, int clientFd) {
// 	int pipe_fd[2];
// 	if (pipe(pipe_fd) == -1) {
// 		std::cerr << "Pipe failed" << std::endl;
// 		return -1;
// 	}
	
// 	pid_t pid = fork();
// 	if (pid < 0) {
// 		std::cerr << "Fork failed" << std::endl;
// 		return -1;
// 	}
	
// 	if (pid == 0) {
// 		close(pipe_fd[0]);
// 		dup2(pipe_fd[1], STDOUT_FILENO);
// 		close(pipe_fd[1]);

// 		char* args[] = {const_cast<char*>(scriptPath.c_str()), NULL};
// 		execvp(args[0], args);
// 		std::cerr << "Exec failed" << std::endl;
// 		exit(1);

// 	} else {
// 		close(pipe_fd[1]);

// 		int flags = fcntl(pipe_fd[0], F_GETFL, 0);
// 		fcntl(pipe_fd[0], F_SETFL, flags | O_NONBLOCK);

// 		struct epoll_event event;
// 		event.data.fd = pipe_fd[0];
// 		event.events = EPOLLIN;
// 		epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_fd[0], &event);

// 		cgiProcesses[pipe_fd[0]] = {pipe_fd[0], client_fd, {}, pid};

// 		return pipe_fd[0];

// 	}
// }

vector<unsigned char> readRequestCGI(const string& scriptPath) {
	vector<unsigned char> output;
	output.reserve(2048);

	int pipe_fd[2];
	if (pipe(pipe_fd) == -1) {
		std::cerr << "Pipe failed" << std::endl;
		return output;
	}
	
	pid_t pid = fork();
	if (pid < 0) {
		std::cerr << "Fork failed" << std::endl;
		return output;
	}
	
	if (pid == 0) {
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);

		char* args[] = {const_cast<char*>(scriptPath.c_str()), NULL};
		execvp(args[0], args);
		std::cerr << "Exec failed" << std::endl;
		exit(1);

	} else {
		close(pipe_fd[1]);

		char buffer[256];
		ssize_t bytesRead;
		long start = getCurrentTimeMs();

		while (getCurrentTimeMs() - start < TIMEOUT) {
			bytesRead = read(pipe_fd[0], buffer, sizeof(buffer));
			if (bytesRead > 0) {
				output.insert(output.end(), buffer, buffer + bytesRead);
			} else {
				break;
			}
		}

		close (pipe_fd[0]);
		waitpid(pid, NULL, 0);
		return output;
	}
}

string getFileExtension(const string& filename) {
	// Find the last occurrence of '.'
	size_t dotPos = filename.find_last_of('.');
	// If no dot is found or it's the last character, return an empty string
	if (dotPos == std::string::npos || dotPos == filename.length() - 1) {
			return "";
	}
	// Return the substring from the dot to the end
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
		req.print();
		cout << "this is cookie " << req.headers["Cookie"] << endl;
		if (req.headers["Cookie"] != "session=login\r"){
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
	string PATH_INFO = replacePath(req.url, configLocation.getRequestPath(), configLocation.getRoot());
	
	std::cout << "REQ URL: " << req.url << std::endl;
	std::cout << "REQUEST PATH: " << configLocation.getRequestPath() << std::endl;
	std::cout << "ROOT: " << configLocation.getRoot() << std::endl;
	std::cout << "PATH_INFO: " << PATH_INFO << std::endl;	
	
	// cannot find file, return 404
	ifstream f((PATH_INFO).c_str());
	if (!f.good()) {
		std::cerr << "file not found" << std::endl;
		return createErrorResponse(configServer, "404");
	}
	
	// if file is executable, serve cgi
	struct stat st;
	if (stat(PATH_INFO.c_str(), &st) == 0 && (st.st_mode & S_IXUSR) != 0) {
		// int cgiPipeFd = readRequestCGI(PATH_INFO, clientFd);
		// if (cgiPipeFd == -1) {
			// return createErrorResponse(configServer, "500");
		// }
		// return "";		// CGI response will be sent later

		vector<unsigned char> file = readRequestCGI(PATH_INFO);
		if (file.empty()) {
			return createErrorResponse(configServer, "500");
		}
		
		string res;
		res.insert(res.end(), file.begin(), file.end());
		return res;
	}

	// if file is not executable, serve static page

	// cookie management for bonus
	string bonusstr = bonusCookie(configServer, req);
	if (!bonusstr.empty())
		return bonusstr;
	
	// if autoindex is true
	if ((configLocation.getAutoIndex() == "on") && req.url[req.url.size() - 1] == '/') {
		string tmp  = listdir(configLocation.getRoot() + req.url);
		vector<unsigned char> file = readRequestFile(configLocation.getRoot() + req.url + configLocation.getIndex());
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
	if (req.url == "/") {
		vector<unsigned char> file = readRequestFile(configLocation.getRoot() + '/' + configLocation.getIndex());
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

string getChunks(string chunks) {
	std::cout << "CHUNKS: " << chunks << std::endl;
	return "";
}

// post handler is used to upload 1 file through the cgi script
string postHandler(Request& req, ConfigServer& configServer, ConfigLocation& configLocation, std::string uploadDirectory) {
	string PATH_INFO = replacePath(req.url, configLocation.getRequestPath(), configLocation.getRoot());
	
	std::cout << "REQ URL: " << req.url << std::endl;
	std::cout << "REQUEST PATH: " << configLocation.getRequestPath() << std::endl;
	std::cout << "ROOT: " << configLocation.getRoot() << std::endl;
	std::cout << "PATH_INFO: " << PATH_INFO << std::endl;	

	if (req.headers["Content-Type"].find("multipart/form-data") != std::string::npos
		&& req.url.find("save_file.py") != std::string::npos
		&& !req.files.empty()
		&& !req.formFields.empty()) {

		string upload_filename = (uploadDirectory + "/" + req.formFields["filename"]).substr(2);
		string upload_content = req.files["filename"];

		std::cout << "uploadfilename " << upload_filename << std::endl;
		setenv("UPLOAD_FILENAME", upload_filename.c_str(), 1);
		setenv("UPLOAD_CONTENT", upload_content.c_str(), 1);

		vector<unsigned char> file = readRequestCGI(PATH_INFO);
		if (file.empty()) {
			return createErrorResponse(configServer, "500");
		}

		string res;
		res.insert(res.end(), file.begin(), file.end());
		return res;
	}
	
	else if (req.headers.find("Transfer-Encoding") != req.headers.end()
		&& req.headers["Transfer-Encoding"].find("chunked") != std::string::npos) {
		
		string upload_filename = req.formFields["filename"];
		string upload_content = getChunks(req.files["body"]);
		setenv("UPLOAD_FILENAME", upload_filename.c_str(), 1);
		setenv("UPLOAD_CONTENT", upload_content.c_str(), 1);
		vector<unsigned char> file = readRequestCGI(PATH_INFO);
		
		if (file.empty()) {
			return createErrorResponse(configServer, "500");
		}

		string res;
		res.insert(res.end(), file.begin(), file.end());
		return res;
	}

	else {
		return createErrorResponse(configServer, "405");
	}
}

// when delete handler is called, it will delete all files in the folder
string deleteHandler(std::string uploadDirectory) {
	std::cout << "A: 0" << uploadDirectory << std::endl;
	// check if directory exists
	struct stat st;
	if (stat(uploadDirectory.c_str(), &st) != 0 || !S_ISDIR(st.st_mode)) {
		return Response::ResBuilder()
			.sc(SC200)
			->mc("Connection", "close")
			->build()
			.toString() + CLRF;
	}
	std::cout << "A: 1" << std::endl;

	// delete files in directory
	DIR *dir = opendir(uploadDirectory.c_str());
	if (!dir) {
		return Response::ResBuilder()
			.sc(SC200)
			->mc("Connection", "close")
			->build()
			.toString() + CLRF;
	}
	std::cout << "A: 2" << std::endl;
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
        // cout << "Invalid directory or error accessing directory." << endl;
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