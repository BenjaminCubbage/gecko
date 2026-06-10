import { HttpRequest } from '@/core/http/httpRequest.js';

export class Patch_ChangeUsername extends HttpRequest {
    constructor(userID, xsrfToken, newUsername) {
        super(
            'PATCH',
            `${import.meta.env.VITE_BACKEND_ORIGIN}/api/users/${userID}`,
            { 'username': newUsername },
            [ { name: 'X-XSRF-TOKEN', value: xsrfToken } ]
        );
    }
}