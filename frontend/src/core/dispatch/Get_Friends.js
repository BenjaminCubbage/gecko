import { HttpRequest } from './HttpRequest.js';

class Get_Friends extends HttpRequest {
    constructor(session) {
        const userID = session.activeUser().json()["user_id"];
        super('GET', `/api/users/${userID}/friends`);
    }
}

export { Get_Friends };