import { HttpRequest } from '@/core/http/httpRequest.js';

class Get_FriendRequests extends HttpRequest {
    constructor(userID) {
        super('GET', `/api/users/${userID}/friend-requests`);
    }
}

export { Get_FriendRequests };