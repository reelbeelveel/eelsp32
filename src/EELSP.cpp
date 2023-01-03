#include "EELSP.h"

EELPROM::EELPROM(){
    EEPROM.begin(EELPROM_MEM_SIZE);
    init_toc();
    EELPROM_STOP = EELPROM_MEM_SIZE;
}

EELPROM::~EELPROM(){
    EEPROM.end();
}

void EELPROM::init_toc() {
    for(int i = 0; i < EELPROM_SEGMENTS; i++){
        this->toc[i].start = 0;
        if(i)
            this->toc[i].start = this->toc[i-1].start + SEGMENT_SIZE;
        this->toc[i].read  = this->toc[i].start;
        this->toc[i].write = this->toc[i].start;
    }
}

void EELPROM::read(int segment, void *data, size_t size){
    if(segment == EELPROM_SEGMENTS)
        throw std::out_of_range("EELPROM::read: out of range. STOP segment is not readable.");
    EEPROM.readBytes(this->toc[segment].read, data, size);
    this->toc[segment].read += size;
}

void EELPROM::write(int segment, uint8_t value){
    EEPROM.write(this->toc[segment].write, value);
    this->toc[segment].write++;
}

void EELPROM::write(int segment, IPAddress value){
    uint8_t *ip = (uint8_t *)&value;
    for (int i = 0; i < 4; i++) {
        EEPROM.write(this->toc[segment].write, ip[i]);
        this->toc[segment].write++;
    }
}

void EELPROM::write(int segment, const void *data, size_t size){
    if(segment == EELPROM_SEGMENTS)
        throw std::out_of_range("EELPROM::write: out of range. STOP segment is not writable.");
    if(this->toc[segment].write + size >= this->toc[segment+1].start)
        throw std::out_of_range("EELPROM::write: out of range. Segment is full.");
    EEPROM.writeBytes(this->toc[segment].write, data, size);
    this->toc[segment].write += size;
}

void EELPROM::reread(int segment){
    if(segment == EELPROM_SEGMENTS)
        throw std::out_of_range("EELPROM::reread: out of range. Nonsense to reread STOP segment.");
    this->toc[segment].read = this->toc[segment].start;
}
void EELPROM::overwrite(int segment){
    if(segment == EELPROM_SEGMENTS)
        throw std::out_of_range("EELPROM::overwrite: out of range. Nonsense to overwrite STOP segment.");
    this->toc[segment].write = this->toc[segment].start;
}

raw_configured_ssid SSID::cast() {
    raw_configured_ssid ssid;
    strcpy(ssid.ssid, this->ssid);
    strcpy(ssid.password, this->_password);
    ssid.ip = this->_ip;
    ssid.gateway = this->_gateway;
    ssid.subnet = this->_subnet;
    ssid.dns1 = this->_dns1;
    ssid.dns2 = this->_dns2;
    return ssid;
}

EELWiFi::EELWiFi()  { load(); }

void EELWiFi::load() {
    raw_configured_ssid raw;
    EELSP->eeprom.reread(WIFI_SEGMENT);
    EELSP->eeprom.read(WIFI_SEGMENT, &raw, sizeof(raw_ssid));
    while (raw.ssid != NULL && raw.ssid[0] != '\0') {
        EELSP->eeprom.read(WIFI_SEGMENT, &raw + sizeof(raw_ssid::ssid), sizeof(raw_ssid) - sizeof(raw_ssid::ssid));
        if(raw.ip == 0x00) {
            SSID ssid = SSID(raw.ssid, raw.password);
            this->ssidMap[raw.ssid] = ssid;
        }
        else {
            EELSP->eeprom.read(WIFI_SEGMENT, &raw + sizeof(raw_ssid), sizeof(raw_configured_ssid) - sizeof(raw_ssid));
            SSID ssid = SSID(raw.ssid, raw.password, raw.ip, raw.gateway, raw.subnet, raw.dns1, raw.dns2);
            ssidMap[raw.ssid] = ssid;
        }
        EELSP->eeprom.read(WIFI_SEGMENT, &raw, sizeof(raw_ssid::ssid));
    }
    
}

void EELWiFi::scan() {
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i) {
        String ssid = WiFi.SSID(i);
        auto it = ssidMap.find(ssid.c_str());
        if(it == ssidMap.end())
            continue;
        this->ssidStack.push(ssid.c_str());
    }
}

void EELWiFi::connect() {
    if(this->ssidStack.empty())
        return;
    
}


void EELWiFi::save() {

    EELSP->eeprom.overwrite(WIFI_SEGMENT);

    for(auto ssid : ssidMap) {
        raw_configured_ssid raw = ssid.second.cast();
        if(raw.ip == NULLADDRESS)
            EELSP->eeprom.write(WIFI_SEGMENT, &raw, 
                (raw.ip == NULLADDRESS) ? sizeof(raw_ssid) : sizeof(raw_configured_ssid));
    }
    EELSP->eeprom.write(WIFI_SEGMENT, NULLADDRESS);
    dirty = false;
}


void EELWiFi::add(const char *ssid, const char *password,
        IPAddress ip = NULLADDRESS,
        IPAddress gateway = NULLADDRESS,
        IPAddress subnet = NULLADDRESS,
        IPAddress dns1 = NULLADDRESS,
        IPAddress dns2 = NULLADDRESS) {
    add(SSID(ssid, password, ip, gateway, subnet, dns1, dns2));
}

void EELWiFi::add(SSID ssid) {
    ssidMap[ssid.ssid] = ssid;
    dirty = true;
}

void EELWiFi::remove(const char *ssid) {
    ssidMap.erase(ssid);
    dirty = true;
    save();
}


bool SSID::connect()
{
    WiFi.mode(WIFI_STA);
    if (_ip != NULLADDRESS)
    {
        if (!WiFi.config(_ip, _gateway, _subnet, _dns1, _dns2))
            Serial.print("STA Failed to configure on ");
        Serial.println(ssid);
        return false;
    }
    WiFi.begin(ssid, _password);
    return true;
}