#ifndef SRCS_INCS_HTTPPOSTRESPONSE_HPP_
#define SRCS_INCS_HTTPPOSTRESPONSE_HPP_

#include <string>
#include <sys/stat.h>
#include <ctime>
#include <CGI.hpp>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <iostream>
#include <FormFile.hpp>
#include <IResponse.hpp>
#include <MimeTypes.hpp>
#include <HttpRequest.hpp>
#include <RequestConfig.hpp>
#include <HttpResponse.hpp>
#include <HttpErrorResponse.hpp>
#include <File.hpp>
#include <AHttpResponse.hpp>
#include <HttpBaseResponse.hpp>


class HttpPostResponse: public HttpBaseResponse {
	public:
		HttpPostResponse(
			RequestConfig *requestConfig,
			HttpRequest *request);

	private:
		bool	IsUploadEnabled_() const;
		bool	IsValidUploadPath_(const std::string &path) const;
		void	SetErrorRawResponse_(int error_code);
		void	Upload_(File file);
		void	HandleCGI_(File file);
		void	HandleUpload_(File file);
};

#endif  // SRCS_INCS_HTTPPOSTRESPONSE_HPP_

// NOTAS, siguientes pasos
// - Crear objetos con métodos comunes. Objetos que encapsulen cierta funcionalidad (acordarse de método de error)
// - Establecer comportamiento para cada uno de los métodos
// - Empalmar con Connection
// - Arreglar tests
