function conflict(reason) {
    switch (reason) {
    case 'username_taken':     return 'That username is taken!';
    default:
        return `Server responded with a conflict error: ${reason}`;
    }
}

function badRequest(reason) {
    switch (reason) {
    case 'username_taken':     return 'That username is taken!';
    case 'username_too_long':  return 'That username is too long';
    case 'username_too_short': return 'That username is too short';
    default:
        return `Server responded with a bad_request error: ${reason}`;
    }
}

export const errorResponseToDisplayString = function(json) {
    if (json?.error?.code)
        switch (json.error.code) {
        case 'bad_request': return badRequest(json.error.reason);
        case 'conflict':    return conflict(json.error.reason);
        default:
            return `Something went wrong. It's probably Ben's fault: ${json.error.code} ${json.error.reason}`;
        }

    return `Something went wrong. It's probably Ben's fault.`;
}