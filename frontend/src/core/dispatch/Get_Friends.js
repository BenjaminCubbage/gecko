import { HttpRequest } from '@/core/http/httpRequest.js';

class Get_Friends extends HttpRequest {
    constructor(userID) {
        super('GET', `/api/users/${userID}/friends`);
    }
}

export { Get_Friends };