import { characterIsAlphaNumeric } from '@/core/string/characterIsAlphaNumeric.js';

export const minUsernameLength = 3;
export const maxUsernameLength = 18;

export function isValidUsernameChar(c) {
    return characterIsAlphaNumeric(c) || c == '_';
}

export function isValidUsername(str) {
    return str.length >= minUsernameLength &&
           str.length <= maxUsernameLength &&
           str.split('').every(isValidUsernameChar);
}

export function removeInvalidUsernameCharacters(str) {
    return str.split('')
        .filter(isValidUsernameChar)
        .join('');
}