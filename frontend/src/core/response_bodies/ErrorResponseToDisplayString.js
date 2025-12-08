function badRequestReason(reason) {
    switch (reason) {
    case 'username_taken':     return 'That username is taken!';
    case 'username_too_long':  return 'That username is too long';
    case 'username_too_short': return 'That username is too short';
    default: 
        return `Server responded with an error: '${reason}'`
    }
}

export const errorResponseToDisplayString = function(json) {
    if (json?.error?.code)
        switch (json.error.code) {
        case 'bad_request': 
            return badRequestReason(json.error.reason);
        }

    return 'Something went wrong. It\'s probably Ben\'s fault';
}