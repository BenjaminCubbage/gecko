export class Device {
    static fromJSON(json) {
        const device = new Device();
        device.deviceID = json['device_id'];
        device.name     = json['name'];

        this.status      = null;
        this.statusError = null;
        return device;
    }

    applyStatusJSON(json) {
        this.status      = json;
        this.statusError = null;
    }

    applyStatusError(error) {
        this.status      = null;
        this.statusError = error;
    }

    updateStatus(deviceStatus) {
        this.status = deviceStatus;
    }
}