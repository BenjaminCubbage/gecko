import { HttpRequest } from '@/core/http/httpRequest.js';

export class Get_LatestImage extends HttpRequest {
    constructor(userID) {
        super('GET', `/api/users/${userID}/latest-image`, null, null, "blob");
    }
}