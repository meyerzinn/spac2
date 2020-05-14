#pragma once

#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/log/trivial.hpp>
#include <deque>

namespace beast = boost::beast;  // from <boost/beast.hpp>
namespace asio = boost::asio;    // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;        // from <boost/asio/ip/tcp.hpp>
namespace http = beast::http;            // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;  // from <boost/beast/websocket.hpp>

namespace spac::net {

/// Connection provides an easy interface to ASIO async websocket connections, using read/write buffers and error
/// handling.
template <typename Derived>
class Connection : public std::enable_shared_from_this<Derived> {
 public:
  beast::error_code write(const std::string& buffer) {
    if (error_) return error_;
    writeQueue_.push_back(buffer);
    if (writeQueue_.size() <= 1)
      ws_.async_write(asio::buffer(writeQueue_.front()),
                      beast::bind_front_handler(&Connection<Derived>::on_write, shared_from_this()));
    return error_;
  }

  bool read(std::string& buffer) {
    if (error_) return false;
    if (!readQueue_.empty()) {
      auto top = readQueue_.front();
      readQueue_.pop_front();
      buffer.assign(top);
      return true;
    }
    return false;
  }

  /// returns the current error state of the connection, or an empty error if there is none.
  beast::error_code error() { return error_; }

  beast::error_code close() {
    // set the error here to guard against further write operations
    if (!error_) error_ = beast::websocket::error::closed;
    ws_.async_close(websocket::close_code::normal,
                    beast::bind_front_handler(&Connection<Derived>::on_close, shared_from_this()));
    return error_;
  }

  static void fail(Connection& conn, beast::error_code ec, char const* what) {
    if (!conn.error_) conn.error_ = ec;
    BOOST_LOG_TRIVIAL(error) << what << ": " << ec.message() << std::endl;
  }

 protected:
  explicit Connection(websocket::stream<beast::ssl_stream<beast::tcp_stream>>&& ws) : ws_(std::move(ws)) {}

  websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws_;
  beast::flat_buffer readBuffer_;
  std::deque<std::string> readQueue_;
  std::deque<std::string> writeQueue_;
  beast::error_code error_;

  void on_write(beast::error_code ec, size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    // Stop writing early on any error, since we don't want to waste time copying buffers we know won't be sent.
    if (ec || error_) return fail(*this, ec, "write");
    writeQueue_.erase(writeQueue_.begin());
    if (!writeQueue_.empty()) {
      ws_.async_write(asio::buffer(writeQueue_.front()),
                      beast::bind_front_handler(&Connection<Derived>::on_write, shared_from_this()));
    }
  }

  void do_read() {
    ws_.async_read(readBuffer_, beast::bind_front_handler(&Connection::on_read, shared_from_this()));
  }

  void on_read(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec) fail(*this, ec, "read");
    readQueue_.push_back(beast::buffers_to_string(readBuffer_.cdata()));
    readBuffer_.clear();
    if (!error_) do_read();
  }

  void on_close(beast::error_code ec) { boost::ignore_unused(ec); }
};

}  // namespace spac::net