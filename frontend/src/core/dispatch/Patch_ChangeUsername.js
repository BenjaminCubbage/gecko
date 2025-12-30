import { HttpRequest } from './HttpRequest.js';

class Patch_ChangeUsername extends HttpRequest {
    constructor(session, newUsername) {
        const url = `/api/users/${session.activeUser().json()["user_id"]}`;
        super(
            'PATCH', 
            url, 
            { 'username': newUsername }, 
            [
                { name: 'X-XSRF-TOKEN', value: session.xsrfCookie() }
            ]
        );
    }
}

export { Patch_ChangeUsername };