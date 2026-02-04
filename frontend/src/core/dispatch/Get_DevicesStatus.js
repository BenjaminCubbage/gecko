import { HttpRequest } from '@/core/http/httpRequest.js';

class Get_DevicesStatus extends HttpRequest {
    constructor(deviceID) {
        super('GET', `/api/devices/${deviceID}/status`);
    }
}

export { Get_DevicesStatus };