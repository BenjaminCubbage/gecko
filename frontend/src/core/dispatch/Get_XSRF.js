import { HttpRequest } from './HttpRequest.js';

class Get_XSRF extends HttpRequest {
    // Asks the server for an XSRF token
    //
    // Response is not in the response body--it's
    // in the Set-Cookie header.
    //
    // XSRF token should be included in all
    // non-GET ops.
    constructor() {
        super('GET', '/auth/xsrf');
    }
}

export { Get_XSRF };