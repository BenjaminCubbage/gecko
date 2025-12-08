import { HttpRequest } from './HttpRequest.js';

class Get_UsersMe extends HttpRequest {
    constructor() {
        super('GET', '/api/users/me');
        const a = 1;
        console.log(a);
    }
}

export { Get_UsersMe };