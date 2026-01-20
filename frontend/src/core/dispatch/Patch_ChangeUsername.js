import { HttpRequest } from './HttpRequest.js';

class Patch_ChangeUsername extends HttpRequest {
    constructor(userID, xsrfToken, newUsername) {
        super(
            'PATCH',
            `/api/users/${userID}`,
            { 'username': newUsername },
            [ { name: 'X-XSRF-TOKEN', value: xsrfToken } ]
        );
    }
}

export { Patch_ChangeUsername };