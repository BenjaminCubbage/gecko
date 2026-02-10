import { HttpRequest } from '@/core/http/httpRequest.js';

export class Get_FriendRequests extends HttpRequest {
    constructor(userID) {
        super('GET', `/api/users/${userID}/friend-requests`);
    }
}