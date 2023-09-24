#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <string>
#include <libudev.h>

using namespace std;

int main()
{
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *entry;

    udev = udev_new();

    if(udev == nullptr)
    {
        std::cerr << "Udev init failed" << endl;
        return 1;
    }

    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "block");
    udev_enumerate_scan_devices(enumerate);

    cout << "Enumerate disks:" << endl;

    devices = udev_enumerate_get_list_entry(enumerate);
    udev_list_entry_foreach(entry, devices)
    {
        const char *path = udev_list_entry_get_name(entry);
        struct udev_device *device = udev_device_new_from_syspath(udev, path);

        const char *devNode = udev_device_get_devnode(device);
        const char *devName = udev_device_get_property_value(device, "ID_FS_LABEL");
        const char *devFSType = udev_device_get_property_value(device, "ID_FS_TYPE");
        const char *devFSUsage = udev_device_get_property_value(device, "ID_FS_USAGE");
        const char *devUUID = udev_device_get_property_value(device, "ID_FS_UUID");
        const char *devMounted = udev_device_get_property_value(device, "mount");
        const char *devFSSize = udev_device_get_property_value(device, "ID_FS_SIZE");

        //        auto entry_prop = udev_device_get_properties_list_entry(device);
        //        udev_list_entry *first;
        //        vector<std::string> props;
        //        udev_list_entry_foreach(first, entry_prop)
        //        {
        //            props.emplace_back(udev_list_entry_get_name(first) + std::string(" = ") + udev_list_entry_get_value(first));
        //        }

        cout << "Device: " << devNode << endl;
        if(devName)
            cout << "Label: " << devName << endl;
        if(devFSType)
            cout << "File system: " << devFSType << endl;
        if(devMounted)
            cout << "Mounted at: " << devMounted << endl;
        if(devFSSize)
            cout << "Size: " << (atoll(devFSSize) / 1024 / 1024 / 1024) << " GiB" << endl;
        if(devFSUsage)
            cout << "Usage: " << devFSUsage << endl;
        if(devUUID)
            cout << "UUID: " << devUUID << endl;

        cout << "---------------------" << endl;

        udev_device_unref(device);
    }

    // Release context
    udev_enumerate_unref(enumerate);
    udev_unref(udev);
    return 0;
}
