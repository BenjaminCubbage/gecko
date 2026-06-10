import { HttpRequest } from '@/core/http/httpRequest.js';

export class Delete_FriendRequest extends HttpRequest {
    constructor(userID, xsrfToken, otherUserID) {
        super(
            'DELETE',
            `${import.meta.env.VITE_BACKEND_ORIGIN}/api/users/${userID}/friend-requests`,
            { 'user_id': otherUserID },
            [ { name: 'X-XSRF-TOKEN', value: xsrfToken } ]
        );
    }
}