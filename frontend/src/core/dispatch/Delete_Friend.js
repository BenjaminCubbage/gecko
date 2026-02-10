import { HttpRequest } from '@/core/http/httpRequest.js';

export class Delete_Friend extends HttpRequest {
    constructor(userID, xsrfToken, otherUserID) {
        super(
            'DELETE',
            `/api/users/${userID}/friends`,
            { 'user_id': otherUserID },
            [ { name: 'X-XSRF-TOKEN', value: xsrfToken } ]
        );
    }
}