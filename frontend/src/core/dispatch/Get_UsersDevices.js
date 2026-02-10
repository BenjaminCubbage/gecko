import { HttpRequest } from '@/core/http/httpRequest.js';

export class Get_UsersDevices extends HttpRequest {
    constructor(userID) {
        super('GET', `/api/users/${userID}/devices`);
    }
}