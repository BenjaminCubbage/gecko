class DeviceStatus {
    static unknown() {
        return this._fromString('unknown');
    }

    static loading() {
        return this._fromString('loading');
    }

    static error() {
        return this._fromString('error');
    }

    static fromJSON(json) {
        return this._fromString(json);
    }

    static _fromString(str) {
        const status = new DeviceStatus();
        status.state = str;
        return status;
    }

    isUnknown() { return this.state === 'unknown'; }
    isLoading() { return this.state === 'loading'; }
    isError()   { return this.state === 'error';   }
    isPending() { return this.state === 'pending'; }
    isOffline() { return this.state === 'offline'; }
    isOnline()  { return this.state === 'online';  }
}

class Device {
    static fromJSON(json, status = null) {
        const device = new Device();
        device.deviceID = json['device_id'];
        device.name     = json['name'];
        device.status   = status;
        return device;
    }

    updateStatus(deviceStatus) {
        this.status = deviceStatus;
    }
}

export { Device, DeviceStatus };