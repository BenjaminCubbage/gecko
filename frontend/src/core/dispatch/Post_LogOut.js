import { HttpRequest } from '@/core/http/httpRequest.js';

class Post_LogOut extends HttpRequest {
    constructor(xsrfToken) {
        const url = '/auth/logout';
        super('POST', url, null, [
            { name: 'X-XSRF-TOKEN', value: xsrfToken }
        ]);
    }
}

export { Post_LogOut };