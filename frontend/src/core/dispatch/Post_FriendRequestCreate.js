import { HttpRequest } from '@/core/http/httpRequest.js';

export class Post_FriendRequestCreate extends HttpRequest {
    constructor(userID, xsrfToken, otherUserID) {
        super(
            'POST',
            `${import.meta.env.VITE_BACKEND_ORIGIN}/api/users/${userID}/friend-requests`,
            { 'user_id': otherUserID },
            [ { name: 'X-XSRF-TOKEN', value: xsrfToken } ]
        );
    }
}