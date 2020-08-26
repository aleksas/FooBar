#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <string>

#include <curl/curl.h>
#include <json/json.h>

size_t append_string(void* buffer, size_t element_size, size_t element_count, std::string* str) {
	if (element_size == 1) {
		str->append((char*)buffer, element_count);
		return element_count;
	} else {
		return 0;
	}
}

bool DownloadStringData(std::string& url, std::string & string_data) {
	CURL* curl;
	CURLcode res;

	res = curl_global_init(CURL_GLOBAL_DEFAULT);

	if (CURLE_OK == res) {
		curl = curl_easy_init();

		if (curl) {
			string_data = "";

			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, append_string);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &string_data);
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

			res = curl_easy_perform(curl);

			curl_easy_cleanup(curl);

			if (CURLE_OK != res)
				std::cout << "curl told us " << res << std::endl;
		}

		curl_global_cleanup();
	}

	return CURLE_OK == res;
}

bool ParseJson(std::string& string_data, Json::Value& root, JSONCPP_STRING& errs) {
	std::stringstream json_stream(string_data);

	Json::CharReaderBuilder builder;
	builder["collectComments"] = true;

	return parseFromStream(builder, json_stream, &root, &errs);
}

int main() {
	std::string url = "https://data.hawaii.gov/api/views/usep-nua7/rows.json?accessType=DOWNLOAD";
	std::string json_data;
	Json::Value root;
	JSONCPP_STRING errs;

	if (DownloadStringData(url, json_data)) {
		if (ParseJson(json_data, root, errs)) {
			std::cout << root << std::endl;
		} else {
			std::cout << "Failed to parse json. " << errs << std::endl;
		}
	} else {
		std::cout << "Failed to download string data." << std::endl;
	}
}
