import { HttpRequest } from './HttpRequest.js';

class Get_UsersDevices extends HttpRequest {
    constructor(userID) {
        super('GET', `/api/users/${userID}/devices`);
    }
}

export { Get_UsersDevices };