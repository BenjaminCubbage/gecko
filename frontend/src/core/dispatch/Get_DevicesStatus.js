import { HttpRequest } from '@/core/http/httpRequest.js';

export class Get_DevicesStatus extends HttpRequest {
    constructor(deviceID) {
        super('GET', `${import.meta.env.VITE_BACKEND_ORIGIN}/api/devices/${deviceID}/status`);
    }
}