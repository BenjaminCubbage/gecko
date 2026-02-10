export class NetworkError extends Error {
    constructor(message = null, options = null) {
        super(message, options);
    }
}

export class HttpError extends Error {
    constructor(status, body = null, message = null, options = null) {
        super(message, options);

        this.status = status;
        this.body   = body;
    }
}

export class PartialCompletionError extends Error {
    constructor(cause, message = null, options = null) {
        super(message, Object.assign({ cause }, options));
    }
}

export class ResourceLockedError extends Error {
    constructor(message = null, options = null) {
        super(message, options);
    }
}