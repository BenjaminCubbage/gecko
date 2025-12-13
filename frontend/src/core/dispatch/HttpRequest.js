class HttpRequest {
    constructor(method, url, body = null, headers = []) {
        this.xhr = new XMLHttpRequest();

        this._successCb      = null;
        this._anyErrorCb     = null;
        this._httpErrorCb    = null;
        this._networkErrorCb = null;

        this.xhr.addEventListener('load',  this._onLoad.bind(this));
        this.xhr.addEventListener('abort', this._onNetworkError.bind(this));
        this.xhr.addEventListener('error', this._onNetworkError.bind(this));

        this.xhr.open(method, url);

        if (body?.constructor != FormData)
            this.xhr.setRequestHeader('Content-Type', 'application/json');

        for (const header of headers)
            this.xhr.setRequestHeader(header.name, header.value);

        if (body?.constructor == FormData) this.xhr.send(body);
        else if (typeof body === "string") this.xhr.send(body);
        else if (typeof body === "object") this.xhr.send(JSON.stringify(body));
        else                               this.xhr.send();
    }

    onSuccess(fn)      { this._successCb      = fn; return this; }
    onError(fn)        { this._anyErrorCb     = fn; return this; }
    onHttpError(fn)    { this._httpErrorCb    = fn; return this; }
    onNetworkError(fn) { this._networkErrorCb = fn; return this; }

    _parseBody() {
        const text = this.xhr.responseText;
        if (!text) return null;
        try { 
            return JSON.parse(text); 
        } catch { 
            return text; 
        }
    }

    _onLoad() {
        const status = this.xhr.status;
        const body = this._parseBody();

        if (status >= 200 && status < 300) {
            this._successCb?.(body, status, this.xhr);
        } else {
            this._httpErrorCb?.(body, status, this.xhr);
            this._anyErrorCb?.(this.xhr);
        }
    }

    _onNetworkError() {
        this._networkErrorCb?.(this.xhr);
        this._anyErrorCb?.(this.xhr);
    }
}

export { HttpRequest };