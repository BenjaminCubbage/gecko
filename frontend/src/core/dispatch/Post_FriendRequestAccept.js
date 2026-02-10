import { HttpRequest } from '@/core/http/httpRequest.js';

export class Post_FriendRequestAccept extends HttpRequest {
    constructor(userID, xsrfToken, otherUserID) {
        super(
            'POST',
            `/api/users/${userID}/friend-requests/accept`,
            { 'user_id': otherUserID },
            [ { name: 'X-XSRF-TOKEN', value: xsrfToken } ]
        );
    }
}