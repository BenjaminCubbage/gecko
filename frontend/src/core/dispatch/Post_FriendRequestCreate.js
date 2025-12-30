import { HttpRequest } from './HttpRequest.js';

class Post_FriendRequestCreate extends HttpRequest {
    constructor(session, otherUserID) {
        const userID = session.activeUser().json()["user_id"];
        super(
            'POST',
            `/api/users/${userID}/friend-requests`,
            { 'user_id': otherUserID }
        );
    }
}

export { Post_FriendRequestCreate };