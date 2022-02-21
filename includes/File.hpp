#pragma once

#include <iostream>
#include <string>
#include <map>
#include "utils.hpp"

class File
{
    std::map<std::string, std::string> types;
    public:
    File() {
        types["gif"] = "image/gif";
        types["png"] = "image/png";
        types["jpg"] = "image/jpg";
        types["css"] = "text/css";
        types["js"] = "text/javascript";
        types["py"] = "text/x-python-script";
        types["html"] = "text/html";
        types["xml"] = "application/xml";
        types["avif"] = "image/avif";
        types["webp"] = "image/webp";
        types["apng"] = "image/apng";
        types["ai"] = "application/postscript";
        types["aif"] = "audio/x-aiff";
        types["aifc"] = "audio/x-aiff";
        types["aiff"] = "audio/x-aiff";
        types["asc"] = "text/plain";
        types["atom"] = "application/atom+xml";
        types["au"] = "audio/basic";
        types["avi"] = "video/x-msvideo";
        types["bcpio"] = "application/x-bcpio";
        types["bin"] = "application/octet-stream";
        types["bmp"] = "image/bmp";
        types["cdf"] = "application/x-netcdf";
        types["cgm"] = "image/cgm";
        types["class"] = "application/octet-stream";
        types["cpio"] = "application/x-cpio";
        types["cpt"] = "application/mac-compactpro";
        types["csh"] = "application/x-csh";
        types["dcr"] = "application/x-director";
        types["dif"] = "video/x-dv";
        types["dir"] = "application/x-director";
        types["djv"] = "image/vnd.djvu";
        types["djvu"] = "image/vnd.djvu";
        types["dll"] = "application/octet-stream";
        types["dmg"] = "application/octet-stream";
        types["dms"] = "application/octet-stream";
        types["doc"] = "application/msword";
        types["docx"] = "application/msword";
        types["dtd"] = "application/xml-dtd";
        types["dv"] = "video/x-dv";
        types["dvi"] = "application/x-dvi";
        types["dxr"] = "application/x-director";
        types["eps"] = "application/postscript";
        types["etx"] = "text/x-setext";
        types["exe"] = "application/octet-stream";
        types["ez"] = "application/andrew-inset";
        types["gif"] = "image/gif";
        types["gram"] = "application/srgs";
        types["grxml"] = "application/srgs+xml";
        types["gtar"] = "application/x-gtar";
        types["hdf"] = "application/x-hdf";
        types["hqx"] = "application/mac-binhex40";
        types["htm"] = "text/html";
        types["ice"] = "x-conference/x-cooltalk";
        types["ico"] = "image/x-icon";
        types["ics"] = "text/calendar";
        types["ief"] = "image/ief";
        types["ifb"] = "text/calendar";
        types["iges"] = "model/iges";
        types["igs"] = "model/iges";
        types["jnlp"] = "application/x-java-jnlp-file";
        types["jp2"] = "image/jp2";
        types["jpe"] = "image/jpeg";
        types["jpeg"] = "image/jpeg";
        types["kar"] = "audio/midi";
        types["latex"] = "application/x-latex";
        types["lha"] = "application/octet-stream";
        types["lzh"] = "application/octet-stream";
        types["m3u"] = "audio/x-mpegurl";
        types["m4a"] = "audio/mp4a-latm";
        types["m4b"] = "audio/mp4a-latm";
        types["m4p"] = "audio/mp4a-latm";
        types["m4u"] = "video/vnd.mpegurl";
        types["m4v"] = "video/x-m4v";
        types["mac"] = "image/x-macpaint";
        types["man"] = "application/x-troff-man";
        types["mathml"] = "application/mathml+xml";
        types["me"] = "application/x-troff-me";
        types["mesh"] = "model/mesh";
        types["mid"] = "audio/midi";
        types["midi"] = "audio/midi";
        types["mif"] = "application/vnd.mif";
        types["mov"] = "video/quicktime";
        types["movie"] = "video/x-sgi-movie";
        types["mp2"] = "audio/mpeg";
        types["mp3"] = "audio/mpeg";
        types["mp4"] = "video/mp4";
        types["mpe"] = "video/mpeg";
        types["mpeg"] = "video/mpeg";
        types["mpg"] = "video/mpeg";
        types["mpga"] = "audio/mpeg";
        types["ms"] = "application/x-troff-ms";
        types["msh"] = "model/mesh";
        types["mxu"] = "video/vnd.mpegurl";
        types["nc"] = "application/x-netcdf"; 
        types["oda"] = "application/oda";
        types["ogg"] = "application/ogg";
        types["pbm"] = "image/x-portable-bitmap";
        types["pct"] = "image/pict";
        types["pdb"] = "chemical/x-pdb";
        types["pdf"] = "application/pdf";
        types["pgm"] = "image/x-portable-graymap";
        types["pgn"] = "application/x-chess-pgn";
        types["pic"] = "image/pict";
        types["pict"] = "image/pict";
        types["pnm"] = "image/x-portable-anymap";
        types["pnt"] = "image/x-macpaint";
        types["pntg"] = "image/x-macpaint";
        types["ppm"] = "image/x-portable-pixmap";
        types["ppt"] = "application/vnd.ms-powerpoint";
        types["ps"] = "application/postscript";
        types["qt"] = "video/quicktime";
        types["qti"] = "image/x-quicktime";
        types["qtif"] = "image/x-quicktime";
        types["ra"] = "audio/x-pn-realaudio";
        types["ram"] = "audio/x-pn-realaudio";
        types["ras"] = "image/x-cmu-raster";
        types["rdf"] = "application/rdf+xml";
        types["rgb"] = "image/x-rgb";
        types["rm"] = "application/vnd.rn-realmedia";
        types["roff"] = "application/x-troff";
        types["rtf"] = "text/rtf";
        types["rtx"] = "text/richtext";
        types["sgm"] = "text/sgml";
        types["sgml"] = "text/sgml";
        types["sh"] = "application/x-sh";
        types["shar"] = "application/x-shar";
        types["silo"] = "model/mesh";
        types["sit"] = "application/x-stuffit";
        types["skd"] = "application/x-koan";
        types["skm"] = "application/x-koan";
        types["skp"] = "application/x-koan";
        types["skt"] = "application/x-koan";
        types["smi"] = "application/smil";
        types["smil"] = "application/smil";
        types["snd"] = "audio/basic";
        types["so"] = "application/octet-stream";
        types["spl"] = "application/x-futuresplash";
        types["src"] = "application/x-wais-source";
        types["sv4cpio"] = "application/x-sv4cpio";
        types["sv4crc"] = "application/x-sv4crc";
        types["svg"] = "image/svg+xml";
        types["swf"] = "application/x-shockwave-flash";
        types["t"] = "application/x-troff";
        types["tar"] = "application/x-tar";
        types["tcl"] = "application/x-tcl";
        types["tex"] = "application/x-tex";
        types["texi"] = "application/x-texinfo";
        types["texinfo"] = "application/x-texinfo";
        types["tif"] = "image/tiff";
        types["tiff"] = "image/tiff";
        types["tr"] = "application/x-troff";
        types["tsv"] = "text/tab-separated-values";
        types["txt"] = "text/plain";
        types["ustar"] = "application/x-ustar";
        types["vcd"] = "application/x-cdlink";
        types["vrml"] = "model/vrml";
        types["vxml"] = "application/voicexml+xml";
        types["wav"] = "audio/x-wav";
        types["wbmp"] = "image/vnd.wap.wbmp";
        types["wbmxl"] = "application/vnd.wap.wbxml";
        types["wml"] = "text/vnd.wap.wml";
        types["wmlc"] = "application/vnd.wap.wmlc";
        types["wmls"] = "text/vnd.wap.wmlscript";
        types["wmlsc"] = "application/vnd.wap.wmlscriptc";
        types["wrl"] = "model/vrml";
        types["xbm"] = "image/x-xbitmap";
        types["xht"] = "application/xhtml+xml";
        types["xhtml"] = "application/xhtml+xml";
        types["xls"] = "application/vnd.ms-excel";
        types["xlsx"] = "application/vnd.ms-excel";
        types["xml"] = "application/xml";
        types["xpm"] = "image/x-xpixmap";
        types["xsl"] = "application/xml";
        types["xslt"] = "application/xslt+xml";
        types["xul"] = "application/vnd.mozilla.xul+xml";
        types["xwd"] = "image/x-xwindowdump";
        types["xyz"] = "chemical/x-xyz";
        types["zip"] = "application/zip";
    }
    
    ~File() {}
    
    void add_type(const std::string &filename, const std::string &type) {
        size_t pos = filename.find_last_of(".");
        if (pos == std::string::npos)
            return ;
        std::string extention = filename.substr(pos + 1);
        std::map<std::string, std::string>::iterator it = types.find(extention);
        if (it == types.end())
            types[extention] = type;
    }

    std::string get_file_type(const std::string &filename) {
        size_t pos = filename.find_last_of(".");
        if (pos == std::string::npos)
            return "application/unknown";
        std::string extention = filename.substr(pos + 1);
        std::map<std::string, std::string>::iterator it = types.find(extention);
        if (it != types.end())
            return it->second;
        return "application/unknown";
    }
};