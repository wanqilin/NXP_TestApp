#include "eventlistenthread.h"
#include <QtDebug>
#include <QThread>


#ifdef OS_WINDOWS
// define GUID
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE,
            0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);

// define GUID_DEVINTERFACE_DISK check usb disk
DEFINE_GUID(GUID_DEVINTERFACE_DISK,
            0x53f56307,0xb6bf,0x11d0,0x94,0xf2,0x00,0xa0,0xc9,0x1e,0xfb,0x8b);
#endif


EventListenThread::EventListenThread() {}
EventListenThread::~EventListenThread() {}

void EventListenThread::process(void)
{
    qDebug()<<"EventListenThread is run!";
    while(1)
    {
        usbCnt = getUSBDeviceCount();
        //if(usbCnt!=0)
        emit RefreshOSD(usbCnt);
        QThread::sleep(1);
    }
}

#ifdef OS_WINDOWS
int EventListenThread::getUSBDeviceCount() {
    // Get usb device info
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(
        &GUID_DEVINTERFACE_DISK, // &GUID_DEVINTERFACE_USB_DEVICE,
        nullptr,
        nullptr,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
        );

    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        qWarning() << "GetDeviceInfo fail";
        return -1;
    }

    // loop device info
    int deviceCount = 0;
    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (int i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); i++) {
        deviceCount++;
    }

    // clean device info
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return deviceCount;
}
#endif
#ifdef OS_UNIX
bool EventListenThread::isUsbStorage(const std::string& devicePath) {
    std::string usbPath = "/sys/class/block/" + devicePath + "/device";
    DIR* dir = opendir(usbPath.c_str());
    if (dir) {
        // If the device directory exists, it is a USB device
        closedir(dir);
        return true;
    }
    return false;
}

int MainWindow::getUSBDeviceCount() {
    int usbCount = 0;

    DIR* dir = opendir("/sys/class/block");
    if (!dir) {
        qDebug() << "Failed to open /sys/class/block";
        return -1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // exclude "." and ".."
        if (entry->d_name[0] == '.')
            continue;

        std::string deviceName(entry->d_name);

        if (isUsbStorage(deviceName)) {
            usbCount++;
            //qDebug() << "Found USB storage: " << deviceName.c_str();
        }
    }

    closedir(dir);
    return usbCount;
}
#endif
