import { HttpRequest } from './HttpRequest.js';

class Post_SharedImage extends HttpRequest {
    constructor(session, idempotencyKey, receiver, gibBlob) {
        const userID = session.activeUser().json()['user_id'];

        const formData = new FormData();
        
        formData.append('metadata', JSON.stringify({ receiver_user_id: receiver }));
        formData.append('content', gibBlob);

        super('POST', `/api/users/${userID}/shared-images`, formData, [
            { name: 'X-XSRF-TOKEN',    value: session.xsrfCookie() },
            { name: 'Idempotency-Key', value: idempotencyKey }
        ]);
    }
};

export { Post_SharedImage };