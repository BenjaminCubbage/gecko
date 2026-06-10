import { HttpRequest } from '@/core/http/httpRequest.js';

export class Get_UsersDevices extends HttpRequest {
    constructor(userID) {
        super('GET', `${import.meta.env.VITE_BACKEND_ORIGIN}/api/users/${userID}/devices`);
    }
}