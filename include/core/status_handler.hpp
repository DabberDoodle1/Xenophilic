#ifndef STATUS_HANDLER_HEADER
#define STATUS_HANDLER_HEADER

#include <string>

enum LogStatus {
    ERROR,
    WARNING,
    SUCCESS
};

class StatusHandler {
public:
    StatusHandler() = delete;
    StatusHandler(const StatusHandler& other) = delete;
    StatusHandler(StatusHandler&& other) = delete;
    StatusHandler& operator=(const StatusHandler& other) = delete;
    StatusHandler& operator=(StatusHandler&& other) = delete;
    ~StatusHandler() = delete;

    static void log(LogStatus status, const char* status_log);
    static inline void log(LogStatus status, const std::string& status_log) {
        log(status, status_log.c_str());
    }
    static void log(LogStatus status, const std::wstring& status_log);
    static inline void log(LogStatus status, const std::u16string& status_log) {
        log(status, std::wstring(status_log.begin(), status_log.end()));
    }
private:
    //static unsigned int alert_box_vao;
};

#endif
