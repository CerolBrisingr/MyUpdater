#include "downloader.h"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "server/httplib.h"

#include <gtest/gtest.h>
#include <sstream>
#include <thread>
#include <chrono>
#include <iostream>
#include <future>
#include <format>

using namespace Updater2;
using namespace std::literals::chrono_literals;

class HttpDownloaderTest : public ::testing::Test {
protected:
    httplib::Server svr;
    int port = -1;
    std::string base_url;
    std::thread server_thread;

    void SetUp() override {
        port = svr.bind_to_any_port("127.0.0.1");
        ASSERT_NE(port, -1) << "Failed to bind server to port!";
        base_url = std::format("http://127.0.0.1:{}", port);
    }

    void start_server() {
        server_thread = std::thread([&svr = this->svr]() {
            svr.listen_after_bind();
            });

        const auto timeout = 1s;
        const auto interval = 10ms;
        const auto start = std::chrono::steady_clock::now();

        while (!svr.is_running() && (std::chrono::steady_clock::now() - start < timeout)) {
            std::this_thread::sleep_for(interval);
        }
        ASSERT_TRUE(svr.is_running()) << "Failed to start server within "
            << std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count()
            << ", session timed out!";
    }

    void TearDown() override {
        svr.stop();
        if (server_thread.joinable()) {
            server_thread.join();
        }
    }

};


TEST_F(HttpDownloaderTest, FetchHelloWorld) {
    svr.Get("/hi", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("Hello World!", "text/plain");
        });

    start_server();

    std::string url = std::format("{}/hi", base_url);
    std::stringstream target{};
    Downloader::fetch(target, url.c_str(), 5l);

    EXPECT_EQ("Hello World!", target.str());
}

/* FUTURE SSL Integration:
template <typename ServerType>
class DownloaderTestBase : public ::testing::Test {
protected:
    ServerType svr;

    // Don't forget configuration for SSL
};

using ServerTypes = ::testing::Types<httplib::Server, httplib::SSLServer>;
TYPED_TEST_SUITE(DownloaderTest, ServerTypes);

TYPED_TEST(DownloaderTest, FetchBasicContent) {
    // Within TYPED_TEST we access members via 'this->'
    this->svr.Get("/hi", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("Hello World!", "text/plain");
    });

    this->start_server();

    std::stringstream target;
    // Downloader::fetch should be fine with both, HTTP and HTTPS
    Downloader::fetch(target, (this->base_url + "/hi").c_str(), 5l);

    EXPECT_EQ("Hello World!", target.str());
}
*/