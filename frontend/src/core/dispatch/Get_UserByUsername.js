import { HttpRequest } from '@/core/http/httpRequest.js';

export class Get_UserByUsername extends HttpRequest {
    constructor(username) {
        super('GET', `${import.meta.env.VITE_BACKEND_ORIGIN}/api/users?username=${username}`);
    }
}