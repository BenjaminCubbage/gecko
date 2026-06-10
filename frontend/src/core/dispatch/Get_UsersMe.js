import { HttpRequest } from '@/core/http/httpRequest.js';

export class Get_UsersMe extends HttpRequest {
    constructor() {
        super('GET', `${import.meta.env.VITE_BACKEND_ORIGIN}/api/users/me`);
    }
}