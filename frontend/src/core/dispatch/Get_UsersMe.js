import { HttpRequest } from '@/core/http/httpRequest.js';

export class Get_UsersMe extends HttpRequest {
    constructor() {
        super('GET', '/api/users/me');
    }
}