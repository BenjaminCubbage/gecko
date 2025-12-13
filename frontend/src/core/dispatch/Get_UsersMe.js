import { HttpRequest } from './HttpRequest.js';

class Get_UsersMe extends HttpRequest {
    constructor() {
        super('GET', '/api/users/me');
    }
}

export { Get_UsersMe };