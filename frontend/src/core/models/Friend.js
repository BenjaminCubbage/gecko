import { User } from './User.js';

class Friend {
    static fromJSON(json) {
        const friend = new Friend();
        friend.acceptedOn = String(json['accepted_on']) ?? null;
        friend.user       = User.fromJSON(json['user']);
        return friend;
    }

    static fromOptimistic(user, acceptedOnDate = null) {
        const friend = new Friend();
        friend.acceptedOn = Friend._stringifyDate(acceptedOnDate);
        friend.user       = user;
        return friend;
    }

    setAcceptedOnNow() {
        this.acceptedOn = Friend._stringifyDate(new Date());
    }

    static _stringifyDate(date) {
        if (date) {
            if (date instanceof Date) {
                let mm   = String(date.getMonth() + 1).padStart(2, '0');
                let dd   = String(date.getDate()).padStart(2, '0');
                let yyyy = date.getFullYear();

                return `${mm}/${dd}/${yyyy}`;
            } else
                return String(date);
        }

        return null;
    }
}

export { Friend }