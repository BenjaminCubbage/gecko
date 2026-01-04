import { HttpRequest } from './HttpRequest.js';

class Get_LatestImage extends HttpRequest {
    constructor(userID) {
        super('GET', `/api/users/${userID}/latest-image`, null, null, "blob");
    }
}

export { Get_LatestImage };