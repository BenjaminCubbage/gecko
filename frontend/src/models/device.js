export class Device {
    static fromJSON(json) {
        const device = new Device();
        device.deviceID = json['device_id'];
        device.name     = json['name'];
        device.status   = json['status'];
        return device;
    }
}