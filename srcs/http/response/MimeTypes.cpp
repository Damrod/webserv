#include <MimeTypes.hpp>

const MimeTypes::MimeTypesMap_
MimeTypes::kMimeTypes_ = CreateMimeTypesMap_();

const MimeTypes::MimeTypesMap_
MimeTypes::CreateMimeTypesMap_() {
	MimeTypesMap_ mt;

	mt.insert(std::make_pair("html", "text/html"));
	mt.insert(std::make_pair("html", "text/html"));
	mt.insert(std::make_pair("htm", "text/html"));
	mt.insert(std::make_pair("shtml", "text/html"));
	mt.insert(std::make_pair("css", "text/css"));
	mt.insert(std::make_pair("xml", "text/xml"));
	mt.insert(std::make_pair("gif", "image/gif"));
	mt.insert(std::make_pair("jpeg", "image/jpeg"));
	mt.insert(std::make_pair("jpg", "image/jpeg"));
	mt.insert(std::make_pair("js", "application/x-javascript"));
	mt.insert(std::make_pair("atom", "application/atom+xml"));
	mt.insert(std::make_pair("rss", "application/rss+xml"));
	mt.insert(std::make_pair("mml", "text/mathml"));
	mt.insert(std::make_pair("txt", "text/plain"));
	mt.insert(std::make_pair("jad", "text/vnd.sun.j2me.app-descriptor"));
	mt.insert(std::make_pair("wml", "text/vnd.wap.wml"));
	mt.insert(std::make_pair("htc", "text/x-component"));
	mt.insert(std::make_pair("png", "image/png"));
	mt.insert(std::make_pair("tif", "image/tiff"));
	mt.insert(std::make_pair("tiff", "image/tiff"));
	mt.insert(std::make_pair("wbmp", "image/vnd.wap.wbmp"));
	mt.insert(std::make_pair("ico", "image/x-icon"));
	mt.insert(std::make_pair("jng", "image/x-jng"));
	mt.insert(std::make_pair("bmp", "image/x-ms-bmp"));
	mt.insert(std::make_pair("svg", "image/svg+xml"));
	mt.insert(std::make_pair("svgz", "image/svg+xml"));
	mt.insert(std::make_pair("webp", "image/webp"));
	mt.insert(std::make_pair("docx",
	"application/vnd.openxmlformats-officedocument.wordprocessingml.document"));
	mt.insert(std::make_pair("ppsx",
	"application/vnd.openxmlformats-officedocument.presentationml.slideshow"));
	mt.insert(std::make_pair("pptx",
	"application/vnd.openxmlformats-officedocument.presentationml.presentation"
	));
	mt.insert(std::make_pair("xlsx",
	"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
	mt.insert(std::make_pair("jar", "application/java-archive"));
	mt.insert(std::make_pair("war", "application/java-archive"));
	mt.insert(std::make_pair("ear", "application/java-archive"));
	mt.insert(std::make_pair("hqx", "application/mac-binhex40"));
	mt.insert(std::make_pair("doc", "application/msword"));
	mt.insert(std::make_pair("pdf", "application/pdf"));
	mt.insert(std::make_pair("ps", "application/postscript"));
	mt.insert(std::make_pair("eps", "application/postscript"));
	mt.insert(std::make_pair("ai", "application/postscript"));
	mt.insert(std::make_pair("rtf", "application/rtf"));
	mt.insert(std::make_pair("xls", "application/vnd.ms-excel"));
	mt.insert(std::make_pair("ppt", "application/vnd.ms-powerpoint"));
	mt.insert(std::make_pair("wmlc", "application/vnd.wap.wmlc"));
	mt.insert(std::make_pair("kml", "application/vnd.google-earth.kml+xml"));
	mt.insert(std::make_pair("kmz", "application/vnd.google-earth.kmz"));
	mt.insert(std::make_pair("7z", "application/x-7z-compressed"));
	mt.insert(std::make_pair("cco", "application/x-cocoa"));
	mt.insert(std::make_pair("jardiff", "application/x-java-archive-diff"));
	mt.insert(std::make_pair("jnlp", "application/x-java-jnlp-file"));
	mt.insert(std::make_pair("run", "application/x-makeself"));
	mt.insert(std::make_pair("pl", "application/x-perl"));
	mt.insert(std::make_pair("pm", "application/x-perl"));
	mt.insert(std::make_pair("prc", "application/x-pilot"));
	mt.insert(std::make_pair("pdb", "application/x-pilot"));
	mt.insert(std::make_pair("rar", "application/x-rar-compressed"));
	mt.insert(std::make_pair("rpm", "application/x-redhat-package-manager"));
	mt.insert(std::make_pair("sea", "application/x-sea"));
	mt.insert(std::make_pair("swf", "application/x-shockwave-flash"));
	mt.insert(std::make_pair("sit", "application/x-stuffit"));
	mt.insert(std::make_pair("tcl", "application/x-tcl"));
	mt.insert(std::make_pair("tk", "application/x-tcl"));
	mt.insert(std::make_pair("der", "application/x-x509-ca-cert"));
	mt.insert(std::make_pair("pem", "application/x-x509-ca-cert"));
	mt.insert(std::make_pair("crt", "application/x-x509-ca-cert"));
	mt.insert(std::make_pair("xpi", "application/x-xpinstall"));
	mt.insert(std::make_pair("xhtml", "application/xhtml+xml"));
	mt.insert(std::make_pair("zip", "application/zip"));
	mt.insert(std::make_pair("bin", "application/octet-stream"));
	mt.insert(std::make_pair("exe", "application/octet-stream"));
	mt.insert(std::make_pair("dll", "application/octet-stream"));
	mt.insert(std::make_pair("deb", "application/octet-stream"));
	mt.insert(std::make_pair("dmg", "application/octet-stream"));
	mt.insert(std::make_pair("eot", "application/octet-stream"));
	mt.insert(std::make_pair("iso", "application/octet-stream"));
	mt.insert(std::make_pair("img", "application/octet-stream"));
	mt.insert(std::make_pair("msi", "application/octet-stream"));
	mt.insert(std::make_pair("msp", "application/octet-stream"));
	mt.insert(std::make_pair("msm", "application/octet-stream"));
	mt.insert(std::make_pair("mid", "audio/midi"));
	mt.insert(std::make_pair("midi", "audio/midi"));
	mt.insert(std::make_pair("kar", "audio/midi"));
	mt.insert(std::make_pair("mp3", "audio/mpeg"));
	mt.insert(std::make_pair("ogg", "audio/ogg"));
	mt.insert(std::make_pair("ra", "audio/x-realaudio"));
	mt.insert(std::make_pair("m4a", "audio/x-m4a"));
	mt.insert(std::make_pair("3gpp", "video/3gpp"));
	mt.insert(std::make_pair("3gp", "video/3gpp"));
	mt.insert(std::make_pair("mpeg", "video/mpeg"));
	mt.insert(std::make_pair("mpg", "video/mpeg"));
	mt.insert(std::make_pair("mov", "video/quicktime"));
	mt.insert(std::make_pair("flv", "video/x-flv"));
	mt.insert(std::make_pair("mng", "video/x-mng"));
	mt.insert(std::make_pair("asx", "video/x-ms-asf"));
	mt.insert(std::make_pair("asf", "video/x-ms-asf"));
	mt.insert(std::make_pair("wmv", "video/x-ms-wmv"));
	mt.insert(std::make_pair("avi", "video/x-msvideo"));
	mt.insert(std::make_pair("mp4", "video/mp4"));
	mt.insert(std::make_pair("webm", "video/webm"));
	mt.insert(std::make_pair("m4v", "video/x-m4v"));
	return mt;
}

std::string	MimeTypes::GetMimeType(const std::string &extension) {
	MimeTypesMap_::const_iterator	it = kMimeTypes_.find(extension);

	if (it != kMimeTypes_.end())
		return it->second;
	return "application/octet-stream";
}
