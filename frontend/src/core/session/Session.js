class Session {
    constructor(activeUser = null, xsrfCookie = null) {
        this._activeUser = activeUser;
        this._xsrfCookie = xsrfCookie;
    }

    activeUser() { return this._activeUser; }
    xsrfCookie() { return this._xsrfCookie; }

    setActiveUser(value) { this._activeUser = value; }
    setXSRFCookie(value) { this._xsrfCookie = value; }
    
    loggedIn() {
        return this._activeUser != null && 
               this._xsrfCookie != null; 
    }
};

export { Session };
