#include <nlohmann/json.hpp>
#include <string>
#include <curl/curl.h>

// Function to place an order
std::string place_order(const std::string& access_token, const std::string& instrument, const std::string& order_type, 
                        const std::string& side, double quantity, double price = 0.0) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    // Initialize CURL
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://test.deribit.com/api/v2/private/buy");

        // JSON payload
        nlohmann::json json_payload = {
            {"jsonrpc", "2.0"},
            {"id", 2},
            {"method", (side == "buy") ? "private/buy" : "private/sell"},
            {"params", {
                {"instrument_name", instrument},
                {"amount", quantity},
                {"type", order_type}
            }}
        };

        if (order_type == "limit") {
            json_payload["params"]["price"] = price;
        }

        std::string payload = json_payload.dump();

        // Authorization header
        std::string auth_header = "Authorization: Bearer " + access_token;

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, auth_header.c_str());

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    return readBuffer;
}
