import { HttpRequest } from '@/core/http/httpRequest.js';

export class Get_Friends extends HttpRequest {
    constructor(userID) {
        super('GET', `/api/users/${userID}/friends`);
    }
}