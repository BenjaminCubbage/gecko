class NetworkError extends Error {
    constructor(message = null, options = null) {
        super(message, options);
    }
}

class HttpError extends Error {
    constructor(status, body = null, message = null, options = null) {
        super(message, options);

        this.status = status;
        this.body   = body;
    }
}

class ResourceLockedError extends Error {
    constructor(message = null, options = null) {
        super(message, options);
    }
}

export { NetworkError, HttpError, ResourceLockedError };