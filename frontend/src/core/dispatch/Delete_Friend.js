import { HttpRequest } from '@/core/http/httpRequest.js';

export class Delete_Friend extends HttpRequest {
    constructor(userID, xsrfToken, otherUserID) {
        super(
            'DELETE',
            `${import.meta.env.VITE_BACKEND_ORIGIN}/api/users/${userID}/friends`,
            { 'user_id': otherUserID },
            [ { name: 'X-XSRF-TOKEN', value: xsrfToken } ]
        );
    }
}