#include "gecko/Decode.h"

#include "gecko/DNS.h"
#include "gecko/FS.h"
#include "gecko/FSCert.h"
#include "gecko/FSTwoPhase.h"
#include "gecko/Log.h"
#include "gecko/MQTTConn.h"
#include "gecko/MQTTPub.h"
#include "gecko/MQTTSub.h"
#include "gecko/Wifi.h"

#include "libraries/inky_frame_7/inky_frame_7.hpp"

#define XSTR(x) STR(x)
#define STR(x) #x

static constexpr int WakeUpFreqMinutes{ 5 };
pimoroni::InkyFrame inky;

inline constexpr uint16_t RGB_Red = 0b011;
inline constexpr uint16_t RGB_Gre = 0b110;
inline constexpr uint16_t RGB_Blu = 0b101;
inline constexpr uint16_t RGB_Yel = 0b010;
inline constexpr uint16_t RGB_Whi = 0b111;
inline constexpr uint16_t RGB_Bla = 0b000;

[[noreturn]] int DeepSleep()
{
    inky.sleep(WakeUpFreqMinutes);
    for (;;);
}

int main()
{
    using namespace Gecko::Embedded;
    using namespace Gecko::Compression;

    inky.init();
    stdio_init_all();
    inky.rtc.reset();

    std::string_view cert;

    char deviceID[50]{};
    char devicePassword[50]{};

    unsigned int deviceIDLen{};
    unsigned int devicePasswordLen{};

    char hostname[50]{};
    unsigned int hostnameLen{};

    char wifiSSID[50]{};
    char wifiPassword[50]{};

    unsigned int wifiSSIDLen{};
    unsigned int wifiPasswordLen{};

    if (!FS::Mount() ||
        !FSCert::ReadCert(&cert) ||
        !FS::ReadFile("/device_id.txt",       sizeof(deviceID),       deviceID,       &deviceIDLen) ||
        !FS::ReadFile("/device_password.txt", sizeof(devicePassword), devicePassword, &devicePasswordLen) ||
        !FS::ReadFile("/hostname.txt",        sizeof(hostname),       hostname,       &hostnameLen) ||
        !FS::ReadFile("/wifi_ssid.txt",       sizeof(wifiSSID),       wifiSSID,       &wifiSSIDLen) ||
        !FS::ReadFile("/wifi_password.txt",   sizeof(wifiPassword),   wifiPassword,   &wifiPasswordLen) ||
        !Wifi::Init() ||
        !Wifi::ConnectSync(wifiSSID, wifiPassword) ||
        !DNS::Init() ||
        !DNS::QueryHostnameSync(hostname) ||
        !MQTTConn::Init(DNS::ResolvedIP(), 8883, cert, deviceID, devicePassword) ||
        !MQTTConn::ConnectSync() ||
        !MQTTSub::Init(deviceID) ||
        !MQTTPub::Init(deviceID))
    {
        DeepSleep();
    }

    char oldLatestImageID[101]{};
    const char* newLatestImageID{};
    unsigned int oldLatestImageIDLen{};
    unsigned int newLatestImageIDLen{};

    FSTwoPhase::ReadCommitted(
        "/latest_image_id.txt",
        100,
        oldLatestImageID,
        &oldLatestImageIDLen);

    if (!MQTTSub::GetLatestImageIDSync(10, &newLatestImageID, &newLatestImageIDLen))
        DeepSleep();

    MQTTPub::PublishHeartbeatSync();

    Log_Info("Main: Old image ID: %s\n", oldLatestImageID);
    Log_Info("Main: New image ID: %s\n", newLatestImageID);

    if (std::strcmp(oldLatestImageID, newLatestImageID) == 0)
        DeepSleep();

    const char* imageBuffer;
    unsigned int imageSize;

    if (!MQTTSub::GetLatestImageSync(10, &imageBuffer, &imageSize) || !imageSize)
    {
        Log_Error("Main: Waited for image but it never arrived\n");
        DeepSleep();
    }

    std::vector<uint8_t> compressed(imageSize);
    std::memcpy(compressed.data(), imageBuffer, imageSize);

    using CBImageHeader = decltype([] (
            void* c1, void* c2,
            const Gecko::Compression::Header& header)
    {
        *reinterpret_cast<size_t*>(c1) = header.width;
        *reinterpret_cast<size_t*>(c2) = header.height;
        return true;
    });

    using CBImageWriter = decltype([] (
            void* c1, void* c2,
            size_t y,
            size_t xStart,
            size_t xEnd,
            bool white)
    {
        const size_t w = *reinterpret_cast<size_t*>(c1);
        const size_t h = *reinterpret_cast<size_t*>(c2);

        const size_t y0 =  y      * inky.bounds.h / h;
        const size_t y1 = (y + 1) * inky.bounds.h / h;

        inky.set_pen(white ? RGB_Whi : RGB_Bla);

        for (size_t x = xStart; x <= xEnd; ++x)
        {
            const size_t x0 =  x      * inky.bounds.w / w;
            const size_t x1 = (x + 1) * inky.bounds.w / w;

            for (size_t py = y0; py < y1; ++py)
            for (size_t px = x0; px < x1; ++px)
                inky.set_pixel({ int(px), int(py) });
        }

        return true;
    });

    size_t w{};
    size_t h{};
    BitStream bs{ std::move(compressed) };

    auto decompressed =
        Decode<CBImageHeader, CBImageWriter>::TryDecompress(
            bs,
            (void*)&w,
            (void*)&h);

    if (!decompressed)
    {
        Log_Error("Main: Failed to decompress bitonal\n");
        DeepSleep();
    }

    inky.update(true);

    FSTwoPhase::WriteAndCommit(
        "/latest_image_id.txt",
        newLatestImageID,
        newLatestImageIDLen + 1,
        true);

    DeepSleep();
}