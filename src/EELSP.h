#ifndef __EELSP32_H__
#define __EELSP32_H__
#include <EEPROM.h>
#include <WiFi.h>
#include <map>
#include <stack>

#define NULLADDRESS IPAddress(0, 0, 0, 0)
#define TIMEOUT 12 /*seconds*/ * 1000
#define EELPROM_MEM_SIZE 65536
#define EELPROM_SEGMENTS (sizeof(EELPROM_SEGMENT)/sizeof(int))
#define EELPROM_STOP toc[EELPROM_SEGMENTS].start

#define SEGMENT_SIZE 2048

class EELSP32;
class EELPROM;
class EELWiFi;
class SSID;

typedef struct raw_ssid {
        char ssid[32];
        char password[64];
        IPAddress terminator = NULLADDRESS;
} rawssid;

typedef struct raw_configured_ssid {
    char ssid[32];
    char password[64];
    IPAddress ip;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress dns1;
    IPAddress dns2;
} raw_configured_ssid;

enum EELPROM_SEGMENT
{
    WIFI_SEGMENT
};

class EELSP32
{
public:
    EELSP32();
    ~EELSP32();
    void init();
    void reset();
    EELWiFi* wifi;
    EELPROM* eeprom;
};

class EELPROM
{
public:
    EELPROM();
    ~EELPROM();

    void read(int segment, void *data, size_t size);
    void reread(int segment);
    void overwrite(int segment);
    void write(int segment, uint8_t value);
    void write(int segment, IPAddress value);
    void write(int segment, const void *data, size_t size);

    private:
        void init_toc();

        typedef struct pointers {
            int start;
            int read;
            int write;
        } pointers;

        pointers toc[EELPROM_SEGMENTS+1];
};

class SSID
{
    friend class EELWiFi;
public:
    SSID(const char *ssid = "", const char *password = "",
         IPAddress ip = NULLADDRESS,
         IPAddress gateway = NULLADDRESS,
         IPAddress subnet = NULLADDRESS,
         IPAddress dns1 = NULLADDRESS,
         IPAddress dns2 = NULLADDRESS) : ssid(ssid),
                                         _password(password),
                                         _ip(ip),
                                         _gateway(gateway),
                                         _subnet(subnet),
                                         _dns1(dns1),
                                         _dns2(dns2) {}

    bool connect();
    raw_configured_ssid cast();
    const char *ssid;
private:
    const char *_password;
    IPAddress _ip;
    IPAddress _gateway;
    IPAddress _subnet;
    IPAddress _dns1;
    IPAddress _dns2;
};
class EELWiFi
{
public:
    EELWiFi();
    ~EELWiFi() { if (dirty) save(); }

    void load();
    void save();
    void scan();

    void add(SSID ssid);
    void add(const char *ssid, const char *password,
             IPAddress ip = NULLADDRESS,
             IPAddress gateway = NULLADDRESS,
             IPAddress subnet = NULLADDRESS,
             IPAddress dns1 = NULLADDRESS,
             IPAddress dns2 = NULLADDRESS);
    void remove(const char *ssid);
    inline void remove(SSID ssid) { remove(ssid.ssid); }
    inline bool connect(const char *ssid) { return ssidMap[ssid].connect(); }

private:
    std::map<const char *, SSID> ssidMap;
    std::stack<const char*> ssidStack;
    unsigned long lastConnectAttempt = 0;
    bool connected = false;
    bool dirty = false;
};

extern EELSP32 EELSP;
#endif