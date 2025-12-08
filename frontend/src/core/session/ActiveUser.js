class ActiveUser {
    constructor(json) {
        this._json = json;
    }

    json() { 
        return this._json; 
    }
}

export { ActiveUser };
