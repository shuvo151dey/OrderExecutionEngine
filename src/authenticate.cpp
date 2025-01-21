#include <iostream>
#include <string>
#include <cstdlib> // For getenv
#include <curl/curl.h>
#include <nlohmann/json.hpp>

// Function to handle the response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
    } catch (std::bad_alloc& e) {
        // Handle memory problem
        return 0;
    }
    return newLength;
}

std::string authenticate() {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    // Retrieve the client ID and client secret from environment variables
    const char* client_id = std::getenv("DERIBIT_CLIENT_ID");
    const char* client_secret = std::getenv("DERIBIT_CLIENT_SECRET");

    if (!client_id || !client_secret) {
        std::cerr << "Environment variables DERIBIT_CLIENT_ID or DERIBIT_CLIENT_SECRET are not set!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Initialize CURL
    curl = curl_easy_init();
    if (curl) {
        // Set the URL for the authentication endpoint
        curl_easy_setopt(curl, CURLOPT_URL, "https://test.deribit.com/api/v2/public/auth");

        // Prepare the JSON-RPC request payload
        nlohmann::json json_payload = {
            {"jsonrpc", "2.0"},
            {"id", 1},
            {"method", "public/auth"},
            {"params", {
                {"client_id", client_id},
                {"client_secret", client_secret},
                {"grant_type", "client_credentials"}
            }}
        };

        std::string payload = json_payload.dump();

        // Set CURL options
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);
            exit(EXIT_FAILURE);
        }

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    // Parse the response to extract the access token
    auto json_response = nlohmann::json::parse(readBuffer);
    std::string access_token = json_response["result"]["access_token"];
    return access_token;
}

