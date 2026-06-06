export const CookieNames = Object.freeze({
    XSRFToken: "__Host-xsrf_token"
});

export class Cookies {
    static getByName(name) {
        return document.cookie
            .split('; ')
            .find(r => r.startsWith(`${name}=`))
            ?.split('=')[1];
    }

    static getXSRFToken() {
        return Cookies.getByName(CookieNames.XSRFToken);
    }
};