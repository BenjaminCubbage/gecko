import { HttpRequest } from './HttpRequest.js';

class Post_LogOut extends HttpRequest {
    constructor(session) {
        const url = '/auth/logout';
        super('POST', url, null, [{ name: 'X-XSRF-TOKEN', value: session.xsrfCookie() }]);
    }
}

export { Post_LogOut };