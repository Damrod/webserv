#ifndef SRCS_INCS_HTTPGETRESPONSE_HPP_
#define SRCS_INCS_HTTPGETRESPONSE_HPP_

#include <string>
#include <sys/stat.h>
#include <ctime>
#include <CGI.hpp>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <iostream>

#include <MimeTypes.hpp>
#include <IResponse.hpp>
#include <AHttpResponse.hpp>
#include <HttpRequest.hpp>
#include <RequestConfig.hpp>
#include <File.hpp>

class HttpGetResponse: public IResponse {
	public:
		HttpGetResponse(
			RequestConfig *requestConfig,
			HttpRequest *request);
		std::string content();

	private:
		// common
		// Llevar a Factory?
		void	CheckKeepAlive_();
		bool	HasAcceptedFormat_(const HttpRequest &request);

		// extrapolable
		void	ExecuteCGI_(const HttpRequest &request, File file); // AddCommonHeaders_, PathExtension_, RequestError_

		// Error
		// La idea sería que esta lógica estuviera en su propia req. Si se detecta error,
		// entonces se construye un objeto error que devuelve una respuesta raw.
		void	RequestError_(const std::size_t error_code); // DefaultStatusReponse_, ServeFile_
		void	DefaultStatusResponse_(const std::size_t status_code); //AddCommonHeaders_

		//  Files
		// La idea sería sacar esto a un objeto externo responsable de abrir ficheros y volcar contenido
		// Si no devuelve nada, entonces lanza excepción, entonces ha habido error y se evalúa el errno
		void	Serve_(File file); //IsRegularFile_, RequestError_, PathError_, AddCommonHeaders_, GetMimeType_


		// unique -> deps externas: base (AddCommonHeaders_, DefaultResponseHeader_)
		void	MovedPermanently_(const HttpRequest &request); // AddCommonHeaders_, DefaultResponseHeader_
		void	ListDirectory_(File file, const std::string &request_path); // AddCommonHeaders_, TryAddDirectoryContent

		RequestConfig *requestConfig_;
		HttpRequest *request_;
		bool	keep_alive_;
		std::size_t errCode;
		std::string raw_response_;
};

#endif  // SRCS_INCS_HTTPGETRESPONSE_HPP_
