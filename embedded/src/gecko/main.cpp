#include "gecko/FS.h"
#include "gecko/FSCert.h"
#include "gecko/FSSafeReadWrite.h"
#include "gecko/Log.h"
#include "gecko/MQTTConn.h"
#include "gecko/MQTTPub.h"
#include "gecko/MQTTSub.h"
#include "gecko/Wifi.h"
#include "libraries/inky_frame_7/inky_frame_7.hpp"

pimoroni::InkyFrame inky;

inline constexpr size_t MQTTConnectRetryAttempts{ 4 };

int main()
{
    using namespace Gecko::Embedded;
    inky.init();
    stdio_init_all();

    std::string_view cert;

    char deviceID[50]{};
    char devicePassword[50]{};

    unsigned int deviceIDLen{};
    unsigned int devicePasswordLen{};

    if (!FS::Mount() ||
        !FSCert::ReadCert(&cert) ||
        !FS::ReadFile("/device_id.txt",       sizeof(deviceID),       deviceID,       &deviceIDLen) ||
        !FS::ReadFile("/device_password.txt", sizeof(devicePassword), devicePassword, &devicePasswordLen) ||
        !Wifi::Init() ||
        !Wifi::ConnectSync(WIFI_SSID, WIFI_PASSWORD) ||
        !MQTTConn::Init(MQTT_IP, MQTT_PORT, cert, deviceID, devicePassword) ||
        !MQTTConn::ConnectSync() ||
        !MQTTSub::Init(deviceID) ||
        !MQTTPub::Init(deviceID))
    {
        sleep_ms(1000);
        inky.sleep();
        return 1;
    }

    char oldLatestImageID[101]{};
    char* newLatestImageID;
    unsigned int oldLatestImageIDLen{};
    unsigned int newLatestImageIDLen{};

    FSSafeReadWrite::ReadFile(
        "/latest_image_id.txt", 
        100, 
        oldLatestImageID, 
        &oldLatestImageIDLen);
        
    if (!MQTTSub::SubscribeToLatestImageIDSync() ||
        !MQTTSub::WaitForLatestImageID(10, &newLatestImageID, &newLatestImageIDLen))
    {
        sleep_ms(1000);
        inky.sleep();
        return false;
    }

    MQTTPub::PublishHeartbeatSync();

    Log_Info("Old image ID: %s\n", oldLatestImageID);
    Log_Info("New image ID: %s\n", newLatestImageID);

    if (std::strcmp(oldLatestImageID, newLatestImageID) == 0)
    {
        Log_Info("Main: No new image\n");
        sleep_ms(1000);
        inky.sleep();
        return false;
    }

    FSSafeReadWrite::WriteFile(
        "/latest_image_id.txt",
        newLatestImageID,
        newLatestImageIDLen,
        true);

    Log_Debug("Main: Preparing to subscribe to the latest "
              "image topic\n");

    // Create test image
    
    uint16_t rgb_Red = 0b011;
    uint16_t rgb_Gre = 0b110;
    uint16_t rgb_Blu = 0b101;
    uint16_t rgb_Yel = 0b010;
    uint16_t rgb_Whi = 0b111;
    uint16_t rgb_Bla = 0b000;

    const int oneSixth = inky.bounds.h / 6;

    for (int y = 0; y < inky.bounds.h; ++y)
    for (int x = 0; x < inky.bounds.w; ++x)
    {
        if (y < oneSixth)          inky.set_pen(rgb_Red);
        else if (y < oneSixth * 2) inky.set_pen(rgb_Gre);
        else if (y < oneSixth * 3) inky.set_pen(rgb_Blu);
        else if (y < oneSixth * 4) inky.set_pen(rgb_Yel);
        else if (y < oneSixth * 5) inky.set_pen(rgb_Whi);
        else                       inky.set_pen(rgb_Bla);

        inky.set_pixel({ x, y });
    }

    inky.update(true);
    inky.sleep();
    return 0;
}