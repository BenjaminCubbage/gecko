import { HttpRequest } from './HttpRequest.js';

class Delete_Friend extends HttpRequest {
    constructor(userID, xsrfToken, otherUserID) {
        super(
            'DELETE',
            `/api/users/${userID}/friends`,
            { 'user_id': otherUserID },
            [ { name: 'X-XSRF-TOKEN', value: xsrfToken } ]
        );
    }
}

export { Delete_Friend };