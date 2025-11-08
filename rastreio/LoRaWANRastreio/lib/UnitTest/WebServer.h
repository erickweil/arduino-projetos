#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <Arduino.h>
#include <functional>
#include <memory>
#include <vector>

struct IPAddress {
    uint8_t bytes[4];
    IPAddress(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3) {
        bytes[0] = b0; bytes[1] = b1; bytes[2] = b2; bytes[3] = b3;
    }
};

//typedef enum http_method HTTPMethod;
enum HTTPMethod {
    HTTP_DELETE = 0, HTTP_GET = 1, HTTP_HEAD = 2, HTTP_POST = 3, HTTP_PUT = 4, HTTP_CONNECT = 5, HTTP_OPTIONS = 6, HTTP_TRACE = 7, HTTP_COPY = 8, HTTP_LOCK = 9, HTTP_MKCOL = 10, HTTP_MOVE = 11, HTTP_PROPFIND = 12, HTTP_PROPPATCH = 13, HTTP_SEARCH = 14, HTTP_UNLOCK = 15, HTTP_BIND = 16, HTTP_REBIND = 17, HTTP_UNBIND = 18, HTTP_ACL = 19, HTTP_REPORT = 20, HTTP_MKACTIVITY = 21, HTTP_CHECKOUT = 22, HTTP_MERGE = 23, HTTP_MSEARCH = 24, HTTP_NOTIFY = 25, HTTP_SUBSCRIBE = 26, HTTP_UNSUBSCRIBE = 27, HTTP_PATCH = 28, HTTP_PURGE = 29, HTTP_MKCALENDAR = 30, HTTP_LINK = 31, HTTP_UNLINK = 32,
};
#define HTTP_ANY (HTTPMethod)(255)

enum HTTPUploadStatus { UPLOAD_FILE_START, UPLOAD_FILE_WRITE, UPLOAD_FILE_END,
                        UPLOAD_FILE_ABORTED };
enum HTTPRawStatus { RAW_START, RAW_WRITE, RAW_END, RAW_ABORTED };
enum HTTPClientStatus { HC_NONE, HC_WAIT_READ, HC_WAIT_CLOSE };
enum HTTPAuthMethod { BASIC_AUTH, DIGEST_AUTH };

#define HTTP_DOWNLOAD_UNIT_SIZE 1436

#ifndef HTTP_UPLOAD_BUFLEN
#define HTTP_UPLOAD_BUFLEN 1436
#endif

#ifndef HTTP_RAW_BUFLEN
#define HTTP_RAW_BUFLEN 1436
#endif

#define HTTP_MAX_DATA_WAIT 5000 //ms to wait for the client to send the request
#define HTTP_MAX_POST_WAIT 5000 //ms to wait for POST data to arrive
#define HTTP_MAX_SEND_WAIT 5000 //ms to wait for data chunk to be ACKed
#define HTTP_MAX_CLOSE_WAIT 2000 //ms to wait for the client to close the connection

#define CONTENT_LENGTH_UNKNOWN ((size_t) -1)
#define CONTENT_LENGTH_NOT_SET ((size_t) -2)

class Uri {
    protected:
        const String _uri;
    public:
        Uri(const char *uri) : _uri(uri) {}
        Uri(const String &uri) : _uri(uri) {}
        virtual ~Uri() {}

        virtual Uri* clone() const {
            return new Uri(_uri);
        };

        virtual bool canHandle(const String &requestUri) {
            return _uri == requestUri;
        }
};

class WebServer;

class RequestHandler {
public:
    virtual ~RequestHandler() { }
    virtual bool canHandle(HTTPMethod method, String uri) { (void) method; (void) uri; return false; }
    virtual bool canUpload(String uri) { (void) uri; return false; }
    virtual bool canRaw(String uri) { (void) uri; return false; }
    virtual bool handle(WebServer& server, HTTPMethod requestMethod, String requestUri) { (void) server; (void) requestMethod; (void) requestUri; return false; }
    //virtual void upload(WebServer& server, String requestUri, HTTPUpload& upload) { (void) server; (void) requestUri; (void) upload; }
    //virtual void raw(WebServer& server, String requestUri, HTTPRaw& raw) { (void) server; (void) requestUri; (void) raw; }

    RequestHandler* next() { return _next; }
    void next(RequestHandler* r) { _next = r; }

private:
    RequestHandler* _next = nullptr;

protected:
    std::vector<String> pathArgs;

public:
    const String& pathArg(unsigned int i) { 
        assert(i < pathArgs.size());
        return pathArgs[i];
    }
};

class WebServer
{
private:
    HTTPMethod  _currentMethod;
    String      _currentUri;    

    int _code;
    String _sent = "";
public:
  WebServer(IPAddress addr, int port = 80) { }
  WebServer(int port = 80) { }
  virtual ~WebServer() { }

// Actually used:
  void send(int code, const char* content_type, const char* content) { 
    _code = code;
    _sent = String(content);
  }
  void sendContent(const char* content, size_t contentLength) { 
    _sent += String(content).substring(0, contentLength);
  }
  void sendContent(const String& content) { 
    _sent += content;
  }

// Stub implementations for unit testing
  virtual void begin() { }
  virtual void begin(uint16_t port) { }
  virtual void handleClient() { }

  virtual void close() { }
  void stop() { }

  bool authenticate(const char * username, const char * password) { return false; }
  void requestAuthentication(HTTPAuthMethod mode = BASIC_AUTH, const char* realm = NULL, const String& authFailMsg = String("") ) { }

  typedef std::function<void(void)> THandlerFunction;
  void on(const Uri &uri, THandlerFunction fn) { }
  void on(const Uri &uri, HTTPMethod method, THandlerFunction fn) { }
  void on(const Uri &uri, HTTPMethod method, THandlerFunction fn, THandlerFunction ufn) { }
  void addHandler(RequestHandler* handler) { }
  // void serveStatic(const char* uri, fs::FS& fs, const char* path, const char* cache_header = NULL ) { }
  void onNotFound(THandlerFunction fn) { }
  void onFileUpload(THandlerFunction ufn) { }

  String uri() { return _currentUri; }
  HTTPMethod method() { return _currentMethod; }
  //virtual WiFiClient client() { return _currentClient; }
  //HTTPUpload& upload() { return *_currentUpload; }
  //HTTPRaw& raw() { return *_currentRaw; }

  String pathArg(unsigned int i) { return ""; } // get request path argument by number
  String arg(String name) { return ""; }        // get request argument value by name
  String arg(int i) { return ""; }              // get request argument value by number
  String argName(int i) { return ""; }          // get request argument name by number
  int args() { return 0; }                     // get arguments count
  bool hasArg(String name) { return false; }       // check if argument exists
  void collectHeaders(const char* headerKeys[], const size_t headerKeysCount) {} // set the request headers to collect
  String header(String name) { return ""; }     // get request header value by name
  String header(int i) { return ""; }           // get request header value by number
  String headerName(int i) { return ""; }       // get request header name by number
  int headers() { return 0; }                  // get header count
  bool hasHeader(String name) { return false; }    // check if header exists

  int clientContentLength() { return 0; }      // return "content-length" of incoming HTTP header from "_currentClient"

  String hostHeader() {return "";}            // get request host header if available or empty String if not

  // send response to the client
  // code - HTTP response code, can be 200 or 404
  // content_type - HTTP content type, like "text/plain" or "image/png"
  // content - actual content body
  //void send(int code, const char* content_type = NULL, const String& content = String("")) { }
  //void send(int code, char* content_type, const String& content) { }
  //void send(int code, const String& content_type, const String& content) { }
  //void sendContent(const String& content) { }
//   void send_P(int code, PGM_P content_type, PGM_P content) { }
//   void send_P(int code, PGM_P content_type, PGM_P content, size_t contentLength) { }

  void enableDelay(boolean value) { }
  void enableCORS(boolean value = true) { }
  void enableCrossOrigin(boolean value = true) { }

  void setContentLength(const size_t contentLength) { }
  void sendHeader(const String& name, const String& value, bool first = false) { }
  
//   void sendContent_P(PGM_P content) { }
//   void sendContent_P(PGM_P content, size_t size) { }

  static String urlDecode(const String& text);

//   template<typename T>
//   size_t streamFile(T &file, const String& contentType, const int code = 200) {
//     _streamFileCore(file.size(), file.name(), contentType, code);
//     return _currentClient.write(file);
//   }
};

#endif
