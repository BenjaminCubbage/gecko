import { HttpRequest } from '@/core/http/httpRequest.js';

class Get_UserByUsername extends HttpRequest {
    constructor(username) {
        super('GET', `/api/users?username=${username}`);
    }
}

export { Get_UserByUsername };