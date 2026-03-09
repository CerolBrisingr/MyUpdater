#include "downloader.h"
#include "certificate_handler.h"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "server/httplib.h"

#include <gtest/gtest.h>
#include <sstream>
#include <thread>
#include <chrono>
#include <iostream>
#include <future>
#include <format>
#include <filesystem>
#include <memory>
#include <algorithm>

using namespace Updater2;
using namespace std::literals::chrono_literals;

namespace {
    inline const Certificates::Handler& getCertificateHandler() {
        static const Certificates::Handler certificateHandler(OPEN_SSL, CA_CONFIG, SERVER_CONFIG);
        return certificateHandler;
    }
    constexpr std::size_t DATA_CHUNK_SIZE = 4;
}

TEST(httpsdownloader, certificates) {
    ASSERT_NO_THROW(getCertificateHandler());
}

std::string setUpServer(std::unique_ptr<httplib::Server>& srv) {
    srv.reset(new httplib::Server());
    return "http";
}

std::string setUpServer(std::unique_ptr<httplib::SSLServer>& srv) {
    auto& certs = getCertificateHandler();
    Downloader::CustomCA::setPath(certs.ca().cert());
    std::string key = certs.server().key();
    std::string cert = certs.server().cert();
    srv.reset(new httplib::SSLServer(cert.c_str(), key.c_str()));
    return "https";

}
std::string setUpInvalidServer(std::unique_ptr<httplib::SSLServer>& srv) {
    auto& certs = getCertificateHandler();
    Downloader::CustomCA::setPath(certs.ca().cert());
    std::string key = certs.timeoutServer().key();
    std::string cert = certs.timeoutServer().cert();
    srv.reset(new httplib::SSLServer(cert.c_str(), key.c_str()));
    return "https";
}

template <typename ServerType>
class HttpDownloaderTest : public ::testing::Test {
protected:
    std::unique_ptr<ServerType> svr;
    int port = -1;
    std::string base_url;
    std::thread server_thread;

    static void SetupStreamRoute(std::unique_ptr<ServerType>& svr, bool succeed = true) {
        svr->Get("/stream", [succeed](const httplib::Request& req, httplib::Response& res) {
            static const std::string data{ "abcdefg" };

            res.set_content_provider(
                data.size(), // Content length
                "text/plain", // Content type
                [succeed](std::size_t offset, std::size_t length, httplib::DataSink& sink) {
                    const auto& d = data;
                    sink.write(&d[offset], (std::min)(length, DATA_CHUNK_SIZE));
                    return succeed; // return 'false' if you want to cancel the process.
                },
                [](bool success) {});
            });
    }

    void setConnection(std::string_view prefix) {
        port = svr->bind_to_any_port("127.0.0.1");
        ERR_print_errors_fp(stderr);
        ASSERT_NE(port, -1) << "Failed to bind server to port!";
        base_url = std::format("{}://127.0.0.1:{}", prefix, port);
    }

    void startServer() {
        server_thread = std::thread([&svr = this->svr]() {
            svr->listen_after_bind();
            });

        const auto timeout = 1s;
        const auto interval = 10ms;
        const auto start = std::chrono::steady_clock::now();

        while (!svr->is_running() && (std::chrono::steady_clock::now() - start < timeout)) {
            std::this_thread::sleep_for(interval);
        }
        ASSERT_TRUE(svr->is_running()) << "Failed to start server within "
            << std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count()
            << ", session timed out!";
    }

    void TearDown() override {
        if (svr) { svr->stop(); }
        if (server_thread.joinable()) {
            server_thread.join();
        }
        Downloader::CustomCA::reset();
    }

};

using MyServerTypes = ::testing::Types<httplib::Server, httplib::SSLServer>;
TYPED_TEST_SUITE(HttpDownloaderTest, MyServerTypes);


TYPED_TEST(HttpDownloaderTest, FetchHelloWorld) {

    std::string prefix = setUpServer(this->svr);
    this->setConnection(prefix);

    this->svr->Get("/hi", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("Hello World!", "text/plain");
        });

    this->startServer();

    std::string url = std::format("{}/hi", this->base_url);
    std::stringstream target{};
    Downloader::fetch(target, url.c_str(), 5l);

    EXPECT_EQ("Hello World!", target.str());
}

TYPED_TEST(HttpDownloaderTest, FetchStream) {

    std::string prefix = setUpServer(this->svr);
    this->setConnection(prefix);

    HttpDownloaderTest<TypeParam>::SetupStreamRoute(this->svr);
    this->startServer();

    std::string url = std::format("{}/stream", this->base_url);
    std::stringstream target{};
    Downloader::fetch(target, url.c_str(), 5l);

    EXPECT_EQ("abcdefg", target.str());
}

TYPED_TEST(HttpDownloaderTest, StreamFail) {

    std::string prefix = setUpServer(this->svr);
    this->setConnection(prefix);

    HttpDownloaderTest<TypeParam>::SetupStreamRoute(this->svr, false);
    this->startServer();

    std::string url = std::format("{}/stream", this->base_url);
    std::stringstream target{};

    // We expect an error, but go on
    EXPECT_ANY_THROW({
        Downloader::fetch(target, url.c_str(), 5l);
    });

    EXPECT_EQ("abcd", target.str());
}

TYPED_TEST(HttpDownloaderTest, InvalidCertificate) {
    if constexpr (!std::is_same_v<TypeParam, httplib::SSLServer>) {
        GTEST_SKIP() << "Skipping Certificate Test for non-SSL server";
    }
    else {
        std::string prefix = setUpInvalidServer(this->svr);
        this->setConnection(prefix);

        this->svr->Get("/hi", [](const httplib::Request&, httplib::Response& res) {
            res.set_content("Hello World!", "text/plain");
            });

        this->startServer();

        std::string url = std::format("{}/hi", this->base_url);
        std::stringstream target{};
        EXPECT_THROW({
            Downloader::fetch(target, url.c_str(), 5l);
            }, std::exception);
    }
}