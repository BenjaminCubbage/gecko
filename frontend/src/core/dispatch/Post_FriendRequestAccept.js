import { HttpRequest } from './HttpRequest.js';

class Post_FriendRequestAccept extends HttpRequest {
    constructor(userID, xsrfToken, otherUserID) {
        super(
            'POST',
            `/api/users/${userID}/friend-requests/accept`,
            { 'user_id': otherUserID },
            [ { name: 'X-XSRF-TOKEN', value: xsrfToken } ]
        );
    }
}

export { Post_FriendRequestAccept };