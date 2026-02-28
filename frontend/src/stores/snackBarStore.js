import {
    computed,
    reactive
} from 'vue';

/*
    Global store for short status messages. It stores up
    to 200 messages at a time, at which point it deletes
    the oldest 100. It therefore reliably stores 100
    messages.
*/
export class SnackBarStore {
    #messages;
    #latestMessage;
    #nextMessageId;

    constructor() {
        this.#messages = reactive([]);

        this.#latestMessage = computed(() => {
            return this.#messages[this.#messages.length - 1];
        });

        this.#nextMessageId = 0;
    }

    get latestMessage() {
        return this.#latestMessage;
    }

    pushMessage(text) {
        this.#messages.push({
            id:        this.#nextMessageId++,
            text:      text
        });

        if (this.#messages.length >= 200)
            this.#messages.splice(0, 100);
    }
}