import { HttpRequest } from '@/core/http/httpRequest.js';

class Delete_FriendRequest extends HttpRequest {
    constructor(userID, xsrfToken, otherUserID) {
        super(
            'DELETE',
            `/api/users/${userID}/friend-requests`,
            { 'user_id': otherUserID },
            [ { name: 'X-XSRF-TOKEN', value: xsrfToken } ]
        );
    }
}

export { Delete_FriendRequest };