const keys = Object.create({
    isPossiblyAuthenticatedHint: 'is_possibly_authenticated_hint'
});

export class AuthHints {
    /*
        isPossiblyAuthenticatedHint: Indicates if the user
        could possibly be logged in.
    */
    static getIsPossiblyAuthenticatedHint(b) {
        return localStorage.getItem(
            keys.isPossiblyAuthenticatedHint) === '1'
                ? true : false;
    }

    static setIsPossiblyAuthenticatedHint(value) {
        void (value 
            ? localStorage.setItem(keys.isPossiblyAuthenticatedHint, '1')
            : localStorage.removeItem(keys.isPossiblyAuthenticatedHint));
    }
}