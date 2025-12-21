import { HttpRequest } from './HttpRequest.js';

class Get_LatestImage extends HttpRequest {
    constructor(session) {
        const userID = session.activeUser().json()["user_id"];
        super('GET', `/api/users/${userID}/latest-image`, null, null, "blob");
    }
}

export { Get_LatestImage };