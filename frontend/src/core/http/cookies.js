const CookieNames = Object.freeze({
    XSRFToken: "__Host-xsrf_token"
});

class Cookies {
    static getByName(name) {
        var match = document.cookie.match(new RegExp('(^| )' + name + '=([^;]+)'));

        return match && match.length === 3
            ? match[2]
            : null;
    }

    static getXSRFToken() {
        return Cookies.getByName(CookieNames.XSRFToken);
    }
};

export { CookieNames, Cookies };