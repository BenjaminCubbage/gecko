class User {
    static fromJSON(json) {
        const user = new User();
        user.userID   = +json['user_id'];
        user.username = String(json['username']);
        return user;
    }
}

export { User }