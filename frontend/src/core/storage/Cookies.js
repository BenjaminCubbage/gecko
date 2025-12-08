class Cookies {
    static byName(name) {
        var match = document.cookie.match(new RegExp('(^| )' + name + '=([^;]+)'));
        if (match && match.length === 3)
            return match[2];
        return null;
    }
};

export { Cookies };