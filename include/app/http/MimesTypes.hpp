/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MimesTypes.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 15:13:14 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/18 12:03:30 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <utility>
#include <vector>
#define _MIME_EXT_TO_HEADER(MIME, EXTENSION) std::make_pair(EXTENSION, MIME),
#define _MIME_HEADER_TO_EXT(MIME, EXTENSION) std::make_pair(MIME, EXTENSION),

#define _MIME(TY, MIME, EXTENSION)			 _MIME_##TY(MIME, EXTENSION)

#define _MIME_ALL(TY)                                                                              \
	_MIME(TY, "application/atom+xml", "atom")                                                      \
	_MIME(TY, "application/java-archive", "ear")                                                   \
	_MIME(TY, "application/java-archive", "jar")                                                   \
	_MIME(TY, "application/java-archive", "war")                                                   \
	_MIME(TY, "application/javascript", "js")                                                      \
	_MIME(TY, "application/json", "json")                                                          \
	_MIME(TY, "application/mac-binhex40", "hqx")                                                   \
	_MIME(TY, "application/msword", "doc")                                                         \
	_MIME(TY, "application/octet-stream", "bin")                                                   \
	_MIME(TY, "application/octet-stream", "deb")                                                   \
	_MIME(TY, "application/octet-stream", "dll")                                                   \
	_MIME(TY, "application/octet-stream", "dmg")                                                   \
	_MIME(TY, "application/octet-stream", "exe")                                                   \
	_MIME(TY, "application/octet-stream", "img")                                                   \
	_MIME(TY, "application/octet-stream", "iso")                                                   \
	_MIME(TY, "application/octet-stream", "msi")                                                   \
	_MIME(TY, "application/octet-stream", "msm")                                                   \
	_MIME(TY, "application/octet-stream", "msp")                                                   \
	_MIME(TY, "application/pdf", "pdf")                                                            \
	_MIME(TY, "application/postscript", "ai")                                                      \
	_MIME(TY, "application/postscript", "eps")                                                     \
	_MIME(TY, "application/postscript", "ps")                                                      \
	_MIME(TY, "application/rss+xml", "rss")                                                        \
	_MIME(TY, "application/rtf", "rtf")                                                            \
	_MIME(TY, "application/vnd.apple.mpegurl", "m3u8")                                             \
	_MIME(TY, "application/vnd.google-earth.kml+xml", "kml")                                       \
	_MIME(TY, "application/vnd.google-earth.kmz", "kmz")                                           \
	_MIME(TY, "application/vnd.ms-excel", "xls")                                                   \
	_MIME(TY, "application/vnd.ms-fontobject", "eot")                                              \
	_MIME(TY, "application/vnd.ms-powerpoint", "ppt")                                              \
	_MIME(TY, "application/vnd.oasis.opendocument.graphics", "odg")                                \
	_MIME(TY, "application/vnd.oasis.opendocument.presentation", "odp")                            \
	_MIME(TY, "application/vnd.oasis.opendocument.spreadsheet", "ods")                             \
	_MIME(TY, "application/vnd.oasis.opendocument.text", "odt")                                    \
	_MIME(TY, "application/vnd.openxmlformats-officedocument.presentationml.presentation", "pptx") \
	_MIME(TY, "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", "xlsx")         \
	_MIME(TY, "application/vnd.openxmlformats-officedocument.wordprocessingml.document", "docx")   \
	_MIME(TY, "application/vnd.wap.wmlc", "wmlc")                                                  \
	_MIME(TY, "application/wasm", "wasm")                                                          \
	_MIME(TY, "application/x-7z-compressed", "7z")                                                 \
	_MIME(TY, "application/x-cocoa", "cco")                                                        \
	_MIME(TY, "application/x-java-archive-diff", "jardiff")                                        \
	_MIME(TY, "application/x-java-jnlp-file", "jnlp")                                              \
	_MIME(TY, "application/x-makeself", "run")                                                     \
	_MIME(TY, "application/x-perl", "pl")                                                          \
	_MIME(TY, "application/x-perl", "pm")                                                          \
	_MIME(TY, "application/x-pilot", "pdb")                                                        \
	_MIME(TY, "application/x-pilot", "prc")                                                        \
	_MIME(TY, "application/x-rar-compressed", "rar")                                               \
	_MIME(TY, "application/x-redhat-package-manager", "rpm")                                       \
	_MIME(TY, "application/x-sea", "sea")                                                          \
	_MIME(TY, "application/x-shockwave-flash", "swf")                                              \
	_MIME(TY, "application/x-stuffit", "sit")                                                      \
	_MIME(TY, "application/x-tcl", "tcl")                                                          \
	_MIME(TY, "application/x-tcl", "tk")                                                           \
	_MIME(TY, "application/x-x509-ca-cert", "crt")                                                 \
	_MIME(TY, "application/x-x509-ca-cert", "crt")                                                 \
	_MIME(TY, "application/x-x509-ca-cert", "pem")                                                 \
	_MIME(TY, "application/x-xpinstall", "xpi")                                                    \
	_MIME(TY, "application/xhtml+xml", "xhtml")                                                    \
	_MIME(TY, "application/xspf+xml", "xspf")                                                      \
	_MIME(TY, "application/zip", "zip")                                                            \
	_MIME(TY, "audio/midi", "kar")                                                                 \
	_MIME(TY, "audio/midi", "mid")                                                                 \
	_MIME(TY, "audio/midi", "midi")                                                                \
	_MIME(TY, "audio/mpeg", "mp3")                                                                 \
	_MIME(TY, "audio/ogg", "ogg")                                                                  \
	_MIME(TY, "audio/x-m4a", "m4a")                                                                \
	_MIME(TY, "audio/x-realaudio", "ra")                                                           \
	_MIME(TY, "font/woff", "woff")                                                                 \
	_MIME(TY, "font/woff2", "woff2")                                                               \
	_MIME(TY, "image/avif", "avif")                                                                \
	_MIME(TY, "image/gif", "gif")                                                                  \
	_MIME(TY, "image/jpeg", "jpeg")                                                                \
	_MIME(TY, "image/jpeg", "jpg")                                                                 \
	_MIME(TY, "image/png", "png")                                                                  \
	_MIME(TY, "image/svg+xml", "svg")                                                              \
	_MIME(TY, "image/svg+xml", "svgz")                                                             \
	_MIME(TY, "image/tiff", "tif")                                                                 \
	_MIME(TY, "image/tiff", "tiff")                                                                \
	_MIME(TY, "image/vnd.wap.wbmp", "wbmp")                                                        \
	_MIME(TY, "image/webp", "webp")                                                                \
	_MIME(TY, "image/x-icon", "ico")                                                               \
	_MIME(TY, "image/x-jng", "jng")                                                                \
	_MIME(TY, "image/x-ms-bmp", "bmp")                                                             \
	_MIME(TY, "text/css", "css")                                                                   \
	_MIME(TY, "text/html", "htm")                                                                  \
	_MIME(TY, "text/html", "html")                                                                 \
	_MIME(TY, "text/html", "shtml")                                                                \
	_MIME(TY, "text/mathml", "mml")                                                                \
	_MIME(TY, "text/plain", "txt")                                                                 \
	_MIME(TY, "text/plain", "cpp")                                                                 \
	_MIME(TY, "text/plain", "hpp")                                                                 \
	_MIME(TY, "text/plain", "c")                                                                   \
	_MIME(TY, "text/plain", "h")                                                                   \
	_MIME(TY, "text/plain", "mk")                                                                  \
	_MIME(TY, "text/plain", "py")                                                                  \
	_MIME(TY, "text/plain", "md")                                                                  \
	_MIME(TY, "text/plain", "toml")                                                                \
	_MIME(TY, "text/vnd.sun.j2me.app-descriptor", "jad")                                           \
	_MIME(TY, "text/vnd.wap.wml", "wml")                                                           \
	_MIME(TY, "text/x-component", "htc")                                                           \
	_MIME(TY, "text/xml", "xml")                                                                   \
	_MIME(TY, "video/3gpp", "3gp")                                                                 \
	_MIME(TY, "video/3gpp", "3gpp")                                                                \
	_MIME(TY, "video/mp2t", "ts")                                                                  \
	_MIME(TY, "video/mp4", "mp4")                                                                  \
	_MIME(TY, "video/mpeg", "mpeg")                                                                \
	_MIME(TY, "video/mpeg", "mpg")                                                                 \
	_MIME(TY, "video/quicktime", "mov")                                                            \
	_MIME(TY, "video/webm", "webm")                                                                \
	_MIME(TY, "video/x-flv", "flv")                                                                \
	_MIME(TY, "video/x-m4v", "m4v")                                                                \
	_MIME(TY, "video/x-mng", "mng")                                                                \
	_MIME(TY, "video/x-ms-asf", "asf")                                                             \
	_MIME(TY, "video/x-ms-asf", "asx")                                                             \
	_MIME(TY, "video/x-ms-wmv", "wmv")                                                             \
	_MIME(TY, "video/x-msvideo", "avi")

namespace mime {
	namespace inner {
		static inline std::map<std::string, std::vector<std::string> > _tab_pair_map_vec(
			const std::pair<std::string, std::string>* pairs,
			std::size_t								   len) {
			std::map<std::string, std::vector<std::string> > out;

			for (; len--; pairs++) {
				if (out.count(pairs->first) == 0)
					out[pairs->first] = std::vector<std::string>();
				out[pairs->first].push_back(pairs->second);
			}
			return out;
		}

		static const std::pair<std::string, std::string> ext_to_header_inner[] = {
			_MIME_ALL(EXT_TO_HEADER)};
		static const std::pair<std::string, std::string> header_to_ext_inner[] = {
			_MIME_ALL(HEADER_TO_EXT)};

	}	// namespace inner

	static const std::map<std::string, std::string> EXT_TO_HEADER(
		&inner::ext_to_header_inner[0],
		&inner::ext_to_header_inner[0] +
			sizeof(inner::ext_to_header_inner) / sizeof(inner::ext_to_header_inner[0]));

	static const std::map<std::string, std::vector<std::string> > HEADER_TO_EXT =
		inner::_tab_pair_map_vec(
			inner::header_to_ext_inner,
			sizeof(inner::header_to_ext_inner) / sizeof(inner::header_to_ext_inner[0]));

	class MimeType {
		private:
			std::string mimetype;

		public:
			MimeType() : mimetype("application/octet-stream") {}
			~MimeType() {};
			MimeType(const MimeType& rhs) : mimetype(rhs.mimetype) {}
			MimeType& operator=(const MimeType& rhs) {
				if (this != &rhs) {
					this->mimetype = rhs.mimetype;
				}
				return (*this);
			}

			static MimeType from_extension(const std::string& ext) {
				MimeType out;
				if (EXT_TO_HEADER.count(ext) == 0)
					out.mimetype = "application/octet-stream";
				else
					out.mimetype = EXT_TO_HEADER.at(ext);
				return out;
			}

			const std::string& getInner() const { return (this->mimetype); }
	};
}	// namespace mime

#undef _MIME
#undef _MIME_ALL
#undef _MIME_HEADER_TO_EXT
#undef _MIME_EXT_TO_HEADER
