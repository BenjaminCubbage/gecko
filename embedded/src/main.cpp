#include "gecko/UncompressedBitonal.h"
#include "gecko/CompressedBitonal.h"
#include "gecko/Decoder.h"

#include "gecko/FS.h"
#include "gecko/FSCert.h"
#include "gecko/FSTwoPhase.h"
#include "gecko/Log.h"
#include "gecko/MQTTConn.h"
#include "gecko/MQTTPub.h"
#include "gecko/MQTTSub.h"
#include "gecko/Wifi.h"

#include "libraries/inky_frame_7/inky_frame_7.hpp"

pimoroni::InkyFrame inky;

inline constexpr size_t MQTTConnectRetryAttempts{ 4 };

inline constexpr uint16_t RGB_Red = 0b011;
inline constexpr uint16_t RGB_Gre = 0b110;
inline constexpr uint16_t RGB_Blu = 0b101;
inline constexpr uint16_t RGB_Yel = 0b010;
inline constexpr uint16_t RGB_Whi = 0b111;
inline constexpr uint16_t RGB_Bla = 0b000;

int WakeUpIn(int minutes)
{
    sleep_ms(50);
    inky.sleep(minutes);
    return 0;
}

int main()
{
    using namespace Gecko::Embedded;
    using namespace Gecko::Compression;

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
        return WakeUpIn(5);

    char oldLatestImageID[101]{};
    const char* newLatestImageID;
    unsigned int oldLatestImageIDLen{};
    unsigned int newLatestImageIDLen{};

    FSTwoPhase::ReadCommitted(
        "/latest_image_id.txt",
        100,
        oldLatestImageID,
        &oldLatestImageIDLen);

    if (!MQTTSub::GetLatestImageIDSync(10, &newLatestImageID, &newLatestImageIDLen))
        return WakeUpIn(5);

    MQTTPub::PublishHeartbeatSync();

    Log_Info("Main: Old image ID: %s\n", oldLatestImageID);
    Log_Info("Main: New image ID: %s\n", newLatestImageID);

    if (std::strcmp(oldLatestImageID, newLatestImageID) == 0)
        /* No new image. */
        return WakeUpIn(5);

    const char* imageBuffer;
    unsigned int imageSize;

    if (!MQTTSub::GetLatestImageSync(10, &imageBuffer, &imageSize) || !imageSize)
    {
        Log_Error("Main: Waited for image but it never arrived\n");
        return WakeUpIn(5);
    }

    std::vector<uint8_t> compressed(imageSize);
    std::memcpy(compressed.data(), imageBuffer, imageSize);

    auto compressedBitonal = CompressedBitonal::TryReadFromBuffer(
        compressed, CompressedBitonal::StorageFormat::GIB);

    if (!compressedBitonal)
    {
        Log_Error("Main: Could't initialize CompressedBitonal object\n");
        return WakeUpIn(5);
    }

    auto writeSpan = [] (void* c1, void* c2,
                         size_t y,
                         size_t xStart,
                         size_t xEnd,
                         bool white)
    {
        const size_t w = (size_t)c1;
        const size_t h = (size_t)c2;

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
    };

    auto decompressed = Decoder::TryDecompressBitonal(
        *compressedBitonal, {
            writeSpan,
            (void*)compressedBitonal->GetWidth(),
            (void*)compressedBitonal->GetHeight()
        });

    if (!decompressed)
    {
        Log_Error("Main: Failed to decompress bitonal\n");
        return WakeUpIn(5);
    }

    inky.update(true);

    FSTwoPhase::WriteAndCommit(
        "/latest_image_id.txt",
        newLatestImageID,
        newLatestImageIDLen + 1,
        true);

    return WakeUpIn(5);
}