import { User } from './user.js';

export const FriendStatus = {
    __proto__: null,

    Active:          Symbol('accepted'),
    PendingIncoming: Symbol('pending_incoming'),
    PendingOutgoing: Symbol('pending_outgoing')
};

export class Friend {
    static fromJSON(json, status) {
        const friend      = new Friend();
        friend.status     = status;
        friend.acceptedOn = json['accepted_on'] ?? null;
        friend.user       = User.fromJSON(json['user']);
        return friend;
    }

    static fromOptimistic(user, acceptedOnDate = null) {
        const friend      = new Friend();
        friend.acceptedOn = Friend._stringifyDate(acceptedOnDate);
        friend.user       = user;
        return friend;
    }

    setAcceptedOnNow() {
        this.acceptedOn = Friend._stringifyDate(new Date());
    }

    static _stringifyDate(date) {
        if (!date)
            return null;

        if (!(date instanceof Date))
            return String(date);

        if (date instanceof Date) {
            const mm   = String(date.getMonth() + 1).padStart(2, '0');
            const dd   = String(date.getDate()).padStart(2, '0');
            const yyyy = date.getFullYear();

            return `${mm}/${dd}/${yyyy}`;
        }
    }
}