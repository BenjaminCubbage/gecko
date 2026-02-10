import { HttpRequest } from '@/core/http/httpRequest.js';

export class Get_DevicesStatus extends HttpRequest {
    constructor(deviceID) {
        super('GET', `/api/devices/${deviceID}/status`);
    }
}