#include "../header/ResHelper.hpp"
#include <unistd.h>
#include <sys/wait.h>

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

vector<unsigned char> readRequestCGI(const string& scriptPath) {
	vector<unsigned char> output;
	output.reserve(2048);
	
	int fd[2];
	if (pipe(fd) == -1) {
		std::cerr << "Pipe failed" << std::endl;
		return output;
	}
	
	pid_t pid = fork();
	if (pid < 0) {
		std::cerr << "Fork failed" << std::endl;
		return output;
	}
	
	if (pid == 0) {
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);

		char* args[] = {const_cast<char*>(scriptPath.c_str()), NULL};
		execvp(args[0], args);
		std::cerr << "Exec failed" << std::endl;
		exit(1);

	} else {
		close(fd[1]);

		char buffer[256];
		ssize_t bytesRead;
		long start = getCurrentTimeMs();
        
		while (getCurrentTimeMs() - start < TIMEOUT) {
			bytesRead = read(fd[0], buffer, sizeof(buffer));
			if (bytesRead > 0) {
				output.insert(output.end(), buffer, buffer + bytesRead);
			} else {
				break;
			}
		}
		
		close(fd[0]);
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

static string bonusCookie(Request& req){
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
			return Response::ResBuilder()
						.sc(SC403)
						->mc("Connection", "close")
						->build()
						.toString() + CLRF;
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

string getHandler(Request& req, ConfigLocation& config) {
	bool idx = (config.getAutoIndex() == "on");
	// cookie management for bonus
	string bonusstr = bonusCookie(req);
	if (!bonusstr.empty())
		return bonusstr;
	
	string filePath = replacePath(req.url, config.getRequestPath(), config.getRoot());
	
	ifstream f((filePath).c_str());

	// cannot find file, return 404
	if (!f.good()){
		return Response::ResBuilder()
			.sc(SC404)
			->mc("Connection", "close")
			->build()
			.toString() + CLRF;
	}

	// if autoindex is true
	if (idx && req.url[req.url.size() - 1] == '/') {
		string tmp  = listdir(config.getRoot() + req.url);
		vector<unsigned char> file = readRequestFile(config.getRoot() + req.url + config.getIndex());
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
		vector<unsigned char> file = readRequestFile(config.getRoot() + '/' + config.getIndex());
		if (file.empty()) {
			return Response::ResBuilder()
			.sc(SC404)
			->mc("Connection", "close")
			->build()
			.toString() + CLRF;
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
	
	// getting cgi files
	if (req.url.find("/cgi-bin") != std::string::npos) {
		vector<unsigned char> file = readRequestCGI(filePath);
		if (file.empty()) {
			return Response::ResBuilder()
			.sc(SC404)
			->mc("Connection", "close")
			->build()
			.toString() + CLRF;
		}

		string res = Response::ResBuilder()
			.sc(SC200)
			->mc("Connection", "close")
			->cl(file.size())
			->build()
			.toString();
    
		res.insert(res.end(), file.begin(), file.end());
		return res;
	}

	// other path
	if (getFileExtension(req.url)==""){
		return Response::ResBuilder()
			.sc(SC404)
			->mc("Connection", "close")
			->build()
			.toString() + CLRF;
	}
	vector<unsigned char> file = readRequestFile(config.getRoot() + req.url);
	if (file.empty()) {
		return Response::ResBuilder()
		.sc(SC404)
		->mc("Connection", "close")
		->build()
		.toString() + CLRF;
	}

	string res = Response::ResBuilder()
		.sc(SC200)
		->ct(MIME::KEY + filetype(config.getRoot()+ req.url))
		->mc("Connection", "close")
		->cl(file.size())
		->build()
		.toString();
	
	res.insert(res.end(), file.begin(), file.end());
	return res;
}

// post handler is used to upload 1 file through the cgi script
string postHandler(Request& req, ConfigLocation& config) {
	req.print();
	
	string filePath = replacePath(req.url, config.getRequestPath(), config.getRoot());


	if (req.files.empty()) {
		std::cout << "NO FILES" << std::endl;
		return "";
	}

	std::cout << "FILES: " << req.files["filename"] << std::endl;
	
	if (req.url.find("/cgi-bin") != std::string::npos) {
		std::cout << "executing cgi" << std::endl;

		setenv("UPLOAD_FILENAME", req.files["filename"].c_str(), 1);
		vector<unsigned char> file = readRequestCGI(filePath);
		std::cout << string(file.begin(), file.end()) << std::endl;
		if (file.empty()) {
			return Response::ResBuilder()
			.sc(SC404)
			->mc("Connection", "close")
			->build()
			.toString();
		}

		string res = Response::ResBuilder()
			.sc(SC201)
			->mc("Connection", "close")
			->cl(file.size())
			->build()
			.toString();
		
		res.insert(res.end(), file.begin(), file.end());
		return res;
	}

	return Response::ResBuilder()
		.sc(SC403)
		->mc("Connection", "close")
		->build()
		.toString();
}

// when delete handler is called, it will delete all files in the folder
string deleteHandler(ConfigLocation& config) {
	std::cout << config.getRoot() << std::endl;
	string dir_path = "www/tmp";

	// check if directory exists
	struct stat st;
	if (stat(dir_path.c_str(), &st) != 0 || !S_ISDIR(st.st_mode)) {
		return Response::ResBuilder()
			.sc(SC200)
			->mc("Connection", "close")
			->build()
			.toString();
	}

	// delete files in directory
	DIR *dir = opendir(dir_path.c_str());
	if (!dir) {
		return Response::ResBuilder()
			.sc(SC200)
			->mc("Connection", "close")
			->build()
			.toString();
	}

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		string file_name = entry->d_name;

		if (file_name == "." || file_name == "..") {
			continue;
		}

		string full_path = dir_path + "/" + file_name;

		struct stat st;
		if (stat(full_path.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
			// Delete file
			unlink(full_path.c_str());
		}
	}
	closedir(dir);
	return Response::ResBuilder()
		.sc(SC200)
		->mc("Connection", "close")
		->build()
		.toString();
}

string otherHandler(){
	string res = Response::ResBuilder()
		.sc(SC406)
		->mc("Connection", "close")
		->build()
		.toString();
	return res;
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