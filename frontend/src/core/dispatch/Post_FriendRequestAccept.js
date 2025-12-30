import { HttpRequest } from './HttpRequest.js';

class Post_FriendRequestAccept extends HttpRequest {
    constructor(session, otherUserID) {
        const userID = session.activeUser().json()["user_id"];
        super(
            'POST',
            `/api/users/${userID}/friend-requests/accept`, 
            { 'user_id': otherUserID }, 
            [
                { name: 'X-XSRF-TOKEN', value: session.xsrfCookie() }
            ]
        );
    }
}

export { Post_FriendRequestAccept };