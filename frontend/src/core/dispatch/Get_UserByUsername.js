import { HttpRequest } from '@/core/http/httpRequest.js';

export class Get_UserByUsername extends HttpRequest {
    constructor(username) {
        super('GET', `/api/users?username=${username}`);
    }
}