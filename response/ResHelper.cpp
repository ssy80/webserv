#include "../header/ResHelper.hpp"

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

    FILE* pipe = popen(scriptPath.c_str(), "r"); // Execute CGI script
    if (!pipe)
		return output;

    char buffer[128];
	size_t bytesRead;
	while ((bytesRead = fread(buffer, 1, sizeof(buffer), pipe)) > 0)
		output.insert(output.end(), buffer, buffer + bytesRead);
    pclose(pipe);
    return output;
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
		return "";
	return map[ext];
}

string getHandler(Request req, ConfigLocation config) {
	bool idx = (config.getAutoIndex() == "on");

	ifstream f((config.getRoot() + req.url).c_str());

	// cannot find file, return 404
	if (!f.good()){
		string res = Response::ResBuilder()
			.sc(SC404)
			->mc("Connection", "close")
			->build()
			.toString();
		return res;
	}

	// if autoindex is true
	if (idx && req.url[req.url.size() - 1] == '/') {
		string tmp  = listdir(config.getRoot());
		string res = Response::ResBuilder()
							.sc(SC200)
							->ct(MIME::KEY + MIME::HTML)
							->cl(tmp.size())
							->mc("Connection", "close")
							->build()
							.toString();
		return (res + tmp);
	}

	// getting index page
	if (req.url == "/") {
		vector<unsigned char> file = readRequestFile(config.getRoot() + config.getIndex());
		
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
	if (req.url == "/cgi-bin/time.py" || req.url == "/cgi-bin/image.py") {
		vector<unsigned char> file = readRequestCGI(config.getRoot() + req.url);
		
		string contentType;
		if (req.url == "/cgi-bin/image.py") {
       		contentType = "Content-Type: image/png";
		} else {
        	contentType = "Content-Type: text/html";
		}
		
		string res = Response::ResBuilder()
			.sc(SC200)
			->ct(contentType)
			->mc("Connection", "close")
			->cl(file.size())
			->build()
			.toString();

		res.insert(res.end(), file.begin(), file.end());
		return res;
	}

	// other path
	vector<unsigned char> file = readRequestFile(config.getRoot() + req.url);
	string res = Response::ResBuilder()
		.sc(SC200)
		->ct(MIME::KEY + filetype(config.getRoot()))
		->mc("Connection", "close")
		->cl(file.size())
		->build()
		.toString();
	
	res.insert(res.end(), file.begin(), file.end());
	return res;
}

string postHandler(Request req, ConfigLocation config) {
	std::ifstream src(req.url.c_str(), ios::binary);
	
	if (!src.good()) {
		string res = Response::ResBuilder()
			.sc(SC404)
			->mc("Connection", "close")
			->build()
			.toString();
		return res;
	}

	string dir = config.getRoot() + "./cache" + req.url;
	std::ofstream dst(dir.c_str(), ios::binary);
	if (!dst.good()) {
		string res = Response::ResBuilder()
			.sc(SC500)
			->mc("Connection", "close")
			->build()
			.toString();
		return res;
	}

	dst << src.rdbuf();
	
	// file saved, return 201
	string res = Response::ResBuilder()
		.sc(SC201)
		->mc("Connection", "close")
		->build()
		.toString();
	return res;
}

string deleteHandler(Request req, ConfigLocation config) {
	string dir = config.getRoot() + "./cache" + req.url;
	ifstream f(dir.c_str());

	// cannot find file, return 404
	if (!f.good()) {
		string res = Response::ResBuilder()
			.sc(SC404)
			->mc("Connection", "close")
			->build()
			.toString();
		return res;
	}

	int status = remove(dir.c_str());
	if (status != 0) {
		// file cannot be deleted, return 500
		string res = Response::ResBuilder()
			.sc(SC500)
			->mc("Connection", "close")
			->build()
			.toString();
		return res;
	}

	// file deleted, return 200
	string res = Response::ResBuilder()
		.sc(SC200)
		->mc("Connection", "close")
		->build()
		.toString();
	return res;
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