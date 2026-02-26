#include <ostream>
#include <string>
#include <format>
#include <stdint.h>
#include <memory>

#include <curl/curl.h>

namespace Updater2::Downloader {

    namespace {
        struct ReceptionContainer {
            std::ostream& target;
            bool success{ true };
            std::string error{ "No error" };

            ReceptionContainer(std::ostream& t)
                :target{ t }
            {}
        };

        class CurlContext {
        public:
            CurlContext() 
                : g_result{ curl_global_init(CURL_GLOBAL_ALL) }
            {}
            ~CurlContext() {
                if (g_result == CURLE_OK) {
                    /* we are done with libcurl, so clean it up */
                    curl_global_cleanup();
                }
            }
            CURLcode result() const {
                return g_result;
            }
        private:
            CURLcode g_result{};
        };

        inline CurlContext& getContext() {
            static CurlContext context{};
            return context;
        }

        inline size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
        {
            auto* container{ static_cast<ReceptionContainer*>(userp) };
            std::size_t realsize{ size * nmemb };

            if (realsize == 0) return 0;

            try {
                container->target.write(static_cast<const char*>(contents), realsize); 
                if (!container->target) {
                    container->success = false;
                    container->error = "Stream state is bad (e.g. disk full)";
                    return 0; // Abort transmission
                }
            }
            catch (const std::exception& e) {
                container->success = false;
                container->error = std::format("Failed to write to target: {}", e.what());
                return 0;
            }
            catch (...) {
                container->success = false;
                container->error = "Unhandled exception";
                return 0;
            }

            return realsize;
        }

        inline void verifySetting(CURLcode returnCode) {
            if (returnCode != CURLE_OK) {
                throw std::runtime_error("Failed to set curl option");
            }
        }

        struct CurlDeleter {
            void operator()(CURL* curl) const {
                if (curl) {
                    curl_easy_cleanup(curl);
                }
            }
        };

        using CurlHandle = std::unique_ptr<CURL, CurlDeleter>; // Resource Management in case of throw
    } // namespace

    inline void fetch(std::ostream& target, const char* address = "https://www.example.com")
    {
        ReceptionContainer container{ target };
        auto& context{ getContext() };
        if (context.result() != CURLE_OK)
            throw std::runtime_error("Failed to init CURL");

        /* init the curl session */
        CurlHandle curl{ curl_easy_init() };
        if (!curl) {
            throw std::runtime_error("Failed to init CURL communication");
        }
        CURL* hCurl{ curl.get() };

        /* specify URL to get */
        verifySetting(curl_easy_setopt(hCurl, CURLOPT_URL, address));

        /* send all data to this function  */
        verifySetting(curl_easy_setopt(hCurl, CURLOPT_WRITEFUNCTION, &writeCallback));

        /* we pass our 'chunk' struct to the callback function */
        verifySetting(curl_easy_setopt(hCurl, CURLOPT_WRITEDATA, static_cast<void*>(&container)));

        /* some servers do not like requests that are made without a user-agent
            field, so we provide one */
        verifySetting(curl_easy_setopt(hCurl, CURLOPT_USERAGENT, "libcurl-agent/1.0"));

        /* Don't send error messages to target */
        verifySetting(curl_easy_setopt(hCurl, CURLOPT_FAILONERROR, 1L));

        /* get it! */
        CURLcode result = curl_easy_perform(hCurl);

        /* check for errors */
        if (!container.success) { // We know more in this case
            throw std::runtime_error(std::format("Write callback encountered an error: {}", container.error));
        }
        if (result != CURLE_OK) { // This error should be curl-specific 
            throw std::runtime_error(std::format("curl_easy_perform() failed: {}", curl_easy_strerror(result)));
        }
    }

} // namespace Updater2::Downloader