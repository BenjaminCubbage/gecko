import { HttpRequest } from './HttpRequest.js';

class Post_SharedImage extends HttpRequest {
    constructor(userID, xsrfToken, idempotencyKey, receiver, gibBlob) {
        const formData = new FormData();
        formData.append('metadata', JSON.stringify({ recipient_device_id: receiver }));
        formData.append('content', gibBlob);

        super('POST', `/api/users/${userID}/shared-images`, formData, [
            { name: 'X-XSRF-TOKEN',    value: xsrfToken },
            { name: 'Idempotency-Key', value: idempotencyKey }
        ]);
    }
};

export { Post_SharedImage };