#include "../header/MIME.hpp"

MIME::~MIME()
{};

const string MIME::KEY = "Content-Type: ";
const string MIME::HTML = "text/html";
const string MIME::CSS = "text/css";
const string MIME::XML = "text/xml";
const string MIME::GIF = "image/gif";
const string MIME::JPEG = "image/jpeg";
const string MIME::JS = "application/javascript";
const string MIME::ATOMXML = "application/atom+xml";
const string MIME::RSSXML = "application/rss+xml";
const string MIME::MATHML = "text/mathml";
const string MIME::TXT = "text/plain";
// image
const string MIME::AVIF = "image/avif";
const string MIME::PNG = "image/png";
const string MIME::SVGXML = "image/svg+xml";
const string MIME::TIFF = "image/tiff";
const string MIME::WEBP = "image/webp";
const string MIME::ICO = "image/x-icon";
const string MIME::JNG = "image/x-jng";
const string MIME::BMP = "image/x-ms-bmp";
// Application
const string MIME::JSON = "application/json";
const string MIME::PDF = "application/pdf";
const string MIME::RTF = "application/rtf";
const string MIME::DOC = "application/msword";
const string MIME::JAR = "application/java-archive";
const string MIME::Z7 = "application/x-7z-compressed";
const string MIME::RAR = "application/x-rar-compressed";
const string MIME::ZIP = "application/zip";
const string MIME::BIN = "application/octet-stream";
// audio
const string MIME::MIDI = "audio/midi";
const string MIME::MPEG = "audio/mpeg";
const string MIME::OGG = "audio/ogg";
const string MIME::M4A = "audio/x-m4a";
const string MIME::REALAUDIO = "audio/x-realaudio";
// video
const string MIME::GPP = "video/3gpp";
const string MIME::MP2T = "video/mp2t";
const string MIME::MP4 = "video/mp4";
const string MIME::MPEGV = "video/mpeg";
const string MIME::QUICKTIME = "video/quicktime";
const string MIME::WEBM = "video/webm";
const string MIME::FLV = "video/x-flv";
const string MIME::M4V = "video/x-m4v";
const string MIME::MNG = "video/x-mng";
const string MIME::ASF = "video/x-ms-asf";
const string MIME::WMV = "video/x-ms-wmv";
const string MIME::AVI = "video/x-msvideo";