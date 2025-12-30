import { HttpRequest } from './HttpRequest.js';

class Get_FriendRequests extends HttpRequest {
    constructor(session) {
        const userID = session.activeUser().json()["user_id"];
        super('GET', `/api/users/${userID}/friend-requests`);
    }
}

export { Get_FriendRequests };