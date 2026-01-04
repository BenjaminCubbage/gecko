import { HttpRequest } from './HttpRequest.js';

class Get_FriendRequests extends HttpRequest {
    constructor(userID) {
        super('GET', `/api/users/${userID}/friend-requests`);
    }
}

export { Get_FriendRequests };